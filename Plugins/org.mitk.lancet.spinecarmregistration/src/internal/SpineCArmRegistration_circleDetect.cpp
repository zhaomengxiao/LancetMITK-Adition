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
#include <itkAddImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkInvertIntensityImageFilter.h>
#include <itkLaplacianImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkShiftScaleImageFilter.h>
#include <itkSubtractImageFilter.h>

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
#include "itkTotalVariationDenoisingImageFilter.h"

#include <list>
#include <vtkDataArray.h>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkPointData.h>


#include "itkCastImageFilter.h"
#include "itkMath.h"
#include "lancetNCC.h"
#include "mitkImageStatisticsHolder.h"
#include "mitkImageToOpenCVImageFilter.h"
#include "mitkITKImageImport.h"
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
	constexpr unsigned int Dimension = 3;
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


	//------------ Start algorithm --------------------
	cout << "\n Edge Based Template Matching Program\n";
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: could not create model...";
		return 0;
	}

	GM.DrawContours(templateImg_convertScale, cvScalar(255, 255, 255), 1);
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
		newPoint[0] = (result.y) * x_spacing; // xy axes in MITK and openCV are different
		newPoint[1] = (result.x) * y_spacing; // xy axes in MITK and openCV are different
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
	double greediness = 0.7;	//deafult value

	double total_time = 0.0;

	int searchNum = m_Controls.lineEdit_searchBallNum->text().toInt(); // the number of steelballs

	double steelballSize = m_Controls.lineEdit_searchBallSize->text().toDouble(); // the diameter of the steelball

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


void SpineCArmRegistration::on_pushButton_CLAHE_clicked()
{
	// cv::Mat image = cv::imread("input.jpg", cv::IMREAD_GRAYSCALE);

	auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("DR")->GetData());

	// -------- Convert mitk image to OpenCV image ------------------
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCvFilter = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCvFilter->SetInputFromTimeSlice(inputImage, 0, 0);
	cv::Mat openCVImage = mitkToOpenCvFilter->GetOpenCVMat();

	double clipLimit = m_Controls.lineEdit_clipLimit->text().toDouble();
	int gridWidth = m_Controls.lineEdit_claheWidth->text().toInt();
	int gridHeight = m_Controls.lineEdit_claheHeight->text().toInt();

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	cv::Mat enhancedImage;
	clahe->apply(openCVImage, enhancedImage);


	// Convert OpenCV image to mitk Image
	mitk::OpenCVToMitkImageFilter::Pointer openCVtoMitkFilter = mitk::OpenCVToMitkImageFilter::New();
	openCVtoMitkFilter->SetOpenCVMat(enhancedImage);
	openCVtoMitkFilter->Update();

	auto mitkEnhancedImage = openCVtoMitkFilter->GetOutput();

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(mitkEnhancedImage);
	tmpNode->SetName("CLAHE DR");

	GetDataStorage()->Add(tmpNode);

	// cv::imshow("Enhanced Image", enhancedImage);
	// cv::waitKey(0);
	// cv::imwrite("enhanced.jpg", enhancedImage);
	// cv::destroyAllWindows();
}

void SpineCArmRegistration::on_pushButton_HE_clicked()
{
	// cv::Mat image = cv::imread("input.jpg", cv::IMREAD_GRAYSCALE);

	auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("DR")->GetData());



	// -------- Convert mitk image to OpenCV image ------------------
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCvFilter = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCvFilter->SetInputFromTimeSlice(inputImage, 0, 0);
	cv::Mat openCVImage = mitkToOpenCvFilter->GetOpenCVMat();

	// double clipLimit = m_Controls.lineEdit_clipLimit->text().toDouble();
	// int gridWidth = m_Controls.lineEdit_claheWidth->text().toInt();
	// int gridHeight = m_Controls.lineEdit_claheHeight->text().toInt();
	//
	// cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	// clahe->setClipLimit(clipLimit);
	// clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));
	//
	// cv::Mat enhancedImage;
	// clahe->apply(openCVImage, enhancedImage);

	cv::Mat equalized_image;
	cv::equalizeHist(openCVImage, equalized_image);

	// Convert OpenCV image to mitk Image
	mitk::OpenCVToMitkImageFilter::Pointer openCVtoMitkFilter = mitk::OpenCVToMitkImageFilter::New();
	openCVtoMitkFilter->SetOpenCVMat(equalized_image);
	openCVtoMitkFilter->Update();

	auto mitkEnhancedImage = openCVtoMitkFilter->GetOutput();

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(mitkEnhancedImage);
	tmpNode->SetName("HE DR");

	GetDataStorage()->Add(tmpNode);

	// cv::imshow("Enhanced Image", enhancedImage);
	// cv::waitKey(0);
	// cv::imwrite("enhanced.jpg", enhancedImage);
	// cv::destroyAllWindows();
}

void SpineCArmRegistration::on_pushButton_outlieFilter_clicked()
{
	auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("DR")->GetData());

	//------------white image------------
	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);

	cout << "Printing dim: " << dim[0] << ", " << dim[1] << ", " << dim[2] << endl;

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
	//whiteImage->AllocateScalars(VTK_INT, 1);

	// fill the image with foreground voxels:
	short insideValue = 0;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	//-----------------


	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = inputImage->GetVtkImageData(0, 0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	cout << "Printing dims: " << dims[0] << ", " << dims[1] << ", " << dims[2] << endl;

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToUnsignedShort();

	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 1; y < dims[1] - 1; y++)
		{
			for (int x = 1; x < dims[0] - 1; x++)
			{
				unsigned short* n = static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y, z));

				unsigned short neighbors[8]
				{
					*(static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y - 1, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y + 1, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y - 1, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y + 1, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y - 1, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y, z))),
					* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y + 1, z)))
				};



				unsigned short* m = static_cast<unsigned short*>(whiteImage->GetScalarPointer(x, y, z));

				// *m = *n;

				if (*(static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y, z))) == 0)
				{
					*m = 0;
				}
				else
				{
					*m = ConvertOutlier3X3(*n, neighbors);
				}

			}
		}
	}



	auto newMitkImage = mitk::Image::New();

	newMitkImage->Initialize(whiteImage, 1, -1, dim[2], dim[1]);

	newMitkImage->SetVolume(whiteImage->GetScalarPointer());

	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	newMitkImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

	// newMitkImage->Initialize(castVtkImage, 1, -1, dim[2], dim[1]);
	//
	// newMitkImage->SetVolume(castVtkImage->GetScalarPointer());

	// newMitkImage->SetGeometry(inputImage->GetGeometry());

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("testNode");
	tmpNode->SetData(newMitkImage);
	GetDataStorage()->Add(tmpNode);
	//
	// m_Controls.textBrowser->append("new button connected!");
}

unsigned short SpineCArmRegistration::ConvertOutlier3X3(unsigned short potentialOutlier, unsigned short neighbors[8])
{
	// Sort neighbors in ascending order
	// int size = sizeof(neighbors) / sizeof(neighbors[0]);
	std::sort(neighbors, neighbors + 8);

	// if (potentialOutlier < neighbors[8] && potentialOutlier > neighbors[0])
	// {
	// 	return potentialOutlier;
	// }

	// Calculate average of all neighbors
	unsigned short sum{ 0 };
	unsigned short smallerSum{ 0 };
	unsigned short largerSum{ 0 };

	for (int i{ 0 }; i < 8; i++)
	{
		sum += neighbors[i];
		if (i < 2)
		{
			smallerSum += neighbors[i];
		}

		if (i > 5)
		{
			largerSum += neighbors[i];
		}

	}

	unsigned short allMean = sum / 8;
	unsigned short largerMean = largerSum / 2;
	unsigned short smallerMean = smallerSum / 2;

	// 0902 commented out
	// if(potentialOutlier > neighbors[7])
	// {
	// 	return largerMean;
	// }
	// if(potentialOutlier < neighbors[0])
	// {
	// 	return smallerMean;
	// }
	//
	// return potentialOutlier;

	// 0902 newly added

	int nonZeroCount{ 0 };

	for (int i{ 0 }; i < 8; i++)
	{
		if (neighbors[i] > 0)
		{
			nonZeroCount += 1;
		}

	}


	unsigned short a = neighbors[8 - nonZeroCount - 1 + (nonZeroCount / 2)];
	return a;




}

void SpineCArmRegistration::on_pushButton_teethDetect_clicked()
{
	// Read in a binary image containing the original edges from Canny
	auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("Canny Edges")->GetData());

	auto inputVtkImage = inputImage->GetVtkImageData(0, 0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	cout << "Printing dims: " << dims[0] << ", " << dims[1] << ", " << dims[2] << endl;

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToUnsignedShort();

	caster->Update();

	auto castVtkImage = caster->GetOutput();

	// ------- Create a white image Start -------------
	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);

	cout << "Printing dim: " << dim[0] << ", " << dim[1] << ", " << dim[2] << endl;

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}
	//-----------------Create white image End ---------------


	//-------- Assign values to the white image Start ---------------

	// Do a sum of the neighboring 40x40 area

	int neighborSize{ 20 }; // radius

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				unsigned short tmpSum{ 0 };

				for (int i = 0; i < (2 * neighborSize); i++)
				{
					for (int j = 0; j < (2 * neighborSize); j++)
					{
						if ((x + i - neighborSize) >= 0 && (x + i - neighborSize) < dims[0] && (y + j - neighborSize) >= 0 && (y + j - neighborSize) < dims[1])
						{
							if (*(static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + i - neighborSize, y + j - neighborSize, z))) > 0)
							{
								tmpSum += 1;
							}

						}
					}
				}

				unsigned short* m = static_cast<unsigned short*>(whiteImage->GetScalarPointer(x, y, z));

				*m = tmpSum;

				// unsigned short* n = static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y, z));
				//
				// unsigned short neighbors[8]
				// {
				// 	*(static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y - 1, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x - 1, y + 1, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y - 1, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x, y + 1, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y - 1, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y, z))),
				// 	* (static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + 1, y + 1, z)))
				// };
				//
				//
				//
				// unsigned short* m = static_cast<unsigned short*>(whiteImage->GetScalarPointer(x, y, z));
				//
				// // *m = *n;
				//
				// *m = ConvertOutlier3X3(*n, neighbors);
			}
		}
	}

	//-------- Assign values to the white image End ---------------



	//-------- Save the painted white image ---------------
	auto newMitkImage = mitk::Image::New();

	newMitkImage->Initialize(whiteImage, 1, -1, dim[2], dim[1]);

	newMitkImage->SetVolume(whiteImage->GetScalarPointer());

	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	newMitkImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("testNode");
	tmpNode->SetData(newMitkImage);
	GetDataStorage()->Add(tmpNode);

}

typedef itk::Image<unsigned short, 2> imageType;
//typedef itk::Image<unsigned short, 2> ImageType;

typedef itk::Image<double, 2> doubleImageType;
typedef itk::ImageRegionIterator<doubleImageType> doubleImageIteratorType;
typedef itk::RescaleIntensityImageFilter<doubleImageType, doubleImageType> rescaleFilterType;
typedef itk::ShiftScaleImageFilter<doubleImageType, doubleImageType> shiftScaleFilterType;
typedef itk::FlipImageFilter< doubleImageType > flipImageFilterType;
typedef itk::BinaryThresholdImageFilter< doubleImageType, doubleImageType > thresholdFilterType;
typedef itk::AddImageFilter< doubleImageType, doubleImageType, doubleImageType > addFilterType;
typedef itk::MultiplyImageFilter< doubleImageType, doubleImageType, doubleImageType > multiplyFilterType;
typedef itk::InvertIntensityImageFilter< doubleImageType, doubleImageType > inversionFilterType;
typedef itk::DiscreteGaussianImageFilter< doubleImageType, doubleImageType> gaussianFilterType;
typedef itk::SubtractImageFilter< doubleImageType, doubleImageType, doubleImageType > subtractFilterType;
typedef itk::TotalVariationDenoisingImageFilter<doubleImageType, doubleImageType> totalVariationFilterType;
typedef itk::BinaryBallStructuringElement<doubleImageType::PixelType, 2> ballType;
typedef itk::GrayscaleMorphologicalClosingImageFilter<doubleImageType, doubleImageType, ballType> closingFilterType;
typedef itk::CannyEdgeDetectionImageFilter<doubleImageType, doubleImageType> cannyEdgeFilterType;
typedef itk::ImageRegionIterator<imageType> imageIteratorType;

////////
 // imageType::Pointer ArrayToImage(unsigned short* inputArray, int width, int height)
 // {
	//  auto image = imageType::New();
	//  const imageType::SizeType size = {
	//    { width, height }
	//  }; // Size along {X,Y,Z}
	//  const imageType::IndexType start = {
	//    { 0, 0 }
	//  }; // First index on {X,Y,Z}
 //
	//  imageType::RegionType region;
	//  region.SetSize(size);
	//  region.SetIndex(start);
 //
	//  // Pixel data is allocated
	//  image->SetRegions(region);
	//  image->Allocate(true); // initialize buffer to zero
 //
	//  //ImageType* image = ImageType::New();
	//  //ImageType::SizeType size;
	//  //size[0] = width; // ??
	//  //size[1] = height; // ??
 //
	//  //ImageType::RegionType region;
	//  //
	//  //region.SetSize(size);
	//  ////const ImageType::RegionType& region1 = region;
	//  //image->SetRegions(region);
	//  //image->Allocate();
 //
	//  // ???????????
	//  //ImageIteratorType* it = new ImageIteratorType();
	//  imageIteratorType it(image, image->GetRequestedRegion());
 //
	//  //itk::ImageRegionIterator<ImageType> imageIterator(image, region);
	//  while (!it.IsAtEnd())
	//  {
	// 	 it.Set(*inputArray);
	// 	 ++it;
	// 	 ++inputArray;
	//  }
	//  return image;
 // }

doubleImageType::Pointer ApplyImAdjust(doubleImageType::Pointer inputImage, double low_in, double high_in, double low_out, double high_out, double max, double gamma = 1)
{
	std::cout << "max value:" << max << "\n";
	double low_in_value = max * low_in;
	double high_in_value = max * high_in;
	double low_out_value = max * low_out;
	double high_out_value = max * high_out;
	std::cout << "low_in_value:" << low_in_value << "\n";
	std::cout << "high_in_value:" << high_in_value << "\n";
	std::cout << "low_out_value:" << low_out_value << "\n";
	std::cout << "high_out_value:" << high_out_value << "\n";

	//int count = 0;
	itk::ImageRegionIterator<doubleImageType> imageIterator(inputImage, inputImage->GetRequestedRegion());

	if( max == 0)
	{
		while (!imageIterator.IsAtEnd())
		{
			imageIterator.Set(0);
			++imageIterator;
		}
	}else
	{
		while (!imageIterator.IsAtEnd())
		{
			//count++;
			auto pixel = imageIterator.Get();

			if (pixel < low_in_value)
			{
				imageIterator.Set(low_out_value);
				++imageIterator;
			}
			else if (pixel > high_in_value)
			{
				imageIterator.Set(high_out_value);
				++imageIterator;
			}
			else
			{
				double partial = (pixel - low_in_value) / (high_in_value - low_in_value);
				double gammaPixel = std::pow(partial, gamma) * (high_out_value - low_out_value) + low_out_value;
				//std::cout << "gamma:" << gammaPixel;
				imageIterator.Set(gammaPixel);
				++imageIterator;
			}
			//if (count % 1000 == 0)
			//{
			//	std::cout << "count:" << count << "\n";
			//}	
		}
	}

	
	return inputImage;
}

double WienerFilterImpl(const cv::Mat& src, cv::Mat& dst, double noiseVariance, const cv::Size& block)
{
	assert(("Invalid block dimensions", block.width % 2 == 1 && block.height % 2 == 1 && block.width > 1 && block.height > 1));
	assert(("src and dst must be one channel grayscale images", src.channels() == 1, dst.channels() == 1));

	int h = src.rows;
	int w = src.cols;

	dst = cv::Mat1b(h, w);

	cv::Mat1d means, sqrMeans, variances;
	cv::Mat1d avgVarianceMat;
	try
	{
		boxFilter(src, means, CV_64F, block, cv::Point(-1, -1), true, cv::BORDER_REPLICATE);
	}
	catch (cv::Exception & e)
	{
		std::cerr << "OpenCV Exception: " << e.what() << std::endl;
	}

	sqrBoxFilter(src, sqrMeans, CV_64F, block, cv::Point(-1, -1), true, cv::BORDER_REPLICATE);

	cv::Mat1d means2 = means.mul(means);
	variances = sqrMeans - (means.mul(means));

	if (noiseVariance < 0) {
		// I have to estimate the noiseVariance
		reduce(variances, avgVarianceMat, 1, cv::REDUCE_SUM, -1);
		reduce(avgVarianceMat, avgVarianceMat, 0, cv::REDUCE_SUM, -1);
		noiseVariance = avgVarianceMat(0, 0) / (h * w);
	}

	for (int r = 0; r < h; ++r) {
		// get row pointers
		uchar const* const srcRow = src.ptr<uchar>(r);
		uchar* const dstRow = dst.ptr<uchar>(r);
		double* const varRow = variances.ptr<double>(r);
		double* const meanRow = means.ptr<double>(r);
		for (int c = 0; c < w; ++c) {
			dstRow[c] = cv::saturate_cast<uchar>(
				meanRow[c] + std::max(0., varRow[c] - noiseVariance) / std::max(varRow[c], noiseVariance) * (srcRow[c] - meanRow[c])
				);
		}
	}
	return noiseVariance;
}

double WienerFilter_calculateVariavce(cv::Mat& inputImage_mat, int blockSize[2])
{
	// initialize src
	cv::Mat src = inputImage_mat;

	// initialize dst
	cv::Mat dst;

	// initialize block
	const cv::Size& block = cv::Size(blockSize[0], blockSize[1]);

	// calculate variance
	return WienerFilterImpl(src, dst, -1, block);
}

cv::Mat WienerFilter_startWiener(cv::Mat& inputImage_mat, int blockSize[2])
{
	// initialize src
	/*auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_circleDetectInput_2->GetSelectedNode()->GetData());
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCvFilter = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCvFilter->SetInputFromTimeSlice(inputImage, 0, 0);*/
	cv::Mat src = inputImage_mat;
	src.convertTo(src, CV_64F, 255.0 / 65535, 0);

	// initialize dst
	cv::Mat dst;

	// initialize noiseVariance
	double noiseVariance = WienerFilter_calculateVariavce(inputImage_mat, blockSize);
	//double noiseVariance = 24820;

	// initialize block
	const cv::Size& block = cv::Size(blockSize[0], blockSize[1]);

	// process image
	WienerFilterImpl(src, dst, noiseVariance, block);

	return dst;
}

void ApplyWienerFilter(imageType::Pointer inputImage_itk, int blockSize[2], imageType::Pointer outputImage_itk)
{
	printf("wiener start");
	//convert itk_image to cv_Mat
	//using ReaderType = itk::ImageFileReader< ImageType >;
	//ReaderType::Pointer reader = ReaderType::New();
	cv::Mat inputImage_mat = itk::OpenCVImageBridge::ITKImageToCVMat<imageType>(inputImage_itk);

	//calculate variance
	// double variance = WienerFilter_calculateVariavce(inputImage_mat, blockSize);
	// cout << "Wiener variance" << variance << endl;

	// denoise by wiener
	cv::Mat dst = WienerFilter_startWiener(inputImage_mat, blockSize);

	// convert cv_Mat to itk_image
	dst.convertTo(dst, CV_16U, 257);

	auto processedImage = itk::OpenCVImageBridge::CVMatToITKImage<imageType>(dst);

	// Deep copy
	outputImage_itk->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage_itk->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<imageType>;
	itk::ImageRegionConstIterator<imageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<imageType> copiedIterator(outputImage_itk, outputImage_itk->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}
	printf("wiener end");
}


doubleImageType::Pointer ApplyNegativeLoG(doubleImageType::Pointer inputImage, double variance, int maxsize)
{
	//image = SimpleITK.RescaleIntensity(image, 0, 1);
	typedef itk::DiscreteGaussianImageFilter<doubleImageType, doubleImageType> GaussianFilterType;
	typedef itk::LaplacianImageFilter<doubleImageType, doubleImageType> LaplacianFilterType;
	typedef itk::InvertIntensityImageFilter<doubleImageType, doubleImageType> InvertFilterType;


	GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
	LaplacianFilterType::Pointer laplacianFilter = LaplacianFilterType::New();
	InvertFilterType::Pointer invertFilter = InvertFilterType::New();



	gaussianFilter->SetVariance(variance);
	gaussianFilter->SetMaximumKernelWidth(maxsize);
	gaussianFilter->SetInput(inputImage);
	gaussianFilter->Update();

	laplacianFilter->SetInput(gaussianFilter->GetOutput());
	laplacianFilter->Update();


	typedef itk::MinimumMaximumImageCalculator<doubleImageType> CalculatorType;
	CalculatorType::Pointer calculator = CalculatorType::New();
	calculator->SetImage(laplacianFilter->GetOutput());

	calculator->Compute();

	auto minValue = calculator->GetMinimum();
	auto maxValue = calculator->GetMaximum();

	invertFilter->SetMaximum(minValue + maxValue);
	invertFilter->SetInput(laplacianFilter->GetOutput());

	invertFilter->Update();

	return invertFilter->GetOutput();
}

doubleImageType::Pointer ApplyImageWeightedAdd(doubleImageType::Pointer inputImage1, doubleImageType::Pointer inputImage2, double weight1, double weight2)
{

	//Image castImage = SimpleITK.Cast(image, PixelIDValueEnum.sitkUInt32);
	//Image castEdgeImage = SimpleITK.Cast(edgeImage, PixelIDValueEnum.sitkFloat32);
	typedef itk::MultiplyImageFilter<doubleImageType, doubleImageType, doubleImageType> MultiFilterType;
	MultiFilterType::Pointer multiFilter1 = MultiFilterType::New();
	MultiFilterType::Pointer multiFilter2 = MultiFilterType::New();

	multiFilter1->SetInput1(inputImage1);
	multiFilter1->SetInput2(weight1);
	multiFilter1->Update();

	multiFilter2->SetInput1(inputImage2);
	multiFilter2->SetInput2(weight2);
	multiFilter2->Update();

	typedef itk::AddImageFilter<doubleImageType, doubleImageType, doubleImageType> AddFilterType;
	AddFilterType::Pointer addFilter = AddFilterType::New();
	addFilter->SetInput1(multiFilter1->GetOutput());
	addFilter->SetInput2(multiFilter2->GetOutput());

	try
	{
		addFilter->Update();
	}
	catch (itk::ExceptionObject & e)
	{
		std::cerr << "ITK Exception caught!" << std::endl;
		std::cerr << "Description: " << e.GetDescription() << std::endl;
		std::cerr << "Location: " << e.GetLocation() << std::endl;
	}

	//inputImage2 = SimpleITK.Cast(inputImage2, PixelIDValueEnum.sitkUInt32);
	return addFilter->GetOutput();
	//image = addFilter.Execute(castImage, edgeImage);
}

void SpineCArmRegistration::DrEnhanceType1()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	//
	// if (attemptNode == nullptr )
	// {
	   //  m_Controls.textBrowser->append("There is no raw image!");
	//
	   //  return;
	// }
	//
	// auto inputMitkImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());
	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());


	auto inputImage = doubleImageType::New();

	mitk::CastToItkImage(inputMitkImage, inputImage);


	typedef itk::CastImageFilter< doubleImageType, imageType > D2ICastFilterType;
	printf("step1\n");
	//Flip
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputImage->GetOrigin());
	printf("step2\n");


	// Collect garbage: extra step
	thresholdFilterType::Pointer thFilter_white_garbage = thresholdFilterType::New();
	thFilter_white_garbage->SetInput(flipped_rawImage);
	thFilter_white_garbage->SetLowerThreshold(65530);
	thFilter_white_garbage->SetUpperThreshold(65536);
	thFilter_white_garbage->SetInsideValue(1);
	thFilter_white_garbage->SetOutsideValue(0);
	thFilter_white_garbage->UpdateLargestPossibleRegion();

	auto white_garbage = thFilter_white_garbage->GetOutput();

	thresholdFilterType::Pointer thFilter_black_garbage = thresholdFilterType::New();
	thFilter_black_garbage->SetInput(flipped_rawImage);
	thFilter_black_garbage->SetLowerThreshold(0);
	thFilter_black_garbage->SetUpperThreshold(47000); // requires fine-tuning
	thFilter_black_garbage->SetInsideValue(1);
	thFilter_black_garbage->SetOutsideValue(0);
	thFilter_black_garbage->UpdateLargestPossibleRegion();

	auto black_garbage = thFilter_black_garbage->GetOutput();


	// 0928 Update: black_garbage region should be retained and not be thoroughly excluded 
	auto multiFilter_blackGarbage = multiplyFilterType::New();
	multiFilter_blackGarbage->SetInput1(black_garbage);
	multiFilter_blackGarbage->SetInput2(flipped_rawImage);
	multiFilter_blackGarbage->UpdateLargestPossibleRegion();
	auto garbage_retain = multiFilter_blackGarbage->GetOutput();

	auto garbage_retain_rescaled = ApplyImAdjust(garbage_retain, 0, 1, 0, 0.17, 47000, 1);

	addFilterType::Pointer addFilter_whole_garbage = addFilterType::New();
	addFilter_whole_garbage->SetInput1(black_garbage);
	addFilter_whole_garbage->SetInput2(white_garbage);
	addFilter_whole_garbage->UpdateLargestPossibleRegion();

	auto whole_garbage = addFilter_whole_garbage->GetOutput();



	//turn 65535 into 60000
	doubleImageIteratorType flipped_it(flipped_rawImage, flipped_rawImage->GetRequestedRegion());
	while (!flipped_it.IsAtEnd())
	{
		if ((int)flipped_it.Get() == 65535)
			flipped_it.Set(60000);
		++flipped_it;
	}


	printf("step3\n");
	//gamma = 1.5

	auto gamma_image1 = ApplyImAdjust(flipped_rawImage, 0.6, 0.9999, 0, 1, 65535.0, 1.5);


	printf("step4\n");
	//pixel <= 9000 turn into 0
	doubleImageIteratorType gamma_it1(gamma_image1, gamma_image1->GetRequestedRegion());
	while (!gamma_it1.IsAtEnd())
	{
		if (gamma_it1.Get() <= 9000)
			gamma_it1.Set(0);
		++gamma_it1;
	}


	auto gamma_image2 = ApplyImAdjust(gamma_image1, 0, 1, 0, 1, 65535.0, 1.3);
	printf("step5\n");
	//wiener

	D2ICastFilterType::Pointer castFilter1 = D2ICastFilterType::New();
	castFilter1->SetInput(gamma_image2);
	castFilter1->UpdateLargestPossibleRegion();
	auto cast_gamma_image = castFilter1->GetOutput();


	int block[2]{ 5,5 };
	imageType::Pointer wiener_image = imageType::New();
	ApplyWienerFilter(cast_gamma_image, block, wiener_image);

	typedef itk::CastImageFilter< imageType, doubleImageType > I2DCastFilterType;
	I2DCastFilterType::Pointer castFilter2 = I2DCastFilterType::New();
	castFilter2->SetInput(wiener_image);
	castFilter2->UpdateLargestPossibleRegion();
	printf("step6\n");
	//LOG
	auto LOG_image = ApplyNegativeLoG(castFilter2->GetOutput(), 5, 20);

	printf("step7\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter1 = rescaleFilterType::New();
	rescaleFilter1->SetInput(LOG_image);
	rescaleFilter1->SetOutputMinimum(0);
	rescaleFilter1->SetOutputMaximum(65535);
	rescaleFilter1->UpdateLargestPossibleRegion();


	double low_in = 30000.0 / 65535.0;
	double high_in = 50000.0 / 65535.0;
	auto new_LOG_image = ApplyImAdjust(rescaleFilter1->GetOutput(), low_in, high_in, 0, 1, 65535, 1.2);

	printf("step8\n");
	//image weighted Add
	auto weighted_image = ApplyImageWeightedAdd(castFilter2->GetOutput(), new_LOG_image, 1, 0.2);

	printf("step9\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	rescaleFilter2->SetInput(weighted_image);
	rescaleFilter2->SetOutputMinimum(0);
	rescaleFilter2->SetOutputMaximum(65535);
	rescaleFilter2->UpdateLargestPossibleRegion();
	printf("step10\n");
	//pixel = 7000 turn into 0
	doubleImageIteratorType weighted_it(rescaleFilter2->GetOutput(), rescaleFilter2->GetOutput()->GetRequestedRegion());
	while (!weighted_it.IsAtEnd())
	{
		if (weighted_it.Get() <= 7000)
			weighted_it.Set(0);
		++weighted_it;
	}



	/////////
	// Garbage
	thresholdFilterType::Pointer thresFilter_garbageBackground = thresholdFilterType::New();
	thresFilter_garbageBackground->SetInput(whole_garbage);
	thresFilter_garbageBackground->SetLowerThreshold(-1);
	thresFilter_garbageBackground->SetUpperThreshold(0.5);
	thresFilter_garbageBackground->SetInsideValue(1);
	thresFilter_garbageBackground->SetOutsideValue(0);
	thresFilter_garbageBackground->UpdateLargestPossibleRegion();

	auto garbageBackground = thresFilter_garbageBackground->GetOutput();

	shiftScaleFilterType::Pointer shiftScale_garbage = shiftScaleFilterType::New();
	shiftScale_garbage->SetInput(whole_garbage);
	shiftScale_garbage->SetScale(10000);
	shiftScale_garbage->Update();

	auto scaled_garbage = shiftScale_garbage->GetOutput();

	multiplyFilterType::Pointer mulFilter_1 = multiplyFilterType::New();
	mulFilter_1->SetInput1(garbageBackground);
	mulFilter_1->SetInput2(rescaleFilter2->GetOutput());
	mulFilter_1->UpdateLargestPossibleRegion();

	auto noGarbage = mulFilter_1->GetOutput();


	addFilterType::Pointer addFilter_1 = addFilterType::New();
	addFilter_1->SetInput1(noGarbage);
	addFilter_1->SetInput2(scaled_garbage);
	addFilter_1->Update();

	addFilterType::Pointer addFilter_2 = addFilterType::New();
	addFilter_2->SetInput1(addFilter_1->GetOutput());
	addFilter_2->SetInput2(garbage_retain_rescaled);
	addFilter_2->Update();

	auto result = addFilter_2->GetOutput();
	// auto result = rescaleFilter2->GetOutput();

	/////////


	//clahe
	//cv::Mat mat_image = itk::OpenCVImageBridge::ITKImageToCVMat<doubleImageType>(rescaleFilter2->GetOutput());
	//double clipLimit = 3;
	//int gridWidth = 4;
	//int gridHeight = 4;

	//cv::Mat imageInput;
	//mat_image.convertTo(imageInput, CV_16UC1);

	//cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	//clahe->setClipLimit(clipLimit);
	//clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	//cv::Mat enhancedImage;
	//clahe->apply(imageInput, enhancedImage);

	//cv::Mat enhancedOutput;
	//enhancedImage.convertTo(enhancedOutput, CV_64FC1);

	//auto claheImage = itk::OpenCVImageBridge::CVMatToITKImage<doubleImageType>(enhancedOutput);


	// Final window level/width and gamma modulation Sept. 24
	double low_in_ = m_Controls.lineEdit_lowIn->text().toDouble();
	double low_out_ = m_Controls.lineEdit_lowOut->text().toDouble();
	double high_in_ = m_Controls.lineEdit_highIn->text().toDouble();
	double high_out_ = m_Controls.lineEdit_highOut->text().toDouble();
	double max_ = m_Controls.lineEdit_max->text().toDouble();
	double gamma_ = m_Controls.lineEdit_gamma->text().toDouble();

	auto final = ApplyImAdjust(result, low_in_, high_in_, low_out_, high_out_, max_, gamma_);

	printf("step11\n");

	D2ICastFilterType::Pointer castFilter3 = D2ICastFilterType::New();
	castFilter3->SetInput(final);
	castFilter3->Update();


	// MITK test view----------------------------
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castFilter3->GetOutput())->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(garbage_retain_rescaled)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetName() + "_1");
	b_node->SetData(b);
	b_node->SetName("b");

	GetDataStorage()->Add(a_node);
	GetDataStorage()->Add(b_node);

	// MITK test view----------------------------

	// return castFilter3->GetOutput();
}

void SpineCArmRegistration::DrEnhanceType1_test()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	//
	// if (attemptNode == nullptr )
	// {
	   //  m_Controls.textBrowser->append("There is no raw image!");
	//
	   //  return;
	// }
	//
	// auto inputMitkImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());
	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());


	auto inputImage = doubleImageType::New();

	mitk::CastToItkImage(inputMitkImage, inputImage);


	typedef itk::CastImageFilter< doubleImageType, imageType > D2ICastFilterType;
	printf("step1\n");
	//Flip
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputImage->GetOrigin());
	printf("step2\n");


	// Collect garbage: extra step
	thresholdFilterType::Pointer thFilter_white_garbage = thresholdFilterType::New();
	thFilter_white_garbage->SetInput(flipped_rawImage);
	thFilter_white_garbage->SetLowerThreshold(65530);
	thFilter_white_garbage->SetUpperThreshold(65536);
	thFilter_white_garbage->SetInsideValue(1);
	thFilter_white_garbage->SetOutsideValue(0);
	thFilter_white_garbage->UpdateLargestPossibleRegion();

	auto white_garbage = thFilter_white_garbage->GetOutput();

	thresholdFilterType::Pointer thFilter_black_garbage = thresholdFilterType::New();
	thFilter_black_garbage->SetInput(flipped_rawImage);
	thFilter_black_garbage->SetLowerThreshold(0);
	thFilter_black_garbage->SetUpperThreshold(47000); // requires fine-tuning
	thFilter_black_garbage->SetInsideValue(1);
	thFilter_black_garbage->SetOutsideValue(0);
	thFilter_black_garbage->UpdateLargestPossibleRegion();

	auto black_garbage = thFilter_black_garbage->GetOutput();


	// 0928 Update: black_garbage region should be retained and not be thoroughly excluded 
	// auto multiFilter_blackGarbage = multiplyFilterType::New();
	// multiFilter_blackGarbage->SetInput1(black_garbage);
	// multiFilter_blackGarbage->SetInput2(flipped_rawImage);
	// multiFilter_blackGarbage->UpdateLargestPossibleRegion();
	// auto garbage_retain = multiFilter_blackGarbage->GetOutput();
	//
	// auto garbage_retain_rescaled = ApplyImAdjust(garbage_retain, 0, 1, 0, 0.17, 47000, 1);

	addFilterType::Pointer addFilter_whole_garbage = addFilterType::New();
	addFilter_whole_garbage->SetInput1(black_garbage);
	addFilter_whole_garbage->SetInput2(white_garbage);
	addFilter_whole_garbage->UpdateLargestPossibleRegion();

	auto whole_garbage = addFilter_whole_garbage->GetOutput();



	//turn 65535 into 60000
	doubleImageIteratorType flipped_it(flipped_rawImage, flipped_rawImage->GetRequestedRegion());
	while (!flipped_it.IsAtEnd())
	{
		if (flipped_it.Get() == 65535)
			flipped_it.Set(60000);
		++flipped_it;
	}


	printf("step3\n");
	//gamma = 1.5

	auto gamma_image1 = ApplyImAdjust(flipped_rawImage, 0.6, 0.9999, 0, 1, 65535.0, 1.5);


	printf("step4\n");
	//pixel <= 9000 turn into 0
	doubleImageIteratorType gamma_it1(gamma_image1, gamma_image1->GetRequestedRegion());
	while (!gamma_it1.IsAtEnd())
	{
		// if (gamma_it1.Get() <= 9000)
			// gamma_it1.Set(0);
		++gamma_it1;
	}


	auto gamma_image2 = ApplyImAdjust(gamma_image1, 0, 1, 0, 1, 65535.0, 1.3);
	printf("step5\n");
	//wiener

	D2ICastFilterType::Pointer castFilter1 = D2ICastFilterType::New();
	castFilter1->SetInput(gamma_image2);
	castFilter1->UpdateLargestPossibleRegion();
	auto cast_gamma_image = castFilter1->GetOutput();


	int block[2]{ 5,5 };
	imageType::Pointer wiener_image = imageType::New();
	ApplyWienerFilter(cast_gamma_image, block, wiener_image);

	typedef itk::CastImageFilter< imageType, doubleImageType > I2DCastFilterType;
	I2DCastFilterType::Pointer castFilter2 = I2DCastFilterType::New();
	castFilter2->SetInput(wiener_image);
	castFilter2->UpdateLargestPossibleRegion();
	printf("step6\n");
	//LOG
	auto LOG_image = ApplyNegativeLoG(castFilter2->GetOutput(), 5, 20);

	printf("step7\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter1 = rescaleFilterType::New();
	rescaleFilter1->SetInput(LOG_image);
	rescaleFilter1->SetOutputMinimum(0);
	rescaleFilter1->SetOutputMaximum(65535);
	rescaleFilter1->UpdateLargestPossibleRegion();


	double low_in = 0;//30000.0 / 65535.0;
	double high_in = 1;//50000.0 / 65535.0;
	auto new_LOG_image = ApplyImAdjust(rescaleFilter1->GetOutput(), low_in, high_in, 0, 1, 65535, 1.2);




	printf("step8\n");
	//image weighted Add
	auto weighted_image = ApplyImageWeightedAdd(castFilter2->GetOutput(), new_LOG_image, 1, 0.45);

	printf("step9\n");
	//rescaleIntensity
	// rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	// rescaleFilter2->SetInput(weighted_image);
	// rescaleFilter2->SetOutputMinimum(0);
	// rescaleFilter2->SetOutputMaximum(65535);
	// rescaleFilter2->UpdateLargestPossibleRegion();
	printf("step10\n");
	//pixel = 7000 turn into 0
	// doubleImageIteratorType weighted_it(rescaleFilter2->GetOutput(), rescaleFilter2->GetOutput()->GetRequestedRegion());
	// while (!weighted_it.IsAtEnd())
	// {
	// 	if (weighted_it.Get() <= 7000)
	// 		weighted_it.Set(0);
	// 	++weighted_it;
	// }



	/////////
	// Garbage
	thresholdFilterType::Pointer thresFilter_garbageBackground = thresholdFilterType::New();
	thresFilter_garbageBackground->SetInput(whole_garbage);
	thresFilter_garbageBackground->SetLowerThreshold(-1);
	thresFilter_garbageBackground->SetUpperThreshold(0.5);
	thresFilter_garbageBackground->SetInsideValue(1);
	thresFilter_garbageBackground->SetOutsideValue(0);
	thresFilter_garbageBackground->UpdateLargestPossibleRegion();

	auto garbageBackground = thresFilter_garbageBackground->GetOutput();

	multiplyFilterType::Pointer mulFilter = multiplyFilterType::New();
	mulFilter->SetInput1(black_garbage);
	mulFilter->SetInput2(weighted_image);
	mulFilter->UpdateLargestPossibleRegion();

	typedef itk::MinimumMaximumImageCalculator<doubleImageType> CalculatorType;
	CalculatorType::Pointer calculator = CalculatorType::New();
	calculator->SetImage(mulFilter->GetOutput());

	calculator->Compute();
	auto maxValue = calculator->GetMaximum();


	auto retain_garbage = ApplyImAdjust(mulFilter->GetOutput(), 0, 1, 0, 1, maxValue, 1);
	
	multiplyFilterType::Pointer mulFilter_1 = multiplyFilterType::New();
	mulFilter_1->SetInput1(garbageBackground);
	mulFilter_1->SetInput2(weighted_image);
	mulFilter_1->UpdateLargestPossibleRegion();

	auto noGarbage = mulFilter_1->GetOutput();


	addFilterType::Pointer addFilter_1 = addFilterType::New();
	addFilter_1->SetInput1(noGarbage);
	addFilter_1->SetInput2(retain_garbage);
	addFilter_1->Update();

	// addFilterType::Pointer addFilter_2 = addFilterType::New();
	// addFilter_2->SetInput1(addFilter_1->GetOutput());
	// addFilter_2->SetInput2(garbage_retain_rescaled);
	// addFilter_2->Update();

	auto result = addFilter_1->GetOutput();

	rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	rescaleFilter2->SetInput(result);
	rescaleFilter2->SetOutputMinimum(0);
	rescaleFilter2->SetOutputMaximum(65535);
	rescaleFilter2->UpdateLargestPossibleRegion();

	auto rescaled_result = rescaleFilter2->GetOutput();

	// Final window level/width and gamma modulation Sept. 24
	double low_in_ = m_Controls.lineEdit_lowIn->text().toDouble();
	double low_out_ = m_Controls.lineEdit_lowOut->text().toDouble();
	double high_in_ = m_Controls.lineEdit_highIn->text().toDouble();
	double high_out_ = m_Controls.lineEdit_highOut->text().toDouble();
	double max_ = m_Controls.lineEdit_max->text().toDouble();
	double gamma_ = m_Controls.lineEdit_gamma->text().toDouble();

	auto final = ApplyImAdjust(rescaled_result, low_in_, high_in_, low_out_, high_out_, max_, gamma_);

	printf("step11\n");

	D2ICastFilterType::Pointer castFilter3 = D2ICastFilterType::New();
	castFilter3->SetInput(final);
	castFilter3->Update();


	// MITK test view----------------------------
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castFilter3->GetOutput())->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(black_garbage)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetName() + "_1");
	b_node->SetData(b);
	b_node->SetName("black_garbage");

	GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

	// MITK test view----------------------------

	// return castFilter3->GetOutput();
}

void SpineCArmRegistration::DrEnhanceType1_intermediate()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	//
	// if (attemptNode == nullptr)
	// {
	   //  m_Controls.textBrowser->append("There is no raw image!");
	//
	   //  return;
	// }
	//
	// auto inputMitkImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());

	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());


	auto inputImage = doubleImageType::New();

	mitk::CastToItkImage(inputMitkImage, inputImage);


	typedef itk::CastImageFilter< doubleImageType, imageType > D2ICastFilterType;
	printf("step1\n");
	//Flip
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputImage->GetOrigin());
	printf("step2\n");


	//turn 65535 into 60000
	doubleImageIteratorType flipped_it(flipped_rawImage, flipped_rawImage->GetRequestedRegion());
	while (!flipped_it.IsAtEnd())
	{
		if ((int)flipped_it.Get() == 65535)
			flipped_it.Set(60000);
		++flipped_it;
	}


	printf("step3\n");
	//gamma = 1.5

	auto gamma_image1 = ApplyImAdjust(flipped_rawImage, 0.6, 0.9999, 0, 1, 65535.0, 1.5);


	printf("step4\n");
	//pixel <= 9000 turn into 0
	doubleImageIteratorType gamma_it1(gamma_image1, gamma_image1->GetRequestedRegion());
	while (!gamma_it1.IsAtEnd())
	{
		if (gamma_it1.Get() <= 9000)
			gamma_it1.Set(0);
		++gamma_it1;
	}


	auto gamma_image2 = ApplyImAdjust(gamma_image1, 0, 1, 0, 1, 65535.0, 1.3);
	printf("step5\n");
	//wiener

	D2ICastFilterType::Pointer castFilter1 = D2ICastFilterType::New();
	castFilter1->SetInput(gamma_image2);
	castFilter1->UpdateLargestPossibleRegion();
	auto cast_gamma_image = castFilter1->GetOutput();


	int block[2]{ 5,5 };
	imageType::Pointer wiener_image = imageType::New();
	ApplyWienerFilter(cast_gamma_image, block, wiener_image);

	typedef itk::CastImageFilter< imageType, doubleImageType > I2DCastFilterType;
	I2DCastFilterType::Pointer castFilter2 = I2DCastFilterType::New();
	castFilter2->SetInput(wiener_image);
	castFilter2->UpdateLargestPossibleRegion();
	printf("step6\n");
	//LOG
	auto LOG_image = ApplyNegativeLoG(castFilter2->GetOutput(), 5, 20);

	printf("step7\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter1 = rescaleFilterType::New();
	rescaleFilter1->SetInput(LOG_image);
	rescaleFilter1->SetOutputMinimum(0);
	rescaleFilter1->SetOutputMaximum(65535);
	rescaleFilter1->UpdateLargestPossibleRegion();


	double low_in = 0;//30000.0 / 65535.0;
	double high_in = 1;// 50000.0 / 65535.0;
	auto new_LOG_image = ApplyImAdjust(rescaleFilter1->GetOutput(), low_in, high_in, 0, 1, 65535, 1.2);

	printf("step8\n");
	//image weighted Add
	auto weighted_image = ApplyImageWeightedAdd(castFilter2->GetOutput(), new_LOG_image, 1, 0.45);

	printf("step9\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	rescaleFilter2->SetInput(weighted_image);
	rescaleFilter2->SetOutputMinimum(0);
	rescaleFilter2->SetOutputMaximum(65535);
	rescaleFilter2->UpdateLargestPossibleRegion();
	printf("step10\n");
	//pixel = 7000 turn into 0
	doubleImageIteratorType weighted_it(rescaleFilter2->GetOutput(), rescaleFilter2->GetOutput()->GetRequestedRegion());
	while (!weighted_it.IsAtEnd())
	{
		if (weighted_it.Get() <= 7000)
			weighted_it.Set(0);
		++weighted_it;
	}

	//clahe
	//cv::Mat mat_image = itk::OpenCVImageBridge::ITKImageToCVMat<doubleImageType>(rescaleFilter2->GetOutput());
	//double clipLimit = 3;
	//int gridWidth = 4;
	//int gridHeight = 4;

	//cv::Mat imageInput;
	//mat_image.convertTo(imageInput, CV_16UC1);

	//cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	//clahe->setClipLimit(clipLimit);
	//clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	//cv::Mat enhancedImage;
	//clahe->apply(imageInput, enhancedImage);

	//cv::Mat enhancedOutput;
	//enhancedImage.convertTo(enhancedOutput, CV_64FC1);

	//auto claheImage = itk::OpenCVImageBridge::CVMatToITKImage<doubleImageType>(enhancedOutput);



	printf("step11\n");

	D2ICastFilterType::Pointer castFilter3 = D2ICastFilterType::New();
	castFilter3->SetInput(rescaleFilter2->GetOutput());
	castFilter3->Update();


	// MITK test view----------------------------
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castFilter3->GetOutput())->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(LOG_image)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName("Type_1_intermediate");
	b_node->SetData(b);
	b_node->SetName("b");

	GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

	// MITK test view----------------------------

	// return castFilter3->GetOutput();
}

void SpineCArmRegistration::DrEnhanceType1_intermediate_test()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	//
	// if (attemptNode == nullptr)
	// {
	   //  m_Controls.textBrowser->append("There is no raw image!");
	//
	   //  return;
	// }
	//
	// auto inputMitkImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());

	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());


	auto inputImage = doubleImageType::New();

	mitk::CastToItkImage(inputMitkImage, inputImage);


	typedef itk::CastImageFilter< doubleImageType, imageType > D2ICastFilterType;
	printf("step1\n");
	//Flip
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputImage->GetOrigin());
	printf("step2\n");


	//turn 65535 into 60000
	doubleImageIteratorType flipped_it(flipped_rawImage, flipped_rawImage->GetRequestedRegion());
	while (!flipped_it.IsAtEnd())
	{
		if ((int)flipped_it.Get() == 65535)
			flipped_it.Set(60000);
		++flipped_it;
	}


	printf("step3\n");
	//gamma = 1.5

	auto gamma_image1 = ApplyImAdjust(flipped_rawImage, 0.6, 0.9999, 0, 1, 65535.0, 1.5);


	printf("step4\n");
	//pixel <= 9000 turn into 0
	doubleImageIteratorType gamma_it1(gamma_image1, gamma_image1->GetRequestedRegion());
	while (!gamma_it1.IsAtEnd())
	{
		if (gamma_it1.Get() <= 9000)
			gamma_it1.Set(0);
		++gamma_it1;
	}


	auto gamma_image2 = ApplyImAdjust(gamma_image1, 0, 1, 0, 1, 65535.0, 1.3);
	printf("step5\n");
	//wiener

	D2ICastFilterType::Pointer castFilter1 = D2ICastFilterType::New();
	castFilter1->SetInput(gamma_image2);
	castFilter1->UpdateLargestPossibleRegion();
	auto cast_gamma_image = castFilter1->GetOutput();


	int block[2]{ 5,5 };
	imageType::Pointer wiener_image = imageType::New();
	ApplyWienerFilter(cast_gamma_image, block, wiener_image);

	typedef itk::CastImageFilter< imageType, doubleImageType > I2DCastFilterType;
	I2DCastFilterType::Pointer castFilter2 = I2DCastFilterType::New();
	castFilter2->SetInput(wiener_image);
	castFilter2->UpdateLargestPossibleRegion();
	printf("step6\n");
	//LOG
	auto LOG_image = ApplyNegativeLoG(castFilter2->GetOutput(), 5, 20);

	printf("step7\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter1 = rescaleFilterType::New();
	rescaleFilter1->SetInput(LOG_image);
	rescaleFilter1->SetOutputMinimum(0);
	rescaleFilter1->SetOutputMaximum(65535);
	rescaleFilter1->UpdateLargestPossibleRegion();


	double low_in = 0;// 30000.0 / 65535.0;
	double high_in = 1;// 50000.0 / 65535.0;
	auto new_LOG_image = ApplyImAdjust(rescaleFilter1->GetOutput(), low_in, high_in, 0, 1, 65535, 1.2);

	printf("step8\n");
	//image weighted Add
	auto weighted_image = ApplyImageWeightedAdd(castFilter2->GetOutput(), new_LOG_image, 1, 0.5);

	printf("step9\n");
	//rescaleIntensity
	rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	rescaleFilter2->SetInput(weighted_image);
	rescaleFilter2->SetOutputMinimum(0);
	rescaleFilter2->SetOutputMaximum(65535);
	rescaleFilter2->UpdateLargestPossibleRegion();
	printf("step10\n");
	//pixel = 7000 turn into 0
	doubleImageIteratorType weighted_it(rescaleFilter2->GetOutput(), rescaleFilter2->GetOutput()->GetRequestedRegion());
	while (!weighted_it.IsAtEnd())
	{
		if (weighted_it.Get() <= 7000)
			weighted_it.Set(0);
		++weighted_it;
	}


	printf("step11\n");

	D2ICastFilterType::Pointer castFilter3 = D2ICastFilterType::New();
	castFilter3->SetInput(rescaleFilter2->GetOutput());
	castFilter3->Update();


	// MITK test view----------------------------
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castFilter3->GetOutput())->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(LOG_image)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName("Type_1_intermediate");
	b_node->SetData(b);
	b_node->SetName("b");

	GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

	// MITK test view----------------------------

	// return castFilter3->GetOutput();
}



////////
void SpineCArmRegistration::DrEnhanceType2()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	// auto attemptNode2 = GetDataStorage()->GetNamedNode("Type_1_intermediate"); // 
	// if (attemptNode == nullptr || attemptNode2 == nullptr)
	// {
	// 	m_Controls.textBrowser->append("There is no raw image!");
	// 	m_Controls.textBrowser->append("There is no processed image!");
	// 	return;
	// }

	// auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());
	auto processedImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("Type_1_intermediate")->GetData());

	auto inputItkImage = doubleImageType::New();
	auto processedItkImage = doubleImageType::New(); // output of Type1
	mitk::CastToItkImage(inputImage, inputItkImage);
	mitk::CastToItkImage(processedImage, processedItkImage);

	// Flipping
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();
	flipImageFilterType::Pointer flipper_processed = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputItkImage);
	flipper_processed->SetInput(processedItkImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();
	flipper_processed->SetFlipAxes(flipAxes);
	flipper_processed->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputItkImage->GetOrigin());

	// auto flipped_processedImage = flipper_processed->GetOutput();
	auto flipped_processedImage = processedItkImage;
	flipped_processedImage->SetOrigin(inputItkImage->GetOrigin());

	// Collect garbage
	thresholdFilterType::Pointer thFilter_white_garbage = thresholdFilterType::New();
	thFilter_white_garbage->SetInput(flipped_rawImage);
	thFilter_white_garbage->SetLowerThreshold(65530);
	thFilter_white_garbage->SetUpperThreshold(65536);
	thFilter_white_garbage->SetInsideValue(1);
	thFilter_white_garbage->SetOutsideValue(0);
	thFilter_white_garbage->UpdateLargestPossibleRegion();

	auto white_garbage = thFilter_white_garbage->GetOutput();

	thresholdFilterType::Pointer thFilter_black_garbage = thresholdFilterType::New();
	thFilter_black_garbage->SetInput(flipped_rawImage);
	thFilter_black_garbage->SetLowerThreshold(0);
	thFilter_black_garbage->SetUpperThreshold(47000); // requires fine-tuning
	thFilter_black_garbage->SetInsideValue(1);
	thFilter_black_garbage->SetOutsideValue(0);
	thFilter_black_garbage->UpdateLargestPossibleRegion();

	auto black_garbage = thFilter_black_garbage->GetOutput();

	// 0928 Update: black_garbage region should be retained and not be thoroughly excluded 
	auto multiFilter_blackGarbage = multiplyFilterType::New();
	multiFilter_blackGarbage->SetInput1(black_garbage);
	multiFilter_blackGarbage->SetInput2(flipped_rawImage);
	multiFilter_blackGarbage->UpdateLargestPossibleRegion();
	auto garbage_retain = multiFilter_blackGarbage->GetOutput();

	auto garbage_retain_rescaled = ApplyImAdjust(garbage_retain, 0, 1, 0, 0.17, 47000, 1);



	addFilterType::Pointer addFilter_whole_garbage = addFilterType::New();
	addFilter_whole_garbage->SetInput1(black_garbage);
	addFilter_whole_garbage->SetInput2(white_garbage);
	addFilter_whole_garbage->UpdateLargestPossibleRegion();

	auto whole_garbage = addFilter_whole_garbage->GetOutput();

	// Unsharp mask and rescaling
	gaussianFilterType::Pointer gaussianFilter = gaussianFilterType::New();
	gaussianFilter->SetVariance(50);
	gaussianFilter->SetInput(flipped_rawImage);
	gaussianFilter->UpdateLargestPossibleRegion();

	auto gaussianedImage = gaussianFilter->GetOutput();

	subtractFilterType::Pointer subtractFilter = subtractFilterType::New();
	subtractFilter->SetInput1(flipped_processedImage);
	subtractFilter->SetInput2(gaussianedImage);
	subtractFilter->UpdateLargestPossibleRegion();

	auto subtractedImage = subtractFilter->GetOutput();

	shiftScaleFilterType::Pointer shiftScaleFilter = shiftScaleFilterType::New();
	shiftScaleFilter->SetInput(subtractedImage);
	shiftScaleFilter->SetScale(1.5);
	shiftScaleFilter->UpdateLargestPossibleRegion();

	auto scaled_subtraction = shiftScaleFilter->GetOutput();

	addFilterType::Pointer addFilter_unsharp = addFilterType::New();
	addFilter_unsharp->SetInput1(scaled_subtraction);
	addFilter_unsharp->SetInput2(flipped_rawImage);

	auto unsharped = addFilter_unsharp->GetOutput();

	rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
	rescaleFilter->SetInput(unsharped);
	rescaleFilter->SetOutputMinimum(0);
	rescaleFilter->SetOutputMaximum(65535);
	rescaleFilter->UpdateLargestPossibleRegion();

	auto unsharped_rescaled = rescaleFilter->GetOutput();


	// Extract edges and the non-edge area (background)
	cannyEdgeFilterType::Pointer cannyEdgeFilter = cannyEdgeFilterType::New();
	cannyEdgeFilter->SetInput(flipped_processedImage); //requires fine tuning
	cannyEdgeFilter->SetVariance(30);
	cannyEdgeFilter->SetLowerThreshold(80);  // requires fine tuning
	cannyEdgeFilter->SetUpperThreshold(400); // requires fine tuning

	auto edges = cannyEdgeFilter->GetOutput();

	thresholdFilterType::Pointer thresFilter_background = thresholdFilterType::New();
	thresFilter_background->SetInput(edges);
	thresFilter_background->SetLowerThreshold(-1);
	thresFilter_background->SetUpperThreshold(0.5);
	thresFilter_background->SetInsideValue(1);
	thresFilter_background->SetOutsideValue(0);
	thresFilter_background->UpdateLargestPossibleRegion();

	auto background = thresFilter_background->GetOutput();

	// etching: non-edge area x unsharped_rescaled
	multiplyFilterType::Pointer mulFilter = multiplyFilterType::New();
	mulFilter->SetInput1(background);
	mulFilter->SetInput2(unsharped_rescaled);
	mulFilter->UpdateLargestPossibleRegion();

	auto edged_unsharp = mulFilter->GetOutput();


	// Total variation 
	totalVariationFilterType::Pointer tvFilter = totalVariationFilterType::New();
	tvFilter->SetInput(edged_unsharp);
	tvFilter->SetNumberIterations(40);
	tvFilter->SetLambda(10.0 / 1000);
	tvFilter->UpdateLargestPossibleRegion();

	auto tvResult = tvFilter->GetOutput();


	// Closing
	ballType binaryBall;
	int closingRadius{ 2 };

	binaryBall.SetRadius(closingRadius);
	binaryBall.CreateStructuringElement();

	closingFilterType::Pointer closeFilter = closingFilterType::New();
	closeFilter->SetInput(tvResult);
	closeFilter->SetKernel(binaryBall);
	closeFilter->UpdateLargestPossibleRegion();

	auto closed = closeFilter->GetOutput();


	// closed + 0.5* processedImage 
	shiftScaleFilterType::Pointer shiftScale_preprocess = shiftScaleFilterType::New();
	shiftScale_preprocess->SetInput(flipped_processedImage);
	shiftScale_preprocess->SetScale(0.5);
	shiftScale_preprocess->Update();

	auto flipped_processed_half = shiftScale_preprocess->GetOutput();

	addFilterType::Pointer addFilter_close_processed = addFilterType::New();
	addFilter_close_processed->SetInput1(flipped_processed_half);
	addFilter_close_processed->SetInput2(closed);
	addFilter_close_processed->Update();

	auto closedAndProcessed = addFilter_close_processed->GetOutput();

	rescaleFilterType::Pointer rescaleFilter_1 = rescaleFilterType::New();
	rescaleFilter_1->SetInput(closedAndProcessed);
	rescaleFilter_1->SetOutputMaximum(65535);
	rescaleFilter_1->SetOutputMinimum(0);
	rescaleFilter_1->UpdateLargestPossibleRegion();

	auto rescaled_closedAndProcessed = rescaleFilter_1->GetOutput();


	// CLAHE
	cv::Mat image = itk::OpenCVImageBridge::ITKImageToCVMat<doubleImageType>(rescaled_closedAndProcessed);
	double clipLimit = 1.5;
	int gridWidth = 4;
	int gridHeight = 4;

	cv::Mat imageInput;

	image.convertTo(imageInput, CV_16UC1);

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	cv::Mat enhancedImage;
	clahe->apply(imageInput, enhancedImage);

	cv::Mat enhancedOutput;
	enhancedImage.convertTo(enhancedOutput, CV_64FC1);

	auto claheImage = itk::OpenCVImageBridge::CVMatToITKImage<doubleImageType>(enhancedOutput);


	// Garbage
	thresholdFilterType::Pointer thresFilter_garbageBackground = thresholdFilterType::New();
	thresFilter_garbageBackground->SetInput(whole_garbage);
	thresFilter_garbageBackground->SetLowerThreshold(-1);
	thresFilter_garbageBackground->SetUpperThreshold(0.5);
	thresFilter_garbageBackground->SetInsideValue(1);
	thresFilter_garbageBackground->SetOutsideValue(0);
	thresFilter_garbageBackground->UpdateLargestPossibleRegion();

	auto garbageBackground = thresFilter_garbageBackground->GetOutput();

	shiftScaleFilterType::Pointer shiftScale_garbage = shiftScaleFilterType::New();
	shiftScale_garbage->SetInput(whole_garbage);
	shiftScale_garbage->SetScale(10000);
	shiftScale_garbage->Update();

	auto scaled_garbage = shiftScale_garbage->GetOutput();

	multiplyFilterType::Pointer mulFilter_1 = multiplyFilterType::New();
	mulFilter_1->SetInput1(garbageBackground);
	mulFilter_1->SetInput2(claheImage);
	mulFilter_1->UpdateLargestPossibleRegion();

	auto clahe_noGarbage = mulFilter_1->GetOutput();


	addFilterType::Pointer addFilter_1 = addFilterType::New();
	addFilter_1->SetInput1(clahe_noGarbage);
	addFilter_1->SetInput2(scaled_garbage);
	addFilter_1->Update();

	addFilterType::Pointer addFilter_2 = addFilterType::New();
	addFilter_2->SetInput1(addFilter_1->GetOutput());
	addFilter_2->SetInput2(garbage_retain_rescaled);
	addFilter_2->Update();

	auto result = addFilter_2->GetOutput();


	// Final window level/width and gamma modulation Sept. 24
	double low_in = m_Controls.lineEdit_lowIn->text().toDouble();
	double low_out = m_Controls.lineEdit_lowOut->text().toDouble();
	double high_in = m_Controls.lineEdit_highIn->text().toDouble();
	double high_out = m_Controls.lineEdit_highOut->text().toDouble();
	double max = m_Controls.lineEdit_max->text().toDouble();
	double gamma = m_Controls.lineEdit_gamma->text().toDouble();

	auto final = ApplyImAdjust(result, low_in, high_in, low_out, high_out, max, gamma);



	typedef itk::CastImageFilter< doubleImageType, imageType > CastFilterType;
	CastFilterType::Pointer castFilter = CastFilterType::New();
	castFilter->SetInput(final);

	auto castResult = castFilter->GetOutput();

	// MITK Test View
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castResult)->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(edged_unsharp)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetName() + "_2");
	b_node->SetData(b);
	b_node->SetName("b");

	GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

}

void SpineCArmRegistration::DrEnhanceType2_test()
{
	// MITK input image
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	// auto attemptNode2 = GetDataStorage()->GetNamedNode("Type_1_intermediate"); // 
	// if (attemptNode == nullptr || attemptNode2 == nullptr)
	// {
	// 	m_Controls.textBrowser->append("There is no raw image!");
	// 	m_Controls.textBrowser->append("There is no processed image!");
	// 	return;
	// }

	// auto inputImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw")->GetData());
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());
	auto processedImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("Type_1_intermediate")->GetData());

	auto inputItkImage = doubleImageType::New();
	auto processedItkImage = doubleImageType::New(); // output of Type1
	mitk::CastToItkImage(inputImage, inputItkImage);
	mitk::CastToItkImage(processedImage, processedItkImage);

	// Flipping
	int param1{ 0 }; // Axis
	flipImageFilterType::Pointer flipper_raw_input = flipImageFilterType::New();
	flipImageFilterType::Pointer flipper_processed = flipImageFilterType::New();

	flipper_raw_input->SetInput(inputItkImage);
	flipper_processed->SetInput(processedItkImage);
	itk::FixedArray<bool, 2> flipAxes;
	for (int i = 0; i < 2; ++i)
	{
		if (i == param1)
		{
			// flipAxes[i] = true;
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper_raw_input->SetFlipAxes(flipAxes);
	flipper_raw_input->UpdateLargestPossibleRegion();
	flipper_processed->SetFlipAxes(flipAxes);
	flipper_processed->UpdateLargestPossibleRegion();

	auto flipped_rawImage = flipper_raw_input->GetOutput();
	flipped_rawImage->SetOrigin(inputItkImage->GetOrigin());

	// auto flipped_processedImage = flipper_processed->GetOutput();
	auto flipped_processedImage = processedItkImage;
	flipped_processedImage->SetOrigin(inputItkImage->GetOrigin());

	// Collect garbage
	thresholdFilterType::Pointer thFilter_white_garbage = thresholdFilterType::New();
	thFilter_white_garbage->SetInput(flipped_rawImage);
	thFilter_white_garbage->SetLowerThreshold(65530);
	thFilter_white_garbage->SetUpperThreshold(65536);
	thFilter_white_garbage->SetInsideValue(1);
	thFilter_white_garbage->SetOutsideValue(0);
	thFilter_white_garbage->UpdateLargestPossibleRegion();

	auto white_garbage = thFilter_white_garbage->GetOutput();

	thresholdFilterType::Pointer thFilter_black_garbage = thresholdFilterType::New();
	thFilter_black_garbage->SetInput(flipped_rawImage);
	thFilter_black_garbage->SetLowerThreshold(0);
	thFilter_black_garbage->SetUpperThreshold(47000); // requires fine-tuning
	thFilter_black_garbage->SetInsideValue(1);
	thFilter_black_garbage->SetOutsideValue(0);
	thFilter_black_garbage->UpdateLargestPossibleRegion();

	auto black_garbage = thFilter_black_garbage->GetOutput();

	// 0928 Update: black_garbage region should be retained and not be thoroughly excluded 
	// auto multiFilter_blackGarbage = multiplyFilterType::New();
	// multiFilter_blackGarbage->SetInput1(black_garbage);
	// multiFilter_blackGarbage->SetInput2(flipped_rawImage);
	// multiFilter_blackGarbage->UpdateLargestPossibleRegion();
	// auto garbage_retain = multiFilter_blackGarbage->GetOutput();
	//
	// auto garbage_retain_rescaled = ApplyImAdjust(garbage_retain, 0, 1, 0, 0.17, 47000, 1);



	addFilterType::Pointer addFilter_whole_garbage = addFilterType::New();
	addFilter_whole_garbage->SetInput1(black_garbage);
	addFilter_whole_garbage->SetInput2(white_garbage);
	addFilter_whole_garbage->UpdateLargestPossibleRegion();

	auto whole_garbage = addFilter_whole_garbage->GetOutput();

	// Unsharp mask and rescaling
	gaussianFilterType::Pointer gaussianFilter = gaussianFilterType::New();
	gaussianFilter->SetVariance(50);
	gaussianFilter->SetInput(flipped_rawImage);
	gaussianFilter->UpdateLargestPossibleRegion();

	auto gaussianedImage = gaussianFilter->GetOutput();

	subtractFilterType::Pointer subtractFilter = subtractFilterType::New();
	subtractFilter->SetInput1(flipped_processedImage);
	subtractFilter->SetInput2(gaussianedImage);
	subtractFilter->UpdateLargestPossibleRegion();

	auto subtractedImage = subtractFilter->GetOutput();

	shiftScaleFilterType::Pointer shiftScaleFilter = shiftScaleFilterType::New();
	shiftScaleFilter->SetInput(subtractedImage);
	shiftScaleFilter->SetScale(1.5);
	shiftScaleFilter->UpdateLargestPossibleRegion();

	auto scaled_subtraction = shiftScaleFilter->GetOutput();

	addFilterType::Pointer addFilter_unsharp = addFilterType::New();
	addFilter_unsharp->SetInput1(scaled_subtraction);
	addFilter_unsharp->SetInput2(flipped_rawImage);

	auto unsharped = addFilter_unsharp->GetOutput();

	rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
	rescaleFilter->SetInput(unsharped);
	rescaleFilter->SetOutputMinimum(0);
	rescaleFilter->SetOutputMaximum(65535);
	rescaleFilter->UpdateLargestPossibleRegion();

	auto unsharped_rescaled = rescaleFilter->GetOutput();


	// Extract edges and the non-edge area (background)
	cannyEdgeFilterType::Pointer cannyEdgeFilter = cannyEdgeFilterType::New();
	cannyEdgeFilter->SetInput(flipped_processedImage); //requires fine tuning
	cannyEdgeFilter->SetVariance(30);
	cannyEdgeFilter->SetLowerThreshold(5);  // requires fine tuning: 80
	cannyEdgeFilter->SetUpperThreshold(10); // requires fine tuning: 400
	cannyEdgeFilter->Update();

	auto edges = cannyEdgeFilter->GetOutput();

	// Todo: use edge density information to decide the enhancement values
	doubleImageIteratorType edges_it(edges, edges->GetRequestedRegion());
	// edges_it.GoToBegin();
	// int edge_count{ 0 };
	// while (!edges_it.IsAtEnd())
	// {
	// 	if (edges_it.Get() > 0)
	// 	{
	// 		edge_count += 1;
	// 	}
	// 		
	// 	++edges_it;
	// }
	// cout << "edge pix num: " << edge_count << endl;

	thresholdFilterType::Pointer thresFilter_background = thresholdFilterType::New();
	thresFilter_background->SetInput(edges);
	thresFilter_background->SetLowerThreshold(-1);
	thresFilter_background->SetUpperThreshold(0.5);
	thresFilter_background->SetInsideValue(1);
	thresFilter_background->SetOutsideValue(0);
	thresFilter_background->UpdateLargestPossibleRegion();

	auto background = thresFilter_background->GetOutput();

	// etching: non-edge area x unsharped_rescaled
	multiplyFilterType::Pointer mulFilter = multiplyFilterType::New();
	mulFilter->SetInput1(background);
	mulFilter->SetInput2(unsharped_rescaled);
	mulFilter->UpdateLargestPossibleRegion();

	auto edged_unsharp = mulFilter->GetOutput();


	// Total variation 
	totalVariationFilterType::Pointer tvFilter = totalVariationFilterType::New();
	tvFilter->SetInput(edged_unsharp);
	tvFilter->SetNumberIterations(40);
	tvFilter->SetLambda(10.0 / 1000);
	tvFilter->UpdateLargestPossibleRegion();

	auto tvResult = tvFilter->GetOutput();


	// Closing
	ballType binaryBall;
	int closingRadius{ 2 };

	binaryBall.SetRadius(closingRadius);
	binaryBall.CreateStructuringElement();

	closingFilterType::Pointer closeFilter = closingFilterType::New();
	closeFilter->SetInput(tvResult);
	closeFilter->SetKernel(binaryBall);
	closeFilter->UpdateLargestPossibleRegion();

	auto closed = closeFilter->GetOutput();


	// closed + 0.5* processedImage 
	shiftScaleFilterType::Pointer shiftScale_preprocess = shiftScaleFilterType::New();
	shiftScale_preprocess->SetInput(flipped_processedImage);
	shiftScale_preprocess->SetScale(0.35);
	shiftScale_preprocess->Update();

	auto flipped_processed_half = shiftScale_preprocess->GetOutput();

	addFilterType::Pointer addFilter_close_processed = addFilterType::New();
	addFilter_close_processed->SetInput1(flipped_processed_half);
	addFilter_close_processed->SetInput2(closed);
	addFilter_close_processed->Update();

	auto closedAndProcessed = addFilter_close_processed->GetOutput();

	rescaleFilterType::Pointer rescaleFilter_1 = rescaleFilterType::New();
	rescaleFilter_1->SetInput(closedAndProcessed);
	rescaleFilter_1->SetOutputMaximum(65535);
	rescaleFilter_1->SetOutputMinimum(0);
	rescaleFilter_1->UpdateLargestPossibleRegion();

	auto rescaled_closedAndProcessed = rescaleFilter_1->GetOutput();


	// CLAHE
	cv::Mat image = itk::OpenCVImageBridge::ITKImageToCVMat<doubleImageType>(rescaled_closedAndProcessed);
	double clipLimit = 1.3;
	int gridWidth = 4;
	int gridHeight = 4;

	cv::Mat imageInput;

	image.convertTo(imageInput, CV_16UC1);

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	cv::Mat enhancedImage;
	clahe->apply(imageInput, enhancedImage);

	cv::Mat enhancedOutput;
	enhancedImage.convertTo(enhancedOutput, CV_64FC1);

	auto claheImage = itk::OpenCVImageBridge::CVMatToITKImage<doubleImageType>(enhancedOutput);


	// Garbage
	thresholdFilterType::Pointer thresFilter_garbageBackground = thresholdFilterType::New();
	thresFilter_garbageBackground->SetInput(whole_garbage);
	thresFilter_garbageBackground->SetLowerThreshold(-1);
	thresFilter_garbageBackground->SetUpperThreshold(0.5);
	thresFilter_garbageBackground->SetInsideValue(1);
	thresFilter_garbageBackground->SetOutsideValue(0);
	thresFilter_garbageBackground->UpdateLargestPossibleRegion();

	auto garbageBackground = thresFilter_garbageBackground->GetOutput();



	// shiftScaleFilterType::Pointer shiftScale_garbage = shiftScaleFilterType::New();
	// shiftScale_garbage->SetInput(whole_garbage);
	// shiftScale_garbage->SetScale(10000);
	// shiftScale_garbage->Update();

	// auto scaled_garbage = shiftScale_garbage->GetOutput();

	multiplyFilterType::Pointer mulFilter_retain = multiplyFilterType::New();
	mulFilter_retain->SetInput1(black_garbage);
	mulFilter_retain->SetInput2(claheImage);
	mulFilter_retain->UpdateLargestPossibleRegion();

	typedef itk::MinimumMaximumImageCalculator<doubleImageType> CalculatorType;
	CalculatorType::Pointer calculator = CalculatorType::New();
	calculator->SetImage(mulFilter_retain->GetOutput());

	calculator->Compute();
	auto maxValue = calculator->GetMaximum();


	auto retain_garbage = ApplyImAdjust(mulFilter_retain->GetOutput(), 0, 1, 0, 1, maxValue, 1.4);


	multiplyFilterType::Pointer mulFilter_1 = multiplyFilterType::New();
	mulFilter_1->SetInput1(garbageBackground);
	mulFilter_1->SetInput2(claheImage);
	mulFilter_1->UpdateLargestPossibleRegion();

	auto clahe_noGarbage = mulFilter_1->GetOutput();


	addFilterType::Pointer addFilter_1 = addFilterType::New();
	addFilter_1->SetInput1(clahe_noGarbage);
	addFilter_1->SetInput2(retain_garbage);
	addFilter_1->Update();

	// addFilterType::Pointer addFilter_2 = addFilterType::New();
	// addFilter_2->SetInput1(addFilter_1->GetOutput());
	// addFilter_2->SetInput2(garbage_retain_rescaled);
	// addFilter_2->Update();

	auto result = addFilter_1->GetOutput();


	rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
	rescaleFilter2->SetInput(result);
	rescaleFilter2->SetOutputMinimum(0);
	rescaleFilter2->SetOutputMaximum(65535);
	rescaleFilter2->UpdateLargestPossibleRegion();

	auto rescaled_result = rescaleFilter2->GetOutput();

	// Final window level/width and gamma modulation Sept. 24
	double low_in = m_Controls.lineEdit_lowIn->text().toDouble();
	double low_out = m_Controls.lineEdit_lowOut->text().toDouble();
	double high_in = m_Controls.lineEdit_highIn->text().toDouble();
	double high_out = m_Controls.lineEdit_highOut->text().toDouble();
	double max = m_Controls.lineEdit_max->text().toDouble();
	double gamma = m_Controls.lineEdit_gamma->text().toDouble();

	auto final = ApplyImAdjust(rescaled_result, low_in, high_in, low_out, high_out, max, gamma);



	typedef itk::CastImageFilter< doubleImageType, imageType > CastFilterType;
	CastFilterType::Pointer castFilter = CastFilterType::New();
	castFilter->SetInput(final);

	auto castResult = castFilter->GetOutput();

	// MITK Test View
	auto a = mitk::Image::New();
	a = mitk::ImportItkImage(castResult)->Clone();
	auto b = mitk::Image::New();
	b = mitk::ImportItkImage(edges)->Clone();

	auto a_node = mitk::DataNode::New();
	auto b_node = mitk::DataNode::New();

	a_node->SetData(a);
	a_node->SetName(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetName() + "_2");
	b_node->SetData(b);
	b_node->SetName("edges");

	GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

}

typedef itk::Image<short, 3> ImageType;
typedef itk::Image<double, 3> DoubleImageType;
typedef itk::FlipImageFilter< ImageType > FlipImageFilterType;
typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > ThresholdFilterType;
typedef itk::AddImageFilter< ImageType, ImageType, ImageType > AddFilterType;
typedef itk::MultiplyImageFilter< ImageType, ImageType, ImageType > MultiplyFilterType;
typedef itk::InvertIntensityImageFilter< ImageType, ImageType > InversionFilterType;
typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType> GaussianFilterType;
typedef itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractFilterType;
typedef itk::TotalVariationDenoisingImageFilter<DoubleImageType, DoubleImageType> TotalVariationFilterType;
typedef itk::BinaryBallStructuringElement<ImageType::PixelType, 3> BallType;
typedef itk::GrayscaleMorphologicalClosingImageFilter<ImageType, ImageType, BallType> ClosingFilterType;



void SpineCArmRegistration::on_pushButton_step_1_3_clicked()
{
	DrEnhanceType1_intermediate();
	DrEnhanceType2();
	DrEnhanceType1();

	//-------Start Step 2: convert the 2D raw image into 3D-------------
	// auto attemptNode = GetDataStorage()->GetNamedNode("raw");
	// if(attemptNode == nullptr)
	// {
	// 	m_Controls.textBrowser->append("There is no raw image!");
	// 	return;
	// }


	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());

	// first we create a blank image
	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);

	cout << "Printing dim: " << dim[0] << ", " << dim[1] << ", " << dim[2] << endl;

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	// fill the blank image with "0" voxels:
	unsigned short insideValue{ 0 };
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	// copy the pixel values of the "raw" to the "blank" image   
	auto inputVtkImage = inputImage->GetVtkImageData(0, 0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	cout << "Printing dims: " << dims[0] << ", " << dims[1] << ", " << dims[2] << endl;

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToUnsignedShort();
	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));

				int* m = static_cast<int*>(whiteImage->GetScalarPointer(x, y, z));

				m[0] = n[0];
			}
		}
	}

	// convert the result into mitk Image
	auto newMitkImage = mitk::Image::New();

	newMitkImage->Initialize(whiteImage, 1, -1, dim[2], dim[1]);

	newMitkImage->SetVolume(whiteImage->GetScalarPointer());

	//--------- End Step 2: convert the 2D raw image into 3D-------------


	//--------- Start Step 3: rescale to 0~3000 and do the right-left flipping -------------

	// rescaling

	DoubleImageType::Pointer floatImage = DoubleImageType::New();
	mitk::CastToItkImage(newMitkImage, floatImage);
	itk::RescaleIntensityImageFilter<DoubleImageType, DoubleImageType>::Pointer filter = itk::RescaleIntensityImageFilter<DoubleImageType, DoubleImageType>::New();
	filter->SetInput(0, floatImage);
	filter->SetOutputMinimum(0);
	filter->SetOutputMaximum(3000);
	filter->Update();
	floatImage = filter->GetOutput();

	newMitkImage = mitk::Image::New();
	newMitkImage->InitializeByItk(floatImage.GetPointer());
	newMitkImage->SetVolume(floatImage->GetBufferPointer());
	cout << "Rescaling successful." << std::endl;

	// flipping
	int param1{ 0 }; // Axis
	FlipImageFilterType::Pointer flipper = FlipImageFilterType::New();

	auto itkImage = ImageType::New();
	mitk::CastToItkImage(newMitkImage, itkImage);

	flipper->SetInput(itkImage);
	itk::FixedArray<bool, 3> flipAxes;
	for (int i = 0; i < 3; ++i)
	{
		if (i == param1)
		{
			flipAxes[i] = true;
		}
		else
		{
			flipAxes[i] = false;
		}
	}
	flipper->SetFlipAxes(flipAxes);
	flipper->UpdateLargestPossibleRegion();

	newMitkImage = mitk::ImportItkImage(flipper->GetOutput())->Clone();
	std::cout << "Image flipping successful." << std::endl;

	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	newMitkImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

	//--------- End Step 3: rescale to 0~3000 and do the right-left flipping -------------

	// save the flipping result
	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("raw_flipped");
	tmpNode->SetData(newMitkImage);
	GetDataStorage()->Add(tmpNode);



}

void SpineCArmRegistration::on_pushButton_step_4_5_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("raw_flipped");
	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("Please take the previous step first!");
		return;
	}

	auto inputImage = dynamic_cast<mitk::Image*>(attemptNode->GetData());

	// apply thresholds
	auto cornerImage = mitk::Image::New();
	auto cornerImage_inverted = mitk::Image::New();
	auto betweenTeethImage = mitk::Image::New();

	auto itkImage = ImageType::New();

	mitk::CastToItkImage(inputImage, itkImage);

	int param1{ 2999 }; int param2{ 3000 };
	ThresholdFilterType::Pointer thFilter = ThresholdFilterType::New();
	thFilter->SetLowerThreshold(param1 < param2 ? param1 : param2);
	thFilter->SetUpperThreshold(param2 > param1 ? param2 : param1);
	thFilter->SetInsideValue(1);
	thFilter->SetOutsideValue(0);
	thFilter->SetInput(itkImage);
	thFilter->UpdateLargestPossibleRegion();

	cornerImage = mitk::ImportItkImage(thFilter->GetOutput())->Clone();

	thFilter->SetInsideValue(0);
	thFilter->SetOutsideValue(1);
	thFilter->UpdateLargestPossibleRegion();
	cornerImage_inverted = mitk::ImportItkImage(thFilter->GetOutput())->Clone();

	int param3{ 0 }; int param4{ 1300 };
	thFilter->SetLowerThreshold(param3 < param4 ? param3 : param4);
	thFilter->SetUpperThreshold(param4 > param3 ? param4 : param3);
	thFilter->SetInsideValue(1);
	thFilter->SetOutsideValue(0);
	thFilter->UpdateLargestPossibleRegion();
	betweenTeethImage = mitk::ImportItkImage(thFilter->GetOutput())->Clone();

	std::cout << "Thresholding successful." << std::endl;

	auto cornerNode = mitk::DataNode::New();
	cornerNode->SetData(cornerImage);

	auto cornerInversionNode = mitk::DataNode::New();
	cornerInversionNode->SetData(cornerImage_inverted);

	auto betweenTeethNode = mitk::DataNode::New();
	betweenTeethNode->SetData(betweenTeethImage);

	cornerNode->SetName("corner");
	betweenTeethNode->SetName("betweenTeeth");
	cornerInversionNode->SetName("corner_Inverted");

	GetDataStorage()->Add(cornerNode);
	GetDataStorage()->Add(cornerInversionNode);
	GetDataStorage()->Add(betweenTeethNode);

	// assemble "corner" and "betweenTeeth" to get "garbage" area
	AddFilterType::Pointer addFilter = AddFilterType::New();
	auto itkImage1 = ImageType::New();
	auto itkImage2 = ImageType::New();
	auto garbageImage = mitk::Image::New();

	mitk::CastToItkImage(betweenTeethImage, itkImage1);
	mitk::CastToItkImage(cornerImage, itkImage2);
	addFilter->SetInput1(itkImage1);
	addFilter->SetInput2(itkImage2);
	addFilter->UpdateLargestPossibleRegion();
	garbageImage = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	auto garbageNode = mitk::DataNode::New();
	garbageNode->SetName("garbage");
	garbageNode->SetData(garbageImage);
	GetDataStorage()->Add(garbageNode);


	// rescale corner by 2100
	auto cornerImage_scaled = mitk::Image::New();

	DoubleImageType::Pointer floatImage_corner = DoubleImageType::New();
	mitk::CastToItkImage(cornerImage, floatImage_corner);

	itk::ShiftScaleImageFilter<DoubleImageType, DoubleImageType>::Pointer filter = itk::ShiftScaleImageFilter<DoubleImageType, DoubleImageType>::New();
	filter->SetInput(0, floatImage_corner);
	filter->SetScale(2100);

	filter->Update();
	floatImage_corner = filter->GetOutput();

	cornerImage_scaled = mitk::Image::New();
	cornerImage_scaled->InitializeByItk(floatImage_corner.GetPointer());
	cornerImage_scaled->SetVolume(floatImage_corner->GetBufferPointer());
	std::cout << "Corner Rescaling successful." << std::endl;

	auto cornerNode_scaled = mitk::DataNode::New();
	cornerNode_scaled->SetData(cornerImage_scaled);
	cornerNode_scaled->SetName("corner_scaled");
	GetDataStorage()->Add(cornerNode_scaled, cornerNode);

	// scale betweenTeeth by 1100
	DoubleImageType::Pointer floatImage_betweenTeeth = DoubleImageType::New();
	mitk::CastToItkImage(betweenTeethImage, floatImage_betweenTeeth);

	filter->SetInput(0, floatImage_betweenTeeth);
	filter->SetScale(1100);

	filter->Update();
	floatImage_betweenTeeth = filter->GetOutput();

	auto betweenTeeth_scaled = mitk::Image::New();
	betweenTeeth_scaled->InitializeByItk(floatImage_betweenTeeth.GetPointer());
	betweenTeeth_scaled->SetVolume(floatImage_betweenTeeth->GetBufferPointer());
	std::cout << "betweenTeeth Rescaling successful." << std::endl;

	auto betweenTeethNode_scaled = mitk::DataNode::New();
	betweenTeethNode_scaled->SetData(betweenTeeth_scaled);
	betweenTeethNode_scaled->SetName("betweenTeeth_scaled");
	GetDataStorage()->Add(betweenTeethNode_scaled, betweenTeethNode);

	// assemble "corner_scaled" and "betweenTeeth_scaled" to become "garbage_scaled"
	auto itkImage3 = ImageType::New();
	auto itkImage4 = ImageType::New();
	auto garbageImage_scaled = mitk::Image::New();

	mitk::CastToItkImage(betweenTeeth_scaled, itkImage3);
	mitk::CastToItkImage(cornerImage_scaled, itkImage4);
	addFilter->SetInput1(itkImage3);
	addFilter->SetInput2(itkImage4);
	addFilter->UpdateLargestPossibleRegion();
	garbageImage_scaled = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	auto garbageNode_scaled = mitk::DataNode::New();
	garbageNode_scaled->SetName("garbage_scaled");
	garbageNode_scaled->SetData(garbageImage_scaled);
	GetDataStorage()->Add(garbageNode_scaled);

	// get garbage_Inverted
	auto garbage_Inverted = mitk::Image::New();
	InversionFilterType::Pointer invFilter = InversionFilterType::New();
	mitk::ScalarType min = garbageImage->GetStatistics()->GetScalarValueMin();
	mitk::ScalarType max = garbageImage->GetStatistics()->GetScalarValueMax();
	invFilter->SetMaximum(max + min);

	auto itkImage5 = ImageType::New();

	mitk::CastToItkImage(garbageImage, itkImage5);

	invFilter->SetInput(itkImage5);
	invFilter->UpdateLargestPossibleRegion();
	garbage_Inverted = mitk::ImportItkImage(invFilter->GetOutput())->Clone();
	std::cout << "garbage inversion successful." << std::endl;

	auto garbageNode_inverted = mitk::DataNode::New();
	garbageNode_inverted->SetName("garbage_inverted");
	garbageNode_inverted->SetData(garbage_Inverted);
	GetDataStorage()->Add(garbageNode_inverted);


	// multiply "raw flipped" with "garbage_Inverted" to get "init"
	MultiplyFilterType::Pointer multFilter = MultiplyFilterType::New();

	auto itkImage6 = ImageType::New();
	auto itkImage7 = ImageType::New();

	mitk::CastToItkImage(garbage_Inverted, itkImage6);
	mitk::CastToItkImage(inputImage, itkImage7);

	multFilter->SetInput1(itkImage6);
	multFilter->SetInput2(itkImage7);
	multFilter->UpdateLargestPossibleRegion();

	auto initImage = mitk::Image::New();
	initImage = mitk::ImportItkImage(multFilter->GetOutput())->Clone();

	auto initImageNode = mitk::DataNode::New();
	initImageNode->SetName("init");
	initImageNode->SetData(initImage);
	GetDataStorage()->Add(initImageNode);


	// add "init" to "garbage_scaled" to get "image" 
	auto itkImage8 = ImageType::New();
	auto itkImage9 = ImageType::New();
	auto image_output = mitk::Image::New();

	mitk::CastToItkImage(garbageImage_scaled, itkImage8);
	mitk::CastToItkImage(initImage, itkImage9);
	addFilter->SetInput1(itkImage8);
	addFilter->SetInput2(itkImage9);
	addFilter->UpdateLargestPossibleRegion();
	image_output = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	auto image_outputNode = mitk::DataNode::New();
	image_outputNode->SetName("image");
	image_outputNode->SetData(image_output);
	GetDataStorage()->Add(image_outputNode);

}

void SpineCArmRegistration::on_pushButton_step_6_10_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("image");
	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("Please take the previous step first!");
		return;
	}
	auto inputImage = dynamic_cast<mitk::Image*>(attemptNode->GetData());

	//-------------- Start Step 6: Apply CLAHE and rescale to 0~3000 ----------------------

	// OpenCV::CLAHE only accepts 2D image, so we need to convert 3D "image" to 2D
	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	short insideValue = 2000;

	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	auto inputVtkImage = inputImage->GetVtkImageData(0, 0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToUnsignedShort();
	//caster->SetOutputScalarTypeToUnsignedChar();
	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));

				int* m = static_cast<int*>(whiteImage->GetScalarPointer(x, y, z));

				m[0] = n[0];

			}
		}
	}

	auto input_2d = mitk::Image::New();

	input_2d->Initialize(whiteImage, 1, -1, -1, dim[1]);

	input_2d->SetVolume(whiteImage->GetScalarPointer());

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("image_2D");
	tmpNode->SetData(input_2d);
	GetDataStorage()->Add(tmpNode);

	// Apply CLAHE



	// -------- Convert mitk image to OpenCV image ------------------
	mitk::ImageToOpenCVImageFilter::Pointer mitkToOpenCvFilter = mitk::ImageToOpenCVImageFilter::New();
	mitkToOpenCvFilter->SetInputFromTimeSlice(input_2d, 0, 0);
	cv::Mat openCVImage = mitkToOpenCvFilter->GetOpenCVMat();

	double clipLimit = 4;
	int gridWidth = 3;
	int gridHeight = 3;

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(cv::Size(gridWidth, gridHeight));

	cv::Mat enhancedImage;
	clahe->apply(openCVImage, enhancedImage);

	// convert OpenCV image to mitk Image
	mitk::OpenCVToMitkImageFilter::Pointer openCVtoMitkFilter = mitk::OpenCVToMitkImageFilter::New();
	openCVtoMitkFilter->SetOpenCVMat(enhancedImage);
	openCVtoMitkFilter->Update();

	auto claheResult = openCVtoMitkFilter->GetOutput();


	// convert 2D "claheResult" to 3D
	caster->SetInputData(claheResult->GetVtkImageData());
	caster->SetOutputScalarTypeToUnsignedShort();
	//caster->SetOutputScalarTypeToInt();
	caster->Update();

	castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));

				int* m = static_cast<int*>(whiteImage->GetScalarPointer(x, y, z));

				m[0] = n[0];

			}
		}
	}
	auto clahe3d = mitk::Image::New();

	clahe3d->Initialize(whiteImage, 1, -1, dim[2], dim[1]);

	clahe3d->SetVolume(whiteImage->GetScalarPointer());



	// rescale "claheResult" to 0~3000 interval
	ImageType::Pointer floatImage = ImageType::New();
	mitk::CastToItkImage(clahe3d, floatImage);
	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer filter = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	filter->SetInput(0, floatImage);
	filter->SetOutputMinimum(0);
	filter->SetOutputMaximum(3000);
	filter->Update();
	floatImage = filter->GetOutput();

	auto rescaledClahe = mitk::Image::New();
	rescaledClahe->InitializeByItk(floatImage.GetPointer());
	rescaledClahe->SetVolume(floatImage->GetBufferPointer());
	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	rescaledClahe->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	cout << "Rescaling successful." << std::endl;

	auto claheNode = mitk::DataNode::New();
	claheNode->SetData(rescaledClahe);
	claheNode->SetName("CLAHE_3000");
	GetDataStorage()->Add(claheNode);

	//-------------- End Step 6: Apply CLAHE and rescale to 0~3000 ----------------------

	//-------------- Start Step 7: Gaussian filter (var 50) followed by subtraction ----------- 
	// var 50 Gaussian filter
	GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
	double gaussianVar{ 50 };
	auto gaussianInput = ImageType::New();
	mitk::CastToItkImage(rescaledClahe, gaussianInput);

	gaussianFilter->SetInput(gaussianInput);
	gaussianFilter->SetVariance(gaussianVar);
	gaussianFilter->UpdateLargestPossibleRegion();

	auto gaussianedImage = mitk::Image::New();

	gaussianedImage = mitk::ImportItkImage(gaussianFilter->GetOutput())->Clone();
	std::cout << "Gaussian filtering successful." << std::endl;

	auto gaussianNode = mitk::DataNode::New();
	gaussianNode->SetData(gaussianedImage);
	gaussianNode->SetName("gaussian");
	GetDataStorage()->Add(gaussianNode);

	// Subtraction: CLAHE_3000 - Gaussian
	SubtractFilterType::Pointer subFilter = SubtractFilterType::New();
	auto subtrInput_0 = ImageType::New();
	auto subtrInput_1 = ImageType::New();
	mitk::CastToItkImage(rescaledClahe, subtrInput_0);
	mitk::CastToItkImage(gaussianedImage, subtrInput_1);

	subFilter->SetInput1(subtrInput_0);
	subFilter->SetInput2(subtrInput_1);
	subFilter->UpdateLargestPossibleRegion();

	auto unsharpMask = mitk::Image::New();
	unsharpMask = mitk::ImportItkImage(subFilter->GetOutput())->Clone();
	auto unsharpMaskNode = mitk::DataNode::New();
	unsharpMaskNode->SetData(unsharpMask);
	unsharpMaskNode->SetName("unsharpMask");
	GetDataStorage()->Add(unsharpMaskNode);
	//-------------- End Step 7: Gaussian filter (var 50) followed by subtraction ----------- 

	//-------------- Start Step 8: UnsharpMask x3 + rescaledCLAHE -----------

	// scale up the unsharpMask by 3
	double scaleScalar{ 1.5 };
	auto scalarScaleInput = ImageType::New();
	mitk::CastToItkImage(unsharpMask, scalarScaleInput);
	itk::ShiftScaleImageFilter<ImageType, ImageType>::Pointer scalarScalefilter = itk::ShiftScaleImageFilter<ImageType, ImageType>::New();
	scalarScalefilter->SetInput(0, scalarScaleInput);
	scalarScalefilter->SetScale(scaleScalar);
	scalarScalefilter->Update();

	scalarScaleInput = scalarScalefilter->GetOutput();

	auto scaledUnsharpMask = mitk::Image::New();
	scaledUnsharpMask->InitializeByItk(scalarScaleInput.GetPointer());
	scaledUnsharpMask->SetVolume(scalarScaleInput->GetBufferPointer());
	std::cout << "UnsharpMask Rescaling successful." << std::endl;

	auto unsharpMaskRescaleNode = mitk::DataNode::New();
	unsharpMaskRescaleNode->SetData(scaledUnsharpMask);
	unsharpMaskRescaleNode->SetName("unsharpMask_x3");
	GetDataStorage()->Add(unsharpMaskRescaleNode);

	// UnsharpMask x3 + rescaledCLAHE
	AddFilterType::Pointer addFilter = AddFilterType::New();
	auto addInput_0 = ImageType::New();
	auto addInput_1 = ImageType::New();
	auto unsharpMaskedImage = mitk::Image::New();

	mitk::CastToItkImage(rescaledClahe, addInput_0);
	mitk::CastToItkImage(scaledUnsharpMask, addInput_1);
	addFilter->SetInput1(addInput_1);
	addFilter->SetInput2(addInput_0);
	addFilter->UpdateLargestPossibleRegion();
	unsharpMaskedImage = mitk::ImportItkImage(addFilter->GetOutput())->Clone();


	// rescale the unsharpMasked result to 0-3000
	short lowerLimit{ 0 };
	short upperLimit{ 3000 };

	ImageType::Pointer rescaleInput = ImageType::New();
	mitk::CastToItkImage(unsharpMaskedImage, rescaleInput);
	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer filterRe = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	filterRe->SetInput(0, rescaleInput);
	filterRe->SetOutputMinimum(lowerLimit);
	filterRe->SetOutputMaximum(upperLimit);
	filterRe->Update();
	rescaleInput = filterRe->GetOutput();

	auto unsharp_rescaled = mitk::Image::New();
	unsharp_rescaled->InitializeByItk(rescaleInput.GetPointer());
	unsharp_rescaled->SetVolume(rescaleInput->GetBufferPointer());
	unsharp_rescaled->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	cout << "Rescaling successful." << std::endl;

	auto unsharp_rescaledNode = mitk::DataNode::New();
	unsharp_rescaledNode->SetData(unsharp_rescaled);
	unsharp_rescaledNode->SetName("enhanced");
	GetDataStorage()->Add(unsharp_rescaledNode);


	// auto unsharpMaskedNode = mitk::DataNode::New();
	// unsharpMaskedNode->SetName("enhanced");
	// unsharpMaskedNode->SetData(unsharpMaskedImage);
	// GetDataStorage()->Add(unsharpMaskedNode);

	//-------------- End Step 8: UnsharpMask x3 + rescaledCLAHE ----------- 

	//-------------- Start Step 9: Canny Edge detection: var 30, upper thres 3, lower thres 0 -----------
	double cannyVar_less{ 30 };
	double cannyUpperThres_less{ 3 };
	double cannyLowerThres_less{ 0 };

	itk::CannyEdgeDetectionImageFilter<DoubleImageType, DoubleImageType>::Pointer cannyFilter = itk::CannyEdgeDetectionImageFilter<DoubleImageType, DoubleImageType>::New();


	auto cannylessInput = DoubleImageType::New();

	mitk::CastToItkImage(unsharpMask, cannylessInput);

	cannyFilter->SetInput(cannylessInput);

	cannyFilter->SetVariance(cannyVar_less);
	cannyFilter->SetUpperThreshold(cannyUpperThres_less);
	cannyFilter->SetLowerThreshold(cannyLowerThres_less);

	cannyFilter->Update();

	auto cannyLessImage = mitk::Image::New();

	// mitk::CastToMitkImage(cannyFilter->GetOutput(), cannyLessImage);

	cannyLessImage = mitk::ImportItkImage(cannyFilter->GetOutput())->Clone();


	auto cannyLessNode = mitk::DataNode::New();
	cannyLessNode->SetName("Edges_less");
	cannyLessNode->SetData(cannyLessImage);
	GetDataStorage()->Add(cannyLessNode);
	//-------------- End Step 9: Canny Edge detection: var 30, upper thres 3, lower thres 0 -----------

	//-------------- Start Step 10: Canny Edge detection: var 30, upper thres 1, lower thres 0 -----------
	double cannyVar_more{ 30 };
	double cannyUpperThres_more{ 1 };
	double cannyLowerThres_more{ 0 };

	cannyFilter->SetVariance(cannyVar_more);
	cannyFilter->SetUpperThreshold(cannyUpperThres_more);
	cannyFilter->SetLowerThreshold(cannyLowerThres_more);

	cannyFilter->Update();

	auto cannyMoreImage = mitk::Image::New();

	cannyMoreImage = mitk::ImportItkImage(cannyFilter->GetOutput())->Clone();

	auto cannyMoreNode = mitk::DataNode::New();
	cannyMoreNode->SetName("Edges_more");
	cannyMoreNode->SetData(cannyMoreImage);
	GetDataStorage()->Add(cannyMoreNode);

	//-------------- End Step 10: Canny Edge detection: var 30, upper thres 1, lower thres 0 -----------

}

void SpineCArmRegistration::on_pushButton_step_11_13_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("Edges_less");
	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("Please take the previous step first!");
		return;
	}
	auto inputImage = dynamic_cast<mitk::Image*>(attemptNode->GetData());

	//--------- Start Step 11: Edges_less density calculation ---------------------

	cout << "----Start edge heat map generation----" << endl;
	auto inputVtkImage = inputImage->GetVtkImageData(0, 0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToUnsignedShort();

	caster->Update();

	auto castVtkImage = caster->GetOutput();

	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);

	cout << "Printing dim: " << dim[0] << ", " << dim[1] << ", " << dim[2] << endl;

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	// Do a sum of the neighboring 40x40 area

	int neighborSize{ 20 }; // radius

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				unsigned short tmpSum{ 0 };

				for (int i = 0; i < (2 * neighborSize); i++)
				{
					for (int j = 0; j < (2 * neighborSize); j++)
					{
						if ((x + i - neighborSize) >= 0 && (x + i - neighborSize) < dims[0] && (y + j - neighborSize) >= 0 && (y + j - neighborSize) < dims[1])
						{
							if (*(static_cast<unsigned short*>(castVtkImage->GetScalarPointer(x + i - neighborSize, y + j - neighborSize, z))) > 0)
							{
								tmpSum += 1;
							}

						}
					}
				}

				unsigned short* m = static_cast<unsigned short*>(whiteImage->GetScalarPointer(x, y, z));

				*m = tmpSum;

			}
		}
	}

	auto heatmapImage = mitk::Image::New();

	heatmapImage->Initialize(whiteImage, 1, -1, dim[2], dim[1]);

	heatmapImage->SetVolume(whiteImage->GetScalarPointer());

	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	heatmapImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Heatmap");
	tmpNode->SetData(heatmapImage);
	GetDataStorage()->Add(tmpNode);

	//--------- End Step 11: Edges_less density calculation ---------------------

	//--------- Start Step 12: Thresholding the Edges_less density heatmap -----------
	int param1{ 0 }; int param2{ 42 };
	ThresholdFilterType::Pointer thFilter = ThresholdFilterType::New();
	auto thresInput = ImageType::New();
	auto teethImage = mitk::Image::New();
	auto backgroundImage = mitk::Image::New();

	mitk::CastToItkImage(dynamic_cast<const mitk::Image*>(GetDataStorage()->GetNamedNode("Heatmap")->GetData()), thresInput);

	thFilter->SetLowerThreshold(param1 < param2 ? param1 : param2);
	thFilter->SetUpperThreshold(param2 > param1 ? param2 : param1);
	thFilter->SetInsideValue(1);
	thFilter->SetOutsideValue(0);
	thFilter->SetInput(thresInput);
	thFilter->UpdateLargestPossibleRegion();

	teethImage = mitk::ImportItkImage(thFilter->GetOutput())->Clone();

	thFilter->SetInsideValue(0);
	thFilter->SetOutsideValue(1);
	thFilter->UpdateLargestPossibleRegion();

	backgroundImage = mitk::ImportItkImage(thFilter->GetOutput())->Clone();

	auto backgroundNode = mitk::DataNode::New();
	backgroundNode->SetData(backgroundImage);
	backgroundNode->SetName("background");
	GetDataStorage()->Add(backgroundNode);

	auto teethNode = mitk::DataNode::New();
	teethNode->SetData(teethImage);
	teethNode->SetName("teeth");
	GetDataStorage()->Add(teethNode);

	//--------- End Step 12: Thresholding the Edges_less density heatmap -----------

	//--------- Start Step 13: "Background" x "Edges_more"
	auto multFilter = MultiplyFilterType::New();

	auto multInput_0 = ImageType::New();
	auto multInput_1 = ImageType::New();

	mitk::CastToItkImage(backgroundImage, multInput_0);
	mitk::CastToItkImage(dynamic_cast<const mitk::Image*>(GetDataStorage()->GetNamedNode("Edges_more")->GetData()), multInput_1);

	multFilter->SetInput1(multInput_0);
	multFilter->SetInput2(multInput_1);
	multFilter->UpdateLargestPossibleRegion();

	auto filteredEdgeImage = mitk::Image::New();
	filteredEdgeImage = mitk::ImportItkImage(multFilter->GetOutput())->Clone();

	auto filteredEdgeImageNode = mitk::DataNode::New();
	filteredEdgeImageNode->SetName("filtered_edges");
	filteredEdgeImageNode->SetData(filteredEdgeImage);
	GetDataStorage()->Add(filteredEdgeImageNode);

	//--------- End Step 13: "Background" x "Edges_more"

}

void SpineCArmRegistration::on_pushButton_step_14_16_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("filtered_edges");
	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("Please take the previous step first!");
		return;
	}
	auto filteredEdges = dynamic_cast<mitk::Image*>(attemptNode->GetData());

	//---------- Start Step 14: "enhanced" X "filtered_Edges_inverted"-----------

	// get filtered_Edges_inverted
	auto filtered_Edges_inverted = mitk::Image::New();
	InversionFilterType::Pointer invFilter = InversionFilterType::New();
	mitk::ScalarType min = filteredEdges->GetStatistics()->GetScalarValueMin();
	mitk::ScalarType max = filteredEdges->GetStatistics()->GetScalarValueMax();
	invFilter->SetMaximum(max + min);

	auto invFilterInput = ImageType::New();

	mitk::CastToItkImage(filteredEdges, invFilterInput);

	invFilter->SetInput(invFilterInput);
	invFilter->UpdateLargestPossibleRegion();
	filtered_Edges_inverted = mitk::ImportItkImage(invFilter->GetOutput())->Clone();
	std::cout << "filtered_Edges inversion successful." << std::endl;

	auto filtered_Edges_inverted_node = mitk::DataNode::New();
	filtered_Edges_inverted_node->SetName("filtered_Edges_inverted");
	filtered_Edges_inverted_node->SetData(filtered_Edges_inverted);
	GetDataStorage()->Add(filtered_Edges_inverted_node);

	// "enhanced" X "filtered_Edges_inverted"
	auto enhanced = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("enhanced")->GetData());

	auto multFilter = MultiplyFilterType::New();

	auto multInput_0 = ImageType::New();
	auto multInput_1 = ImageType::New();

	mitk::CastToItkImage(enhanced, multInput_0);
	mitk::CastToItkImage(filtered_Edges_inverted, multInput_1);

	multFilter->SetInput1(multInput_0);
	multFilter->SetInput2(multInput_1);
	multFilter->UpdateLargestPossibleRegion();

	auto edged_unsharp = mitk::Image::New();
	edged_unsharp = mitk::ImportItkImage(multFilter->GetOutput())->Clone();

	auto edged_unsharpNode = mitk::DataNode::New();
	edged_unsharpNode->SetName("edged_unsharp");
	edged_unsharpNode->SetData(edged_unsharp);
	GetDataStorage()->Add(edged_unsharpNode);

	//------- End Step 14: "enhanced" X "filtered_Edges_inverted"---------

	//------- Start Step 15: Apply total variation filter and Closing filter --------------
	// total variation filter 
	int iterNum{ 40 }; int lamdaOverThousand{ 10 };

	DoubleImageType::Pointer tvInput = DoubleImageType::New();
	mitk::CastToItkImage(edged_unsharp, tvInput);

	TotalVariationFilterType::Pointer TVFilter
		= TotalVariationFilterType::New();
	TVFilter->SetInput(tvInput.GetPointer());
	TVFilter->SetNumberIterations(iterNum);
	TVFilter->SetLambda(double(lamdaOverThousand) / 1000.);
	TVFilter->UpdateLargestPossibleRegion();

	auto edged_unsharp_tv = mitk::Image::New();
	edged_unsharp_tv = mitk::ImportItkImage(TVFilter->GetOutput())->Clone();

	auto edged_unsharpTVNode = mitk::DataNode::New();
	edged_unsharpTVNode->SetName("edged_unsharp_tv");
	edged_unsharpTVNode->SetData(edged_unsharp_tv);
	GetDataStorage()->Add(edged_unsharpTVNode);

	// Closing filter
	BallType binaryBall;
	int closingRadius{ 2 };

	auto closingInput = ImageType::New();
	mitk::CastToItkImage(edged_unsharp_tv, closingInput);

	binaryBall.SetRadius(closingRadius);
	binaryBall.CreateStructuringElement();

	ClosingFilterType::Pointer closeFilter = ClosingFilterType::New();
	closeFilter->SetInput(closingInput);
	closeFilter->SetKernel(binaryBall);
	closeFilter->UpdateLargestPossibleRegion();

	auto closedImage = mitk::Image::New();
	closedImage = mitk::ImportItkImage(closeFilter->GetOutput())->Clone();
	std::cout << "Closing successful." << std::endl;

	auto closedNode = mitk::DataNode::New();
	closedNode->SetName("closed");
	closedNode->SetData(closedImage);
	GetDataStorage()->Add(closedNode);

	//------- End Step 15: Apply total variation filter and closing filter--------------

	//------- Start Step 16: rescale "closed" image and add to "enhanced" image --------------
	//rescale "closed"
	short lowerLimit{ 0 };
	short upperLimit{ 3000 };

	ImageType::Pointer rescaleInput = ImageType::New();
	mitk::CastToItkImage(closedImage, rescaleInput);
	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer filter = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	filter->SetInput(0, rescaleInput);
	filter->SetOutputMinimum(lowerLimit);
	filter->SetOutputMaximum(upperLimit);
	filter->Update();
	rescaleInput = filter->GetOutput();

	auto closed_rescaled = mitk::Image::New();
	closed_rescaled->InitializeByItk(rescaleInput.GetPointer());
	closed_rescaled->SetVolume(rescaleInput->GetBufferPointer());
	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	closed_rescaled->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	cout << "Rescaling successful." << std::endl;

	auto closed_rescaledNode = mitk::DataNode::New();
	closed_rescaledNode->SetData(closed_rescaled);
	closed_rescaledNode->SetName("closed_rescaled");
	GetDataStorage()->Add(closed_rescaledNode);

	// Add to "enhanced"
	AddFilterType::Pointer addFilter = AddFilterType::New();
	auto addInput_0 = ImageType::New();
	auto addInput_1 = ImageType::New();
	auto enhanced_CLAHE = mitk::Image::New();

	mitk::CastToItkImage(dynamic_cast<const mitk::Image*>(GetDataStorage()->GetNamedNode("enhanced")->GetData()), addInput_0);
	mitk::CastToItkImage(closed_rescaled, addInput_1);
	addFilter->SetInput1(addInput_1);
	addFilter->SetInput2(addInput_0);
	addFilter->UpdateLargestPossibleRegion();
	enhanced_CLAHE = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	auto enhanced_CLAHENode = mitk::DataNode::New();
	enhanced_CLAHENode->SetName("enhanced_CLAHE");
	enhanced_CLAHENode->SetData(enhanced_CLAHE);
	GetDataStorage()->Add(enhanced_CLAHENode);


	//------- Start Step 16: rescale "closed" image and add to "enhanced" image --------------

}

void SpineCArmRegistration::on_pushButton_step_17_clicked()
{
	// multiply with "garbage_invert"
	auto garbage_inverted = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("garbage_inverted")->GetData());
	auto enhanced_CLAHE = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("enhanced_CLAHE")->GetData());


	auto multFilter = MultiplyFilterType::New();

	auto multInput_0 = ImageType::New();
	auto multInput_1 = ImageType::New();

	mitk::CastToItkImage(garbage_inverted, multInput_0);
	mitk::CastToItkImage(enhanced_CLAHE, multInput_1);

	multFilter->SetInput1(multInput_0);
	multFilter->SetInput2(multInput_1);
	multFilter->UpdateLargestPossibleRegion();

	auto cleaned = mitk::Image::New();
	cleaned = mitk::ImportItkImage(multFilter->GetOutput())->Clone();

	auto cleanedNode = mitk::DataNode::New();
	cleanedNode->SetName("final");
	cleanedNode->SetData(cleaned);
	GetDataStorage()->Add(cleanedNode);

	// Rescale to 3000 (unsigned short)
	//------- Start Step 16: rescale "closed" image and add to "enhanced" image --------------
	//rescale "closed"
	int lowerLimit{ 0 };
	int upperLimit{ 3000 };

	ImageType::Pointer rescaleInput = ImageType::New();
	mitk::CastToItkImage(cleaned, rescaleInput);
	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer filter = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	filter->SetInput(0, rescaleInput);
	filter->SetOutputMinimum(lowerLimit);
	filter->SetOutputMaximum(upperLimit);
	filter->Update();
	rescaleInput = filter->GetOutput();

	auto closed_rescaled = mitk::Image::New();
	closed_rescaled->InitializeByItk(rescaleInput.GetPointer());
	closed_rescaled->SetVolume(rescaleInput->GetBufferPointer());
	auto identityMatrix = vtkMatrix4x4::New();
	identityMatrix->Identity();
	closed_rescaled->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	cout << "Rescaling successful." << std::endl;

	auto closed_rescaledNode = mitk::DataNode::New();
	closed_rescaledNode->SetData(closed_rescaled);
	closed_rescaledNode->SetName("final_rescaled");
	GetDataStorage()->Add(closed_rescaledNode);

}



// void SpineCArmRegistration::ApplyAddFilter(us_short_ImageType::Pointer inputImage_1
// 	, us_short_ImageType::Pointer inputImage_2
// 	, us_short_ImageType::Pointer &outputImage)
// {
// 	static itk::AddImageFilter<us_short_ImageType, us_short_ImageType, us_short_ImageType>::Pointer addfilter;
// 	addfilter = itk::AddImageFilter<us_short_ImageType, us_short_ImageType, us_short_ImageType>::New();
//
// 	addfilter->SetInput1(inputImage_1);
// 	addfilter->SetInput2(inputImage_2);
// 	addfilter->UpdateLargestPossibleRegion();
//
// 	outputImage = addfilter->GetOutput();
// }


void SpineCArmRegistration::ApplyAddFilter(us_short_ImageType::Pointer inputImage_1
	, us_short_ImageType::Pointer inputImage_2, us_short_ImageType::Pointer outputImage)
{
	// Apply the filter and get the processed image
	auto addfilter = itk::AddImageFilter<us_short_ImageType, us_short_ImageType, us_short_ImageType>::New();

	addfilter->SetInput1(inputImage_1);
	addfilter->SetInput2(inputImage_2);
	addfilter->UpdateLargestPossibleRegion();

	auto processedImage = addfilter->GetOutput();


	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}

}

void SpineCArmRegistration::ApplyMultiplyFilter(us_short_ImageType::Pointer inputImage_1, us_short_ImageType::Pointer inputImage_2, us_short_ImageType::Pointer outputImage)
{
	// Apply the filter and get the processed image
	auto multiplyFilter = itk::MultiplyImageFilter<us_short_ImageType, us_short_ImageType, us_short_ImageType>::New();

	multiplyFilter->SetInput1(inputImage_1);
	multiplyFilter->SetInput2(inputImage_2);
	multiplyFilter->UpdateLargestPossibleRegion();

	auto processedImage = multiplyFilter->GetOutput();


	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}
}

void SpineCArmRegistration::ApplySubtractFilter(us_short_ImageType::Pointer inputImage_1
	, us_short_ImageType::Pointer inputImage_2, us_short_ImageType::Pointer outputImage)
{
	// Apply the filter and get the processed image
	auto subtractFilter = itk::SubtractImageFilter<us_short_ImageType, us_short_ImageType, us_short_ImageType>::New();

	subtractFilter->SetInput1(inputImage_1);
	subtractFilter->SetInput2(inputImage_2);
	subtractFilter->UpdateLargestPossibleRegion();

	auto processedImage = subtractFilter->GetOutput();


	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}

}

void SpineCArmRegistration::ApplyGaussianFilter(us_short_ImageType::Pointer inputImage,
	double variance, us_short_ImageType::Pointer outputImage)
{
	auto gaussianFilter = itk::DiscreteGaussianImageFilter<us_short_ImageType, us_short_ImageType>::New();

	gaussianFilter->SetInput(inputImage);
	gaussianFilter->SetVariance(variance);
	gaussianFilter->UpdateLargestPossibleRegion();

	auto processedImage = gaussianFilter->GetOutput();

	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}
}

void SpineCArmRegistration::ApplyRescaleToIntervalFilter(us_short_ImageType::Pointer inputImage, int upperLimit, int lowerLimit, us_short_ImageType::Pointer outputImage)
{
	auto rescaleFilter =
		itk::RescaleIntensityImageFilter<us_short_ImageType, us_short_ImageType>::New();

	rescaleFilter->SetInput(inputImage);
	rescaleFilter->SetOutputMaximum(upperLimit);
	rescaleFilter->SetOutputMinimum(lowerLimit);
	rescaleFilter->UpdateLargestPossibleRegion();

	auto processedImage = rescaleFilter->GetOutput();

	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}
}

void SpineCArmRegistration::ApplyShiftAndScaleFilter(us_short_ImageType::Pointer inputImage, int shiftFactor, int scaleFactor, us_short_ImageType::Pointer outputImage)
{
	auto shiftScaleFilter = itk::ShiftScaleImageFilter< us_short_ImageType, us_short_ImageType >::New();
	shiftScaleFilter->SetInput(inputImage);
	shiftScaleFilter->SetShift(shiftFactor);
	shiftScaleFilter->SetScale(scaleFactor);
	shiftScaleFilter->UpdateLargestPossibleRegion();

	auto processedImage = shiftScaleFilter->GetOutput();

	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<us_short_ImageType>;
	itk::ImageRegionConstIterator<us_short_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<us_short_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}
}

void SpineCArmRegistration::ApplyLaplacianFilter(us_short_ImageType::Pointer inputImage, double_ImageType::Pointer outputImage)
{
	auto imageTypeCaster_0 = itk::CastImageFilter<us_short_ImageType, double_ImageType>::New();

	auto laplacianFilter = itk::LaplacianImageFilter< double_ImageType, double_ImageType >::New();

	imageTypeCaster_0->SetInput(inputImage);
	imageTypeCaster_0->Update();

	double_ImageType::Pointer doubleInput = imageTypeCaster_0->GetOutput();

	laplacianFilter->SetInput(doubleInput);
	laplacianFilter->UpdateLargestPossibleRegion();

	auto processedImage = laplacianFilter->GetOutput();

	// Deep copy
	outputImage->SetRegions(processedImage->GetLargestPossibleRegion());
	outputImage->Allocate();

	// Create iterators for the original and copied images
	using IteratorType = itk::ImageRegionIterator<double_ImageType>;
	itk::ImageRegionConstIterator<double_ImageType> originalIterator(processedImage, processedImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<double_ImageType> copiedIterator(outputImage, outputImage->GetLargestPossibleRegion());

	// Perform the deep copy using iterators
	while (!originalIterator.IsAtEnd())
	{
		copiedIterator.Set(originalIterator.Get());
		++originalIterator;
		++copiedIterator;
	}

}


//void SpineCArmRegistration::on_pushButton_EasyItkTest_clicked()
//{
//
//	us_short_ImageType::Pointer addIput1 = us_short_ImageType::New();
//	us_short_ImageType::Pointer addIput2 = us_short_ImageType::New();
//	//us_short_ImageType::Pointer outputImage = us_short_ImageType::New();
//	double_ImageType::Pointer outputImage = double_ImageType::New();
//
//	auto input1 = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw_1")->GetData());
//	auto input2 = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("raw_2")->GetData());
//
//	mitk::CastToItkImage(input1, addIput1);
//	mitk::CastToItkImage(input2, addIput2);
//
//	// Test addFilter
//	// ApplyAddFilter(addIput1, addIput2, outputImage);
//
//	// Test Gaussian Filter
//	// ApplyGaussianFilter(addIput1, 50, outputImage);
//
//	// Test rescaleToInterval filter
//	// ApplyRescaleToIntervalFilter(addIput1, 3000, 0, outputImage);
//
//	// Test shift and scale filter
//	// ApplyShiftAndScaleFilter(addIput1, -1000, 2, outputImage);
//
//	// Test multiply filter
//	// ApplyMultiplyFilter(addIput1, addIput2, outputImage);
//
//	// Test subtract filter
//	// ApplySubtractFilter(addIput1, addIput2, outputImage);
//
//	// Test laplacian filter
//	ApplyLaplacianFilter(addIput1, outputImage);
//
//	// Test output
//	auto output = mitk::Image::New();
//	mitk::CastToMitkImage(outputImage, output);
//
//	auto tmpNode = mitk::DataNode::New();
//	tmpNode->SetName("testResult");
//	tmpNode->SetData(output);
//	GetDataStorage()->Add(tmpNode);
//}

void SpineCArmRegistration::on_pushButton_EasyItkTest_clicked()
{
	DrEnhanceType1_intermediate_test();
	DrEnhanceType2_test();
	DrEnhanceType1_test();

	// auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetData());
	//
	// auto inputItkImage = doubleImageType::New();
	// auto processedItkImage = doubleImageType::New(); // output of Type1
	// mitk::CastToItkImage(inputImage, inputItkImage);
	//
	// // Extract edges and the non-edge area (background)
	// cannyEdgeFilterType::Pointer cannyEdgeFilter = cannyEdgeFilterType::New();
	// cannyEdgeFilter->SetInput(inputItkImage); //requires fine tuning
	// cannyEdgeFilter->SetVariance(30);
	// cannyEdgeFilter->SetLowerThreshold(5);  // requires fine tuning: 80
	// cannyEdgeFilter->SetUpperThreshold(10); // requires fine tuning: 400
	// cannyEdgeFilter->Update();
	//
	// auto edges = cannyEdgeFilter->GetOutput();
	//
	// // Todo: use edge density information to decide the enhancement values
	// doubleImageIteratorType edges_it(edges, edges->GetRequestedRegion());
	// doubleImageIteratorType inputItkImage_it(inputItkImage, inputItkImage->GetRequestedRegion());
	// double bright_count{ 0 };
	// double edge_count{ 0 };
	// while (!edges_it.IsAtEnd())
	// {
	// 	if (inputItkImage_it.Get() > 50000 && edges_it.Get() > 0)
	// 	{
	// 		edge_count += 1;
	// 	}
	//
	// 	if(inputItkImage_it.Get() > 50000)
	// 	{
	// 		bright_count += 1;
	// 	}
	//
	//
	// 	++inputItkImage_it;
	// 	++edges_it;
	// }
	//
	//
	//
	// // while (!edges_it.IsAtEnd())
	// // {
	// // 	if (edges_it.Get() > 0)
	// // 	{
	// // 		edge_count += 1;
	// // 	}
	// //
	// // 	++edges_it;
	// // }
	//
	// m_Controls.textBrowser->append("Edge pix num: " + QString::number(100*edge_count/bright_count)+"%");
	//
	// // MITK Test View
	// auto a = mitk::Image::New();
	// // a = mitk::ImportItkImage(castResult)->Clone();
	// auto b = mitk::Image::New();
	// b = mitk::ImportItkImage(edges)->Clone();
	//
	// auto a_node = mitk::DataNode::New();
	// auto b_node = mitk::DataNode::New();
	//
	// a_node->SetData(a);
	// a_node->SetName(m_Controls.mitkNodeSelectWidget_dentalDR->GetSelectedNode()->GetName() + "_2");
	// b_node->SetData(b);
	// b_node->SetName("edges");
	//
	// // GetDataStorage()->Add(a_node);
	// GetDataStorage()->Add(b_node);

}



