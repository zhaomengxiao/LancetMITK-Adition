#pragma once
#include <iostream>
#include <AimPositionAPI.h>
#include <QString>
#include <qdialog.h>
#include <QApplication>
#include <QFileDialog>
#include <QTimer>
#include <qthread.h>
#include <QLabel>
#include <mitkIRenderWindowPart.h>
#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkAxesActor.h>
#include <eigen3/Eigen/Dense>
#include <qwidget.h>
#include <qobject.h>
#include "robotRegistration.h"
//Agile Robots Diana7
#include "DianaAPI.h"
#include "DianaAPIDef.h"
#include "Environment.h"
#include "FunctionOptDef.h"
#include "PrintDataHelper.h"

namespace lancetAlgorithm
{
	class DianaAimHardwareService : public QObject
	{
		Q_OBJECT
	public slots:
		void UpdateCamera();
	public:
		DianaAimHardwareService();
		~DianaAimHardwareService();
		void ConnectCamera();
		void StartCamera();
		void InitToolsName(std::vector<std::string> toolsName, std::vector<QLabel*>* labels = nullptr);

		Eigen::Vector3d GetTipByName(std::string aToolName);
		vtkSmartPointer<vtkMatrix4x4> GetMatrixByName(std::string aToolName);

	public:
		void ConnectRobot();
		void RobotPowerOn();
		void RobotPowerOff();
		void SetTCP2Flange();
		void RecordIntialPos();
		void GoToInitPos();
		void Translate(const double x, const double y, const double z);
		void Translate(const double axis[3], double length);
		void Rotate(double x, double y, double z, double angle);
		void Rotate(const double axis[3], double angle);
		void WaitMove(const char* m_RobotIpAddress);
		void RobotTransformInTCP(const double* matrix);
		void RobotTransformInBase(const double* matrix);
		void SetTCP(vtkMatrix4x4* matrix);
		bool SetVelocity(int vel);
		int GetVelocity();
		void StopMove();
		vtkSmartPointer<vtkMatrix4x4> GetRobotBase2RobotEnd();
		vtkSmartPointer<vtkMatrix4x4> GetEnd2TCP();
		vtkSmartPointer<vtkMatrix4x4> GetBase2TCP();
		double* GetRobotImpeda();
		bool SetRobotImpeda(double* aData);
		bool CleanRobotErrorInfo();
		std::vector<double> GetJointsElectricCurrent();
		bool SetPositionMode();
		bool SetJointImpendanceMode();
		bool SetCartImpendanceMode();
		std::vector<std::vector<double>> GetJointsPositionRange();
		std::vector<double> GetJointAngles();
		bool SetJointAngles(double* angles);
		vtkSmartPointer<vtkMatrix4x4> GetPositionPosByJointAngles(double* aDirection, double aLength);
	public:
		void CapturePose(bool translationOnly);
		bool AverageNavigationData(std::string aRFName, int timeInterval, int intervalNum, double matrixArray[16]);
		void AverageNavigationData(double camera2EndRF[16], double camera2BaseRF[16], int timeInterval = 100, int intervalNum = 20);

		int CaptureRobot();
		int ResetRobotRegistration();
		void RobotAutoRegistration();
		vtkSmartPointer<vtkMatrix4x4> GetBaseRF2BaseMatrix();
		vtkSmartPointer<vtkMatrix4x4> GetEnd2EndRFMatrix();
		void SetBaseRF2BaseMatrix(vtkMatrix4x4* aMatrix);
		void SetEnd2EndRFMatrix(vtkMatrix4x4* aMatrix);

	signals:
		void CameraUpdateClock();
	private:
		T_AimToolDataResult* GetNewToolData();
		/*
		* @berief 更新相机到工具矩阵
		* @param ToolData 工具数据指针
		* @param Name 工具名称
		* @param aCamera2Tool 相机到工具矩阵
		* @param label 标签指针
		*/
		bool UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const std::string Name);

		vtkSmartPointer<vtkMatrix4x4> GetMatrixByRotationAndTranslation(Eigen::Matrix3d rotation, Eigen::Vector3d translation);

	private:
		AimHandle m_AimHandle = NULL;
		E_Interface m_EI;
		T_MarkerInfo markerSt;
		T_AimPosStatusInfo statusSt;
		E_ReturnValue rlt;
		QTimer* m_AimoeVisualizeTimer{ nullptr };
		Eigen::Vector3d m_CameraTranslation;
		Eigen::Matrix3d m_CameraRotation;
		Eigen::Vector3d m_ProbeTip;

		bool m_FemurRFValidity = false;
		bool m_TibiaRFValidity = false;
		bool m_ProbeValidity = false;
		double m_InitialPos[6] = { 0,0,0,0,0,0 };
		RobotRegistration m_RobotRegistration;

	private:
		const char* m_RobotIpAddress = "192.168.10.75";
		vtkSmartPointer<vtkMatrix4x4> m_TBaseRF2Base;
		vtkSmartPointer<vtkMatrix4x4> m_TFlange2EndRF;
		std::map<std::string, vtkSmartPointer<vtkMatrix4x4>> m_ReferenceMap;
		std::map<std::string, Eigen::Vector3d> m_ToolTipMap;
		std::map<std::string, QLabel*> m_LabelMap;
		double* m_RobotJointAngles;
	};
}



