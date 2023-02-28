/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <itkAddImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMultiplyImageFilter.h>
#include <itkResampleImageFilter.h>
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetTha3DimageGenerator.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkImageCast.h"
#include "mitkSurfaceToImageFilter.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include "surfaceregistraion.h"

typedef itk::Image<short, 3> ImageType;
typedef itk::Image<double, 3> DoubleImageType;
typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > ThresholdFilterType;
typedef itk::MultiplyImageFilter< ImageType, ImageType, ImageType > MultiplyFilterType;
typedef itk::AddImageFilter< DoubleImageType, DoubleImageType, ImageType > AddFilterType;


lancet::Tha3DImageGenerator::Tha3DImageGenerator():
m_PelvisImage(mitk::Image::New()),
m_FemurImage_R(mitk::Image::New()),
m_FemurImage_L(mitk::Image::New()),
m_CupSurface(mitk::Surface::New()),
m_LinerSurface(mitk::Surface::New()),
m_StemSurface(mitk::Surface::New()),
m_BallHeadSurface(mitk::Surface::New())
{
	
}

lancet::Tha3DImageGenerator::~Tha3DImageGenerator()
{

}

mitk::Image::Pointer lancet::Tha3DImageGenerator::GetStenciledImage(mitk::Surface::Pointer inputSurface, short StencilValue)
{
	
	vtkNew<vtkPolyData> surfacePolyData;
	surfacePolyData->DeepCopy(inputSurface->GetVtkPolyData());

	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(inputSurface->GetGeometry()->GetVtkMatrix());
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(surfacePolyData);
	tmpTransFilter->Update();

	auto objectSurface = mitk::Surface::New();
	objectSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());

	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 0.2, 0.2, 0.2 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = objectSurface->GetGeometry();
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
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = StencilValue;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	auto imageToCrop = mitk::Image::New();
	imageToCrop->Initialize(whiteImage);
	imageToCrop->SetVolume(whiteImage->GetScalarPointer());


	// Apply the stencil
	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetImage(imageToCrop);
	surfaceToImageFilter->SetInput(objectSurface);
	surfaceToImageFilter->SetBackgroundValue(0);
	surfaceToImageFilter->SetReverseStencil(false);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();

	return convertedImage;
}

mitk::Image::Pointer lancet::Tha3DImageGenerator::GetThresholdedImage(mitk::Image::Pointer inputImage, int lowerBound, int upperBound)
{
	// Apply the thresholdFilter
	auto itkInputImage = ImageType::New();
	mitk::CastToItkImage(inputImage, itkInputImage);

	ThresholdFilterType::Pointer thFilter = ThresholdFilterType::New();
	thFilter->SetLowerThreshold(lowerBound);
	thFilter->SetUpperThreshold(upperBound);
	thFilter->SetInsideValue(1);
	thFilter->SetOutsideValue(0);
	thFilter->SetInput(itkInputImage);
	thFilter->UpdateLargestPossibleRegion();

	// auto itkBinaryImage = ImageType::New();
	// itkBinaryImage = thFilter->GetOutput()->Clone();
	
	// Apply the multiplyFilter
	MultiplyFilterType::Pointer multFilter = MultiplyFilterType::New();
	multFilter->SetInput1(itkInputImage);
	multFilter->SetInput2(thFilter->GetOutput());
	multFilter->UpdateLargestPossibleRegion();

	mitk::Image::Pointer outputImage = mitk::Image::New();
	outputImage = mitk::ImportItkImage(multFilter->GetOutput())->Clone();
	return outputImage;
}

mitk::Image::Pointer lancet::Tha3DImageGenerator::AddImage(mitk::Image::Pointer largerImage, mitk::Image::Pointer smallerImage)
{
	// Resample the smaller image to the same size as the input image
	itk::BSplineInterpolateImageFunction<DoubleImageType, double>::Pointer bspl_interpolator
		= itk::BSplineInterpolateImageFunction<DoubleImageType, double>::New();
	bspl_interpolator->SetSplineOrder(3);

	DoubleImageType::Pointer itkLargerImage = DoubleImageType::New();
	DoubleImageType::Pointer itkSmallerImage = DoubleImageType::New();

	mitk::CastToItkImage(largerImage, itkLargerImage);
	mitk::CastToItkImage(smallerImage, itkSmallerImage);

	itk::ResampleImageFilter< DoubleImageType, DoubleImageType >::Pointer resampleFilter = itk::ResampleImageFilter< DoubleImageType, DoubleImageType >::New();
	resampleFilter->SetInput(itkSmallerImage);
	resampleFilter->SetReferenceImage(itkLargerImage);
	resampleFilter->SetUseReferenceImage(true);

	resampleFilter->SetInterpolator(bspl_interpolator);

	resampleFilter->SetDefaultPixelValue(0);

	try
	{
		resampleFilter->UpdateLargestPossibleRegion();
	}
	catch (const itk::ExceptionObject & e)
	{
		MITK_WARN << "Updating resampling filter failed. ";
		MITK_WARN << "REASON: " << e.what();
	}

	DoubleImageType::Pointer itkResampledImage = resampleFilter->GetOutput();

	// Apply the addFilter
	AddFilterType::Pointer addFilter = AddFilterType::New();
	addFilter->SetInput1(resampleFilter->GetOutput());
	addFilter->SetInput2(itkLargerImage);
	addFilter->UpdateLargestPossibleRegion();

	mitk::Image::Pointer outputImage = mitk::Image::New();
	outputImage = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	return outputImage;
}

mitk::Image::Pointer lancet::Tha3DImageGenerator::AddSameSizeImage(mitk::Image::Pointer image_0, mitk::Image::Pointer image_1)
{
	DoubleImageType::Pointer itkImage_0 = DoubleImageType::New();
	DoubleImageType::Pointer itkImage_1 = DoubleImageType::New();

	mitk::CastToItkImage(image_0, itkImage_0);
	mitk::CastToItkImage(image_1, itkImage_1);

	// Apply the addFilter
	AddFilterType::Pointer addFilter = AddFilterType::New();
	addFilter->SetInput1(itkImage_0);
	addFilter->SetInput2(itkImage_1);
	addFilter->UpdateLargestPossibleRegion();

	mitk::Image::Pointer outputImage = mitk::Image::New();
	outputImage = mitk::ImportItkImage(addFilter->GetOutput())->Clone();

	return outputImage;
}


bool lancet::Tha3DImageGenerator::GetImapntsAvailablity()
{
	if(m_BallHeadSurface->GetVtkPolyData() != nullptr &&
		m_StemSurface->GetVtkPolyData() != nullptr &&
		m_CupSurface->GetVtkPolyData() != nullptr &&
		m_LinerSurface->GetVtkPolyData() != nullptr)
	{
		return true;
	}
	
	return false;
	
}

bool lancet::Tha3DImageGenerator::GetBoneAvailablity()
{
	if(m_PelvisImage->GetVtkImageData() != nullptr && 
		m_FemurImage_R->GetVtkImageData() != nullptr &&
		m_FemurImage_L->GetVtkImageData() != nullptr)
	{
		return true;
	}

	return true;
}

mitk::Image::Pointer lancet::Tha3DImageGenerator::Generate3Dimage()
{
	
	if(GetBoneAvailablity())
	{
		// pelvisImage + femurImage
		auto thres_pelvisImage = GetThresholdedImage(m_PelvisImage, -200, 5000);
		auto thres_rightFemurImage = GetThresholdedImage(m_FemurImage_R, -200, 5000);
		auto thres_leftFemurImage = GetThresholdedImage(m_FemurImage_L, -200, 5000);

		auto pelvisAndRightFemur = AddImage(thres_pelvisImage, thres_rightFemurImage);
		auto pelvisAndFemurs = AddImage(pelvisAndRightFemur, thres_leftFemurImage);
		
		if(GetImapntsAvailablity())
		{
			auto cupImage = GetStenciledImage(m_CupSurface, 2500);
			auto linerImage = GetStenciledImage(m_LinerSurface, 2000);
			auto stemImage = GetStenciledImage(m_StemSurface, 2500);
			auto ballHeadImage = GetStenciledImage(m_BallHeadSurface, 2500);
		
			auto addedCup = AddImage(pelvisAndFemurs,cupImage);
			auto addedLiner = AddImage(addedCup, linerImage);
			auto addedStem = AddImage(addedLiner, stemImage);
			auto addedballHead = AddImage(addedStem, ballHeadImage);
		
			return addedballHead;
		
		}else
		{
			return pelvisAndFemurs;
		}

	}
		return  m_PelvisImage;

	


}

