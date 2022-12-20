/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef SurgicalSimulate_h
#define SurgicalSimulate_h

#include <QmitkAbstractView.h>


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
#include "ui_SurgicalSimulateControls.h"
#include <mutex>

/**
  \brief SurgicalSimulate

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class SurgicalSimulate : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;


public slots:
  //Step1:Use a kuka Tracking Device
  void UseKuka();
  void DisConnectKuka();
  //
  void StartTracking();
  void OnKukaVisualizeTimer();
  void OnSelfCheck();
  //Step1:Use a NDI Vega Tracking Device
  void UseVega();
  void DisConnectVega();
  void OnVegaVisualizeTimer();

  //Step2:Robot Registration;
  void OnRobotCapture();
  void OnAutoMove();
  void OnResetRobotRegistration();
  void OnSaveRobotRegistraion();
  void OnUsePreRobotRegitration();
  void OnSetTCP();

  //Step3:NavigationObject Registration

  //Step4: Create Surgical Plane
  void OnCaptureProbeAsSurgicalPlane();

  //Step5: Run the Surgical Plane by robot auto move to position
  void OnAutoPositionStart();

  //TEST
  void UseVirtualDevice1();
  void OnVirtualDevice1VisualizeTimer();
  void UseVirtualDevice2();
  void OnVirtualDevice2VisualizeTimer();

  //ShowToolStatus
  void ShowToolStatus_Vega();
  void ShowToolStatus_Kuka();
  void UpdateToolStatusWidget();

  //RobotControl
  void SendCommand();

  void StartServo();
  void StopServo();
  void InitProbe();

  void BindRobotToProbe();
protected:
  void threadUpdateFriTransform();

  virtual void CreateQtPartControl(QWidget* parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer>& nodes) override;


  // [Step2 Robot Registration]
  /// \brief Generate poses that robot needs to move for registration.
  ///We take the present pose of the robot arm as the initial pose, first translating five poses, and then moving five poses with rotation.
  void GeneratePoses();

  void CapturePose(bool translationOnly);


  //*********Helper Function****************
  RobotRegistration m_RobotRegistration;
  
  mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
                                                       mitk::NavigationData::Pointer nd_ref);
public:
  ~SurgicalSimulate() override;
protected:
  Ui::SurgicalSimulateControls m_Controls;


  lancet::KukaRobotDevice_New::Pointer m_KukaTrackingDevice;
  //vega trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  //kuka trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_KukaSource;

  lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  lancet::ApplyDeviceRegistratioinFilter::Pointer m_KukaApplyRegistrationFilter;
  QTimer* m_KukaVisualizeTimer{nullptr};
  QTimer* m_VegaVisualizeTimer{nullptr};
  mitk::NavigationToolStorage::Pointer m_KukaToolStorage;
  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;

  //robot registration
  unsigned int m_IndexOfRobotCapture{0};
  std::array<vtkMatrix4x4*, 10> m_AutoPoses{};
  mitk::AffineTransform3D::Pointer m_RobotRegistrationMatrix;//

  //surgical plane
  lancet::PointPath::Pointer m_SurgicalPlan;
  mitk::AffineTransform3D::Pointer m_T_robot;

  //filter test
  mitk::VirtualTrackingDevice::Pointer m_VirtualDevice1;
  mitk::VirtualTrackingDevice::Pointer m_VirtualDevice2;
  mitk::TrackingDeviceSource::Pointer m_VirtualDevice1Source;
  mitk::TrackingDeviceSource::Pointer m_VirtualDevice2Source;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice1Visualizer;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VirtualDevice2Visualizer;
  QTimer* m_VirtualDevice1Timer{nullptr};
  QTimer* m_VirtualDevice2Timer{nullptr};
  mitk::NavigationToolStorage::Pointer m_VirtualDevice1ToolStorage;
  mitk::NavigationToolStorage::Pointer m_VirtualDevice2ToolStorage;
  QTimer* m_ToolStatusTimer{nullptr}; //<<< tracking timer that updates the status widgets

  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;
  std::vector<mitk::NavigationData::Pointer> m_KukaNavigationData;

  //fri test
  // lancet::FriManager m_FriManager;
  std::thread m_friThread;
  mitk::AffineTransform3D::Pointer m_ProbeRealTimePose;
  mitk::AffineTransform3D::Pointer m_ProbeInitPose;
  mitk::AffineTransform3D::Pointer T_probe2robotEndRF;
  bool m_KeepUpdateFriTransform{true};
  double m_offset[3]{ 0,0,0 };
  // Image registration using NavigationObject structure
 
	// Assemble a navigationObject with a Parent node;
	// the name of the Parent node becomes the ReferencFrameName of the navigationObject
	// the Parent node contains the surface for visualization and the following sub-nodes:
	// 1. "surfaceToRf_matrix": optional; surface frame to reference frame transformation;
	//   uses a point-set containing 4 points (4 columns of the matrix) to represent;
	//   if empty, construct an identity matrix as default in NavigationObject and a subNode point-set as well;
	// 2. "surface_backup": optional; a backup of the original surface data;
	//   if empty, duplicate a surface as a subNode
	// 3. "landmark_surface": compulsory, landmark points on the surface in the surface frame
	//   if empty, prompt warning
	// 4. "landmark_rf": optional, collected landmark points in the reference frame;
	//   if empty, create an empty point-set in NavigationObject
	// 5. "icp_surface": optional, icp points on the surface in the surface frame
	//   if empty, do nothing
	// 6. "icp_rf": optional, collected icp points in the reference frame, only for visualization purposes;
	//   if empty, do nothing
  bool AssembleNavigationObjectFromDataNode(mitk::DataNode* parentNode, lancet::NavigationObject* assembledObject);

  bool RetoreDataNodeFromNavigationObject(mitk::DataNode* parentNode, lancet::NavigationObject* assembledObject);

  lancet::ApplySurfaceRegistratioinFilter::Pointer m_surfaceRegistrationFilter;
  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer m_surfaceRegistrationStaticImageFilter;

  void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);

  lancet::NavigationObject::Pointer navigatedImage;
  mitk::AffineTransform3D::Pointer m_imageRegistrationMatrix; // image(surface) to Rf matrix


  bool SetupNavigatedImage();
  bool CollectLandmarkProbe();
  bool CollectIcpProbe();
  bool ApplySurfaceRegistration();
  bool ApplyPreexistingImageSurfaceRegistration();

  bool ApplySurfaceRegistration_staticImage();
  bool ApplyPreexistingImageSurfaceRegistration_staticImage();

  // Get the coordinate of the image point in the robot (internal) base frame
  bool InterpretImagePoint();

  // Get the coordinate of the image line in the robot (internal) base frame
  bool InterpretImageLine();


  // Use butterfly to calibrate the probe
  bool TouchProbeCalibrationPoint1();
  bool TouchProbeCalibrationPoint2();
  bool TouchProbeCalibrationPoint3();
  double m_probeOffset[3]{ 0,0,0 };

  // Collect and evaluate image checkPoint after image registration
  bool ProbeImageCheckPoint();

  // Check the distance between the probe and image after image registration
  bool ProbeSurface();

};

#endif // SurgicalSimulate_h
