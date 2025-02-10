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
#include <tuple>
#include <vtkTransformPolyDataFilter.h>

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
