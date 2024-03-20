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
#include "QmitkSingleNodeSelectionWidget.h"

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

	// Connect Staubli
	void OnConnected();
	void OnDisConnected();
	void OnPowerOn();
	void OnPowerOff();
	void OnSendCommand();
	void OnUpdateToolPosition(QString, LToolAttitudeMessage);

	// Connect Vega
	void on_pushButton_connectVega_clicked();
	void OnVegaVisualizeTimer();
	void ShowToolStatus_Vega();

	// Staubli registration
	void on_pushButton_captureRobot_clicked();
	void on_pushButton_resetRobotReg_clicked();
	void on_pushButton_saveRobotRegist_clicked();
	void on_pushButton_usePreRobotRegit_clicked();

    // Image registration
	bool on_pushButton_assembleNavigationObject_clicked();
	bool on_pushButton_applyPreImageRegistrationNew_clicked();
	bool on_pushButton_collectLandmark_clicked();
	bool on_pushButton_collectIcp_clicked();
	bool on_pushButton_applyRegistrationNew_clicked();


    // Set line accuracy TCP
	bool on_pushButton_setTcpPrecisionTest_clicked();

	// Confirm target line
	bool on_pushButton_confirmImageTargetLine_clicked();

    // Go to target position
	bool on_pushButton_startAutoPosition_clicked();

	// show flange position
	void on_pushButton_showFlange_clicked();

	// show tool position
	void on_pushButton_showTool_clicked();


	void UpdateToolVisual();
	void UpdateFlangeVisual();
protected:

	void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
	void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);


	void on_pushButton_recordInitial_clicked();
	void on_pushButton_goToInitial_clicked();
	

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

	virtual void CreateQtPartControl(QWidget* parent) override;

	virtual void SetFocus() override;

	/// \brief called by QmitkFunctionality when DataManager's selection has changed
	virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
		const QList<mitk::DataNode::Pointer>& nodes) override;


	Ui::LhyTestControls m_Controls;

	//Staubli trackingDeviceSource
	mitk::TrackingDeviceSource::Pointer m_StaubliSource;


	lancet::StaubliRobotDevice::Pointer m_StaubliTrackingDevice;
	Ui::LhyTestControls m_StaubliControls;

	QString sFootAllowed = "0";

	vtkNew<vtkMatrix4x4> m_RobotPosition; // current robot base to flange matrix
	vtkNew<vtkMatrix4x4> m_BaseToFlange_target; // target robot pose


	mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
	mitk::TrackingDeviceSource::Pointer m_VegaSource;
	lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
	QTimer* m_VegaVisualizeTimer{ nullptr };
	std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;

	// Robot registration
	void CapturePose(bool translationOnly);


	unsigned int m_IndexOfRobotCapture{ 0 };
	std::array<vtkMatrix4x4*, 10> m_AutoPoses{};
	vtkNew<vtkMatrix4x4> m_RobotRegistrationMatrix; // from robot base RF to robot base
	vtkNew<vtkMatrix4x4> m_FlangeToToolMatrix; // TCP matrix

	RobotRegistration m_RobotRegistration;

	bool AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval /*milisecond*/, int intervalNum, double matrixArray[16]);

	vtkMatrix4x4* getVtkMatrix4x4(mitk::NavigationData::Pointer nd);

	mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
		mitk::NavigationData::Pointer nd_ref);

	// Image registration
	lancet::NavigationObject::Pointer m_NavigatedImage;
	vtkNew<vtkMatrix4x4> m_ImageRegistrationMatrix; // image(surface) to ObjectRf matrix
	lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer m_SurfaceRegistrationStaticImageFilter;


};

#endif // LhyTest_h
