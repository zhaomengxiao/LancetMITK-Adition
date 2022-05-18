/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "drr.h"

#include "itkCenteredEuler3DTransform.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRayCastInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "mitkImageAccessByItk.h"
#include "mitkImageCast.h"
#include "mitkITKImageImport.h"
#include <itkShiftScaleImageFilter.h>
#include <mitkImageToItk.h>

template <typename TPixel, unsigned VDimension>
void DrrFilter::ItkImageProcessing(const itk::Image<TPixel, VDimension> *itkImage)
{
    // Software Guide : BeginLatex
    //
    // Although we generate a 2D projection of the 3D volume for the
    // purposes of the interpolator both images must be three dimensional.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet

    typedef itk::Image<TPixel, VDimension> InputImageType;
    typedef itk::Image<TPixel, VDimension> OutputImageType;

    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // For the purposes of this example we assume the input volume has
    // been loaded into an \code{itk::Image image}.
    //
    // Software Guide : EndLatex

    // Print out the details of the input volume

    if (m_verbose)
    {
        unsigned int i;
        const typename InputImageType::SpacingType spacing = itkImage->GetSpacing();
        std::cout << std::endl << "Input ";

        typename InputImageType::RegionType region = itkImage->GetBufferedRegion();
        region.Print(std::cout);

        std::cout << "  Resolution: [";
        for (i = 0; i < VDimension; i++)
        {
            std::cout << spacing[i];
            if (i < VDimension - 1)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;

        const typename InputImageType::PointType origin = itkImage->GetOrigin();
        std::cout << "  Origin: [";
        for (i = 0; i < VDimension; i++)
        {
            std::cout << origin[i];
            if (i < VDimension - 1)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl << std::endl;
    }

    // Software Guide : BeginLatex
    //
    // Creation of a \code{ResampleImageFilter} enables coordinates for
    // each of the pixels in the DRR image to be generated. These
    // coordinates are used by the \code{RayCastInterpolateImageFunction}
    // to determine the equation of each corresponding ray which is cast
    // through the input volume.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    typedef itk::ResampleImageFilter<InputImageType, InputImageType> FilterType;

    typename FilterType::Pointer filter = FilterType::New();

    filter->SetInput(itkImage);
    filter->SetDefaultPixelValue(0);
    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // An Euler transformation is defined to position the input volume.
    // The \code{ResampleImageFilter} uses this transform to position the
    // output DRR image for the desired view.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    typedef itk::CenteredEuler3DTransform<double> TransformType;

    TransformType::Pointer transform = TransformType::New();

    transform->SetComputeZYX(true);

    TransformType::OutputVectorType translation;

    translation[0] = m_tx;
    translation[1] = m_ty;
    translation[2] = m_tz;

    // constant for converting degrees into radians
    const double dtr = (std::atan(1.0) * 4.0) / 180.0;

    transform->SetTranslation(translation);
    transform->SetRotation(dtr * m_rx, dtr * m_ry, dtr * m_rz);

    typename InputImageType::PointType imOrigin = itkImage->GetOrigin();
    typename InputImageType::SpacingType imRes = itkImage->GetSpacing();

    typedef typename InputImageType::RegionType InputImageRegionType;
    typedef typename InputImageRegionType::SizeType InputImageSizeType;

    InputImageRegionType imRegion = itkImage->GetBufferedRegion();
    InputImageSizeType imSize = imRegion.GetSize();

    imOrigin[0] += imRes[0] * static_cast<double>(imSize[0]) / 2.0;
    imOrigin[1] += imRes[1] * static_cast<double>(imSize[1]) / 2.0;
    imOrigin[2] += imRes[2] * static_cast<double>(imSize[2]) / 2.0;

    TransformType::InputPointType center;
    center[0] = m_cx + imOrigin[0];
    center[1] = m_cy + imOrigin[1];
    center[2] = m_cz + imOrigin[2];

    transform->SetCenter(center);

    if (m_verbose)
    {
        std::cout << "Image size: "
            << imSize[0] << ", " << imSize[1] << ", " << imSize[2]
            << std::endl << "   resolution: "
            << imRes[0] << ", " << imRes[1] << ", " << imRes[2]
            << std::endl << "   origin: "
            << imOrigin[0] << ", " << imOrigin[1] << ", " << imOrigin[2]
            << std::endl << "   center: "
            << center[0] << ", " << center[1] << ", " << center[2]
            << std::endl << "Transform: " << transform << std::endl;
    }
    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // The \code{RayCastInterpolateImageFunction} is instantiated and passed the transform
    // object. The \code{RayCastInterpolateImageFunction} uses this
    // transform to reposition the x-ray source such that the DRR image
    // and x-ray source move as one around the input volume. This coupling
    // mimics the rigid geometry of the x-ray gantry.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    typedef itk::RayCastInterpolateImageFunction<InputImageType, double>
        InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetTransform(transform);
    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // We can then specify a threshold above which the volume's
    // intensities will be integrated.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    interpolator->SetThreshold(m_threshold);
    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // The ray-cast interpolator needs to know the initial position of the
    // ray source or focal point. In this example we place the input
    // volume at the origin and halfway between the ray source and the
    // screen. The distance between the ray source and the screen
    // is the "source to image distance" \code{sid} and is specified by
    // the user.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    typename InterpolatorType::InputPointType focalpoint;

    focalpoint[0] = imOrigin[0];
    focalpoint[1] = imOrigin[1];
    focalpoint[2] = imOrigin[2] - 1024 / 2.;

    interpolator->SetFocalPoint(focalpoint);
    // Software Guide : EndCodeSnippet

    if (m_verbose)
    {
        std::cout << "Focal Point: "
            << focalpoint[0] << ", "
            << focalpoint[1] << ", "
            << focalpoint[2] << std::endl;
    }

    // Software Guide : BeginLatex
    //
    // Having initialised the interpolator we pass the object to the
    // resample filter.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet
    interpolator->Print(std::cout);

    filter->SetInterpolator(interpolator);
    filter->SetTransform(transform);
    // Software Guide : EndCodeSnippet

    // Software Guide : BeginLatex
    //
    // The size and resolution of the output DRR image is specified via the
    // resample filter.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet

    // setup the scene
    typename InputImageType::SizeType size;

    size[0] = m_dx; // number of pixels along X of the 2D DRR image
    size[1] = m_dy; // number of pixels along Y of the 2D DRR image
    size[2] = 1;  // only one slice

    filter->SetSize(size);

    typename InputImageType::SpacingType spacing;

    spacing[0] = m_sx;  // pixel spacing along X of the 2D DRR image [mm]
    spacing[1] = m_sy;  // pixel spacing along Y of the 2D DRR image [mm]
    spacing[2] = 1.0; // slice thickness of the 2D DRR image [mm]

    filter->SetOutputSpacing(spacing);

    // Software Guide : EndCodeSnippet

    if (m_verbose)
    {
        std::cout << "Output image size: "
            << size[0] << ", "
            << size[1] << ", "
            << size[2] << std::endl;

        std::cout << "Output image spacing: "
            << spacing[0] << ", "
            << spacing[1] << ", "
            << spacing[2] << std::endl;
    }

    // Software Guide : BeginLatex
    //
    // In addition the position of the DRR is specified. The default
    // position of the input volume, prior to its transformation is
    // half-way between the ray source and screen and unless specified
    // otherwise the normal from the "screen" to the ray source passes
    // directly through the centre of the DRR.
    //
    // Software Guide : EndLatex

    // Software Guide : BeginCodeSnippet

    double origin[VDimension];

    origin[0] = imOrigin[0] + m_o2Dx - m_sx * ((double)m_dx - 1.) / 2.;
    origin[1] = imOrigin[1] + m_o2Dy - m_sy * ((double)m_dy - 1.) / 2.;
    origin[2] = imOrigin[2] + m_sid / 2.;

    filter->SetOutputOrigin(origin);
    // Software Guide : EndCodeSnippet

    if (m_verbose)
    {
        std::cout << "Output image origin: "
            << origin[0] << ", "
            << origin[1] << ", "
            << origin[2] << std::endl;
    }


    filter->Update();

    // typedef itk::RescaleIntensityImageFilter<
    //     InputImageType, InputImageType> RescaleFilterType;
    // typename RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    // rescaler->SetOutputMinimum(0);
    // rescaler->SetOutputMaximum(255);
    // rescaler->SetInput(filter->GetOutput());
    // rescaler->Update();

    // get  Pointer to output image
    mitk::Image::Pointer resultImage = this->GetOutput();
    // write into output image
    mitk::CastToMitkImage(filter->GetOutput(), resultImage);
    //m_out = mitk::ImportItkImage(rescaler->GetOutput())->Clone();
}

DrrFilter::DrrFilter() = default;

DrrFilter::~DrrFilter()= default;

void DrrFilter::GenerateOutputInformation()
{
    mitk::Image::Pointer inputImage = (mitk::Image*)this->GetInput();
    mitk::Image::Pointer output = this->GetOutput();
    itkDebugMacro(<< "GenerateOutputInformation()");
    if (inputImage.IsNull())
        return;
}

void DrrFilter::GenerateData()
{
    mitk::Image::ConstPointer inputImage = this->GetInput(0);
    if (inputImage->GetDimension() != 3)
    {
        MITK_ERROR << "DrrFilter:GenerateData works only with 3D images, sorry.";
        itkExceptionMacro("DrrFilter:GenerateData works only with 3D images, sorry.");
        return;
    }
    AccessFixedDimensionByItk(inputImage.GetPointer(), ItkImageProcessing, 3);
}

void DrrFilter::SetObjTranslate(double tx, double ty, double tz)
{
    m_tx = tx;
    m_ty = ty;
    m_tz = tz;
}

void DrrFilter::SetObjRotate(double rx, double ry, double rz)
{
    m_rx = rx;
    m_ry = ry;
    m_rz = rz;

}

