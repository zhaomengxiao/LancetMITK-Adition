﻿#include "lancetNCC.h"
#include <stdio.h>
#include <tchar.h>

GeoMatch::GeoMatch(void)
{
	noOfCordinates = 0;  // Initilize  no of cppodinates in model points
	modelDefined = false;
}


int GeoMatch::CreateGeoMatchModel(const void* templateArr, double maxContrast, double minContrast)
{

	CvMat* gx = nullptr;		//Matrix to store X derivative
	CvMat* gy = nullptr;		//Matrix to store Y derivative
	CvMat* nmsEdges = nullptr;		//Matrix to store temp restult
	CvSize Ssize;

	// Convert IplImage to Matrix for integer operations
	CvMat srcstub, * src = (CvMat*)templateArr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1)
	{
		return 0;
	}
	
	// set width and height
	Ssize.width = src->width;
	Ssize.height = src->height;
	modelHeight = src->height;		//Save Template height
	modelWidth = src->width;			//Save Template width
	
	noOfCordinates = 0;											//initialize	
	cordinates = new CvPoint[modelWidth * modelHeight];		//Allocate memory for coordinates of selected points in template image

	edgeMagnitude = new double[modelWidth * modelHeight];		//Allocate memory for edge magnitude for selected points
	edgeDerivativeX = new double[modelWidth * modelHeight];			//Allocate memory for edge X derivative for selected points
	edgeDerivativeY = new double[modelWidth * modelHeight];			////Allocate memory for edge Y derivative for selected points


	// Calculate gradient of Template
	gx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);		//create Matrix to store X derivative
	gy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);		//create Matrix to store Y derivative
	cvSobel(src, gx, 1, 0, 3);		//gradient in X direction			
	cvSobel(src, gy, 0, 1, 3);	    //gradient in Y direction

	nmsEdges = cvCreateMat(Ssize.height, Ssize.width, CV_32F);		//create Matrix to store Final nmsEdges
	const short* _sdx;
	const short* _sdy;
	double fdx, fdy;
	double MagG, DirG;
	double MaxGradient = -99999.99;
	double direction;
	int* orients = new int[Ssize.height * Ssize.width];
	int count = 0, i, j; // count variable;

	double** magMat;
	CreateDoubleMatrix(magMat, Ssize);

	// initialize magMat
	for(int m{0}; m < Ssize.height; m++)
	{
		for(int n{0}; n < Ssize.width; n++)
		{
			magMat[m][n] = 0.0;
		}
	}

	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			_sdx = (short*)(gx->data.ptr + gx->step * i); // Get the entire row
			_sdy = (short*)(gy->data.ptr + gy->step * i); // Get the entire row
			fdx = _sdx[j]; fdy = _sdy[j];        // read x, y derivatives

			MagG = sqrt((fdx * fdx) + (fdy * fdy)); //Magnitude = Sqrt(gx^2 +gy^2)

			double a = ceil(fdy * 100000.0);
			double b = ceil(fdx * 100000.0);

			direction = cvFastArctan(a, b);	 //Direction = invtan (Gy / Gx)
			magMat[i][j] = MagG;

			if (MagG > MaxGradient)
				MaxGradient = MagG; // get maximum gradient value for normalizing.
			// cout << "MagG: " << MagG <<endl;

			// get closest angle from 0, 45, 90, 135 set
			if ((direction > 0.0 && direction < 22.5) || (direction > 157.5 && direction < 202.5) || (direction > 337.5 && direction < 360))
				direction = 0.0;
			else if ((direction > 22.5 && direction < 67.5) || (direction > 202.5 && direction < 247.5))
				direction = 45.0;
			else if ((direction > 67.5 && direction < 112.5) || (direction > 247.5 && direction < 292.5))
				direction = 90.0;
			else if ((direction > 112.5 && direction < 157.5) || (direction > 292.5 && direction < 337.5))
				direction = 135.0;
			else
				direction = 0.0;

			orients[count] = (int)direction;
			count++;


		}
	}

	count = 0; // init count
	// non maximum suppression
	double leftPixel, rightPixel;
	
	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			switch (orients[count])
			{
			case 0:
				leftPixel = magMat[i][j - 1];
				rightPixel = magMat[i][j + 1];
				break;
			case 45:
				leftPixel = magMat[i - 1][j + 1];
				rightPixel = magMat[i + 1][j - 1];
				break;
			case 90:
				leftPixel = magMat[i - 1][j];
				rightPixel = magMat[i + 1][j];
				break;
			case 135:
				leftPixel = magMat[i - 1][j - 1];
				rightPixel = magMat[i + 1][j + 1];
				break;
			}
			// compare current pixels value with adjacent pixels
			if ((magMat[i][j] < leftPixel) || (magMat[i][j] < rightPixel))
				(nmsEdges->data.ptr + nmsEdges->step * (i))[j] = 0;
			else
			{
				(nmsEdges->data.ptr + nmsEdges->step * (i))[j] = (uchar)(magMat[i][j] / MaxGradient * 255); // Normalized to [0,255] 
				
			}
			count++;
		}
	}


	int RSum = 0, CSum = 0;
	int curX, curY;
	int flag = 1;

	//Hysteresis threshold
	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			_sdx = (short*)(gx->data.ptr + gx->step * i); // Row vector
			_sdy = (short*)(gy->data.ptr + gy->step * i); // Row vector
			fdx = _sdx[j]; fdy = _sdy[j]; // read x,y derivatives

			MagG = sqrt(fdx * fdx + fdy * fdy); // Magnitude = Sqrt(gx^2 +gy^2)
			DirG = cvFastArctan((float)fdy, (float)fdx);	 // Direction = tan(y/x)

			////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]= MagG;
			flag = 1;
			if (((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i)))[j]) < maxContrast)
			{
				if (((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i)))[j]) < minContrast)
				{

					(nmsEdges->data.ptr + (nmsEdges->step) * (i))[j] = 0;
					flag = 0; // remove from edge
					////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
				}
				else
				{   // if any of 8 neighboring pixel is not greater than max contrast remove from edge
					if ((((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i - 1)))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i - 1)))[j]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i - 1)))[j + 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * i))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * i))[j + 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i + 1)))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i + 1)))[j]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + (nmsEdges->step) * (i + 1)))[j + 1]) < maxContrast))
					{
						(nmsEdges->data.ptr + (nmsEdges->step) * i)[j] = 0;
						flag = 0;
						////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
					}
				}

			}



			// save selected edge information
			curX = i;	curY = j; // current x and current y
			if (flag != 0)
			{
				if (abs(fdx) > 0.000001 || abs(fdy) >  0.000001)
				{
					RSum = RSum + curX;	CSum = CSum + curY; // Row sum and column sum for center of gravity

					cordinates[noOfCordinates].x = curX;
					cordinates[noOfCordinates].y = curY;
					edgeDerivativeX[noOfCordinates] = fdx;
					edgeDerivativeY[noOfCordinates] = fdy;

					//handle divided by zero
					if (abs(MagG) > 0.000001)
						edgeMagnitude[noOfCordinates] = 1 / MagG;  // gradient magnitude 
					else
						edgeMagnitude[noOfCordinates] = 0;

					noOfCordinates++;
				}
			}
		}
	}
	// cout << "noOfCordinates: " << noOfCordinates;
	centerOfGravity.x = RSum / noOfCordinates; // center of gravity
	centerOfGravity.y = CSum / noOfCordinates;	// center of gravity

	cout << "Center of gravity x: " << centerOfGravity.x << endl;
	cout << "RSum: " << RSum << endl;
	cout << "noOfCordinates: " << noOfCordinates << endl;

	// change coordinates to reflect center of gravity
	for (int m = 0; m < noOfCordinates; m++)
	{
		int temp;

		temp = cordinates[m].x;
		cordinates[m].x = temp - centerOfGravity.x;
		temp = cordinates[m].y;
		cordinates[m].y = temp - centerOfGravity.y;
	}

	////cvSaveImage("Edges.bmp",imgGDir);

	// free alocated memories
	delete[] orients;
	////cvReleaseImage(&imgGDir);
	cvReleaseMat(&gx);
	cvReleaseMat(&gy);
	cvReleaseMat(&nmsEdges);

	ReleaseDoubleMatrix(magMat, Ssize.height);

	modelDefined = true;
	return 1;
}


double GeoMatch::FindGeoMatchModel(const void* srcarr, double minScore, double greediness, CvPoint* resultPoint)
{
	CvMat* Sdx = nullptr, * Sdy = nullptr;

	double resultScore = 0.0;
	double partialSum = 0.0;
	double sumOfCoords = 0.0;
	double partialScore = 0.0;
	const short* _Sdx = nullptr;
	const short* _Sdy = nullptr;
	int i = 0, j = 0, m = 0;			// count variables
	double iTx = 0.0, iTy = 0.0, iSx = 0.0, iSy = 0.0;
	double gradMag = 0.0;
	int curX = 0, curY = 0;

	double** matGradMag = nullptr;  //Gradient magnitude matrix

	CvMat srcstub;
	CvMat* src = (CvMat*)srcarr;
	CvMat* copy_src = nullptr;
	copy_src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(copy_src->type) != CV_8UC1 || !modelDefined)
	{
		return 0;
	}

	// source image size
	CvSize Ssize = CvSize(); 
	Ssize.width = copy_src->width;
	Ssize.height = copy_src->height;

	CreateDoubleMatrix(matGradMag, Ssize); // create image to save gradient magnitude  values

	Sdx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // X derivatives
	Sdy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // y derivatives

	cvSobel(copy_src, Sdx, 1, 0, 3);  // find X derivatives
	cvSobel(copy_src, Sdy, 0, 1, 3); // find Y derivatives

	// stopping criteria to search for model
	double normMinScore = (minScore / (double)noOfCordinates); // precompute minimum score 
	double normGreediness = ((1.0 - greediness * minScore) / (1.0 - greediness)) / (double)noOfCordinates; // precompute greedniness 

	for (i = 0; i < Ssize.height; i++)
	{
		_Sdx = (short*)(Sdx->data.ptr + Sdx->step * i);
		_Sdy = (short*)(Sdy->data.ptr + Sdy->step * i);

		for (j = 0; j < Ssize.width; j++)
		{
			iSx = _Sdx[j];  // X derivative of Source image
			iSy = _Sdy[j];  // Y derivative of Source image

			gradMag = sqrt((iSx * iSx) + (iSy * iSy)); //Magnitude = Sqrt(dx^2 +dy^2)

			if (abs(gradMag) > 0.000001) // hande divide by zero
				matGradMag[i][j] = 1.0 / gradMag;   // 1/Sqrt(dx^2 +dy^2)
			else
				matGradMag[i][j] = 0.0;

		}
	}
	for (i = 0; i < Ssize.height; i++)
	{
		for (j = 0; j < Ssize.width; j++)
		{
			partialSum = 0.0; // initialize partialSum measure
			for (m = 0; m < noOfCordinates; m++)
			{
				curX = i + cordinates[m].x;	// template X coordinate
				curY = j + cordinates[m].y; // template Y coordinate
				iTx = edgeDerivativeX[m];	// template X derivative
				iTy = edgeDerivativeY[m];    // template Y derivative

				if (curX<0 || curY<0 || curX>Ssize.height - 1 || curY>Ssize.width - 1)
					continue;

				_Sdx = (short*)(Sdx->data.ptr + Sdx->step * curX);
				_Sdy = (short*)(Sdy->data.ptr + Sdy->step * curX);

				iSx = _Sdx[curY]; // get corresponding  X derivative from source image
				iSy = _Sdy[curY]; // get corresponding  Y derivative from source image

				if ((abs(iSx) > 0.000001 || abs(iSy) > 0.000001) && (abs(iTx) > 0.000001 || abs(iTy) > 0.000001))
				{
					//partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge magnitude of(Template)* edge magnitude of(Source))
					partialSum = partialSum + ((iSx * iTx) + (iSy * iTy)) * (edgeMagnitude[m] * matGradMag[curX][curY]);

				}

				sumOfCoords = m + 1.0;
				partialScore = partialSum / sumOfCoords;
				// check termination criteria
				// if partial score score is less than the score than needed to make the required score at that position
				// break searching at that coordinate.
				if (partialScore < (MIN((minScore - 1.0) + normGreediness * sumOfCoords, normMinScore * sumOfCoords)))
					break;

			}

			if ((partialScore > resultScore))
			{
				resultScore = partialScore; //  Match score
				resultPoint->x = i;			// result coordinate X		
				resultPoint->y = j;			// result coordinate Y
			}
		}
	}

	// free used resources and return score
	ReleaseDoubleMatrix(matGradMag, Ssize.height);
	cvReleaseMat(&Sdx);
	cvReleaseMat(&Sdy);

	return resultScore;
}

double GeoMatch::FindGeoMatchModelEnhanced(const void* srcarr, double minScore,
	double greediness, mitk::PointSet::Pointer foundPts,
	double pixelSize_x, double pixelSize_y,
	double steelballSize /*mm*/, CvPoint* resultPoint)
{
	CvMat* Sdx = 0, * Sdy = 0;

	double resultScore = 0;
	double partialSum = 0;
	double sumOfCoords = 0;
	double partialScore;
	const short* _Sdx;
	const short* _Sdy;
	int i, j, m;			// count variables
	double iTx, iTy, iSx, iSy;
	double gradMag;
	int curX, curY;

	double** matGradMag;  //Gradient magnitude matrix

	CvMat srcstub, * src = (CvMat*)srcarr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1 || !modelDefined)
	{
		return 0;
	}

	// source image size
	CvSize Ssize;
	Ssize.width = src->width;
	Ssize.height = src->height;

	CreateDoubleMatrix(matGradMag, Ssize); // create image to save gradient magnitude  values

	Sdx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // X derivatives
	Sdy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // y derivatives

	cvSobel(src, Sdx, 1, 0, 3);  // find X derivatives
	cvSobel(src, Sdy, 0, 1, 3); // find Y derivatives

	// stopping criteria to search for model
	double normMinScore = minScore / noOfCordinates; // precompute minimum score 
	double normGreediness = ((1 - greediness * minScore) / (1 - greediness)) / noOfCordinates; // precompute greedniness 

	for (i = 0; i < Ssize.height; i++)
	{
		_Sdx = (short*)(Sdx->data.ptr + Sdx->step * (i));
		_Sdy = (short*)(Sdy->data.ptr + Sdy->step * (i));

		for (j = 0; j < Ssize.width; j++)
		{
			iSx = _Sdx[j];  // X derivative of Source image
			iSy = _Sdy[j];  // Y derivative of Source image

			gradMag = sqrt((iSx * iSx) + (iSy * iSy)); //Magnitude = Sqrt(dx^2 +dy^2)

			if (gradMag != 0) // hande divide by zero
				matGradMag[i][j] = 1 / gradMag;   // 1/Sqrt(dx^2 +dy^2)
			else
				matGradMag[i][j] = 0;

		}
	}
	for (i = 0; i < Ssize.height; i++)
	{
		for (j = 0; j < Ssize.width; j++)
		{
			partialSum = 0; // initialize partialSum measure
			for (m = 0; m < noOfCordinates; m++)
			{
				// zzhou -----------------------
				// Stop searching if the current Point is near the foundPts
				int foundPtNum = foundPts->GetSize();
				bool isNearFoundPts = false;
				for (int k{0}; k < foundPtNum; k ++)
				{
					double x_tmp = (j + 0.5) * pixelSize_x;
					double y_tmp = (i + 0.5) * pixelSize_y;
					double x_found = foundPts->GetPoint(k)[0];
					double y_found = foundPts->GetPoint(k)[1];

					double distance = sqrt(pow(x_tmp-x_found,2)+ pow(y_tmp - y_found, 2));

					if(distance < 3 * steelballSize)
					{
						isNearFoundPts = true;
						break;
					}
				}

				if (isNearFoundPts == true)
				{
					break;
				}

				// zzhou ------------------------

				curX = i + cordinates[m].x;	// template X coordinate
				curY = j + cordinates[m].y; // template Y coordinate
				iTx = edgeDerivativeX[m];	// template X derivative
				iTy = edgeDerivativeY[m];    // template Y derivative

				if (curX<0 || curY<0 || curX>Ssize.height - 1 || curY>Ssize.width - 1)
					continue;

				_Sdx = (short*)(Sdx->data.ptr + Sdx->step * (curX));
				_Sdy = (short*)(Sdy->data.ptr + Sdy->step * (curX));

				iSx = _Sdx[curY]; // get corresponding  X derivative from source image
				iSy = _Sdy[curY]; // get corresponding  Y derivative from source image

				if ((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
				{
					//partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge magnitude of(Template)* edge magnitude of(Source))
					partialSum = partialSum + ((iSx * iTx) + (iSy * iTy)) * (edgeMagnitude[m] * matGradMag[curX][curY]);

				}

				sumOfCoords = m + 1;
				partialScore = partialSum / sumOfCoords;
				// check termination criteria
				// if partial score score is less than the score than needed to make the required score at that position
				// break searching at that coordinate.
				if (partialScore < (MIN((minScore - 1) + normGreediness * sumOfCoords, normMinScore * sumOfCoords)))
					break;

			}
			if (partialScore > resultScore)
			{
				resultScore = partialScore; //  Match score
				resultPoint->x = i;			// result coordinate X		
				resultPoint->y = j;			// result coordinate Y
			}
		}
	}

	// free used resources and return score
	ReleaseDoubleMatrix(matGradMag, Ssize.height);
	cvReleaseMat(&Sdx);
	cvReleaseMat(&Sdy);

	return resultScore;
}


bool GeoMatch::FindAllGeoMatchModel(const void* srcarr, double minScore, double greediness,
	int matchNum, double pixelSize_x, double pixelSize_y, 
	double steelballSize /*mm*/, mitk::PointSet::Pointer resultPointSet)
{
	for (int i{0}; i < matchNum; i ++)
	{
		CvPoint tmp_result;
		double tmp_score = FindGeoMatchModelEnhanced(srcarr,  minScore,
			greediness, resultPointSet, 
			pixelSize_x, pixelSize_y,
			steelballSize, &tmp_result);

		if(tmp_score > minScore)
		{
			mitk::Point3D newPoint;
			newPoint[0] = tmp_result.y * pixelSize_x;
			newPoint[1] = tmp_result.x * pixelSize_y;
			newPoint[2] = 0;

			resultPointSet->InsertPoint(newPoint);
		}

	}

	int finalFoundNum = resultPointSet->GetSize();
	if (finalFoundNum == matchNum)
	{
		return true;
	}

	return false;
}

// destructor
GeoMatch::~GeoMatch(void)
{
	delete[] cordinates;
	delete[] edgeMagnitude;
	delete[] edgeDerivativeX;
	delete[] edgeDerivativeY;
}

//allocate memory for doubel matrix
void GeoMatch::CreateDoubleMatrix(double**& matrix, CvSize size)
{
	matrix = new double* [size.height];
	for (int iInd = 0; iInd < size.height; iInd++)
		matrix[iInd] = new double[size.width];
}
// release memory
void GeoMatch::ReleaseDoubleMatrix(double**& matrix, int size)
{
	for (int iInd = 0; iInd < size; iInd++)
		delete[] matrix[iInd];
}


// draw contours around result image
void GeoMatch::DrawContours(IplImage* source, CvPoint COG, CvScalar color, int lineWidth)
{
	CvPoint point;
	point.y = COG.x;
	point.x = COG.y;
	for (int i = 0; i < noOfCordinates; i++)
	{
		point.y = cordinates[i].x + COG.x;
		point.x = cordinates[i].y + COG.y;
		cvLine(source, point, point, color, lineWidth);
	}
}

// draw contour at template image
void GeoMatch::DrawContours(IplImage* source, CvScalar color, int lineWidth)
{
	CvPoint point;
	for (int i = 0; i < noOfCordinates; i++)
	{
		point.y = cordinates[i].x + centerOfGravity.x;
		point.x = cordinates[i].y + centerOfGravity.y;
		cvLine(source, point, point, color, lineWidth);
	}
}

