#pragma once
#ifndef DIANAROBOT_h
#define DIANAROBOT_h

#include <AbstractRobot.h>
#include <PrintDataHelper.h>
#include "DianaAPI.h"
#include "DianaAPIDef.h"
#include "Environment.h"
#include "FunctionOptDef.h"
#include "MitkLancetHardwareDeviceExports.h"
class MITKLANCETHARDWAREDEVICE_EXPORT DianaRobot : public AbstractRobot
{
public:
	DianaRobot();
	void Connect() override;

	void PowerOn() override;

	void PowerOff() override;

	void Translate(double x, double y, double z) override;

	void Translate(double* aDirection, double aLength) override;

	void Rotate(double* aDirection, double aAngle) override;

	void RecordInitialPos() override;

	void GoToInitialPos() override;

	void SetTCPToFlange() override;

	bool SetTCP(vtkMatrix4x4* aMatrix) override;

	std::vector<std::vector<double>> GetJointAngleLimits() override;
	std::vector<double> GetJointAngles() override;

	void SetJointAngles(double* aJointAngles) override;

	vtkSmartPointer<vtkMatrix4x4> GetBaseToTCP() override;
	vtkSmartPointer<vtkMatrix4x4> GetFlangeToTCP() override;
	vtkSmartPointer<vtkMatrix4x4> GetBaseToFlange() override;

	void RobotTransformInBase(double* aMatrix) override;
	void RobotTransformInTCP(double* aMatrix) override;

	std::vector<double> GetCartImpeda() override;
	bool SetCartImpeda(double* aImpeda) override;

	bool SetVelocity(double aVelocity) override;

	void WaitMove() override;

private:
	double m_InitialPos[6] = { 0,0,0,0,0,0 };
};
#endif
