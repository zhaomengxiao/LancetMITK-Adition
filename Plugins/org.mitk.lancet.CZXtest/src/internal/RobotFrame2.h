#pragma once
#include <vtkAxesActor.h>
#include <vtkSmartPointer.h>
#include <mitkDataStorage.h>
#include <vtkTransform.h>
#include "PKARenderHelper.h"
#include "RobotBase.h"
#include "RobotJoint.h"
#include "CalculationHelper.h"
#include "PrintDataHelper.h"
#include <mitkILinkedRenderWindowPart.h>
#include "JsonSerailizeHelper.h"
#include "PKAData.h"

class RobotFrame2
{
public:
	RobotFrame2(mitk::DataStorage* dataStorage, std::vector<RobotJoint*> robotJoints, RobotBase* base, RobotTool* aRobotTool = nullptr);
	void Display(mitk::IRenderWindowPart* part);
	void Hide();
	
	void DisplayRobotJoints();
	void HideRobotJoints();
	void DisplayRobotAxes();
	void HideRobotAxes();
	void UpdateJoints(double* jointAngles);
private:
	void UpdateJointToLink(int aJointId, double aPosition);
	void Update(int aJointId = 0);
	void UpdateBaseToLink(int aJointId = 0);
	void Render(int aJointId);

private:
	double FromDegreeToRadian(double Degree);
	void LoadSingleMitkFile(std::string filePath, std::string nodeName);

private:
	std::vector<RobotJoint*> m_RobotJoints;
	mitk::DataStorage* m_dataStorage;
	RobotJoint* m_EndJoint = nullptr;
	RobotBase* m_Base;
	double m_JointAngles[7] = { 0, 0, 0, 0, 0, 0 ,0 };
	RobotTool* m_RobotTool = nullptr;

	bool m_IsFirstDispaly = true;
};

