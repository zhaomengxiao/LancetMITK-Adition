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

	void SetJointAngles(double* aJointAngles) override;

	vtkSmartPointer<vtkMatrix4x4> GetBaseToTCP() override;

	vtkSmartPointer<vtkMatrix4x4> GetFlangeToTCP() override;

	vtkSmartPointer<vtkMatrix4x4> GetBaseToFlange() override;

	void RobotTransformInBase(double* aMatrix) override;

	void RobotTransformInTCP(double* aMatrix) override;

	//std::vector<double> GetCartStiffParams();

	//void SetCartStiffParams(double* aStiff);

	//std::vector<double> GetCartDampParams();

	//void SetCartDampParams(double* aStiff);

	std::vector<std::vector<double>> GetJointAngleLimits() override;
private:
	Eigen::Matrix3d GetRotationMatrixByEuler(double rx, double ry, double rz);
	vtkSmartPointer<vtkMatrix4x4> GetMatrixByRotationAndTranslation(Eigen::Matrix3d aRotation, Eigen::Vector3d aTranslation);
	Eigen::Vector3d GetEulerByMatrix(vtkMatrix4x4* m);

	Eigen::Matrix3d GetRotationPartByMatrix(vtkMatrix4x4* m);
	Eigen::Vector3d GetTranslationPartByMatrix(vtkMatrix4x4* m);
	Eigen::Vector3d CalculateZYXEulerByRotation(Eigen::Matrix3d m);
private:
	vtkSmartPointer<vtkMatrix4x4> m_InitialPos;
	vtkSmartPointer<vtkMatrix4x4> m_FlangeToTCP;
private:
	// 定义需要转换的空间位置变量
	double dCoord_X = 0; double dCoord_Y = 0; double dCoord_Z = 0;
	double dCoord_Rx = 0; double dCoord_Ry = 0; double dCoord_Rz = 0;
	// 定义工具坐标变量
	double dTcp_X = 0; double dTcp_Y = 0; double dTcp_Z = 0;
	double dTcp_Rx = 0; double dTcp_Ry = 0; double dTcp_Rz = 0;
	// 定义用户坐标变量
	double dUcs_X = 0; double dUcs_Y = 0; double dUcs_Z = 0;
	double dUcs_Rx = 0; double dUcs_Ry = 0; double dUcs_Rz = 0;
	// 定义转换后的空间位置结果
	double dTarget_X = 0; double dTarget_Y = 0; double dTarget_Z = 90;
	double dTarget_Rx = 0; double dTarget_Ry = 90; double dTarget_Rz = 0;

	// 定义工具坐标变量
	string sTcpName = "TCP";
	// 定义用户坐标变量
	string sUcsName = "Base";
	// 定义运动速度
	double dVelocity = 50;
	// 定义运动加速度
	double dAcc = 50;
	// 定义过渡半径
	double dRadius = 50;
	// 定义是否使用关节角度
	int nIsUseJoint = 0;
	// 定义是否使用检测 DI 停止
	int nIsSeek = 0;
	// 定义检测的 DI 索引
	int nIOBit = 0;
	// 定义检测的 DI 状态
	int nIOState = 0;
	// 定义路点 ID 
	string strCmdID = "0";
};

#endif