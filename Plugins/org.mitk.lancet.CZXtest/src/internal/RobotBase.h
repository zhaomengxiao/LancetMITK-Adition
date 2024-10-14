#pragma once
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkAxesActor.h>
#include "JointPartDescription.h"
#include "PKARenderHelper.h"
class RobotBase
{
public:
	RobotBase(JointPartDescription description);
	vtkSmartPointer<vtkAxesActor> GetBaseAxesActor();
	JointPartDescription GetDescription();
	void UpdateImage2Base(vtkMatrix4x4* matrix);
	vtkSmartPointer<vtkMatrix4x4> GetImage2Base();
private:
	vtkSmartPointer<vtkAxesActor> axesActor;
	JointPartDescription m_JointPartDescription;
	vtkSmartPointer<vtkMatrix4x4> m_Image2Base;
};

