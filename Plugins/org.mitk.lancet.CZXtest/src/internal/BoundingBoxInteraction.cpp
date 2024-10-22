#include "BoundingBoxInteraction.h"

BoundingBoxInteraction::BoundingBoxInteraction(mitk::DataStorage* dataStorage, mitk::IRenderWindowPart* renderWindowPart)
{
	mitk::RegisterBoundingShapeObjectFactory();
	m_DataStorage = dataStorage;
	m_BoundingShapeInteractor = mitk::BoundingShapeInteractor::New();
	CreateBoundingShapeInteractor(false);
	if (m_RenderWindowPart == nullptr)
	{
		std::cout << "BoundingBoxInteraction renderWindowPart is nullptr" << std::endl;
	}
	m_RenderWindowPart = renderWindowPart;
}

void BoundingBoxInteraction::DisplayBoundingBox(std::string nodeName, std::string boundingBoxNmae)
{
	auto node = m_DataStorage->GetNamedNode(nodeName);
	m_CurrentModelNodeName = nodeName;
	if (!node)
		return;

	// get current timestep to support 3d+t images
	//auto renderWindowPart = m_RenderWindowPart(mitk::WorkbenchUtil::IRenderWindowPartStrategy::OPEN);
	auto timePoint = m_RenderWindowPart->GetSelectedTimePoint();
	auto imageGeometry = node->GetData()->GetTimeGeometry()->GetGeometryForTimePoint(timePoint);

	auto boundingBox = mitk::GeometryData::New();
	boundingBox->SetGeometry(static_cast<mitk::Geometry3D*>(this->InitializeWithImageGeometry(imageGeometry)));
	mitk::DataNode::Pointer boundingBoxNode;
	boundingBoxNode = m_DataStorage->GetNamedNode(boundingBoxNmae);
	if (boundingBoxNode)
	{
		boundingBoxNode->SetData(boundingBox);
	}
	else
	{
		boundingBoxNode = mitk::DataNode::New();
		boundingBoxNode->SetData(boundingBox);
		boundingBoxNode->SetProperty("name", mitk::StringProperty::New(boundingBoxNmae));
		boundingBoxNode->SetProperty("color", mitk::ColorProperty::New(1.0, 1.0, 1.0));
		boundingBoxNode->SetProperty("opacity", mitk::FloatProperty::New(0.8));

		boundingBoxNode->SetProperty("layer", mitk::IntProperty::New(99));
		boundingBoxNode->AddProperty("handle size factor", mitk::DoubleProperty::New(/*1.0*/8.0 / 40.0));
		boundingBoxNode->SetBoolProperty("pickable", true);

		if (!m_DataStorage->Exists(boundingBoxNode))
		{
			m_DataStorage->Add(boundingBoxNode, node);
		}
	}

	boundingBoxNode->SetVisibility(true);
	
	m_BoundingShapeInteractor->EnableInteraction(true);
	m_BoundingShapeInteractor->SetDataNode(boundingBoxNode);
	mitk::RenderingManager::GetInstance()->InitializeViews();
	PKARenderHelper::ResetAllRenderWindow(m_RenderWindowPart);
}

void BoundingBoxInteraction::CutModel(std::string ClippedNodeName)
{
	if (!m_BoundingShapeInteractor->GetDataNode())
		return;
	auto bounds = m_BoundingShapeInteractor->GetDataNode()->GetData()->GetGeometry()->GetBounds();
	double boundsArray[6] = { bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5] };
	//PrintDataHelper::CoutArray(boundsArray, 6, "boundArray: ");

	vtkSmartPointer<vtkPlaneCollection> collection = vtkSmartPointer<vtkPlaneCollection>::New();

	vtkSmartPointer<vtkPlane> planeXmin = vtkSmartPointer<vtkPlane>::New();
	planeXmin->SetOrigin(bounds[0], 0, 0);
	planeXmin->SetNormal(1, 0, 0);

	vtkSmartPointer<vtkPlane> planeXmax = vtkSmartPointer<vtkPlane>::New();
	planeXmax->SetOrigin(bounds[1], 0, 0);
	planeXmax->SetNormal(-1, 0, 0);

	vtkSmartPointer<vtkPlane> planeYmin = vtkSmartPointer<vtkPlane>::New();
	planeYmin->SetOrigin(0, bounds[2], 0);
	planeYmin->SetNormal(0, 1, 0);

	vtkSmartPointer<vtkPlane> planeYmax = vtkSmartPointer<vtkPlane>::New();
	planeYmax->SetOrigin(0, bounds[3], 0);
	planeYmax->SetNormal(0, -1, 0);

	vtkSmartPointer<vtkPlane> planeZmin = vtkSmartPointer<vtkPlane>::New();
	planeZmin->SetOrigin(0, 0, bounds[4]);
	planeZmin->SetNormal(0, 0, 1);

	vtkSmartPointer<vtkPlane> planeZmax = vtkSmartPointer<vtkPlane>::New();
	planeZmax->SetOrigin(0, 0, bounds[5]);
	planeZmax->SetNormal(0, 0, -1);

	collection->AddItem(planeXmin);
	collection->AddItem(planeXmax);
	collection->AddItem(planeYmin);
	collection->AddItem(planeYmax);
	collection->AddItem(planeZmin);
	collection->AddItem(planeZmax);

	// 获取你想要剪裁的模型数据
	auto surfaceNode = m_DataStorage->GetNamedNode(m_CurrentModelNodeName);
	auto matrix = surfaceNode->GetData()->GetGeometry()->GetVtkMatrix();
	auto polyData = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetVtkPolyData();

	vtkSmartPointer<vtkClipClosedSurface> clipper = vtkSmartPointer<vtkClipClosedSurface>::New();

	clipper->SetInputData(polyData);
	clipper->SetGenerateFaces(true);
	clipper->SetClippingPlanes(collection);

	clipper->Update();
	vtkSmartPointer<vtkPolyData> clippedPolyData = vtkSmartPointer<vtkPolyData>::New();
	clippedPolyData = clipper->GetOutput();
	PKARenderHelper::AddPolyData2DataStorage(m_DataStorage, ClippedNodeName, clippedPolyData);
	//m_BoundingShapeInteractor->GetDataNode()->SetVisibility(false);
	PKARenderHelper::DisplaySingleNode(m_DataStorage, ClippedNodeName);
}

void BoundingBoxInteraction::CreateBoundingShapeInteractor(bool rotationEnabled)
{
	m_BoundingShapeInteractor->LoadStateMachine("BoundingShapeInteraction.xml", us::ModuleRegistry::GetModule("MitkBoundingShape"));
	m_BoundingShapeInteractor->SetEventConfig("BoundingShapeMouseConfig.xml", us::ModuleRegistry::GetModule("MitkBoundingShape"));

	m_BoundingShapeInteractor->SetRotationEnabled(rotationEnabled);
}

mitk::Geometry3D::Pointer BoundingBoxInteraction::InitializeWithImageGeometry(const mitk::BaseGeometry* geometry) const
{
	// convert a BaseGeometry into a Geometry3D (otherwise IO is not working properly)
	if (geometry == nullptr)
		mitkThrow() << "Geometry is not valid.";

	auto boundingGeometry = mitk::Geometry3D::New();
	boundingGeometry->SetBounds(geometry->GetBounds());
	auto bounds = boundingGeometry->GetBounds();
	double boundsArray[6] = { bounds[0],bounds[1],bounds[2],bounds[3],bounds[4],bounds[5] };
	//PrintDataHelper::CoutArray(boundsArray, 6, "boundsArray: ");
	boundingGeometry->SetImageGeometry(geometry->GetImageGeometry());
	boundingGeometry->SetOrigin(geometry->GetOrigin());
	boundingGeometry->SetSpacing(geometry->GetSpacing());
	boundingGeometry->SetIndexToWorldTransform(geometry->GetIndexToWorldTransform()->Clone());
	boundingGeometry->Modified();
	return boundingGeometry;
}
