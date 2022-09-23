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

  Ui::KukaRobotControlControls m_Controls;

  virtual void CreateQtPartControl(QWidget *parent) override;
	
  virtual void SetFocus() override;
  
  /// Kuka connection
  mitk::NavigationToolStorage::Pointer  m_KukaToolStorage;
  mitk::TrackingDeviceSource::Pointer m_KukaTrackingDeviceSource;
  lancet::KukaRobotDevice::Pointer m_KukaTrackingDevice;
  lancet::NavigationObjectVisualizationFilter::Pointer m_KukaVisualizer;
  QTimer* m_KukaVisualizeTimer{ nullptr };

  bool LoadToolStorage();
  bool ConnectKuka();

  bool StartKukaTracking();

  /// Kuka self check
  bool RobotArmSelfCheck();



	/// Mode 0: robot base space
	///	Mode 1: robot internal TCP (flange) space
	///	Mode 2: .....
       // double m_matrixArray_flangeToSaw[16]{ 1,0,0,0, 0,1,0,0, 0,0,1,0 ,0,0,0,1 };

  int m_movementSpaceMode{0};
  vtkMatrix4x4* m_matrix_flangeSpaceToMovementSpace;
  bool ConfigureflangeToMovementSpace(/*m_movementSpaceMode, m_matrix_robotBaseToMovementSpace*/);

  /// Kuka translation
  bool TranslateX_plus();
  bool TranslateY_plus();
  bool TranslateZ_plus();
  bool TranslateX_minus();
  bool TranslateY_minus();
  bool TranslateZ_minus();

  /// Kuka rotation
  bool RotateX_plus();
  bool RotateY_plus();
  bool RotateZ_plus();
  bool RotateX_minus();
  bool RotateY_minus();
  bool RotateZ_minus();

  // Convert the transform matrix from movement space to robot base space
  bool InterpretMovementAsInBaseSpace(vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase);

  

};

#endif // KukaRobotControl_h
