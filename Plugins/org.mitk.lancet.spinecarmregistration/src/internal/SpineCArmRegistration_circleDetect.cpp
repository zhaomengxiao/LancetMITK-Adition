/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryIWorkbenchWindow.h>

// Qmitk
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkShiftScaleImageFilter.h>

#include "SpineCArmRegistration.h"

#include <mitkImage.h>
#include <mitkImageCaster.h>

#include "itkHoughTransform2DCirclesImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkThresholdImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include <list>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>


#include "itkCastImageFilter.h"
#include "itkMath.h"
#include "lancetNCC.h"
#include "mitkImageToOpenCVImageFilter.h"
#include "mitkOpenCVToMitkImageFilter.h"
#include "mitkPointSet.h"



void SpineCArmRegistration::DetectCircles()
{
	using PixelType = double;
	using AccumulatorPixelType = unsigned;
	using RadiusPixelType = double;
	constexpr unsigned int Dimension = 2;
	using ImageType = itk::Image<PixelType, Dimension>;
	ImageType::IndexType localIndex;
	using AccumulatorImageType = itk::Image<AccumulatorPixelType, Dimension>;

	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_circleDetectInput->GetSelectedNode()->GetData());
	
	ImageType::Pointer localImage = ImageType::New();
	mitk::CastToItkImage(inputMitkImage, localImage);

	typedef itk::ShiftScaleImageFilter< ImageType, ImageType > ShiftScaleType0;
	ShiftScaleType0::Pointer shiftScale_mask = ShiftScaleType0::New();
	shiftScale_mask->SetInput(localImage);
	shiftScale_mask->SetShift(0);
	shiftScale_mask->SetScale(1);
	shiftScale_mask->Update();
	

	using HoughTransformFilterType =
		itk::HoughTransform2DCirclesImageFilter<PixelType,
		AccumulatorPixelType,
		RadiusPixelType>;

	HoughTransformFilterType::Pointer houghFilter = HoughTransformFilterType::New();

	houghFilter->SetInput(shiftScale_mask->GetOutput());
	houghFilter->SetNumberOfCircles(m_Controls.lineEdit_circleNum->text().toUInt());
	houghFilter->SetMinimumRadius(m_Controls.lineEdit_RadiusMin->text().toDouble());
	houghFilter->SetMaximumRadius(m_Controls.lineEdit_RadiusMax->text().toDouble());

	houghFilter->SetSweepAngle(m_Controls.lineEdit_SweepAngle->text().toDouble());
	houghFilter->SetSigmaGradient(m_Controls.lineEdit_SigmaGradient->text().toDouble());
	houghFilter->SetVariance(m_Controls.lineEdit_BlurVariance->text().toDouble());
	houghFilter->SetDiscRadiusRatio(m_Controls.lineEdit_RadiusToRemove->text().toDouble());

	houghFilter->Update();

	AccumulatorImageType::Pointer localAccumulator = houghFilter->GetOutput();

	HoughTransformFilterType::CirclesListType circles;
	circles = houghFilter->GetCircles();
	m_Controls.textBrowser->append("Found " + QString::number(circles.size()) + " circles.");


	using OutputPixelType = unsigned char;
	using OutputImageType = itk::Image<OutputPixelType, Dimension>;

	OutputImageType::Pointer localOutputImage = OutputImageType::New();

	OutputImageType::RegionType region;
	region.SetSize(localImage->GetLargestPossibleRegion().GetSize());
	region.SetIndex(localImage->GetLargestPossibleRegion().GetIndex());
	localOutputImage->SetRegions(region);
	localOutputImage->SetOrigin(localImage->GetOrigin());
	localOutputImage->SetSpacing(localImage->GetSpacing());
	localOutputImage->Allocate(true); // initializes buffer to zero

	using CirclesListType = HoughTransformFilterType::CirclesListType;
	CirclesListType::const_iterator itCircles = circles.begin();

	while (itCircles != circles.end())
	{
		std::cout << "Center: ";
		std::cout << (*itCircles)->GetCenterInObjectSpace() << std::endl;
		std::cout << "Radius: " << (*itCircles)->GetRadiusInObjectSpace()[0]
			<< std::endl;


		for (double angle = 0; angle <= itk::Math::twopi;
			angle += itk::Math::pi / 60.0)
		{
			const HoughTransformFilterType::CircleType::PointType centerPoint =
				(*itCircles)->GetCenterInObjectSpace();
			using IndexValueType = ImageType::IndexType::IndexValueType;
			localIndex[0] = itk::Math::Round<IndexValueType>(
				centerPoint[0] +
				(*itCircles)->GetRadiusInObjectSpace()[0] * std::cos(angle));
			localIndex[1] = itk::Math::Round<IndexValueType>(
				centerPoint[1] +
				(*itCircles)->GetRadiusInObjectSpace()[0] * std::sin(angle));
			OutputImageType::RegionType outputRegion =
				localOutputImage->GetLargestPossibleRegion();

			if (outputRegion.IsInside(localIndex))
			{
				localOutputImage->SetPixel(localIndex, 255);
			}
		}
		itCircles++;
	}


	// auto outputMitkImage2 = mitk::Image::New();
	//
	// mitk::CastToMitkImage(shiftScale_mask->GetOutput(), outputMitkImage2);
	//
	// auto tmpNode2 = mitk::DataNode::New();
	// tmpNode2->SetName("Inverted image");
	// tmpNode2->SetData(outputMitkImage2);
	// GetDataStorage()->Add(tmpNode2);


	auto outputMitkImage = mitk::Image::New();

	mitk::CastToMitkImage(localOutputImage, outputMitkImage);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Extracted circles");
	tmpNode->SetData(outputMitkImage);
	GetDataStorage()->Add(tmpNode);

}

void SpineCArmRegistration::GetCannyEdge()
{
	using PixelType = double;
	constexpr unsigned int Dimension = 2;
	using ImageType = itk::Image<PixelType, Dimension>;

	using CannyFilterType =
		itk::CannyEdgeDetectionImageFilter<ImageType, ImageType>;

	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_circleDetectInput->GetSelectedNode()->GetData());

	ImageType::Pointer localImage = ImageType::New();
	mitk::CastToItkImage(inputMitkImage, localImage);

	auto cannyFilter = CannyFilterType::New();
	cannyFilter->SetInput(localImage);

	cannyFilter->SetVariance(m_Controls.lineEdit_cannyVariance->text().toDouble());
	cannyFilter->SetUpperThreshold(m_Controls.lineEdit_cannyUpper->text().toDouble());
	cannyFilter->SetLowerThreshold(m_Controls.lineEdit_cannyLower->text().toDouble());

	cannyFilter->Update();

	auto outputMitkImage = mitk::Image::New();

	mitk::CastToMitkImage(cannyFilter->GetOutput(), outputMitkImage);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Canny Edge");
	tmpNode->SetData(outputMitkImage);
	GetDataStorage()->Add(tmpNode);

}


int SpineCArmRegistration::TestNCC()
{
	GeoMatch GM = GeoMatch();
	double lowThreshold = 10.0;		//deafult value; should be within [0,255]; threshold for gradient collection
	double highThreashold = 100.0;	//deafult value; should be within [0,255]; threshold for gradient collection

	double minScore = 0.65;		//deafult value
	double greediness = 0.7;		//deafult value

	double total_time = 0.0;
	double score = 0.0;
	CvPoint result = CvPoint();

	auto mitkSearchImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_search->GetSelectedNode()->GetData());
	auto mitkTemplateImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_template->GetSelectedNode()->GetData());

	double x_spacing = mitkSearchImage->GetGeometry()->GetSpacing()[0];
	double y_spacing = mitkSearchImage->GetGeometry()->GetSpacing()[1];

	// -------- Convert mitk Images to OpenCV images ------------------
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCv_template = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCv_template->SetInputFromTimeSlice(mitkTemplateImage, 0, 0);
	cv::Mat openCVImage_template = mitkToOpenCv_template->GetOpenCVMat();
	IplImage copy_template = cvIplImage(openCVImage_template);
	IplImage* templateImage = &copy_template;

	if (templateImage == NULL)
	{
		cout << "\nERROR: Could not load Template Image.\n";
		return 0;
	}

	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCv_search = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCv_search->SetInputFromTimeSlice(mitkSearchImage,0,0);
	cv::Mat openCVImage_search = mitkToOpenCv_search->GetOpenCVMat();
	IplImage copy_search = cvIplImage(openCVImage_search);
	IplImage* searchImage = &copy_search;
	// -------------------------------------------------
	
	//---------- Get the max and min pixel value of the search image for rescaling ---------------
	auto inputVtkImage = mitkSearchImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxPixel{ 0.0 };
	double tmpMinPixel{ 10000.0 };

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxPixel)
			{
				tmpMaxPixel = double(*inSI);
			}
			if (double(*inSI) < tmpMinPixel)
			{
				tmpMinPixel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}
	// ---------------------------------------------
	
	if (searchImage == NULL)
	{
		cout << "\nERROR: Could not load Search Image.";
		return 0;
	}


	//------------ Rescale from IPL_DEPTH_16U to IPL_DEPTH_8U (0-255)  -------------------

	CvSize templateSize = cvSize(templateImage->width, templateImage->height);
	IplImage* templateImg_convertScale = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);
	double scale = 255.0 / (tmpMaxPixel - tmpMinPixel);
	double shift = -tmpMinPixel * scale;
	cvConvertScale(templateImage, templateImg_convertScale, scale, shift);

	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* searchImg_convertScale = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);
	cvConvertScale(searchImage, searchImg_convertScale, scale, shift);

	//--------------------------------------------

	//-------------- Convert color image to gray image --------------------

	IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);

	if (templateImage->nChannels == 3)
	{
		cvCvtColor(templateImg_convertScale, grayTemplateImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(templateImg_convertScale, grayTemplateImg);
	}


	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	if (searchImage->nChannels == 3)
	{
		cvCvtColor(searchImg_convertScale, graySearchImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(searchImg_convertScale, graySearchImg);
	}

	//----------------------------------------------------------------------


	//------------ Start algorithm --------------------
	cout << "\n Edge Based Template Matching Program\n";
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: could not create model...";
		return 0;
	}
	
	GM.DrawContours(templateImg_convertScale, cvScalar(255,255,255), 1);
	cout << " Shape model created.." << "with  Low Threshold = " << lowThreshold << " High Threshold = " << highThreashold << endl;
	cout << " Finding Shape Model.." << " Minimum Score = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, &result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;


	if (score > minScore) // if score is at least 0.4
	{
		cout << " Found at [" << result.x << ", " << result.y << "]\n Score = " << score << "\n Searching Time = " << total_time * 1000 << "ms";
		// GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 1);
		GM.DrawContours(searchImg_convertScale, result, cvScalar(255, 255, 255), 1);

		m_Controls.textBrowser->append("Score:" + QString::number(score, 'f', 5));

		// --------- Add mitk::PointSet as result -----------
		auto extracted_Pset = mitk::PointSet::New();
		mitk::Point3D newPoint;
		newPoint[0] = (result.y ) * x_spacing; // xy axes in MITK and openCV are different
		newPoint[1] = (result.x ) * y_spacing; // xy axes in MITK and openCV are different
		newPoint[2] = 0;

		extracted_Pset->InsertPoint(newPoint);

		auto newNode = mitk::DataNode::New();
		newNode->SetData(extracted_Pset);
		newNode->SetName("Detected object");

		GetDataStorage()->Add(newNode);

	}
	else
		cout << " Object Not found";

	cout << "\n ------------------------------------\n\n";

	//------------Display result---------------
	cvNamedWindow("Template", CV_WINDOW_AUTOSIZE);
	cvShowImage("Template", templateImg_convertScale);
	cvNamedWindow("Search Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Search Image", searchImg_convertScale);
	// wait for both windows to be closed before releasing images
	cvWaitKey(0);
	cvDestroyWindow("Search Image");
	cvDestroyWindow("Template");
	// cvReleaseImage(&searchImage);
	// cvReleaseImage(&graySearchImg);
	// cvReleaseImage(&templateImage);
	// cvReleaseImage(&grayTemplateImg);

	return 1;
}


int SpineCArmRegistration::on_pushButton_recursiveSearch_clicked()
{
	GeoMatch GM = GeoMatch();
	double lowThreshold = 10.0;		//deafult value; should be within [0,255]; threshold for gradient collection
	double highThreashold = 100.0;	//deafult value; should be within [0,255]; threshold for gradient collection

	double minScore = 0.4;		//deafult value
	double greediness = 0.7;		//deafult value

	double total_time = 0.0;

	int searchNum = 5; // the number of steelballs

	double steelballSize = 4.0; // the diameter of the steelball

	auto mitkSearchImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_search->GetSelectedNode()->GetData());
	auto mitkTemplateImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_template->GetSelectedNode()->GetData());

	double x_spacing = mitkSearchImage->GetGeometry()->GetSpacing()[0];
	double y_spacing = mitkSearchImage->GetGeometry()->GetSpacing()[1];

	// -------- Convert mitk Images to OpenCV images ------------------
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCv_template = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCv_template->SetInputFromTimeSlice(mitkTemplateImage, 0, 0);
	cv::Mat openCVImage_template = mitkToOpenCv_template->GetOpenCVMat();
	IplImage copy_template = cvIplImage(openCVImage_template);
	IplImage* templateImage = &copy_template;

	if (templateImage == NULL)
	{
		cout << "\nERROR: Could not load Template Image.\n";
		return 0;
	}

	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCv_search = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCv_search->SetInputFromTimeSlice(mitkSearchImage, 0, 0);
	cv::Mat openCVImage_search = mitkToOpenCv_search->GetOpenCVMat();
	IplImage copy_search = cvIplImage(openCVImage_search);
	IplImage* searchImage = &copy_search;
	// -------------------------------------------------

	//---------- Get the max and min pixel value of the search image for rescaling ---------------
	auto inputVtkImage = mitkSearchImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxPixel{ 0.0 };
	double tmpMinPixel{ 10000.0 };

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxPixel)
			{
				tmpMaxPixel = double(*inSI);
			}
			if (double(*inSI) < tmpMinPixel)
			{
				tmpMinPixel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}
	// ---------------------------------------------

	if (searchImage == NULL)
	{
		cout << "\nERROR: Could not load Search Image.";
		return 0;
	}


	//------------ Rescale from IPL_DEPTH_16U to IPL_DEPTH_8U (0-255)  -------------------

	CvSize templateSize = cvSize(templateImage->width, templateImage->height);
	IplImage* templateImg_convertScale = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);
	double scale = 255.0 / (tmpMaxPixel - tmpMinPixel);
	double shift = -tmpMinPixel * scale;
	cvConvertScale(templateImage, templateImg_convertScale, scale, shift);

	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* searchImg_convertScale = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);
	cvConvertScale(searchImage, searchImg_convertScale, scale, shift);

	//--------------------------------------------

	//-------------- Convert color image to gray image --------------------

	IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);

	if (templateImage->nChannels == 3)
	{
		cvCvtColor(templateImg_convertScale, grayTemplateImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(templateImg_convertScale, grayTemplateImg);
	}


	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	if (searchImage->nChannels == 3)
	{
		cvCvtColor(searchImg_convertScale, graySearchImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(searchImg_convertScale, graySearchImg);
	}

	//----------------------------------------------------------------------


	// --------------------- Start algorithm ------------------

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: could not create model...";
		return 0;
	}
	
	cout << " Shape model created.." << "with  Low Threshold = " << lowThreshold << " High Threshold = " << highThreashold << endl;

	cout << " Finding Shape Model.." << " Minimum Score = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();

	auto resultPset = mitk::PointSet::New();
	GM.FindAllGeoMatchModel(graySearchImg, minScore, greediness, 
		searchNum, x_spacing, y_spacing, steelballSize,
		resultPset);

	auto newNode = mitk::DataNode::New();
	newNode->SetData(resultPset);
	newNode->SetName("Detected objects");
	GetDataStorage()->Add(newNode);

	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;
	cout << "Total time: " << total_time << endl;

	m_Controls.textBrowser->append("Found " + QString::number(resultPset->GetSize()) + " balls.");

	return resultPset->GetSize();
}













