#include "RobotBase.h"

RobotBase::RobotBase(JointPartDescription description)
{
	axesActor = PKARenderHelper::GenerateAxesActor();
	m_JointPartDescription = description;
	m_Image2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	m_Image2Base->Identity();
}

vtkSmartPointer<vtkAxesActor> RobotBase::GetBaseAxesActor()
{
	return axesActor;
}

JointPartDescription RobotBase::GetDescription()
{
	return m_JointPartDescription;
}

void RobotBase::UpdateImage2Base(vtkMatrix4x4* matrix)
{
	m_Image2Base->DeepCopy(matrix);
}

vtkSmartPointer<vtkMatrix4x4> RobotBase::GetImage2Base()
{
	return m_Image2Base;
}
