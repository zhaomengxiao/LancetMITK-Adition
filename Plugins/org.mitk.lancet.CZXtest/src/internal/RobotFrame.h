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
class RobotFrame
{
public:
	RobotFrame(mitk::DataStorage* dataStorage, std::vector<RobotJoint*> robotJoints,RobotBase* base, RobotTool* aRobotTool = nullptr);
	void AddRobot(std::string configPath);
	void AddAxis(mitk::IRenderWindowPart* renderWindowPart);
	void UpdateJoints(int jointId, double aPosition);
	void UpdateJoints(double* aPosition);
	void DisplayRobot();
	void HideRobot();
	void DispalyAxes();
	void HideAxes();
	void DisplayTool();
	void HideTool();
	vtkSmartPointer<vtkAxesActor> GetJointAxesActor(int index);
	double* GetCurrentJointAngles();
	double GetJointAngleByIndex(int index);
	void SaveJoints2Links(std::string fileName);
private:
	void UpdateBaseToLink(int jointID, double position);
	void Update(int id = 0);
	void UpdateBaseToTool(int id);
	void UpdateForRender(int startId);
	vtkSmartPointer<vtkMatrix4x4> CalculateJointToLinkMatrix(vtkMatrix4x4* TImage2PreJoint, vtkMatrix4x4* TImage2Joint);

	nlohmann::json MatrixToJsonArray(vtkSmartPointer<vtkMatrix4x4> matrix);

	void SaveMatricesToJsonFile(const std::vector<RobotJoint*>& robotJoints, const std::string& filename);

	vtkSmartPointer<vtkMatrix4x4> JsonArrayToMatrix(const nlohmann::json& jMatrix);

	void LoadMatricesFromJsonFile(const std::string& filename, std::vector<RobotJoint*>& roboJoints);
	
	vtkSmartPointer<vtkMatrix4x4> CalculateTFemurImage2BaseMatrix();
	vtkSmartPointer<vtkMatrix4x4> CalculateTCamera2BaseMatrix();
private:
	std::vector<RobotJoint*> m_RobotJoints;
	mitk::DataStorage* m_dataStorage;
	RobotJoint* m_EndJoint = nullptr;
	RobotBase* m_Base;
	double m_JointAngles[7] = { 0, 0, 0, 0, 0, 0 ,0};
	RobotTool* m_RobotTool = nullptr;
};