#include "RobotFrame2.h"

RobotFrame2::RobotFrame2(mitk::DataStorage* dataStorage, std::vector<RobotJoint*> robotJoints, RobotBase* base, RobotTool* aRobotTool)
{
	m_dataStorage = dataStorage;
	m_RobotJoints = robotJoints;
	m_Base = base;
	if (aRobotTool)
		m_RobotTool = aRobotTool;
}
void RobotFrame2::Display(mitk::IRenderWindowPart* part)
{
	if (m_IsFirstDispaly)
	{
		//this->LoadSingleMitkFile(m_Base->GetDescription().FileName, m_Base->GetDescription().Name);
		for (auto joint : m_RobotJoints)
		{
			//this->LoadSingleMitkFile(joint->Path, joint->Name);
			PKARenderHelper::AddActor(part, joint->GetJointAxesActor());
		}
		m_IsFirstDispaly = !m_IsFirstDispaly;
	}
	//DisplayRobotJoints();
	DisplayRobotAxes();
	double joints[7] = { 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0 };
	UpdateJoints(joints);
}

void RobotFrame2::Hide()
{
	this->HideRobotJoints();
	this->HideRobotAxes();
}

void RobotFrame2::DisplayRobotJoints()
{
	m_dataStorage->GetNamedNode(m_Base->GetDescription().FileName)->SetVisibility(true);
	for (auto joint : m_RobotJoints)
	{
		m_dataStorage->GetNamedNode(joint->Name)->SetVisibility(true);
	}
}

void RobotFrame2::HideRobotJoints()
{
	m_dataStorage->GetNamedNode(m_Base->GetDescription().FileName)->SetVisibility(false);
	for (auto joint : m_RobotJoints)
	{
		m_dataStorage->GetNamedNode(joint->Name)->SetVisibility(false);
	}
}

void RobotFrame2::DisplayRobotAxes()
{
	for (auto joint : m_RobotJoints)
	{
		joint->GetJointAxesActor()->SetVisibility(true);
	}
}

void RobotFrame2::HideRobotAxes()
{
	for (auto joint : m_RobotJoints)
	{
		joint->GetJointAxesActor()->SetVisibility(false);
	}
}

void RobotFrame2::UpdateJoints(double* jointAngles)
{
	for (int i = 0; i < m_RobotJoints.size(); ++i)
	{
		UpdateJointToLink(i, jointAngles[i]);
	}
	Update();
}

void RobotFrame2::UpdateJointToLink(int aJointId, double aPosition)
{
	auto joint = m_RobotJoints[aJointId];

	double d = joint->GetJointDH()[0];
	double a = joint->GetJointDH()[1];
	double alpha = joint->GetJointDH()[2];
	double theta = joint->GetJointDH()[3] + FromDegreeToRadian(aPosition);

	double matrix[16] = 
	{
		std::cos(theta), -std::sin(theta) * std::cos(alpha), std::sin(theta)* std::sin(alpha), a*std::cos(theta),
		std::sin(theta), std::cos(theta)*std::cos(alpha), -std::cos(theta)* std::sin(alpha), a*std::sin(theta),
		              0, std::sin(alpha),                                   std::cos(alpha), d,
	                   0, 0, 0, 1
	};
	
	joint->UpdateJointToLink(matrix);
}

void RobotFrame2::Update(int aJointId)
{
	UpdateBaseToLink(aJointId);
	Render(aJointId);
}

void RobotFrame2::UpdateBaseToLink(int aJointId)
{
	std::cout << "UpdateBaseToLink" << std::endl;
	for (int i = aJointId; i < m_RobotJoints.size(); ++i)
	{
		if (i == 0)
		{
			m_RobotJoints[i]->UpdateBaseToLink(m_RobotJoints[i]->GetJointToLink());
		}
		else
		{
			vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkMatrix4x4> preBase2Link = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkMatrix4x4> currentJoint2Link = vtkSmartPointer<vtkMatrix4x4>::New();
			preBase2Link->DeepCopy(m_RobotJoints[i - 1]->GetBaseToLink());
			currentJoint2Link->DeepCopy(m_RobotJoints[i]->GetJointToLink());
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->PreMultiply();
			transform->SetMatrix(preBase2Link);
			transform->Concatenate(currentJoint2Link);
			transform->Update();

			transform->GetMatrix(matrix);
			m_RobotJoints[i]->UpdateBaseToLink(matrix);
		}
	}
	std::cout << "UpdateBaseToLink finished" << std::endl;
}

void RobotFrame2::Render(int aJointId)
{
	for (int i = aJointId; i < m_RobotJoints.size(); ++i)
	{
		auto joint = m_RobotJoints[i];
		auto actor = joint->GetJointAxesActor();
		actor->SetUserMatrix(joint->GetBaseToLink());
		//m_dataStorage->GetNamedNode(joint->Name)->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(joint->GetBaseToLink());
	}

	//render for tool
}

double RobotFrame2::FromDegreeToRadian(double Degree)
{
	return Degree / 180 * PI;
}

void RobotFrame2::LoadSingleMitkFile(std::string filePath, std::string nodeName)
{
	auto Node = m_dataStorage->GetNamedNode(nodeName);
	if (!Node)
	{
		mitk::DataNode::Pointer node = mitk::DataNode::New();
		node->SetData(mitk::IOUtil::Load(filePath)[0]);

		node->SetName(nodeName);

		m_dataStorage->Add(node);
	}
	else
	{
		Node->SetData(mitk::IOUtil::Load(filePath)[0]);
	}
}
