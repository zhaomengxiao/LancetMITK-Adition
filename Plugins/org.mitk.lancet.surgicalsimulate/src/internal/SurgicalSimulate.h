/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef SurgicalSimulate_h
#define SurgicalSimulate_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>


#include "kukaRobotDevice.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "mitkTrackingDeviceSource.h"
#include "robotRegistration.h"
#include "ui_SurgicalSimulateControls.h"

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
  void StartTracking();
  void OnKukaVisualizeTimer();
  void OnSelfCheck();
  //Step1:Use a NDI Vega Tracking Device
  void UseVega();
  void OnVegaVisualizeTimer();

  //Step2:Robot Registration;
  void OnRobotCapture();
  void OnAutoMove();
  void OnResetRobotRegistration();

  
 
protected:

  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  

  // [Step2 Robot Registration]
    /// \brief Generate poses that robot needs to move for registration.
    ///We take the present pose of the robot arm as the initial pose, first translating five poses, and then moving five poses with rotation.
    void GeneratePoses();

    void CapturePose(bool translationOnly);

  //*********Helper Function****************
  RobotRegistration m_RobotRegistration;
  mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd, mitk::NavigationData::Pointer nd_ref);

  Ui::SurgicalSimulateControls m_Controls;


  lancet::KukaRobotDevice::Pointer m_KukaTrackingDevice;
  //vega trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  //kuka trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_KukaSource;

  lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  QTimer* m_KukaVisualizeTimer{ nullptr };
  QTimer* m_VegaVisualizeTimer{ nullptr };
  mitk::NavigationToolStorage::Pointer  m_KukaToolStorage;
  mitk::NavigationToolStorage::Pointer  m_VegaToolStorage;

  //robot registration
  unsigned int m_IndexOfRobotCapture{0};
  std::array<vtkMatrix4x4*, 10> m_AutoPoses{};
};

#endif // SurgicalSimulate_h
