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












