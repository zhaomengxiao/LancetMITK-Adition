/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef DianaSeven_h
#define DianaSeven_h
#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>

//Agile Robots Diana7
#include "DianaAPI.h"
#include "DianaAPIDef.h"
#include "Environment.h"
#include "FunctionOptDef.h"

//Eigen
#include <eigen3/Eigen/Dense>

//QT
#include <QTimer>
#include <cstring> // ����memcpy_s������ͷ�ļ�
#include <chrono>
#include <thread>
#include "qstring.h"
#include <qthread.h>
//vega
#include <QtWidgets\qfiledialog.h>
#include "mitkNavigationToolStorageDeserializer.h"
#include <lancetVegaTrackingDevice.h>
#include "lancetTrackingDeviceSourceConfigurator.h"
//Matrix
#include <mitkMatrixConvert.h>
#include "mitkTrackingDeviceSource.h"
#include "mitkVirtualTrackingDevice.h"
#include "DianaAimHardwareService.h"
//#include "PrintDataHelper.h"
#include "ui_DianaSevenControls.h"
#include "PrecisionTab.h"

/**
  \brief DianaSeven

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class DianaSeven : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  //Diana Robot
  void initDianaNet();
  void StopDiana();

  void PosAccuracy();
  void PosRepeatability();

  void move_zzj();
  void move_test();
  void Refmove();
  void wait_move(const char* m_RobotIpAddress);

  void GoInitial();
  void Position1();
  void Position2();
  void Position3();
  void Position4();

  void OpenHandGuiding();
  void closeHandGuiding();
  void changeToCartImpendance();
  void changeToJointImpendance();

  void cleanAllErrorInfo();

  void setTcp();

  //Vega
  void UseVega();
  void OnVegaVisualizeTimer();
  void UpdateToolStatusWidget();
  void ShowToolStatus_Vega();
  void on_pushButton_getProbeInit_clicked();

  void ReciveRobotData();
  void ServoP();
  void OnServoPSendCommand();
  void closeServopOff();

  void ComputeProbeToProbeNewMatrix();


  /////////////////////////////////////////////////////
  void ConnectRobotBtnClicked();
  void PowerOnBtnClicked();
  void PowerOffBtnClicked();
  void ConnectCameraClicked();
  void UpdateCameraBtnClicked();
  void HandleUpdateRenderRequest();

  void ReadRobotJointAnglesBtnClicked();
  void SetRobotJointAnglesBtnClicked();
  void GetRobotJointsLimitBtnClicked();
  void ReadRobotImpedaBtnClicked();
  void SetRobotImpedaBtnClicked();

  void AppendToolTipBtnClicked();
  void AppendToolMatrixBtnClicked();
protected:
  virtual void CreateQtPartControl(QWidget *parent) override;
  virtual void SetFocus() override;
  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;
  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

  void InitGlobalVariable();

  void TargetPointData();
  void InitHardwareDeviceTabConnection();
  void InitRobotRegistrationTabConnection();
  bool Translate(const double axis[3]);
  bool Rotate(const double axis[3]);


  //vega trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_VegaSource;

  QTimer m_ServoPTimer;
  //mitk::TrackingDeviceSource::Pointer m_VegaSource;
  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  QTimer* m_VegaVisualizeTimer{ nullptr };
  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;

  vtkSmartPointer<vtkMatrix4x4> m_ndiToprobe;
  vtkSmartPointer<vtkMatrix4x4> m_currentProbeMatrix;
  vtkSmartPointer<vtkMatrix4x4> m_probeToProbeNewMatrix;
  vtkSmartPointer<vtkMatrix4x4> m_init_robotMatrix;

  // ����ռ�λ�ñ���
  double dX = 0; double dY = 0; double dZ = 0;
  double dRx = 0; double dRy = 0; double dRz = 0;
  std::vector<double> dCoord = { dX, dY, dZ, dRx, dRy, dRz };
  std::vector<double> dTargetPoint = { 0,0,0,0,0,0 };
  lancetAlgorithm::DianaAimHardwareService* m_DianaAimHardwareService;
  std::vector<QLineEdit*> m_JointAngleLineEdits;
  std::vector<QLineEdit*> m_ImpedaLineEdits;
public:
	const char* m_RobotIpAddress = "192.168.10.75";

	double vel = 0.1;
	double acc = 0.2;

	int zv_shaper_order = 0;
	double zv_shaper_frequency = 0;
	double zv_shaper_damping_ratio = 0;
	bool avoid_singular = false;

	double ref_joints[7] = { 0.0 };
	double joints[7] = { 0.0 };

	double pose[6] = {};

	double forward_Matrix[16];//���������ľ���
	double radians = 0;
	double joints_final[7] = {0.0};

	double activeTcpPose[6] = { -113.079,145.354,-97.1639,57.4587,-87.4711,-26.5523 };

	Ui::DianaSevenControls m_Controls;
	PrecisionTab* m_PrecisionTab;
};

#endif // DianaSeven_h
