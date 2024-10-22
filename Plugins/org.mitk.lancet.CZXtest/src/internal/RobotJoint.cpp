#include "RobotJoint.h"

RobotJoint::RobotJoint(JointPartDescription jointPartDescription/*, RobotJoint* ParentJoint = nullptr*/)
{
	m_Joint2Link = vtkSmartPointer<vtkMatrix4x4>::New();
	m_Image2Base = vtkSmartPointer<vtkMatrix4x4>::New();

	m_AxesActor = PKARenderHelper::GenerateAxesActor();
	m_JointPartDescription = jointPartDescription;
	Name = m_JointPartDescription.Name;
	this->Path = m_JointPartDescription.FileName;
}

vtkSmartPointer<vtkAxesActor> RobotJoint::GetJointAxesActor()
{
	return m_AxesActor;
}

vtkSmartPointer<vtkMatrix4x4> RobotJoint::GetBaseToLink()
{
	return m_Image2Base;
}

vtkSmartPointer<vtkMatrix4x4> RobotJoint::GetJointToLink()
{
	return m_Joint2Link;
}

void RobotJoint::UpdateJointToLink(vtkMatrix4x4* matrix)
{
	m_Joint2Link->DeepCopy(matrix);
}

void RobotJoint::UpdateJointToLink(double* matrix)
{
	std::string str = "UpdateJointToLink " + m_JointPartDescription.Name;
	PrintDataHelper::CoutMatrix(str.c_str(), matrix);
	m_Joint2Link->DeepCopy(matrix);
}

void RobotJoint::UpdateBaseToLink(vtkMatrix4x4* matrix)
{
	std::string str = "UpdateBaseToLink " + m_JointPartDescription.Name;
	PrintDataHelper::CoutMatrix(str.c_str(), matrix);
	m_Image2Base->DeepCopy(matrix);
}

double* RobotJoint::GetJointDH()
{
	return m_JointPartDescription.DH;
}

RobotTool::RobotTool(JointPartDescription aJointPartDescription)
{
	m_Joint2Tool = vtkSmartPointer<vtkMatrix4x4>::New();
	m_Base2Tool = vtkSmartPointer<vtkMatrix4x4>::New();

	m_AxesActor = PKARenderHelper::GenerateAxesActor();
	m_ToolPartDescription = aJointPartDescription;
	Name = m_ToolPartDescription.Name;
	this->Path = m_ToolPartDescription.FileName;
}

vtkSmartPointer<vtkAxesActor> RobotTool::GetToolAxesActor()
{
	return m_AxesActor;
}

vtkSmartPointer<vtkMatrix4x4> RobotTool::GetBaseToTool()
{
	return m_Base2Tool;
}

vtkSmartPointer<vtkMatrix4x4> RobotTool::GetJointToTool()
{
	return m_Joint2Tool;
}

void RobotTool::UpdateJointToTool(vtkMatrix4x4* matrix)
{
	m_Joint2Tool->DeepCopy(matrix);
}

void RobotTool::UpdateJointToTool(double* matrix)
{
	m_Joint2Tool->DeepCopy(matrix);
}

void RobotTool::UpdateBaseToTool(vtkMatrix4x4* matrix)
{
	m_Base2Tool->DeepCopy(matrix);
}
