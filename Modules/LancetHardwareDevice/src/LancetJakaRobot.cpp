#include "LancetJakaRobot.h"

LancetJakaRobot::LancetJakaRobot()
{
	m_InitialPos = vtkSmartPointer<vtkMatrix4x4>::New();
	m_FlangeToTCP = vtkSmartPointer<vtkMatrix4x4>::New();
}

void LancetJakaRobot::Connect()
{
}

void LancetJakaRobot::PowerOn()
{
}

void LancetJakaRobot::PowerOff()
{
}

void LancetJakaRobot::Translate(double x, double y, double z)
{
}

void LancetJakaRobot::Translate(double* aDirection, double aLength)
{
}

void LancetJakaRobot::Rotate(double* aDirection, double aAngle)
{
}

void LancetJakaRobot::RecordInitialPos()
{
}

void LancetJakaRobot::GoToInitialPos()
{
}

void LancetJakaRobot::SetTCPToFlange()
{
}

bool LancetJakaRobot::SetTCP(vtkMatrix4x4* aMatrix)
{
	return false;
}

std::vector<double> LancetJakaRobot::GetJointAngles()
{
	return std::vector<double>();
}

void LancetJakaRobot::SetJointAngles(std::vector<double> aJointAngles)
{
}

vtkSmartPointer<vtkMatrix4x4> LancetJakaRobot::GetBaseToTCP()
{
	return vtkSmartPointer<vtkMatrix4x4>();
}

vtkSmartPointer<vtkMatrix4x4> LancetJakaRobot::GetFlangeToTCP()
{
	return vtkSmartPointer<vtkMatrix4x4>();
}

vtkSmartPointer<vtkMatrix4x4> LancetJakaRobot::GetBaseToFlange()
{
	return vtkSmartPointer<vtkMatrix4x4>();
}

void LancetJakaRobot::RobotTransformInBase(double* aMatrix)
{
}

void LancetJakaRobot::RobotTransformInTCP(double* aMatrix)
{
}

std::vector<double> LancetJakaRobot::GetCartStiffParams()
{
	return std::vector<double>();
}

bool LancetJakaRobot::SetCartStiffParams(std::vector<double> aStiff)
{
	return false;
}

std::vector<double> LancetJakaRobot::GetCartDampParams()
{
	return std::vector<double>();
}

bool LancetJakaRobot::SetCartDampParams(std::vector<double> aDamp)
{
	return false;
}

std::vector<std::vector<double>> LancetJakaRobot::GetJointAngleLimits()
{
	return std::vector<std::vector<double>>();
}

bool LancetJakaRobot::SetVelocity(double aVelocity)
{
	return false;
}

void LancetJakaRobot::WaitMove()
{
}
