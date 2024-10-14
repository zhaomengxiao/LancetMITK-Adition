#include "IntraBone.h"

lancetAlgorithm::IntraBone::IntraBone(mitk::DataStorage* dataStorage)
{
	//m_TCamera2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	//m_TCamera2TibiaRF = vtkSmartPointer<vtkMatrix4x4>::New();

	//m_TCamera2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	//m_TCamera2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();

	//m_TFemurRF2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	//m_TTibiaRF2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
	m_DataStorage = dataStorage;
	m_FemurProsMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
}

void lancetAlgorithm::IntraBone::UpdateMechanical()
{
	//PKAData::m_TCamera2FemurImage->Identity();
	//vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	//TFemurRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
	//TFemurRF2Camera->Invert();
	//vtkMatrix4x4::Multiply4x4(TFemurRF2Camera, PKAData::m_TCamera2FemurImage, PKAData::m_TFemurRF2FemurImage);
	//
	//PKAData::m_IntraFemurMechanicalAxis = CalculationHelper::TransformByMatrix(PKAData::m_FemurMechanicalAxis, PKAData::m_TFemurRF2FemurImage);

	//PKAData::m_TCamera2TibiaImage->Identity();
	//vtkSmartPointer<vtkMatrix4x4> TTibiaRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	//TTibiaRF2Camera->DeepCopy(PKAData::m_TCamera2TibiaRF);
	//TTibiaRF2Camera->Invert();
	//vtkMatrix4x4::Multiply4x4(TTibiaRF2Camera, PKAData::m_TCamera2TibiaImage, PKAData::m_TTibiaRF2TibiaImage);
	//PKAData::m_IntraTibiaMechanicalAxis = CalculationHelper::TransformByMatrix(PKAData::m_TibiaMechanicalAxis, PKAData::m_TTibiaRF2TibiaImage);
}

double lancetAlgorithm::IntraBone::CalculateAngle()
{
	double angle = CalculationHelper::CalculateAngle(PKAData::m_IntraFemurMechanicalAxis, PKAData::m_IntraTibiaMechanicalAxis);

	double rad = angle * 180 / PI;

	return rad;
}

void lancetAlgorithm::IntraBone::UpdateDisplayModel()
{
	//获得切割后的femurSurface
	auto femurClippedSurface = (mitk::Surface*)m_DataStorage->GetNamedNode
	("femurSurface")->GetData();

	//获得术前规划好的prosNode 假体
	auto tempFemurPros = (mitk::Surface*)m_DataStorage->GetNamedNode("tempFemurProsNodeName")->GetData();

	vtkNew<vtkMatrix4x4> TCamera2FemurRF;
	vtkNew<vtkMatrix4x4> TFemur2FemurRF;
	vtkNew<vtkMatrix4x4> TCamera2Femur;
	TCamera2FemurRF->DeepCopy(PKAData::m_TCamera2FemurRF);
	TFemur2FemurRF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
	TFemur2FemurRF->Invert();

	//股骨假体跟随变换（此处世界坐标到股骨的矩阵改变）
	vtkMatrix4x4* m = femurClippedSurface->GetGeometry()->GetVtkMatrix();
	Eigen::Matrix3d RotationGlobal2Femur;
	Eigen::Vector3d TranslationGlobal2Femur;
	RotationGlobal2Femur = CalculationHelper::GetRotationFromMatrix4x4(m);
	TranslationGlobal2Femur = CalculationHelper::GetTranslationFromMatrix4x4(m);
	Eigen::Matrix3d femurRG2I = RotationGlobal2Femur * RotationFemur2Implant;;
	Eigen::Vector3d femurVG2I = RotationGlobal2Femur * TranslationFemur2Implant + TranslationGlobal2Femur;

	m_FemurProsMatrix = tempFemurPros->GetGeometry()->GetVtkMatrix();

	//胫骨变换
	auto tibiaClippedSurface = PKARenderHelper::GetSurfaceNodeByName(m_DataStorage, tempTibiaClippedNodeName.c_str());
	auto fibulaClippedSurface = PKARenderHelper::GetSurfaceNodeByName(m_DataStorage,tempFibulaClippedNodeName.c_str());
	auto tempTibiaTray = PKARenderHelper::GetSurfaceNodeByName(m_DataStorage,tempTibiaTrayNodeName.c_str());
	auto tempTibiaInsert = PKARenderHelper::GetSurfaceNodeByName(m_DataStorage,tempTibiaInsertNodeName.c_str());
	vtkNew<vtkMatrix4x4> TCamera2TibiaRF;
	vtkNew<vtkMatrix4x4> TTibia2TibiaRF;
	vtkNew<vtkMatrix4x4> TCamera2Tibia; 

	TCamera2TibiaRF->DeepCopy(PKAData::m_TCamera2TibiaRF);
	TTibia2TibiaRF->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);
	TTibia2TibiaRF->Invert();
	double ref[3] = { 0, 0, 0 };

	vtkNew<vtkTransform> comTransform;
	vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurRF2Camera->DeepCopy(TCamera2FemurRF);
	TFemurRF2Camera->Invert();
	vtkSmartPointer<vtkMatrix4x4> TTibiaRF2Tibia = vtkSmartPointer<vtkMatrix4x4>::New();
	TTibiaRF2Tibia->DeepCopy(TTibia2TibiaRF);
	TTibiaRF2Tibia->Invert();

	//最后得到Femur 到Tibia的转换关系
	comTransform->PreMultiply();
	comTransform->SetMatrix(TFemur2FemurRF);
	comTransform->Concatenate(TFemurRF2Camera);
	comTransform->Concatenate(TCamera2TibiaRF);
	comTransform->Concatenate(TTibiaRF2Tibia);

	mitk::ApplyTransformMatrixOperation* op = new mitk::ApplyTransformMatrixOperation
	(mitk::OpAPPLYTRANSFORMMATRIX, comTransform->GetMatrix(), ref);
	tibiaClippedSurface->GetGeometry()->ExecuteOperation(op);
	if (fibulaClippedSurface)
	{
		fibulaClippedSurface->GetGeometry()->ExecuteOperation(op);
	}

	//胫骨假体跟随变换（此处世界坐标到胫骨的矩阵改变）
	vtkMatrix4x4* tibiaSurfaceTransform = tibiaClippedSurface->GetGeometry()->GetVtkMatrix();
	Eigen::Matrix3d RotationGlobal2Tibia ;
	Eigen::Vector3d TranslationGlobal2Tibia;
	RotationGlobal2Tibia = CalculationHelper::GetRotationFromMatrix4x4(tibiaSurfaceTransform); 
	TranslationGlobal2Tibia = CalculationHelper::GetTranslationFromMatrix4x4(tibiaSurfaceTransform);
	Eigen::Matrix3d tibiaRG2I = RotationGlobal2Tibia * RotationTibia2Tray;
	Eigen::Vector3d tibiaVG2I = RotationGlobal2Tibia * TranslationTibia2Tray + TranslationGlobal2Tibia;

	auto trans1 = vtkMatrix4x4::New();
	trans1 = CalculationHelper::GetMatrixByRotationAndTranslation(tibiaRG2I, tibiaVG2I);
	op = new mitk::ApplyTransformMatrixOperation(mitk::OpAPPLYTRANSFORMMATRIX, trans1, ref);
	tempTibiaTray->GetGeometry()->ExecuteOperation(op);
	tempTibiaInsert->GetGeometry()->ExecuteOperation(op);
	delete op;
	m_TibiaTrayMatrix = tempTibiaTray->GetGeometry()->GetVtkMatrix();
	m_TibiaInsertMatrix = tempTibiaInsert->GetGeometry()->GetVtkMatrix();

	DrawMechanicalAxis(rightRenderer, leftRenderer, tibiaClippedSurface->GetGeometry()->GetVtkMatrix());//画力线

	////NDI看见股骨
	//if (switchAngle || this->ui->widgetToolState->isToolValid(TrackToolType::Femur))
	//{
	//	m_DataStorage->GetNamedNode(tempFemurClippedNodeName)->SetColor(1, 1, 1);
	//	m_DataStorage->GetNamedNode(tempFemurProsNodeName)->SetColor(0, 1, 0);
	//}
	//else
	//{
	//	m_DataStorage->GetNamedNode(tempFemurClippedNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//	m_DataStorage->GetNamedNode(tempFemurProsNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//}
	////NDI看见胫骨
	//if (switchAngle || this->ui->widgetToolState->isToolValid(TrackToolType::Tibia))
	//{
	//	m_DataStorage->GetNamedNode(tempTibiaClippedNodeName)->SetColor(1, 1, 1);
	//	m_DataStorage->GetNamedNode(tempTibiaTrayNodeName)->SetColor(0, 0, 1);
	//	m_DataStorage->GetNamedNode(tempTibiaInsertNodeName)->SetColor(0, 0, 1);
	//	if (fibulaClippedSurface)
	//	{
	//		m_DataStorage->GetNamedNode(tempFibulaClippedNodeName)->SetColor(1, 1, 1);
	//	}
	//}
	//else
	//{
	//	m_DataStorage->GetNamedNode(tempTibiaClippedNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//	m_DataStorage->GetNamedNode(tempTibiaTrayNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//	m_DataStorage->GetNamedNode(tempTibiaInsertNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//	if (fibulaClippedSurface)
	//	{
	//		m_DataStorage->GetNamedNode(tempFibulaClippedNodeName)->SetColor(164.0 / 255.0, 164.0 / 255.0, 164.0 / 255.0);
	//	}
	//}

	//updateFemurProsMatrix();
	//updatetibiaProsMatrix();
	//updateUIData();

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

/// <summary>
/// 初始化并显示假体的位置
/// </summary>
void lancetAlgorithm::IntraBone::InitPros()
{
	mitk::DataNode::Pointer femurProsNode = mitk::DataNode::New();
	mitk::DataNode::Pointer tibiaTrayNode = mitk::DataNode::New();
	mitk::DataNode::Pointer tibiaInsertNode = mitk::DataNode::New();

	auto femurProsData = mitk::IOUtil::Load(femurProsDataPath);
	auto tibiaTrayData = mitk::IOUtil::Load(tibiaTrayDataPath);
	auto tibiaInsertData = mitk::IOUtil::Load(tibiaInsertDataPath);
	mitk::Surface* femurPros = (mitk::Surface*)femurProsData[0].GetPointer();
	mitk::Surface* tibiaTray = (mitk::Surface*)tibiaTrayData[0].GetPointer();
	mitk::Surface* tibiaInsert = (mitk::Surface*)tibiaInsertData[0].GetPointer();

	femurProsNode->SetData(femurPros);
	tibiaTrayNode->SetData(tibiaTray);
	tibiaInsertNode->SetData(tibiaInsert);

	femurProsNode->SetName(tempFemurProsNodeName);
	tibiaTrayNode->SetName(tempTibiaTrayNodeName);
	tibiaInsertNode->SetName(tempTibiaInsertNodeName);
	femurProsNode->SetColor(0, 1, 0);
	femurProsNode->SetFloatProperty("material.diffuseCoefficient", 0.75);
	femurProsNode->SetFloatProperty("material.specularCoefficient", 0);
	tibiaTrayNode->SetColor(0, 0, 1);
	tibiaInsertNode->SetColor(0, 0, 1);
	tibiaTrayNode->SetFloatProperty("material.diffuseCoefficient", 0.75);
	tibiaTrayNode->SetFloatProperty("material.specularCoefficient", 0);
	tibiaInsertNode->SetFloatProperty("material.diffuseCoefficient", 0.75);
	tibiaInsertNode->SetFloatProperty("material.specularCoefficient", 0);
	m_DataStorage->Add(femurProsNode);
	m_DataStorage->Add(tibiaTrayNode);
	m_DataStorage->Add(tibiaInsertNode);

	vtkNew<vtkMatrix4x4> femurProsMatrix;
	femurProsMatrix->DeepCopy(PKAData::G_FemurProsthesisMatrix);
	double p[3] = { 0, 0, 0 };
	if (femurProsNode != nullptr && femurProsMatrix != nullptr)
	{
		mitk::ApplyTransformMatrixOperation* op = new mitk::ApplyTransformMatrixOperation
		(mitk::OpAPPLYTRANSFORMMATRIX, femurProsMatrix, p);
		femurPros->GetGeometry()->ExecuteOperation(op);
		delete op;
	}
	vtkNew<vtkMatrix4x4> tibiaTrayMatrix;
	tibiaTrayMatrix->DeepCopy(PKAData::G_TibiaTrayMatrix);
	if (tibiaTrayNode != nullptr && tibiaTrayMatrix != nullptr)
	{
		mitk::ApplyTransformMatrixOperation* op = new mitk::ApplyTransformMatrixOperation
		(mitk::OpAPPLYTRANSFORMMATRIX, tibiaTrayMatrix, p);
		tibiaTray->GetGeometry()->ExecuteOperation(op);
		tibiaInsert->GetGeometry()->ExecuteOperation(op);
		delete op;
	}
	
	//reuqest Render TODO
}

void lancetAlgorithm::IntraBone::UpdateTibiaProsMatrix()
{
	//更新 胫骨托的位置
	//更新  垫的位置
	// 更新胫骨的位置
	// 更新
	//重新计算角度

	//for (int i = 0; i < 3; i++)
	//{
	//	MainStatic::G_TibiaVB2I[i] = tibiaVB2I[i];
	//}
	//for (int i = 0; i < 3; i++)
	//{
	//	for (int j = 0; j < 3; j++)
	//	{
	//		MainStatic::G_TibiaRB2I[i * 3 + j] = tibiaRB2I(i, j);
	//	}
	//}
}


//绘制术中准备力线位置
void lancetAlgorithm::IntraBone::DrawMechanicalAxis(vtkRenderer* femurRenderer, vtkRenderer* tibiaRenderer, vtkMatrix4x4* tibiaMatrix)
{
	vtkRenderer* leftRenderer = tibiaRenderer;
	vtkRenderer* rightRenderer = femurRenderer;
	auto leftCamera = leftRenderer->GetActiveCamera();
	auto rightCamera = rightRenderer->GetActiveCamera();
	leftCamera->SetClippingRange(0.01, 1000);
	rightCamera->SetClippingRange(0.01, 1000);

	auto leftCameraPos = leftCamera->GetPosition();
	auto leftCameraFocal = leftCamera->GetFocalPoint();
	Eigen::Vector3d leftV = Eigen::Vector3d( leftCameraPos[0] - leftCameraFocal[0],
					   leftCameraPos[1] - leftCameraFocal[1],
					   leftCameraPos[2] - leftCameraFocal[2]
	);
	auto rightCameraPos = rightCamera->GetPosition();
	auto rightCameraFocal = rightCamera->GetFocalPoint();
	Eigen::Vector3d rightV = Eigen::Vector3d( rightCameraPos[0] - rightCameraFocal[0],
						rightCameraPos[1] - rightCameraFocal[1],
						rightCameraPos[2] - rightCameraFocal[2]
	);
	double halfLen = leftV.norm()/10;
	leftV.normalize();
	leftV *= halfLen;

	halfLen = rightV.norm ()/ 10;
	rightV.normalize();
	rightV *= halfLen;

	Eigen::Vector3d femurLeftPoint1 = FemurMechanicalStart + leftV;

	Eigen::Vector3d femurLeftPoint2 = FemurMechanicalEnd + leftV;

	vtkNew<vtkLineSource> femurLineLeft;
	femurLineLeft->SetPoint1(femurLeftPoint1.data());
	femurLineLeft->SetPoint2(femurLeftPoint2.data());
	femurLineLeft->Update();
	vtkNew<vtkDataSetMapper> femurMapperLeft;
	femurMapperLeft->SetInputConnection(femurLineLeft->GetOutputPort());
	vtkNew<vtkActor> femurActorLeft;
	femurActorLeft->SetMapper(femurMapperLeft);
	femurActorLeft->GetProperty()->SetColor(0, 1, 0);
	femurActorLeft->GetProperty()->SetLineWidth(3);
	if (femurLeftActor != nullptr)
	{
		leftRenderer->RemoveActor(femurLeftActor);
	}
	leftRenderer->AddActor(femurActorLeft);
	femurLeftActor = femurActorLeft;

	//胫骨的需要变换
	Eigen::Vector3d tibiaP1 = CalculationHelper::TransformByMatrix(TibiaMechanicalStart, tibiaMatrix);

	Eigen::Vector3d tibiaP2 = CalculationHelper::TransformByMatrix(TibiaMechanicalEnd, tibiaMatrix);

	Eigen::Vector3d tibiaLeftPoint1 = tibiaP1 + leftV;

	Eigen::Vector3d tibiaLeftPoint2 = tibiaP2 + leftV;

	vtkNew<vtkLineSource> tibiaLineLeft;
	tibiaLineLeft->SetPoint1(tibiaLeftPoint1.data());
	tibiaLineLeft->SetPoint2(tibiaLeftPoint2.data());
	tibiaLineLeft->Update();
	vtkNew<vtkDataSetMapper> tibiaMapperLeft;
	tibiaMapperLeft->SetInputConnection(tibiaLineLeft->GetOutputPort());
	vtkNew<vtkActor> tibiaActorLeft;
	tibiaActorLeft->SetMapper(tibiaMapperLeft);
	tibiaActorLeft->GetProperty()->SetColor(0, 1, 0);
	tibiaActorLeft->GetProperty()->SetLineWidth(3);
	if (tibiaLeftActor != nullptr)
	{
		leftRenderer->RemoveActor(tibiaLeftActor);
	}
	leftRenderer->AddActor(tibiaActorLeft);
	tibiaLeftActor = tibiaActorLeft;
}



