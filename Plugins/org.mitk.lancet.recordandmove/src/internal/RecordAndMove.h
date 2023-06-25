/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef RecordAndMove_h
#define RecordAndMove_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_RecordAndMoveControls.h"


//#include "kukaRobotDevice.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"
#include "lancetKukaRobotDevice.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetPathPoint.h"
#include "mitkTrackingDeviceSource.h"
#include "mitkVirtualTrackingDevice.h"
#include "robotRegistration.h"
#include "lancetNavigationSceneFilter.h"
#include <mutex>
// ************************* 为什么不可以用 *************************************
//include "udpRobotiInfoClient.h"
/**
  \brief RecordAndMove

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class RecordAndMove : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  

public slots:
	void UseKuka();
	void UseVega();
	void ShowToolStatus_Vega();
	void ShowToolStatus_Kuka();
	void OnKukaVisualizeTimer();
	void OnVegaVisualizeTimer();
	void UpdateToolStatusWidget();

	void SetAsTarget();
	void MoveToHomePosition();

	void CapturePose(bool translationOnly);
	void OnRobotCapture();
	void OnAutoMove();
	void MoveAlongA();
	void MoveAlongB();
	void MoveAlongC();
	void MoveAlongX();
	void MoveAlongY();
	void MoveAlongZ();



	void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
	void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
	bool SetupNavigatedImage();
	bool CollectLandmarkProbe();
	bool CollectIcpProbe();
	bool ApplySurfaceRegistration();
	
	void MovePTPStop();
	void InitCTSteelballCenterSelector(QmitkSingleNodeSelectionWidget* widget);
	bool InterpretImagePoint();
	bool MoveToTargetPoint();
	bool InterpretImageLine();
	bool MoveToTargetLine();
	void OnSaveRobotRegistration();
	void OnUsePreRobotRegistration();
	bool OnUsePreNDIRegistration();

protected:
  lancet::ApplySurfaceRegistratioinFilter::Pointer m_surfaceRegistrationFilter;
  lancet::NavigationObject::Pointer navigatedImage;
  mitk::AffineTransform3D::Pointer m_imageRegistrationMatrix; // image(surface) to Rf matrix
  mitk::Point3D m_pointPositionInNDI;
  mitk::Point3D m_pointPositionInRobotBase;
  std::array<double, 6> CTSteelPointCenterTransformationInRobotBaseFrame;

  mitk::Point3D CTSteelPointCenterPositionInRobotBaseFrame;
  std::array<double, 6> m_Target;
  std::array<double, 6> m_HomePosition = {-751.08,196.235,542.805,-3.0812,-0.21958,3.05353};

  mitk::AffineTransform3D::Pointer m_T_robot;

  bool m_ThreadRecord_Flag;
  std::thread m_ThreadRecord_Handler;

  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  //*********Helper Function****************
  RobotRegistration m_RobotRegistration;

  mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	  mitk::NavigationData::Pointer nd_ref);

  Ui::RecordAndMoveControls m_Controls;

  lancet::KukaRobotDevice_New::Pointer m_KukaTrackingDevice;
  //vega trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  //kuka trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_KukaSource;

  lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  lancet::ApplyDeviceRegistratioinFilter::Pointer m_KukaApplyRegistrationFilter;
  QTimer* m_KukaVisualizeTimer{ nullptr };
  QTimer* m_VegaVisualizeTimer{ nullptr };
  mitk::NavigationToolStorage::Pointer m_KukaToolStorage;
  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;

  //filter test
  mitk::VirtualTrackingDevice::Pointer m_VirtualDevice1;
  mitk::VirtualTrackingDevice::Pointer m_VirtualDevice2;
  mitk::TrackingDeviceSource::Pointer m_VirtualDevice1Source;
  mitk::TrackingDeviceSource::Pointer m_VirtualDevice2Source;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice1Visualizer;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice2Visualizer;
  QTimer* m_VirtualDevice1Timer{ nullptr };
  QTimer* m_VirtualDevice2Timer{ nullptr };
  mitk::NavigationToolStorage::Pointer m_VirtualDevice1ToolStorage;
  mitk::NavigationToolStorage::Pointer m_VirtualDevice2ToolStorage;
  QTimer* m_ToolStatusTimer{ nullptr }; //<<< tracking timer that updates the status widgets

  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;
  std::vector<mitk::NavigationData::Pointer> m_KukaNavigationData;

  //fri test
  // lancet::FriManager m_FriManager;
  std::thread m_friThread;
  mitk::AffineTransform3D::Pointer m_ProbeRealTimePose;
  mitk::AffineTransform3D::Pointer m_ProbeInitPose;
  mitk::AffineTransform3D::Pointer T_probe2robotEndRF;
  bool m_KeepUpdateFriTransform{ true };
  bool preciousHandGuiding_select = false;
  bool m_handGuidingOn = true;
  //double m_offset[3]{ 0,0,0 };

  //use navigation scene filter
  lancet::NavigationSceneFilter::Pointer m_NavigationSceneFilter;
  NavigationScene::Pointer m_NavigationScene;

  //robot registration
  unsigned int m_IndexOfRobotCapture{ 0 };
  std::array<vtkMatrix4x4*, 10> m_AutoPoses{};
  mitk::AffineTransform3D::Pointer m_RobotRegistrationMatrix;//
};

#endif // RecordAndMove_h
