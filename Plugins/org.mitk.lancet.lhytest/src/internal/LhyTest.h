/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef LhyTest_h
#define LhyTest_h

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>
#include "ui_LhyTestControls.h"
#include <robotapi/include/robotapi_Staubli.h>


#include "StaubliRobotDevice.h"
#include "mitkVirtualTrackingDevice.h"
#include "mitkVirtualTrackingTool.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"
#include "lancetPathPoint.h"
#include "mitkTrackingDeviceSource.h"
#include "robotRegistration.h"


/**
  \brief LhyTest

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class LhyTest : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  LhyTest();
public slots:
	void OnConnected();
	void OnDisConnected();
	void OnPowerOn();
	void OnPowerOff();
	void OnSendCommand();
	void OnUpdateToolPosition(QString, LToolAttitudeMessage);


protected:
	// Move Staubli robot
	vtkMatrix4x4* m_initial_robotBaseToFlange;
	bool RecordInitial();
	bool GoToInitial();
	bool InterpretMovementAsInBaseSpace(vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase);
	/// Staubli translation
	bool TranslateX_plus();
	bool TranslateY_plus();
	bool TranslateZ_plus();
	bool TranslateX_minus();
	bool TranslateY_minus();
	bool TranslateZ_minus();

	/// Staubli rotation
	bool RotateX_plus();
	bool RotateY_plus();
	bool RotateZ_plus();
	bool RotateX_minus();
	bool RotateY_minus();
	bool RotateZ_minus();
protected:

  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  /// \brief Called when the user clicks the GUI button
  void DoImageProcessing();

  Ui::LhyTestControls m_Controls;

  //Staubli trackingDeviceSource
  mitk::TrackingDeviceSource::Pointer m_StaubliSource;

protected:
	lancet::StaubliRobotDevice::Pointer m_StaubliTrackingDevice;
	Ui::LhyTestControls m_StaubliControls;

	bool SetPrecisionTestTcp();
	QString sFootAllowed = "0";

	vtkNew<vtkMatrix4x4> m_RobotPosition;
};

#endif // LhyTest_h
