#include "ToolDisplayHelper.h"


lancetAlgorithm::ToolDisplayHelper::ToolDisplayHelper(mitk::DataStorage* dataStorage, mitk::IRenderWindowPart* iRenderWindowPart,
	AimCamera* aCamera)
{
	m_DataStorage = dataStorage;
	m_IRenderWindowPart = iRenderWindowPart;
	m_DrillEndMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_Camera = aCamera;
}

void lancetAlgorithm::ToolDisplayHelper::Start()
{
	if (!m_UpdateToolTimer)
	{
		m_UpdateToolTimer = new QTimer(this);
	}

	connect(m_UpdateToolTimer, SIGNAL(timeout()), this, SLOT(UpdateTool()));
	m_UpdateToolTimer->setInterval(100);
}

void lancetAlgorithm::ToolDisplayHelper::Stop()
{
	if (m_UpdateToolTimer) {
		disconnect(m_UpdateToolTimer, &QTimer::timeout, this, &ToolDisplayHelper::UpdateTool);
	}
}

void lancetAlgorithm::ToolDisplayHelper::AddModle(PKAMarker p, mitk::IRenderWindowPart* renderWindowPart)
{
	std::cout << "ToolDisplayHelper::AddModle" << std::endl;
	if (m_ToolSet.find(p) != m_ToolSet.end())
		return;
	std::cout << "Cannot find Tool in ToolSet" << std::endl;
	if (p == PKAMarker::PKADrill)
	{
		m_DrillEndActor = vtkSmartPointer<vtkAxesActor>::New();
		m_DrillEndActor = PKARenderHelper::GenerateAxesActor(25);
		PKARenderHelper::AddActor(renderWindowPart, m_DrillEndActor);
	}
	m_ToolSet.insert(p).second;
	auto axesActor = PKARenderHelper::GenerateAxesActor(20);
	m_AxesMapper.insert(std::pair(p, axesActor));
	PKARenderHelper::LoadSingleMitkFile(m_DataStorage,to_path(p), to_string(p));
	PKARenderHelper::AddActor(renderWindowPart, axesActor);
	std::cout << "ToolDisplayHelper AddActor" << std::endl;
	
	auto renderWindow= renderWindowPart->GetQmitkRenderWindow("3d");
	renderWindow->ResetView();
	std::cout << "ToolDisplayHelper ResetView" << std::endl;
}

void lancetAlgorithm::ToolDisplayHelper::RemoveModel(PKAMarker p)
{
	std::cout << "ToolDisplayHelper::RemoveModel" << std::endl;
	if (m_ToolSet.erase(p) > 0) {  // 如果成功移除
		PKARenderHelper::RemoveNodeIsExist(m_DataStorage, to_string(p)); // 移除其渲染
		auto axesActor = m_AxesMapper.at(p);
		m_AxesMapper.erase(p);
		PKARenderHelper::RemoveActor(m_IRenderWindowPart, axesActor);
	}
	auto renderWindow = m_IRenderWindowPart->GetQmitkRenderWindow("3d");
	renderWindow->ResetView();
}

void lancetAlgorithm::ToolDisplayHelper::UpdateTool()
{
	Eigen::Vector3d currentDrillEnd;
	Eigen::Matrix3d currentDrilRotation;
	vtkSmartPointer<vtkAxesActor> originAxesActor;
	vtkSmartPointer<vtkMatrix4x4> TImage2RF = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	mitk::BaseGeometry::Pointer geo;
	for (auto& tool : m_ToolSet) {
		
		geo = m_DataStorage->GetNamedNode(to_string(tool))->GetData()->GetGeometry();
		switch (tool)
		{
		case PKAMarker::PKADrill:
		{
			vtkSmartPointer<vtkMatrix4x4> TCamera2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkMatrix4x4> TImage2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkMatrix4x4> TImage2DrillEnd = vtkSmartPointer<vtkMatrix4x4>::New();
			currentDrillEnd = CalculationHelper::TransformByMatrix(m_DrillEndPoints, PKAData::m_TCamera2Drill);

			TCamera2Drill->DeepCopy(PKAData::m_TCamera2Drill);
			currentDrilRotation = CalculationHelper::GetRotationFromMatrix4x4(TCamera2Drill);
			m_DrillEndMatrix->DeepCopy(CalculationHelper::GetMatrixByRotationAndTranslation(currentDrilRotation, currentDrillEnd));
			//设置坐标原点的matrix
			originAxesActor = m_AxesMapper.at(tool);
			if(m_IsUseFemurRegistration)
			{
				TImage2RF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
				TRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
				//PrintDataHelper::CoutMatrix("m_DrillEndActor", TImage2DrillEnd);
			}
			if (m_IsUseTibiaRegistration)
			{
				TImage2RF->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);
				TRF2Camera->DeepCopy(PKAData::m_TCamera2TibiaRF);
				//PrintDataHelper::CoutMatrix("m_DrillEndActor", TTibiaImage2DrillEnd);
			}
			TImage2RF->Invert();
			TRF2Camera->Invert();

			TImage2Drill->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TImage2RF, TRF2Camera, PKAData::m_TCamera2Drill));
			TImage2DrillEnd->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TImage2RF, TRF2Camera, m_DrillEndMatrix));
			originAxesActor->SetUserMatrix(TImage2Drill);
			geo->SetIndexToWorldTransformByVtkMatrix(TImage2Drill);
			m_DrillEndActor->SetUserMatrix(TImage2DrillEnd);
			//PrintDataHelper::CoutMatrix("TImage2DrillEnd", TImage2DrillEnd);
			break; 
		}
		case PKAMarker::PKAProbe:
		{
			vtkSmartPointer<vtkMatrix4x4> TCamera2Probe = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkMatrix4x4> TImage2ProbeTip = vtkSmartPointer<vtkMatrix4x4>::New();

			TCamera2Probe->DeepCopy(PKAData::m_TCamera2Probe);
			originAxesActor = m_AxesMapper.at(tool);
			if(m_IsUseFemurRegistration)
			{
				TImage2RF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
				TRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
			}
			if (m_IsUseTibiaRegistration)
			{
				TImage2RF->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);
				TRF2Camera->DeepCopy(PKAData::m_TCamera2TibiaRF);
			}
			TImage2RF->Invert();
			TRF2Camera->Invert();
			TImage2ProbeTip->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TImage2RF, TRF2Camera, TCamera2Probe));
			geo->SetIndexToWorldTransformByVtkMatrix(TImage2ProbeTip);
			originAxesActor->SetUserMatrix(TImage2ProbeTip);
			break;
		}
		case PKAMarker::PKABluntProbe:
		{
			vtkSmartPointer<vtkMatrix4x4> TCamera2BluntProbe = vtkSmartPointer<vtkMatrix4x4>::New();
			TCamera2BluntProbe->DeepCopy(PKAData::m_TCamera2BluntProbe);
			geo->SetIndexToWorldTransformByVtkMatrix(TCamera2BluntProbe);
			auto bluntProbeActor = m_AxesMapper.at(tool);

			bluntProbeActor->SetUserMatrix(TCamera2BluntProbe);
			break;
		}
		default:
			break;
		}
	}
}

void lancetAlgorithm::ToolDisplayHelper::UsingFemurRegistration()
{
	m_IsUseFemurRegistration = true;
	m_IsUseTibiaRegistration = false;
}

void lancetAlgorithm::ToolDisplayHelper::UsingTibiaRegistration()
{
	m_IsUseFemurRegistration = false;
	m_IsUseTibiaRegistration = true;
}
