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


#include "itkCastImageFilter.h"
#include "itkMath.h"
#include "lancetNCC.h"

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
	GeoMatch GM;
	int lowThreshold = 10;		//deafult value; should be within [0,255]
	int highThreashold = 100;	//deafult value; should be within [0,255]

	double minScore = 0.5;		//deafult value
	double greediness = 0.8;		//deafult value

	double total_time = 0;
	double score = 0;
	CvPoint result;

	auto sourceQbyteArray = m_Controls.lineEdit_NccSource->text().toLatin1();
	auto sourcePath = sourceQbyteArray.data();
	auto templateQbyteArray = m_Controls.lineEdit_NccTemplate->text().toLatin1();
	auto templatePath = templateQbyteArray.data();


	//IplImage* templateImage = cvLoadImage("D:/Data/spine/2. Two projection project/TestNCC/Template.jpg", -1);
	IplImage* templateImage = cvLoadImage(templatePath, -1);

	if (templateImage == NULL)
	{
		cout << "\nERROR: Could not load Template Image.\n";
		return 0;
	}

	//IplImage* searchImage = cvLoadImage("D:/Data/spine/2. Two projection project/TestNCC/Search1.jpg", -1);
	IplImage* searchImage = cvLoadImage(sourcePath, -1);

	if (searchImage == NULL)
	{
		cout << "\nERROR: Could not load Search Image.";
		return 0;
	}

	CvSize templateSize = cvSize(templateImage->width, templateImage->height);
	IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);


	// Convert color image to gray image.
	if (templateImage->nChannels == 3)
	{
		cvCvtColor(templateImage, grayTemplateImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(templateImage, grayTemplateImg);
	}
	cout << "\n Edge Based Template Matching Program\n";
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: could not create model...";
		return 0;
	}
	// GM.DrawContours(templateImage, CV_RGB(255, 0, 0), 1);
	GM.DrawContours(templateImage, cvScalar(255,0,0), 1);
	cout << " Shape model created.." << "with  Low Threshold = " << lowThreshold << " High Threshold = " << highThreashold << endl;
	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image. 
	if (searchImage->nChannels == 3)
		cvCvtColor(searchImage, graySearchImg, CV_RGB2GRAY);
	else
	{
		cvCopy(searchImage, graySearchImg);
	}
	cout << " Finding Shape Model.." << " Minumum Score = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, &result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;

	if (score > minScore) // if score is atleast 0.4
	{
		cout << " Found at [" << result.x << ", " << result.y << "]\n Score = " << score << "\n Searching Time = " << total_time * 1000 << "ms";
		// GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 1);
		GM.DrawContours(searchImage, result, cvScalar(0, 255, 0), 1);
	}
	else
		cout << " Object Not found";

	cout << "\n ------------------------------------\n\n";
	cout << "\n Press any key to exit!";

	//Display result
	cvNamedWindow("Template", CV_WINDOW_AUTOSIZE);
	cvShowImage("Template", templateImage);
	cvNamedWindow("Search Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Search Image", searchImage);
	// wait for both windows to be closed before releasing images
	cvWaitKey(0);
	cvDestroyWindow("Search Image");
	cvDestroyWindow("Template");
	cvReleaseImage(&searchImage);
	cvReleaseImage(&graySearchImg);
	cvReleaseImage(&templateImage);
	cvReleaseImage(&grayTemplateImg);

	return 1;
}













