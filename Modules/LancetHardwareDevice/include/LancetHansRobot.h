#pragma once
#ifndef LANCETHANSROBOT_h
#define LANCETHANSROBOT_h

#include "AbstractRobot.h"
#include <HR_Pro.h>
#include <vtkMath.h>
#include "MitkLancetHardwareDeviceExports.h"
class MITKLANCETHARDWAREDEVICE_EXPORT LancetHansRobot : public AbstractRobot
{
	//Q_OBJECT
public:
	LancetHansRobot();
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

	std::vector<double> GetJointAngles() override;

	void SetJointAngles(std::vector<double> aJointAngles) override;

	vtkSmartPointer<vtkMatrix4x4> GetBaseToTCP() override;

	vtkSmartPointer<vtkMatrix4x4> GetFlangeToTCP() override;

	vtkSmartPointer<vtkMatrix4x4> GetBaseToFlange() override;

	void RobotTransformInBase(double* aMatrix) override;

	void RobotTransformInTCP(double* aMatrix) override;

	std::vector<double> GetCartStiffParams() override;

	bool SetCartStiffParams(std::vector<double> aStiff) override;

	std::vector<double> GetCartDampParams() override;

	bool SetCartDampParams(std::vector<double> aDamp) override;

	std::vector<std::vector<double>> GetJointAngleLimits() override;

	bool SetVelocity(double aVelocity) override;

	void WaitMove() override;
private:
	Eigen::Matrix3d GetRotationMatrixByEuler(double rx, double ry, double rz);
	vtkSmartPointer<vtkMatrix4x4> GetMatrixByRotationAndTranslation(Eigen::Matrix3d aRotation, Eigen::Vector3d aTranslation);
	Eigen::Vector3d GetEulerByMatrix(vtkMatrix4x4* m);

	Eigen::Matrix3d GetRotationPartByMatrix(vtkMatrix4x4* m);
	Eigen::Vector3d GetTranslationPartByMatrix(vtkMatrix4x4* m);
	Eigen::Vector3d CalculateZYXEulerByRotation(Eigen::Matrix3d m);

	std::vector<double> CalculateInverse(Eigen::Vector3d aTranslation, Eigen::Vector3d aEulerAngle);

	std::vector<double> CalculateForward(std::vector<double> aJointAngles);
private:
	vtkSmartPointer<vtkMatrix4x4> m_InitialPos;
	vtkSmartPointer<vtkMatrix4x4> m_FlangeToTCP;
private:
	// ���幤���������
	string sTcpName = "TCP";
	// �����û��������
	string sUcsName = "Base";
	// �����˶��ٶ�
	double dVelocity = 50;
	// �����˶����ٶ�
	double dAcc = 50;
	// ������ɰ뾶
	double dRadius = 50;
	// �����Ƿ�ʹ�ùؽڽǶ�
	int nIsUseJoint = 1;
	// �����Ƿ�ʹ�ü�� DI ֹͣ
	int nIsSeek = 0;
	// ������� DI ����
	int nIOBit = 0;
	// ������� DI ״̬
	int nIOState = 0;
	// ����·�� ID 
	string strCmdID = "0";

	int nMoveType = 0;
};

#endif