#pragma once
#include <vtkAxesActor.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <mitkDataStorage.h>
#include "PKARenderHelper.h"
#include "JointPartDescription.h"
#include "PrintDataHelper.h"
class RobotJoint
{
public:
	RobotJoint(JointPartDescription jointPartDescription);
	vtkSmartPointer<vtkAxesActor> GetJointAxesActor();
	vtkSmartPointer<vtkMatrix4x4> GetBaseToLink();
	vtkSmartPointer<vtkMatrix4x4> GetJointToLink();
	void UpdateJointToLink(vtkMatrix4x4* matrix);
	void UpdateJointToLink(double* matrix);
	void UpdateBaseToLink(vtkMatrix4x4* matrix);
	double* GetJointDH();
public:
	std::string Name;
	std::string Path;
private:
	vtkSmartPointer<vtkAxesActor> m_AxesActor;
	vtkSmartPointer<vtkMatrix4x4> m_Joint2Link;
	vtkSmartPointer<vtkMatrix4x4> m_Image2Base;
	JointPartDescription m_JointPartDescription;
}; 

class RobotTool 
{
public:
	RobotTool(JointPartDescription aJointPartDescription);
	vtkSmartPointer<vtkAxesActor> GetToolAxesActor();
	vtkSmartPointer<vtkMatrix4x4> GetBaseToTool();
	vtkSmartPointer<vtkMatrix4x4> GetJointToTool();
	void UpdateJointToTool(vtkMatrix4x4* matrix);
	void UpdateJointToTool(double* matrix);
	void UpdateBaseToTool(vtkMatrix4x4* matrix);
public:
	std::string Name;
	std::string Path;

private:
	vtkSmartPointer<vtkAxesActor> m_AxesActor;
	vtkSmartPointer<vtkMatrix4x4> m_Joint2Tool;
	vtkSmartPointer<vtkMatrix4x4> m_Base2Tool;
	JointPartDescription m_ToolPartDescription;
};

