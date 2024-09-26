#pragma once
#include "PKAData.h"
#include <qobject.h>
#include <QString>
#include <QFileDialog>
#include <QtConcurrent>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qmetaobject.h>
#include <qlabel.h>
#include <qmetaobject.h>
#include <QtWidgets\qfiledialog.h>
#include <QFuture>
#include <mitkNavigationToolStorageDeserializer.h>
#include <mitkMatrixConvert.h>
#include <mitkDataStorage.h>
#include <lancetVegaTrackingDevice.h>
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetTrackingDeviceSourceConfigurator.h"
#include <QmitkToolTrackingStatusWidget.h>
#include <mitkRenderingManager.h>
#include <qmessagebox.h>
#include "CalculationHelper.h"
#include "robotRegistration.h"
#include "FileIO.h"
namespace lancetAlgorithm
{
	class PKAKukaVegaHardwareDevice :public QObject
	{
	public:
		PKAKukaVegaHardwareDevice(mitk::DataStorage* dataStorage);
		void ConnectKuka();
		void StartKukaTracking(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget = nullptr);
		void ShowKukaToolStatus(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget);
		void SelfCheckBtnClicked();

		bool HandlerCameraDisconnection();
		bool HandleRobotDisconnection();

		void ConnectNDI(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget = nullptr);

		void ShowVegaToolStatus(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget);

		void VerifyProbe(QProgressBar* bar, QPushButton* button, QLabel* rmsLabel);


		void UpdateHardware();
		lancet::NavigationObjectVisualizationFilter::Pointer GetRobotVisualizer();
		lancet::NavigationObjectVisualizationFilter::Pointer GetCameraVisualizer();

		Eigen::Vector3d GetToolInCameraPos(std::string toolName);
		void KukaSelfCheck();

		bool Translate(const double axis[3], double length, int type);
		bool InterpretMovementAsInBaseSpace(int moveType, vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase);
		bool Rotate(const double axis[3], double degree, int type);

		void DisplayRobotEndDeviation();


		void AutoRobotRegistration();
		/// <summary>
		/// 这是对于Surgical Simulate 中机械臂自动移动的改写，在原来代码中，点击自动移动按钮之后，再点击Capture
		/// 第一步，第二部机械臂移动到预设的初始位姿，记录Caputr Robot
		/// </summary>
		mitk::AffineTransform3D::Pointer RobotAutoMove();
		bool IsArrivedTarget(mitk::AffineTransform3D::Pointer target);

		void CaptureRobot();
		void OldAutoMove();
		void RecordInitPos();
		void GoToInitPos();
		void CapturePose(bool translationOnly);
		int ResetRobotRegistration();

		/// <summary>
		/// 平均导航数据并计算转换矩阵
		/// </summary>
		/// <param name="ndPtr"></param>
		/// <param name="timeInterval"></param>
		/// <param name="intervalNum"></param>
		/// <param name="matrixArray"></param>
		/// <returns></returns>
		bool AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval, int intervalNum, double matrixArray[16]);
		mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd, mitk::NavigationData::Pointer nd_ref);
		void ReuseRobotRegistation();
		void SaveRobotRegistration();

		void SetTcpToFlange();


	private:
		mitk::DataStorage* m_DataStorage;
		lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
		lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
		std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;
		std::vector<mitk::NavigationData::Pointer> m_KukaNavigationData;
		RobotRegistration m_RobotRegistration;
		QFutureWatcher<void> RobotMoveTaskWatcher;
		vtkMatrix4x4* m_initial_robotBaseToFlange;
		lancet::ApplyDeviceRegistratioinFilter::Pointer m_KukaApplyRegistrationFilter;
		mitk::AffineTransform3D::Pointer m_RobotRegistrationMatrix;
		QTimer* m_KukaVisualizeTimer{ nullptr };
		QTimer* m_ProbeVerifyTimer{ nullptr };
		bool m_IsVerifyProbe = false;
		int m_ProbeRecordCount = 0;
		std::vector<Eigen::Vector3d> m_ProbePosVec;
		QMetaObject::Connection m_ProbeVerifyConnection;
	};
}

