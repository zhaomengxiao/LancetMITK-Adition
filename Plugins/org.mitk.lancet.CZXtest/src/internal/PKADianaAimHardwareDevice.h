//#pragma once
//#include <iostream>
//#include <AimPositionAPI.h>
//#include <QString>
//#include <qdialog.h>
//#include <QApplication>
//#include <QFileDialog>
//#include <QTimer>
//#include <QLabel>
//#include <mitkIRenderWindowPart.h>
//#include <vtkNew.h>
//#include <vtkMatrix4x4.h>
//#include <vtkAxesActor.h>
//#include <eigen3/Eigen/Dense>
//#include "CalculationHelper.h"
//#include "PKAData.h"
//#include <qwidget.h>
//#include <qobject.h>
//#include "PrintDataHelper.h"
//#include "robotRegistration.h"
//#include "PKARenderHelper.h"
////Agile Robots Diana7
//#include "org_mitk_lancet_CZXtest_Export.h"
//namespace lancetAlgorithm
//{
//	class CZXTEST_EXPORT PKADianaAimHardwareDevice : public QObject
//	{
//		Q_OBJECT
//	public slots:
//		void UpdateCamera();
//	public:
//		PKADianaAimHardwareDevice();
//		void ConnectCamera();
//		
//		void InitQLabels(std::vector<QLabel*> labels);
//
//		Eigen::Vector3d GetProbeTip();
//		Eigen::Vector3d GetDrillEndInCamera();
//		bool IsFemurRFValid();
//		bool IsProbeValid();
//		bool IsTibiaRFValid();
//
//	public:
//		void ConnectRobot();
//		void RobotPowerOn();
//		void RobotPowerOff();
//		void SetTCP2Flange();
//		void RecordIntialPos();
//		void GoToInitPos();
//		void Translate(const double x, const double y, const double z);
//		void Translate(const double axis[3], double length);
//		void Rotate(double x, double y, double z, double angle);
//		void Rotate(const double axis[3], double angle);
//		void WaitMove(const char* m_RobotIpAddress);
//		void RobotTransformInTCP(const double* matrix);
//		void RobotTransformInBase(const double* matrix);
//		void SetTCP(vtkMatrix4x4* matrix);
//		vtkSmartPointer<vtkMatrix4x4> CalculateFlangeToDrillEnd();
//		bool SetVelocity(int vel);
//		int GetVelocity();
//		void StopMove();
//		vtkSmartPointer<vtkMatrix4x4> GetRobotBase2RobotEnd();
//		vtkSmartPointer<vtkMatrix4x4> GetEnd2TCP();
//		vtkSmartPointer<vtkMatrix4x4> GetBase2TCP();
//		void DisplayBase2TCP(mitk::IRenderWindowPart* renderPart);
//		void HideBase2Tcp(mitk::IRenderWindowPart* renderPart);
//		void UpdateTCPAxesActor();
//		double* GetRobotImpeda();
//		bool SetRobotImpeda(double* aData);
//		bool CleanRobotErrorInfo();
//		std::vector<double> GetJointsElectricCurrent();
//		bool SetPositionMode();
//		bool SetJointImpendanceMode();
//		bool SetCartImpendanceMode();
//		std::vector<std::vector<double>> GetJointsPositionRange();
//		std::vector<double> GetJointAngles();
//		bool SetJointAngles(double* angles);
//		std::vector<std::vector<double>> GetRobotDH();
//	public:
//		void CapturePose(bool translationOnly);
//		bool AverageNavigationData(vtkMatrix4x4* TCamera2RF, int timeInterval, int intervalNum, double matrixArray[16]);
//		int CaptureRobot();
//		int ResetRobotRegistration();
//		void RobotAutoMove();
//		void RobotAutoRegistration();
//
//
//	private:
//		T_AimToolDataResult* GetNewToolData();
//		/*
//		* @berief 更新相机到工具矩阵
//		* @param ToolData 工具数据指针
//		* @param Name 工具名称
//		* @param aCamera2Tool 相机到工具矩阵
//		* @param label 标签指针
//		*/
//		bool UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const char* Name, vtkMatrix4x4*, QLabel* label = nullptr);
//	private:
//		AimHandle m_AimHandle = NULL;
//		E_Interface m_EI;
//		T_MarkerInfo markerSt;
//		T_AimPosStatusInfo statusSt;
//		E_ReturnValue rlt;
//		QTimer* m_AimoeVisualizeTimer{ nullptr };
//		Eigen::Vector3d m_CameraTranslation;
//		Eigen::Matrix3d m_CameraRotation;
//		std::vector<QLabel*> m_Labels;
//		Eigen::Vector3d m_ProbeTip;
//		Eigen::Vector3d m_DrillEndPoints = Eigen::Vector3d(230.492, -85, -49.800);
//		bool m_FemurRFValidity = false;
//		bool m_TibiaRFValidity = false;
//		bool m_ProbeValidity = false;
//		double m_InitialPos[6] = {0,0,0,0,0,0};
//		RobotRegistration m_RobotRegistration;
//	private:
//		const char* m_RobotIpAddress = "192.168.10.75";
//		vtkSmartPointer<vtkAxesActor> m_RobotTCPAxesActor;
//		vtkSmartPointer<vtkMatrix4x4> m_TBaseRF2Base;
//		vtkSmartPointer<vtkMatrix4x4> m_TFlange2EndRF;
//	};
//}
//
