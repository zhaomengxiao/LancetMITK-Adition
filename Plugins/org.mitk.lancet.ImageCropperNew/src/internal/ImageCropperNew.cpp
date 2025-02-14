/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "ImageCropperNew.h"

// Qt
#include <QMessageBox>
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"

// mitk image
#include <mitkImage.h>
#include "mitkGizmo_noscale.h"
#include <usModuleRegistry.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateProperty.h>
#include <mitkNodePredicateFunction.h>

// vtk
#include <vtkExtractVOI.h>
#include <vtkCubeSource.h>

// others
#include <itkResampleImageFilter.h>
#include <tuple>
#include <vtkCellData.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkImageCast.h"
#include "mitkImageWriteAccessor.h"
#include "mitkMatrixConvert.h"

const std::string ImageCropperNew::VIEW_ID = "org.mitk.views.imagecroppernew";
ImageCropperNew::ImageCropperNew(QObject*)
	: m_ParentWidget(nullptr)
	, m_MoveObjectInteractor(nullptr)
	, m_CropOutsideValue(0)
{
	CreateBoundingShapeInteractor();
}

ImageCropperNew::~ImageCropperNew()
{
	//disable interactor
	if (m_MoveObjectInteractor != nullptr)
	{
		m_MoveObjectInteractor->SetDataNode(nullptr);
		m_MoveObjectInteractor->EnableInteraction(false);
	}
}
void ImageCropperNew::SetFocus()
{
}

void ImageCropperNew::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  m_Controls.imageSelectionWidget->SetDataStorage(GetDataStorage());
  m_Controls.imageSelectionWidget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object")));
  m_Controls.imageSelectionWidget->SetSelectionIsOptional(true);
  m_Controls.imageSelectionWidget->SetAutoSelectNewNodes(true);
  m_Controls.imageSelectionWidget->SetEmptyInfo(QString("Please select an image node"));
  m_Controls.imageSelectionWidget->SetPopUpTitel(QString("Select image node"));
  InitSurfaceSelector(m_Controls.surfaceSelectionWidget);

  //connect(m_Controls.surfaceSelectionWidget, &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged, this, &ImageCropperNew::SurfaceIcpSourceChanged);
  connect(m_Controls.pushButton_FastImageCrop, &QPushButton::clicked, this, &ImageCropperNew::on_pushButton_ImageCropper_clicked);
  connect(m_Controls.buttonCreateNewBoundingBox, &QPushButton::clicked, this, &ImageCropperNew::on_pushButton_ImageCropper_showboundingshape_clicked);
  connect(m_Controls.pushButton_resample, &QPushButton::clicked, this, &ImageCropperNew::on_pushButton_resample_clicked);
  connect(m_Controls.pushButton_resample_type2, &QPushButton::clicked, this, &ImageCropperNew::on_pushButton_resample_type2_clicked);

}

bool ImageCropperNew::GetHardenedImage(mitk::Image::Pointer inputMitkImage, mitk::Image::Pointer outputMitkImage)
{
	/* In general, an affine transform can be described as Y = M * X + T, where T is translation,
	 * M = R * S * H, R is rotation, S is pure diagonal scaling, H is shearing
	 * R and S are commutative, S and H are not commutative
	 * Since we rarely handle image with shearing in MITK, we will neglect shearing in the following to simplify the code
	 * Only 3D images are accepted by this function while 2D images are not accepted 
	 */

	if(inputMitkImage->GetDimension() < 2)
	{
		return false;
	}

	//-------0. Obtain the scaling and the rotation part of the input image geometry---------
	auto inputGeo = inputMitkImage->GetGeometry();
	auto inputVtkMatrix = inputGeo->GetVtkMatrix();
	auto inputSpacing = inputGeo->GetSpacing();
	auto inputImageDim = inputMitkImage->GetDimensions();
	auto inputOffset = inputGeo->GetIndexToWorldTransform()->GetOffset();

	typedef itk::Image<short, 3> ITKImageType; // requires further extension to process different pixType
	auto inputItkImage = ITKImageType::New();
	mitk::CastToItkImage(inputMitkImage, inputItkImage);

	auto vtkScaleMatrix = vtkMatrix4x4::New();
	vtkScaleMatrix->Identity();
	vtkScaleMatrix->SetElement(0, 0, inputSpacing[0]);
	vtkScaleMatrix->SetElement(1, 1, inputSpacing[1]);
	vtkScaleMatrix->SetElement(2, 2, inputSpacing[2]);

	auto vtkScaleMatrixInverse = vtkMatrix4x4::New();
	vtkScaleMatrixInverse->Identity();
	vtkScaleMatrixInverse->SetElement(0, 0, 1 / inputSpacing[0]);
	vtkScaleMatrixInverse->SetElement(1, 1, 1 / inputSpacing[1]);
	vtkScaleMatrixInverse->SetElement(2, 2, 1 / inputSpacing[2]);

	auto vtkRotTrans = vtkTransform::New();
	vtkRotTrans->PreMultiply();
	vtkRotTrans->Identity();
	vtkRotTrans->SetMatrix(inputVtkMatrix);
	vtkRotTrans->Concatenate(vtkScaleMatrixInverse);
	vtkRotTrans->Update();

	// the matrix of Rotation with translation
	auto vtkRotMatrix = vtkRotTrans->GetMatrix();

	//-------1. Make a copy of the inputImage and remove its rotation and translation -------------
	auto helperMitkImage = mitk::Image::New();
	mitk::CastToMitkImage(inputItkImage, helperMitkImage);

	helperMitkImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkScaleMatrix);
	helperMitkImage->Update();
	

	//-------2. Calculate the AABB bounds of the inputMitkImage in world space ----------------
	mitk::Point3D a0, a1, a2, a3, a4, a5, a6, a7, p0, p1, p2, p3, p4, p5, p6, p7;
	mitk::FillVector3D(a0, 0, 0, 0);
	mitk::FillVector3D(a1, inputImageDim[0], 0, 0);
	mitk::FillVector3D(a2, 0, inputImageDim[1], 0);
	mitk::FillVector3D(a3, 0, 0, inputImageDim[2]);
	mitk::FillVector3D(a4, inputImageDim[0], inputImageDim[1], 0);
	mitk::FillVector3D(a5, inputImageDim[0], 0, inputImageDim[2]);
	mitk::FillVector3D(a6, 0, inputImageDim[1], inputImageDim[2]);
	mitk::FillVector3D(a7, inputImageDim[0], inputImageDim[1], inputImageDim[2]);

	inputGeo->IndexToWorld(a0, p0);
	inputGeo->IndexToWorld(a1, p1);
	inputGeo->IndexToWorld(a2, p2);
	inputGeo->IndexToWorld(a3, p3);
	inputGeo->IndexToWorld(a4, p4);
	inputGeo->IndexToWorld(a5, p5);
	inputGeo->IndexToWorld(a6, p6);
	inputGeo->IndexToWorld(a7, p7);

	double inputAABBLowerBounds[3]{ p0[0], p0[1], p0[2] };
	double inputAABBUpperBounds[3]{ p0[0], p0[1], p0[2] };

	for (int i{ 0 }; i < 3; i++)
	{
		if (inputAABBLowerBounds[i] > p0[i]) inputAABBLowerBounds[i] = p0[i];
		if (inputAABBLowerBounds[i] > p1[i]) inputAABBLowerBounds[i] = p1[i];
		if (inputAABBLowerBounds[i] > p2[i]) inputAABBLowerBounds[i] = p2[i];
		if (inputAABBLowerBounds[i] > p3[i]) inputAABBLowerBounds[i] = p3[i];
		if (inputAABBLowerBounds[i] > p4[i]) inputAABBLowerBounds[i] = p4[i];
		if (inputAABBLowerBounds[i] > p5[i]) inputAABBLowerBounds[i] = p5[i];
		if (inputAABBLowerBounds[i] > p6[i]) inputAABBLowerBounds[i] = p6[i];
		if (inputAABBLowerBounds[i] > p7[i]) inputAABBLowerBounds[i] = p7[i];

		if (inputAABBUpperBounds[i] < p0[i]) inputAABBUpperBounds[i] = p0[i];
		if (inputAABBUpperBounds[i] < p1[i]) inputAABBUpperBounds[i] = p1[i];
		if (inputAABBUpperBounds[i] < p2[i]) inputAABBUpperBounds[i] = p2[i];
		if (inputAABBUpperBounds[i] < p3[i]) inputAABBUpperBounds[i] = p3[i];
		if (inputAABBUpperBounds[i] < p4[i]) inputAABBUpperBounds[i] = p4[i];
		if (inputAABBUpperBounds[i] < p5[i]) inputAABBUpperBounds[i] = p5[i];
		if (inputAABBUpperBounds[i] < p6[i]) inputAABBUpperBounds[i] = p6[i];
		if (inputAABBUpperBounds[i] < p7[i]) inputAABBUpperBounds[i] = p7[i];
	}

	//--------3. Resample the itkImage contained in the helperMitkImage with the rotation and the translation
	auto helperItkImage = ITKImageType::New();
	mitk::CastToItkImage(helperMitkImage, helperItkImage);


	////////// Debug
	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(helperMitkImage);
	tmpNode->SetName("Debug");

	GetDataStorage()->Add(tmpNode, m_Controls.imageSelectionWidget->GetSelectedNode());
	// return true;
	////////// Debug

	typedef itk::AffineTransform<double, 3> TransformType;
	TransformType* itkTransform = TransformType::New();

	// itk::Vector<double, 3> axis;
	// axis[0] = 0;
	// axis[1] = 0;
	// axis[2] = 1;
	// itkTransform->Rotate3D(axis, 0 * itk::Math::pi / 180.0, false);

	mitk::TransferVtkMatrixToItkTransform(vtkRotMatrix, itkTransform);

	ITKImageType::SizeType outputSize;
	outputSize[0] = static_cast<int>(ceil((inputAABBUpperBounds[0] - inputAABBLowerBounds[0]) / inputSpacing[0]));
	outputSize[1] = static_cast<int>(ceil((inputAABBUpperBounds[1] - inputAABBLowerBounds[1]) / inputSpacing[1]));
	outputSize[2] = static_cast<int>(ceil((inputAABBUpperBounds[2] - inputAABBLowerBounds[2]) / inputSpacing[2]));
	
	double outputOrigin[3]{ inputAABBLowerBounds[0], inputAABBLowerBounds[1], inputAABBLowerBounds[2] };
	
	typedef itk::ResampleImageFilter<ITKImageType, ITKImageType> ResampleFilterType;
	ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
	
	resampleFilter->SetTransform(itkTransform);
	resampleFilter->SetInput(helperItkImage);
	// resampleFilter->SetSize(inputItkImage->GetLargestPossibleRegion().GetSize()); // Maintain the original size
	resampleFilter->SetSize(outputSize);
	resampleFilter->SetOutputSpacing(helperItkImage->GetSpacing());
	// resampleFilter->SetOutputOrigin(inputItkImage->GetOrigin());
	resampleFilter->SetOutputOrigin(outputOrigin);
	// resampleFilter->SetOutputDirection(inputItkImage->GetDirection());
	resampleFilter->SetOutputDirection(helperItkImage->GetDirection());
	resampleFilter->SetDefaultPixelValue(-1000);  // Background value
	resampleFilter->Update();
	
	mitk::CastToMitkImage(resampleFilter->GetOutput(), outputMitkImage);

	return true;

}

template <typename ITKImageType>
mitk::Image::Pointer ImageCropperNew::ResampleITKImage(typename ITKImageType::Pointer itkImage)
{
	// Step 1: Define the rotation transform (in this case, an affine transform)
	typedef itk::AffineTransform<double, ITKImageType::ImageDimension> TransformType;
	typename TransformType::Pointer transform = TransformType::New();

	// Step 2: Set up rotation
	// Rotate around the center of the image
	typename ITKImageType::PointType center;
	itk::Vector<double, 3> axis;
	axis[0] = 1;
	axis[1] = 0;
	axis[2] = 0;
	//itkImage->TransformIndexToPhysicalPoint(itkImage->GetLargestPossibleRegion().GetIndex(), center);
	//transform->Translate(-center);  // Move the origin to the center
	transform->Rotate3D(axis, 45 * itk::Math::pi / 180.0, false);  // Rotate around the center (2D rotation)
	// transform->Translate(center);  // Move the origin back to the original position

	// Step 3: Set up the resample filter
	typedef itk::ResampleImageFilter<ITKImageType, ITKImageType> ResampleFilterType;
	typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();

	resampleFilter->SetTransform(transform);
	resampleFilter->SetInput(itkImage);
	resampleFilter->SetSize(itkImage->GetLargestPossibleRegion().GetSize()); // Maintain the original size
	resampleFilter->SetOutputSpacing(itkImage->GetSpacing());
	resampleFilter->SetOutputOrigin(itkImage->GetOrigin());
	resampleFilter->SetOutputDirection(itkImage->GetDirection());
	resampleFilter->SetDefaultPixelValue(-1000);  // Background value

	// Step 4: Perform the resampling
	resampleFilter->Update();

	// Step 5: Convert the rotated ITK image back to an MITK image
	mitk::Image::Pointer mitkImage = mitk::Image::New();
	mitk::CastToMitkImage(resampleFilter->GetOutput(), mitkImage);

	return mitkImage;
}


void ImageCropperNew::on_pushButton_resample_type2_clicked()
{
	auto inputMitkImage = dynamic_cast<mitk::Image*>(m_Controls.imageSelectionWidget->GetSelectedNode()->GetData());

	typedef itk::Image<int, 3> ITKImageType;
	auto inputItkImage = ITKImageType::New();
	mitk::CastToItkImage(inputMitkImage, inputItkImage);

	// auto outputMitkImage = ResampleITKImage<ITKImageType>(inputItkImage);
	auto outputMitkImage = mitk::Image::New();
	GetHardenedImage(inputMitkImage, outputMitkImage);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(outputMitkImage);
	tmpNode->SetName("Resampled");

	GetDataStorage()->Add(tmpNode, m_Controls.imageSelectionWidget->GetSelectedNode());
}


void ImageCropperNew::on_pushButton_resample_clicked()
{
	// ------ Retrieve the spacing -----------
	double outputSpacing[3];
	outputSpacing[0] = m_Controls.lineEdit_x_step->text().toDouble();
	outputSpacing[1] = m_Controls.lineEdit_y_step->text().toDouble();
	outputSpacing[2] = m_Controls.lineEdit_z_step->text().toDouble();

	//------- Construct a blank mitk::Image that occupies the AABB of the input mitk::Image -------------
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.imageSelectionWidget->GetSelectedNode()->GetData());
	auto inputVtkImage = inputImage->GetVtkImageData();
	auto inputImageDim = inputImage->GetDimensions();
	auto inputGeometry = inputImage->GetGeometry();

	mitk::Point3D a0, a1, a2, a3, a4, a5, a6, a7, p0, p1, p2, p3, p4, p5, p6, p7;
	mitk::FillVector3D(a0, 0, 0, 0);
	mitk::FillVector3D(a1, inputImageDim[0], 0, 0);
	mitk::FillVector3D(a2, 0, inputImageDim[1], 0);
	mitk::FillVector3D(a3, 0, 0, inputImageDim[2]);
	mitk::FillVector3D(a4, inputImageDim[0], inputImageDim[1], 0);
	mitk::FillVector3D(a5, inputImageDim[0], 0, inputImageDim[2]);
	mitk::FillVector3D(a6, 0, inputImageDim[1], inputImageDim[2]);
	mitk::FillVector3D(a7, inputImageDim[0], inputImageDim[1], inputImageDim[2]);

	inputGeometry->IndexToWorld(a0, p0);
	inputGeometry->IndexToWorld(a1, p1);
	inputGeometry->IndexToWorld(a2, p2);
	inputGeometry->IndexToWorld(a3, p3);
	inputGeometry->IndexToWorld(a4, p4);
	inputGeometry->IndexToWorld(a5, p5);
	inputGeometry->IndexToWorld(a6, p6);
	inputGeometry->IndexToWorld(a7, p7);

	// MITK_INFO << p0;

	double outputLowerBounds[3]{ p0[0], p0[1], p0[2] };
	double outputUpperBounds[3]{ p0[0], p0[1], p0[2] };

	for (int i{ 0 }; i < 3; i++)
	{
		if (outputLowerBounds[i] > p0[i]) outputLowerBounds[i] = p0[i];
		if (outputLowerBounds[i] > p1[i]) outputLowerBounds[i] = p1[i];
		if (outputLowerBounds[i] > p2[i]) outputLowerBounds[i] = p2[i];
		if (outputLowerBounds[i] > p3[i]) outputLowerBounds[i] = p3[i];
		if (outputLowerBounds[i] > p4[i]) outputLowerBounds[i] = p4[i];
		if (outputLowerBounds[i] > p5[i]) outputLowerBounds[i] = p5[i];
		if (outputLowerBounds[i] > p6[i]) outputLowerBounds[i] = p6[i];
		if (outputLowerBounds[i] > p7[i]) outputLowerBounds[i] = p7[i];

		if (outputUpperBounds[i] < p0[i]) outputUpperBounds[i] = p0[i];
		if (outputUpperBounds[i] < p1[i]) outputUpperBounds[i] = p1[i];
		if (outputUpperBounds[i] < p2[i]) outputUpperBounds[i] = p2[i];
		if (outputUpperBounds[i] < p3[i]) outputUpperBounds[i] = p3[i];
		if (outputUpperBounds[i] < p4[i]) outputUpperBounds[i] = p4[i];
		if (outputUpperBounds[i] < p5[i]) outputUpperBounds[i] = p5[i];
		if (outputUpperBounds[i] < p6[i]) outputUpperBounds[i] = p6[i];
		if (outputUpperBounds[i] < p7[i]) outputUpperBounds[i] = p7[i];
	}

	int outputDim[3];
	for (int i = 0; i < 3; i++)
	{
		outputDim[i] = static_cast<int>(ceil((outputUpperBounds[i] - outputLowerBounds[i]) / outputSpacing[i]));
	}

	auto imageData = vtkImageData::New();
	imageData->SetDimensions(outputDim);
	imageData->SetSpacing(outputSpacing);

	imageData->AllocateScalars(VTK_INT, 1);  // 1 component (scalar value)

	int* data = static_cast<int*>(imageData->GetScalarPointer());
	int numVoxels = outputDim[0] * outputDim[1] * outputDim[2];

	std::fill_n(data, numVoxels, m_Controls.lineEdit_fringeValue->text().toInt());

	imageData->SetOrigin(outputLowerBounds);

	auto mitkImage = mitk::Image::New();

	mitkImage->Initialize(imageData);

	auto outputGeo = mitkImage->GetGeometry();

	// Resampling
	for(int z{0}; z < outputDim[2]; z++)
	{
		for (int y{ 0 }; y < outputDim[1]; y++)
		{
			for (int x{ 0 }; x < outputDim[0]; x++)
			{
				mitk::Point3D tmpPoint, worldPoint, inputPoint;
				mitk::FillVector3D(tmpPoint, x, y, z);
				outputGeo->IndexToWorld(tmpPoint, worldPoint);
				inputGeometry->WorldToIndex(worldPoint, inputPoint);

				if (inputPoint[0] < 0) continue;
				if (inputPoint[0] >= inputImageDim[0]) continue;
				if (inputPoint[1] < 0) continue;
				if (inputPoint[1] >= inputImageDim[1]) continue;
				if (inputPoint[2] < 0) continue;
				if (inputPoint[2] >= inputImageDim[2]) continue;

				int* voxel = static_cast<int*>(imageData->GetScalarPointer(x, y, z));
				int* voxel_target = static_cast<int*>(inputVtkImage->GetScalarPointer(inputPoint[0], inputPoint[1], inputPoint[2]));
				voxel[0] = voxel_target[0];
			}
		}
	}

	mitkImage->SetVolume(imageData->GetScalarPointer());

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(mitkImage);
	tmpNode->SetName("resampled");

	GetDataStorage()->Add(tmpNode, m_Controls.imageSelectionWidget->GetSelectedNode());
}


void ImageCropperNew::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}


void ImageCropperNew::TurnOffAllNodesVisibility()
{
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}

	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);
}

void ImageCropperNew::ResetView()
{
	auto iRenderWindowPart = GetRenderWindowPart();
	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	renderWindow->ResetView();
}


std::tuple<std::array<int, 6>, mitk::Point3D, mitk::Point3D> ImageCropperNew::CalculateOverlapImageBound(const mitk::Image::Pointer& image, const mitk::Surface::Pointer& surface)
{
	auto boundingBox = surface->GetGeometry()->CalculateBoundingBoxRelativeToTransform(nullptr);
	mitk::BoundingBox::BoundsArrayType bounds = boundingBox->GetBounds();

	mitk::Image::IndexType startIndex, endIndex;
	mitk::Point3D minPoint, maxPoint;

	mitk::FillVector3D(minPoint, bounds[0], bounds[2], bounds[4]);
	mitk::FillVector3D(maxPoint, bounds[1], bounds[3], bounds[5]);

	// Corner pts of the bounding box
	mitk::Point3D p0,p1,p2,p3,p4,p5,p6,p7;
	mitk::FillVector3D(p0, bounds[0], bounds[2], bounds[4]);
	mitk::FillVector3D(p1, bounds[0], bounds[2], bounds[5]);
	mitk::FillVector3D(p2, bounds[0], bounds[3], bounds[4]);
	mitk::FillVector3D(p3, bounds[1], bounds[2], bounds[4]);
	mitk::FillVector3D(p4, bounds[0], bounds[3], bounds[5]);
	mitk::FillVector3D(p5, bounds[1], bounds[2], bounds[5]);
	mitk::FillVector3D(p6, bounds[1], bounds[3], bounds[4]);
	mitk::FillVector3D(p7, bounds[1], bounds[3], bounds[5]);

	// Convert the corner pts into the image index space
	mitk::Image::IndexType i0, i1, i2, i3, i4, i5, i6, i7, minIndex, maxIndex;
	image->GetGeometry()->WorldToIndex(p0, minIndex);
	image->GetGeometry()->WorldToIndex(p1, maxIndex);
	image->GetGeometry()->WorldToIndex(p0, i0);
	image->GetGeometry()->WorldToIndex(p1, i1);
	image->GetGeometry()->WorldToIndex(p2, i2);
	image->GetGeometry()->WorldToIndex(p3, i3);
	image->GetGeometry()->WorldToIndex(p4, i4);
	image->GetGeometry()->WorldToIndex(p5, i5);
	image->GetGeometry()->WorldToIndex(p6, i6);
	image->GetGeometry()->WorldToIndex(p7, i7);

	// Construct an AABB in the index space
	for(int i{0}; i < 3; i++)
	{
		if (minIndex[i] > i0[i]) minIndex[i] = i0[i];
		if (minIndex[i] > i1[i]) minIndex[i] = i1[i];
		if (minIndex[i] > i2[i]) minIndex[i] = i2[i];
		if (minIndex[i] > i3[i]) minIndex[i] = i3[i];
		if (minIndex[i] > i4[i]) minIndex[i] = i4[i];
		if (minIndex[i] > i5[i]) minIndex[i] = i5[i];
		if (minIndex[i] > i6[i]) minIndex[i] = i6[i];
		if (minIndex[i] > i7[i]) minIndex[i] = i7[i];

		if (maxIndex[i] < i0[i]) maxIndex[i] = i0[i];
		if (maxIndex[i] < i1[i]) maxIndex[i] = i1[i];
		if (maxIndex[i] < i2[i]) maxIndex[i] = i2[i];
		if (maxIndex[i] < i3[i]) maxIndex[i] = i3[i];
		if (maxIndex[i] < i4[i]) maxIndex[i] = i4[i];
		if (maxIndex[i] < i5[i]) maxIndex[i] = i5[i];
		if (maxIndex[i] < i6[i]) maxIndex[i] = i6[i];
		if (maxIndex[i] < i7[i]) maxIndex[i] = i7[i];

		if (minIndex[i] < 0) minIndex[i] = 0;
		if (maxIndex[i] >= image->GetDimension(i)) maxIndex[i] = image->GetDimension(i) - 1;
	}

	// Convert the minIndex and maxIndex pts into the world space
	mitk::Point3D minPoint_convert, maxPoint_convert;
	image->GetGeometry()->IndexToWorld(minIndex,minPoint_convert);
	image->GetGeometry()->IndexToWorld(maxIndex, maxPoint_convert);

	int Xmin = static_cast<int>(minIndex[0]);
	int Ymin = static_cast<int>(minIndex[1]);
	int Zmin = static_cast<int>(minIndex[2]);
	int Xmax = static_cast<int>(maxIndex[0]);
	int Ymax = static_cast<int>(maxIndex[1]);
	int Zmax = static_cast<int>(maxIndex[2]);

	return std::make_tuple(std::array<int, 6>{Xmin, Xmax, Ymin, Ymax, Zmin, Zmax}, minPoint_convert, maxPoint_convert);

}

mitk::Image::Pointer ImageCropperNew::ConvertVtkToMitk(vtkImageData* vtkImage)
{
	auto vtkImageOrigin = vtkImage->GetOrigin();

	auto image_node = m_Controls.imageSelectionWidget->GetSelectedNode();
	auto image_clip = dynamic_cast<mitk::Image*>(image_node->GetData());

	auto mitkImageGeo = image_clip->GetGeometry();

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->DeepCopy(mitkImageGeo->GetVtkMatrix());
	tmpMatrix->SetElement(0, 3, vtkImageOrigin[0]);
	tmpMatrix->SetElement(1, 3, vtkImageOrigin[1]);
	tmpMatrix->SetElement(2, 3, vtkImageOrigin[2]);

	mitk::Image::Pointer mitkImage = mitk::Image::New();
	mitkImage->Initialize(vtkImage);
	mitkImage->SetVolume(vtkImage->GetScalarPointer());
	mitkImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);
	return mitkImage;
}

vtkSmartPointer<vtkImageData> ImageCropperNew::ExtractImageRegionByBound(vtkSmartPointer<vtkImageData>& inputImage, int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, const mitk::Point3D minPoint, const mitk::Point3D maxPoint)
{

	vtkSmartPointer<vtkExtractVOI> extractVOI = vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputData(inputImage);
	extractVOI->SetVOI(xmin, xmax, ymin, ymax, zmin, zmax);
	extractVOI->Update();
	vtkSmartPointer<vtkImageData> extractedImage = extractVOI->GetOutput();

	// The mitk::Image's spacing is not necessarily stored in the corresponding vtkImageData, there's a latency 
	auto image_node = m_Controls.imageSelectionWidget->GetSelectedNode();
	auto image_clip = dynamic_cast<mitk::Image*>(image_node->GetData());
	auto spacing_ = image_clip->GetGeometry()->GetSpacing();

	double spacing[3]{spacing_[0],spacing_[1],spacing_[2]};

	double newOrigin[3];
	newOrigin[0] = minPoint[0];
	newOrigin[1] = minPoint[1];
	newOrigin[2] = minPoint[2];

	// Match the fix point
	extractedImage->SetOrigin(newOrigin);
	extractedImage->SetSpacing(spacing);
	return extractedImage;
}

void ImageCropperNew::on_pushButton_ImageCropper_clicked() {
	if (GetDataStorage()->GetNamedNode("Crop Box") == nullptr)
	{
		//m_Controls.textBrowser->append("bounding shape cube is missing");
		return;
	}
	auto image_node = m_Controls.imageSelectionWidget->GetSelectedNode();

	auto surface_node = GetDataStorage()->GetNamedNode("Crop Box");

	auto surface_clip = dynamic_cast<mitk::Surface*>(surface_node->GetData());
	auto image_clip = dynamic_cast<mitk::Image*>(image_node->GetData());

	auto [bound, minPoint, maxPoint] = CalculateOverlapImageBound(image_clip, surface_clip);

	vtkSmartPointer<vtkImageData> vtkImageToReam = image_clip->GetVtkImageData();
	vtkSmartPointer<vtkImageData> croppedImage = ExtractImageRegionByBound(vtkImageToReam,
		bound[0], bound[1], bound[2], bound[3], bound[4], bound[5], minPoint, maxPoint);

	mitk::Image::Pointer mitkCroppedImage = ConvertVtkToMitk(croppedImage);

	mitk::DataNode::Pointer imageNode = mitk::DataNode::New();
	imageNode->SetData(mitkCroppedImage);
	imageNode->SetName(image_node->GetName() + " Bounding Shape_cropped");
	GetDataStorage()->Add(imageNode);
	TurnOffAllNodesVisibility();
	auto attemptNode = GetDataStorage()->GetNamedNode(image_node->GetName() + " Bounding Shape_cropped");
	attemptNode->SetVisibility(true);
	ResetView();
}

void ImageCropperNew::on_pushButton_ImageCropper_showboundingshape_clicked() {
	// Generat Surface for Clip
	CreateMoveableBoundingBox();
	on_pushButton_AddInteractor_clicked();

	auto surface_node = GetDataStorage()->GetNamedNode("Crop Box");
	auto image_node = m_Controls.imageSelectionWidget->GetSelectedNode();

	auto surface_clip = dynamic_cast<mitk::Surface*>(surface_node->GetData());
	auto image_clip = dynamic_cast<mitk::Image*>(image_node->GetData());

	// Add Gizmo to Surface Node and disable move and rotate
	if (mitk::Gizmo_noscale::HasGizmo_noscaleAttached(surface_node, GetDataStorage()) == 0) {
		mitk::Gizmo_noscale::AddGizmo_noscaleToNode(surface_node, GetDataStorage(), false, false, true);
	}
	//else {
	//	mitk::Gizmo_noscale::RemoveGizmo_noscaleFromNode(surface_node, GetDataStorage());
	//}
}

// New Interaction Test
void ImageCropperNew::CreateBoundingShapeInteractor()
{
	if (m_MoveObjectInteractor.IsNull())
	{
		m_MoveObjectInteractor = mitk::MoveObjectInteractor::New();
		m_MoveObjectInteractor->LoadStateMachine("MoveObjectInteraction.xml", us::ModuleRegistry::GetModule("MitkMoveObject"));
		m_MoveObjectInteractor->SetEventConfig("MoveObjectMouseConfig.xml", us::ModuleRegistry::GetModule("MitkMoveObject"));
	}
	// m_BoundingShapeInteractor->SetRotationEnabled(rotationEnabled);
}

void ImageCropperNew::CreateMoveableBoundingBox() {
	if (GetDataStorage()->GetNamedNode("Crop Box") != nullptr) {
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Crop Box"));
	}

	auto image_node = m_Controls.imageSelectionWidget->GetSelectedNode();
	auto image = dynamic_cast<mitk::Image*>(image_node->GetData());

	auto geometry = image->GetGeometry();
	mitk::Point3D origin = geometry->GetOrigin(); 
	mitk::Vector3D spacing = geometry->GetSpacing(); 
	mitk::Vector3D extent = geometry->GetExtentInMM(0);
	mitk::BoundingBox::BoundsArrayType bounds = geometry->GetBounds();

	mitk::Image::IndexType startIndex, endIndex;
	mitk::Point3D minPoint, maxPoint;

	mitk::FillVector3D(minPoint, bounds[0], bounds[2], bounds[4]);
	mitk::FillVector3D(maxPoint, bounds[1], bounds[3], bounds[5]);

	double halfSize[3];
	halfSize[0] = extent[0] / 1.0; 
	halfSize[1] = extent[1] / 1.0; 
	halfSize[2] = extent[2] / 1.0; 

	mitk::Point3D center = geometry->GetCenter(); 

	vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource->SetXLength(halfSize[0]); 
	cubeSource->SetYLength(halfSize[1]); 
	cubeSource->SetZLength(halfSize[2]); 
	cubeSource->SetCenter(center[0], center[1], center[2]); 
	cubeSource->Update();

	mitk::Surface::Pointer surface = mitk::Surface::New();
	surface->SetVtkPolyData(cubeSource->GetOutput());

	mitk::DataNode::Pointer cubeNode = mitk::DataNode::New();
	cubeNode->SetData(surface);
	cubeNode->SetName("Crop Box"); 

	cubeNode->SetProperty("opacity", mitk::FloatProperty::New(0.5)); 

	GetDataStorage()->Add(cubeNode);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void ImageCropperNew::on_pushButton_AddInteractor_clicked() {

	auto MoveableNode = GetDataStorage()->GetNamedNode("Crop Box");

	m_MoveObjectInteractor->EnableInteraction(true);
	m_MoveObjectInteractor->SetDataNode(MoveableNode);
}
