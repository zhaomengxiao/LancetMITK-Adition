/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef SpineDemo_h
#define SpineDemo_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "mitkTrackingDeviceSource.h"
#include "ui_SpineDemoControls.h"

/**
  \brief SpineDemo

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class SpineDemo : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  Ui::SpineDemoControls m_Controls;

  void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);

  void on_pushButton_connectCamera_clicked();
  void OnVegaVisualizeTimer();
  void ShowToolStatus_Vega();

  void on_pushButton_initCheckPoint_3d_clicked();
  mitk::NavigationData::Pointer GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	  mitk::NavigationData::Pointer nd_ref);

	// image registration
  void on_pushButton_detectBalls_3d_clicked();

	// planning
  void on_pushButton_startPlan_3d_clicked();
  void on_pushButton_U_ax_clicked();
  void on_pushButton_D_ax_clicked();
  void on_pushButton_L_ax_clicked();
  void on_pushButton_R_ax_clicked();
  void on_pushButton_counter_ax_clicked();
  void on_pushButton_clock_ax_clicked();

  void on_pushButton_U_sag_clicked();
  void on_pushButton_D_sag_clicked();
  void on_pushButton_L_sag_clicked();
  void on_pushButton_R_sag_clicked();
  void on_pushButton_counter_sag_clicked();
  void on_pushButton_clock_sag_clicked();

  void on_pushButton_U_cor_clicked();
  void on_pushButton_D_cor_clicked();
  void on_pushButton_L_cor_clicked();
  void on_pushButton_R_cor_clicked();
  void on_pushButton_counter_cor_clicked();
  void on_pushButton_clock_cor_clicked();

  void MoveImplantToTrajectory();

  void FocusOnImplant();


  void RoboticsCalibrationByFoot();

	// Variables

  // Camera connection
  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  QTimer* m_VegaVisualizeTimer{ nullptr };
  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;

  // Image registration
  lancet::NavigationObject::Pointer m_NavigatedImage;
  vtkNew<vtkMatrix4x4> m_ImageRegistrationMatrix; // image(surface) to ObjectRf matrix
  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer m_SurfaceRegistrationStaticImageFilter;

  // Patient check point
  double m_InitPatientCheckpoint[3]{ 0,0,0 };


  

};

#endif // SpineDemo_h
