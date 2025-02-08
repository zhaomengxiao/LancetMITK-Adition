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

	//转换物理坐标到图像坐标
	image->GetGeometry()->WorldToIndex(minPoint, startIndex);
	image->GetGeometry()->WorldToIndex(maxPoint, endIndex);

	for (int i = 0; i < 3; ++i) {
		if (startIndex[i] < 0) startIndex[i] = 0;
		if (endIndex[i] >= image->GetDimension(i)) endIndex[i] = image->GetDimension(i) - 1;
		if (startIndex[i] > endIndex[i]) std::swap(startIndex[i], endIndex[i]);  // 确保顺序正确
	}
	int Xmin = static_cast<int>(startIndex[0]);
	int Ymin = static_cast<int>(startIndex[1]);
	int Zmin = static_cast<int>(startIndex[2]);
	int Xmax = static_cast<int>(endIndex[0]);
	int Ymax = static_cast<int>(endIndex[1]);
	int Zmax = static_cast<int>(endIndex[2]);

	// Xmin, Xmax, Ymin, Ymax, Zmin, Zmax are the diagonal indices of the image, but they are problematic, since
	// // the corner pts of the AABB in the world space don't necessarily correspond to the AABB corner pts in the image space -- 20250207

	return std::make_tuple(std::array<int, 6>{Xmin, Xmax, Ymin, Ymax, Zmin, Zmax}, minPoint, maxPoint);
}

mitk::Image::Pointer ImageCropperNew::ConvertVtkToMitk(vtkImageData* vtkImage)
{
	mitk::Image::Pointer mitkImage = mitk::Image::New();
	mitkImage->Initialize(vtkImage);
	mitkImage->SetVolume(vtkImage->GetScalarPointer());
	return mitkImage;
}

vtkSmartPointer<vtkImageData> ImageCropperNew::ExtractImageRegionByBound(vtkSmartPointer<vtkImageData>& inputImage, int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, const mitk::Point3D minPoint, const mitk::Point3D maxPoint)
{

	vtkSmartPointer<vtkExtractVOI> extractVOI = vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputData(inputImage);
	extractVOI->SetVOI(xmin, xmax, ymin, ymax, zmin, zmax);
	extractVOI->Update();
	vtkSmartPointer<vtkImageData> extractedImage = extractVOI->GetOutput();

	// 计算新的原点
	double originalOrigin[3];
	inputImage->GetOrigin(originalOrigin);  // 原始图像的原点
	double spacing[3];
	inputImage->GetSpacing(spacing);  // 原始图像的像素间距

	// 根据 VOI 起始位置计算新原点
	double clipedOrigin[3];
	extractedImage->GetOrigin(clipedOrigin);

	double newOrigin[3];
	newOrigin[0] = minPoint[0];
	newOrigin[1] = minPoint[1];
	newOrigin[2] = minPoint[2];

	//newOrigin[0] = originalOrigin[0] + xmin * spacing[0];
	//newOrigin[1] = originalOrigin[1] + ymin * spacing[1];
	//newOrigin[2] = originalOrigin[2] + zmin * spacing[2];

	// 设置提取图像的新原点和间距，使物理位置一致
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

	//1.找到重合区域的Xmin,Ymin,Zmin,Xmax,Ymax,Zmax
	auto [bound, minPoint, maxPoint] = CalculateOverlapImageBound(image_clip, surface_clip);
	//2.使用extractVOI,裁剪图像
	vtkSmartPointer<vtkImageData> vtkImageToReam = image_clip->GetVtkImageData();
	vtkSmartPointer<vtkImageData> croppedImage = ExtractImageRegionByBound(vtkImageToReam,
		bound[0], bound[1], bound[2], bound[3], bound[4], bound[5], minPoint, maxPoint);
	//3.vtkImage转换为mitkImage
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
	mitk::Vector3D extent = geometry->GetExtentInMM(0); // 图像的物理尺寸
	mitk::BoundingBox::BoundsArrayType bounds = geometry->GetBounds();

	mitk::Image::IndexType startIndex, endIndex;
	mitk::Point3D minPoint, maxPoint;

	mitk::FillVector3D(minPoint, bounds[0], bounds[2], bounds[4]);
	mitk::FillVector3D(maxPoint, bounds[1], bounds[3], bounds[5]);

	//double halfSize[3];
	//halfSize[0] = std::abs((minPoint[0] - maxPoint[0]) / 4.0); // 长
	//halfSize[1] = std::abs((minPoint[1] - maxPoint[1]) / 4.0); // 宽
	//halfSize[2] = std::abs((minPoint[2] - maxPoint[2]) / 4.0); // 高

	double halfSize[3];
	halfSize[0] = extent[0] / 1.0; // 长
	halfSize[1] = extent[1] / 1.0; // 宽
	halfSize[2] = extent[2] / 1.0; // 高

	mitk::Point3D center = geometry->GetCenter(); 

	// 使用 VTK 生成切割用长方体
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
	//cubeNode->SetProperty("color", mitk::ColorProperty::New(1.0, 0.0, 0.0)); // 设置颜色为红色
	cubeNode->SetProperty("opacity", mitk::FloatProperty::New(0.5)); 

	GetDataStorage()->Add(cubeNode);

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void ImageCropperNew::on_pushButton_AddInteractor_clicked() {

	auto MoveableNode = GetDataStorage()->GetNamedNode("Crop Box");

	m_MoveObjectInteractor->EnableInteraction(true);
	m_MoveObjectInteractor->SetDataNode(MoveableNode);
}
