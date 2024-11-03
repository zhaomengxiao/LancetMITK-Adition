#include "PKARenderHelper.h"


void PKARenderHelper::DrawLine(double* p1, double* p2, double* color, double lineWidth, vtkRenderer* render)
{
	vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
	line->SetPoint1(p1);
	line->SetPoint2(p2);
	line->Update();
	vtkNew<vtkDataSetMapper> mapper;
	mapper->SetInputConnection(line->GetOutputPort());
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(color);
	actor->GetProperty()->SetLineWidth(3);
	render->AddActor(actor);
}

void PKARenderHelper::DrawPlane(mitk::DataStorage* dataStorage, std::string planeNodeName)
{
	vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
	plane->SetCenter(0, 0, 0);
	plane->SetNormal(1, 0, 0);
	plane->SetOrigin(-20.0, -20.0, 0.0); // 平面的一个角
	plane->SetPoint1(20.0, -20.0, 0.0);  // 平面的另一个角，定义宽度
	plane->SetPoint2(-20.0, 20.0, 0.0);  // 平面的另一个角，定义高度
	plane->Update();

	mitk::Surface::Pointer surface = mitk::Surface::New();
	surface->SetVtkPolyData(plane->GetOutput());
	surface->Modified();

	mitk::DataNode::Pointer dataNode = mitk::DataNode::New();
	dataNode->SetData(surface);
	dataNode->SetName(planeNodeName);

	dataStorage->Add(dataNode);
}

vtkSmartPointer<vtkPolyData> PKARenderHelper::BuildAxis(const mitk::Point3D& center, const mitk::Vector3D& axis, double halflength)
{

	// Define all sizes relative to absolute size (thus that the gizmo will appear
	// in the same relative size for huge (size >> 1) and tiny (size << 1) objects).
	// This means that the gizmo will appear very different when a scene contains _both_
	// huge and tiny objects at the same time, but when the users zooms in on his
	// object of interest, the gizmo will always have the same relative size.
	const double shaftRadius = halflength * 0.02;
	const double arrowHeight = shaftRadius * 6;
	const int tubeSides = 15;

	// poly data appender to collect cones and tube that make up the axis
	vtkSmartPointer<vtkAppendPolyData> axisSource = vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
	// arrow tips at 110% of radius
	cone->SetCenter(center[0] + axis[0] * (halflength * 1.1 + arrowHeight * 0.5),
		center[1] + axis[1] * (halflength * 1.1 + arrowHeight * 0.5),
		center[2] + axis[2] * (halflength * 1.1 + arrowHeight * 0.5));

	cone->SetDirection(axis[0], axis[1], axis[2]);
	cone->SetRadius(shaftRadius * 3);
	cone->SetHeight(arrowHeight);
	cone->SetResolution(tubeSides);
	cone->CappingOn();
	cone->Update();
	axisSource->AddInputData(cone->GetOutput());


	// build the axis itself (as a tube around the line defining the axis)
	vtkSmartPointer<vtkPolyData> shaftSkeleton = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> shaftPoints = vtkSmartPointer<vtkPoints>::New();
	shaftPoints->InsertPoint(0, (center /*- axis * halflength * 1.1*/).GetDataPointer());
	shaftPoints->InsertPoint(1, (center + axis * halflength * 1.1).GetDataPointer());
	shaftSkeleton->SetPoints(shaftPoints);

	vtkSmartPointer<vtkCellArray> shaftLines = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType shaftLinePoints[] = { 0, 1 };
	shaftLines->InsertNextCell(2, shaftLinePoints);
	shaftSkeleton->SetLines(shaftLines);

	vtkSmartPointer<vtkTubeFilter> shaftSource = vtkSmartPointer<vtkTubeFilter>::New();
	shaftSource->SetInputData(shaftSkeleton);
	shaftSource->SetNumberOfSides(tubeSides);
	shaftSource->SetVaryRadiusToVaryRadiusOff();
	shaftSource->SetRadius(shaftRadius);
	shaftSource->Update();

	axisSource->AddInputData(shaftSource->GetOutput());
	axisSource->Update();

	// assemble axis and ring
	vtkSmartPointer<vtkAppendPolyData> appenderGlobal = vtkSmartPointer<vtkAppendPolyData>::New();
	appenderGlobal->AddInputData(axisSource->GetOutput());
	appenderGlobal->Update();

	// make everything shiny by adding normals
	vtkSmartPointer<vtkPolyDataNormals> normalsSource = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsSource->SetInputConnection(appenderGlobal->GetOutputPort());
	normalsSource->ComputePointNormalsOn();
	normalsSource->ComputeCellNormalsOff();
	normalsSource->SplittingOn();
	normalsSource->Update();

	vtkSmartPointer<vtkPolyData> result = normalsSource->GetOutput();
	return result;

}

void PKARenderHelper::DrawLoaclCoordinateSystem(mitk::DataStorage* dataStorage, vtkMatrix4x4* matrix, const char* name, double* color)
{
	mitk::Point3D center;
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	mitk::Vector3D xdirection;
	xdirection[0] = 1;
	xdirection[1] = 0;
	xdirection[2] = 0;

	mitk::Vector3D ydirection;
	ydirection[0] = 0;
	ydirection[1] = 1;
	ydirection[2] = 0;

	mitk::Vector3D zdirection;
	zdirection[0] = 0;
	zdirection[1] = 0;
	zdirection[2] = 1;

	vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer<vtkAppendPolyData>::New();
	double xColor[3] = { 255,0,0 };
	double yColor[3] = { 0,255,0 };
	double zColor[3] = { 0,0,255 };
	appender->AddInputData(PKARenderHelper::BuildAxis(center, xdirection));
	appender->AddInputData(PKARenderHelper::BuildAxis(center, ydirection));
	appender->AddInputData(PKARenderHelper::BuildAxis(center, zdirection));

	appender->Update();
	mitk::Surface::Pointer surface = mitk::Surface::New();
	surface->SetVtkPolyData(appender->GetOutput());

	mitk::DataNode::Pointer dataNode = mitk::DataNode::New();

	dataNode->SetData(surface);
	dataNode->SetName(name);
	dataNode->SetVisibility(true);
	dataNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrix);
	dataNode->SetColor(color[0], color[1], color[2]);
	dataStorage->Add(dataNode);
}

vtkSmartPointer<vtkPolyData> PKARenderHelper::DisplayDirection(mitk::DataStorage* dataStorage, Eigen::Vector3d start, Eigen::Vector3d end, const char* name, double* color, int length)
{
	mitk::DataNode* previousNode = dataStorage->GetNamedNode(name);
	if (previousNode)
	{
		dataStorage->Remove(previousNode);
	}
	Eigen::Vector3d direction = end - start;
	direction.normalize();
	Eigen::Vector3d extendedStart = start - direction * length;
	Eigen::Vector3d extendedEnd = start + direction * length;
	vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(extendedStart[0], extendedStart[1], extendedStart[2]);
	//lineSource->SetPoint1(start[0], start[1], start[2]);
	lineSource->SetPoint2(extendedEnd[0], extendedEnd[1], extendedEnd[2]);
	lineSource->Update();

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->DeepCopy(lineSource->GetOutput());

	mitk::Surface::Pointer lineSurface = mitk::Surface::New();
	lineSurface->SetVtkPolyData(polyData);

	mitk::DataNode::Pointer lineSurfaceNode = mitk::DataNode::New();
	lineSurfaceNode->SetData(lineSurface);
	lineSurfaceNode->SetName(name);
	lineSurfaceNode->SetVisibility(true);
	lineSurfaceNode->SetColor(color[0], color[1], color[2]);

	dataStorage->Add(lineSurfaceNode);
	return polyData;
}

void PKARenderHelper::LoadSingleMitkFile(mitk::DataStorage* dataStorage, std::string filePath, std::string nodeName)
{
	auto Node = dataStorage->GetNamedNode(nodeName);
	if (!Node)
	{
		mitk::DataNode::Pointer node = mitk::DataNode::New();
		node->SetData(mitk::IOUtil::Load(filePath)[0]);

		node->SetName(nodeName);

		dataStorage->Add(node);
	}
	else
	{
		Node->SetData(mitk::IOUtil::Load(filePath)[0]);
	}
}

void PKARenderHelper::AddPointSetToMitk(mitk::DataStorage* dataStorage, std::vector<Eigen::Vector3d> Points, std::string Name, int Size, double* Color)
{
	RemoveNodeIsExist(dataStorage, Name);
	mitk::PointSet::Pointer pointSet = mitk::PointSet::New();
	for (int i = 0; i < Points.size(); i++)
	{
		mitk::PointSet::PointType pt;
		pt[0] = Points[i][0];
		pt[1] = Points[i][1];
		pt[2] = Points[i][2];
		//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "d", 3, Points[i].data());
		pointSet->SetPoint(i, pt);
	}

	mitk::DataNode::Pointer pointSetNode = mitk::DataNode::New();
	pointSetNode->SetData(pointSet);
	pointSetNode->SetName(Name.c_str());
	pointSetNode->SetFloatProperty("pointsize", Size);
	pointSetNode->SetColor(Color[0], Color[1], Color[2]);

	dataStorage->Add(pointSetNode);
}

void PKARenderHelper::AddvtkPointsToMitk(mitk::DataStorage* dataSrorage, vtkPoints* points, std::string name, int Size, double* Color, bool visibility)
{
	RemoveNodeIsExist(dataSrorage, name);
	mitk::PointSet::Pointer pointSet = mitk::PointSet::New();
	mitk::DataNode::Pointer dataNode = mitk::DataNode::New();
	for (int i = 0; i < points->GetNumberOfPoints(); ++i)
	{
		double* pointData = points->GetPoint(i);
		mitk::PointSet::PointType pt;
		pt[0] = pointData[0];
		pt[1] = pointData[1];
		pt[2] = pointData[2];
		pointSet->InsertPoint(i, pt);
	}
	dataNode->SetData(pointSet);
	dataNode->SetName(name);
	dataNode->SetFloatProperty("pointsize", Size);
	dataNode->SetColor(Color[0], Color[1], Color[2]);
	dataNode->SetVisibility(visibility);

	dataSrorage->Add(dataNode);
}

void PKARenderHelper::TransformModel(mitk::DataStorage* dataStorage, const char* name, vtkMatrix4x4* matrix)
{
	auto dataNode = dataStorage->GetNamedNode(name);
	if (dataNode)
	{
		auto geometry = dataNode->GetData()->GetGeometry();
		geometry->SetIndexToWorldTransformByVtkMatrix(matrix);
	}
}

mitk::DataNode::Pointer PKARenderHelper::AddPolyData2DataStorage(mitk::DataStorage* dataStorage, std::string name, vtkPolyData* polyData, double r, double g, double b, mitk::DataNode* parent)
{
	auto surface = mitk::Surface::New();
	surface->SetVtkPolyData(polyData);
	mitk::DataNode::Pointer node;
	node = dataStorage->GetNamedNode(name);
	if (node)
	{
		auto node = dataStorage->GetNamedNode(name);
		node->SetData(surface);
	}
	else
	{
		node = mitk::DataNode::New();
		node->SetData(surface);
		node->SetName(name);
		node->SetColor(r, g, b);
		node->SetFloatProperty("material.specularCoefficient", 0);

		dataStorage->Add(node, parent);
	}
	return node;
}

void PKARenderHelper::TranslateModel(double* direction, mitk::DataNode::Pointer dataNode, double length)
{
	auto data = dynamic_cast<mitk::Surface*>(dataNode->GetData());

	// mitk::Point3D normalizedDirection;
	double directionLength = sqrt((pow(direction[0], 2) + pow(direction[1], 2) + pow(direction[2], 2)));
	if (directionLength == 0)
		return;
	mitk::Point3D movementVector;
	movementVector[0] = length * direction[0] / directionLength;
	movementVector[1] = length * direction[1] / directionLength;
	movementVector[2] = length * direction[2] / directionLength;


	auto* doOp = new mitk::PointOperation(mitk::OpMOVE, 0, movementVector, 0);
	// execute the Operation
	// here no undo is stored, because the movement-steps aren't interesting.
	// only the start and the end is interesting to store for undo.
	data->GetGeometry()->ExecuteOperation(doOp);
	delete doOp;
}

void PKARenderHelper::TranslateModel(mitk::DataNode::Pointer dataNode, double* pos)
{
	auto data = dataNode->GetData();

	// Create movement vector based on specified x, y, z components
	mitk::Point3D movementVector;
	movementVector[0] = pos[0];
	movementVector[1] = pos[1];
	movementVector[2] = pos[2];

	// Execute the move operation
	auto* doOp = new mitk::PointOperation(mitk::OpMOVE, 0, movementVector, 0);
	data->GetGeometry()->ExecuteOperation(doOp);
	delete doOp;
}

void PKARenderHelper::RotateModel(double* direction, double* center, mitk::DataNode::Pointer dataNode, double angle)
{
	auto data = dataNode->GetData();
	double normalizedDirection[3];
	double directionLength = sqrt((pow(direction[0], 2) + pow(direction[1], 2) + pow(direction[2], 2)));
	if (directionLength == 0)
	{
		return;
	}

	normalizedDirection[0] = direction[0] / directionLength;
	normalizedDirection[1] = direction[1] / directionLength;
	normalizedDirection[2] = direction[2] / directionLength;

	mitk::Point3D rotateCenter{ center };
	mitk::Vector3D rotateAxis{ normalizedDirection };
	auto* doOp = new mitk::RotationOperation(mitk::OpROTATE, rotateCenter, rotateAxis, angle);
	// execute the Operation
	// here no undo is stored, because the movement-steps aren't interesting.
	// only the start and the end is interesting to store for undo.
	data->GetGeometry()->ExecuteOperation(doOp);
	delete doOp;
}

vtkSmartPointer<vtkAxesActor> PKARenderHelper::GenerateAxesActor(double axexLength)
{
	vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	//axesActor->SetConeRadius(100);
	//axesActor->SetCylinderRadius(50);
	axesActor->SetTotalLength(axexLength, axexLength, axexLength);
	axesActor->SetAxisLabels(false);
	axesActor->Modified();

	return axesActor;
}

void PKARenderHelper::InitSurfaceSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(dataStorage);
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void PKARenderHelper::InitImageSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(dataStorage);
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Image>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void PKARenderHelper::InitPointSetSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(dataStorage);
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));

}

void PKARenderHelper::InitBoundingBoxSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(dataStorage);
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::GeometryData>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))));
	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a bounding box"));
	widget->SetPopUpTitel(QString("Select bounding box node"));
}

mitk::Surface::Pointer PKARenderHelper::GetSurfaceNodeByName(mitk::DataStorage* dataStorage, const std::string nodeName)
{
	auto dataNode = dataStorage->GetNamedNode(nodeName);
	if (dataNode != nullptr)
	{
		return (mitk::Surface*)dataNode->GetData();
	}
	return nullptr;
}

void PKARenderHelper::RemoveNodeIsExist(mitk::DataStorage* dataStorage, std::string name)
{
	mitk::DataNode::Pointer dataNode = dataStorage->GetNamedNode(name);
	if (dataNode)
	{
		dataStorage->Remove(dataNode);
	}
}

void PKARenderHelper::RemoveAllNode(mitk::DataStorage* dataStorage)
{
	mitk::DataStorage::SetOfObjects::ConstPointer allNodes = dataStorage->GetAll();
	for (auto it = allNodes->Begin(); it != allNodes->End(); ++it) {
		dataStorage->Remove(it->Value());
	}
}

Eigen::Vector3d PKARenderHelper::GetFirstPointFromDataStorage(mitk::DataStorage* dataStorage, std::string nodeName)
{
	auto* pointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(nodeName)->GetData());
	mitk::Point3D point = pointSet->GetPoint(0);
	return Eigen::Vector3d(point[0], point[1], point[2]);
}

bool PKARenderHelper::IsContainsNamedNode(mitk::DataStorage* dataStorage, std::string nodeName)
{
	if (dataStorage->GetNamedNode(nodeName))
	{
		return true;
	}
	return false;
}

void PKARenderHelper::HideAllNode(mitk::DataStorage* dataStorage)
{
	// 获取所有的节点
	mitk::DataStorage::SetOfObjects::ConstPointer allNodes = dataStorage->GetAll();

	// 遍历所有节点并隐藏它们
	for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
	{
		mitk::DataNode::Pointer node = it->Value();
		node->SetVisibility(false);
	}

	// 刷新渲染窗口
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void PKARenderHelper::DisplaySingleNode(mitk::DataStorage* dataStorage, std::string nodeName)
{
	// 获取所有的节点
	mitk::DataStorage::SetOfObjects::ConstPointer allNodes = dataStorage->GetAll();

	// 遍历所有节点并隐藏它们
	for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
	{
		mitk::DataNode::Pointer node = it->Value();
		if (node->GetName() == nodeName)
			continue;
		node->SetVisibility(false);
	}

	// 刷新渲染窗口
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void PKARenderHelper::DisplaySingleNode(mitk::DataStorage* dataStorage, mitk::DataNode::Pointer node)
{
	// 获取所有的节点
	mitk::DataStorage::SetOfObjects::ConstPointer allNodes = dataStorage->GetAll();

	// 遍历所有节点并隐藏它们
	for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
	{
		mitk::DataNode::Pointer n = it->Value();
		if (n == node)
			continue;
		n->SetVisibility(false);
	}

	// 刷新渲染窗口
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void PKARenderHelper::ResetAllRenderWindow(mitk::IRenderWindowPart* renderWindowPart)
{
	auto renderWindows = renderWindowPart->GetQmitkRenderWindows();

	for (auto item : renderWindows)
	{
		item->ResetView();
	}
}

void PKARenderHelper::AddActor(mitk::IRenderWindowPart* renderWindowPart3D, vtkActor* actor)
{
	QmitkRenderWindow* mitkRenderWindow = renderWindowPart3D->GetQmitkRenderWindow("3d");

	vtkRenderWindow* renderWindow = mitkRenderWindow->GetVtkRenderWindow();

	vtkSmartPointer<vtkRenderer> renderer;

	renderer = renderWindow->GetRenderers()->GetFirstRenderer();

	renderer->AddActor(actor);
}

void PKARenderHelper::AddActor(mitk::IRenderWindowPart* renderWindowPart3D, vtkAxesActor* actor)
{
	if (!actor)
		return;
	QmitkRenderWindow* mitkRenderWindow = renderWindowPart3D->GetQmitkRenderWindow("3d");

	vtkRenderWindow* renderWindow = mitkRenderWindow->GetVtkRenderWindow();

	vtkSmartPointer<vtkRenderer> renderer;

	renderer = renderWindow->GetRenderers()->GetFirstRenderer();
	renderer->AddActor(actor);
}

void PKARenderHelper::RemoveActor(mitk::IRenderWindowPart* renderWindowPart3d, vtkAxesActor* actor)
{
	QmitkRenderWindow* mitkRenderWindow = renderWindowPart3d->GetQmitkRenderWindow("3d");

	vtkRenderWindow* renderWindow = mitkRenderWindow->GetVtkRenderWindow();

	vtkSmartPointer<vtkRenderer> renderer;

	renderer = renderWindow->GetRenderers()->GetFirstRenderer();
	renderer->RemoveActor(actor);
}







