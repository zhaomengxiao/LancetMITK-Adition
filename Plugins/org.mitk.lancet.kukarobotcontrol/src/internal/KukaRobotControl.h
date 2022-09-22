/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef KukaRobotControl_h
#define KukaRobotControl_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "kukaRobotDevice.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "mitkNavigationToolStorage.h"
#include "mitkTrackingDeviceSource.h"
#include "ui_KukaRobotControlControls.h"

/**
  \brief KukaRobotControl

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class KukaRobotControl : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// Slots

  /// Kuka connection
  bool LoadToolStorage();
  bool ConnectKuka(); // connection and micro-service registration

  /// Kuka movement
  bool RobotArmSelfCheck();


  /// Member variables
  
  Ui::KukaRobotControlControls m_Controls;
  mitk::NavigationToolStorage::Pointer  m_KukaToolStorage;
  mitk::TrackingDeviceSource::Pointer m_KukaTrackingDeviceSource;
  lancet::KukaRobotDevice::Pointer m_KukaTrackingDevice;
  lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;

};

#endif // KukaRobotControl_h
