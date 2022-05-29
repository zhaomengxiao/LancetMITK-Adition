/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef SpineCTRegistration_h
#define SpineCTRegistration_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_SpineCTRegistrationControls.h"

// #include <eigen3/Eigen/Eigen>

/**
  \brief SpineCTRegistration

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class SpineCTRegistration : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;
  virtual void SetFocus() override;
  void InitNodeSelector(QmitkSingleNodeSelectionWidget *widget);

  // Member variables
  Ui::SpineCTRegistrationControls m_Controls;
  mitk::DataNode *m_CtImageDataNode{nullptr};
  mitk::DataNode *m_MovingSurfaceDataNode{nullptr};
  mitk::DataNode *m_LandmarkSrcPointsetDataNode{nullptr};
  mitk::DataNode *m_LandmarkTargetPointsetDataNode{nullptr};
  mitk::DataNode *m_IcpSrcSurfaceDataNode{nullptr};
  mitk::DataNode *m_IcpTargetPointsetDataNode{nullptr};

  Eigen::Matrix4d m_TmpRegistrationResult;
    

  // QT slots

  // User selects a node
  void ChangeCtImage(QmitkSingleNodeSelectionWidget::NodeList);
  void ChangeMovingSurface(QmitkSingleNodeSelectionWidget::NodeList);
  void ChangeLandmarkSrcPointset(QmitkSingleNodeSelectionWidget::NodeList);
  void ChangeLandmarkTargetPointset(QmitkSingleNodeSelectionWidget::NodeList);
  void ChangeIcpSrcSurface(QmitkSingleNodeSelectionWidget::NodeList);
  void ChangeIcpTargetPointset(QmitkSingleNodeSelectionWidget::NodeList);
  void SpineCTRegistration::ChangeDEMO_Pointset_extractedballs(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/);
  void SpineCTRegistration::ChangeDEMO_Pointset_stlballs(QmitkSingleNodeSelectionWidget::NodeList /*nodes*/);

  // Extract steelball centers as a pointset
  void GetSteelballCenters();

  // Coarse reconstruction of the vertebrae
  void ReconstructSpineSurface();

  // Reset the origin of an mitk::Image to (0, 0, 0), realign the image's axes to the standard xyz axes
  void ResetImage();

  // Landmark registration (pointset to pointset)
  void LandmarkRegistration();

  // ICP registration (surface to pointset)
  void IcpRegistration();

  // Update the registration result in the UI
  void UpdateRegistrationMatrixInUI();

  // Apply the calculated registration transform to the moving object
  void TransformSurface();
  void TransformImage();



  // DEMO Spine IGT in MITK
  // double * m_G_NDIcamera_CTdrf_matrix[16];
  // double * m_G_NDIcamera_trackingDRF_matrix[16];
	// double * m_G_NDIcamera_tool_matrix[16];

  double* m_G_renderWindow_patientCT_matrix = nullptr; // the image must be named as "Patient_CT"
  double* m_G_patientCT_CTdrf_matrix = nullptr;

 // Get m_G_patientCT_CTdrf_matrix, and move patientCT, so that F_tracking_DRF becomes the rendering window coordinate system
  void DEMOregistration();
  
  mitk::DataNode* m_DEMO_Pointset_extractedballs;

  mitk::DataNode* m_DEMO_Pointset_stlballs;




};

#endif // SpineCTRegistration_h
