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

  // NDI tool design

  int metric[3]{0, 0, 0}; // a metric to measure whether the new tool meets all 3 design requirements ( {1, 1, 1} )

  double inputTool[8] // {x0, x1, x2, x3, y0, y1, y2, y3}
  {
    0, 60, 0, 50,
    70, 0, 0, 85
  };

  // {x0, x1, x2, x3, y0, y1, y2, y3}
  // double optimizedTool[8]{0}; 


  // {x0, x1, ...xN, y0, y1, ...yN}
  double existingTools[16]{0, 70, 60, 0, 0, 0, 50, 50, 0, 0, 70, 0, 70, 51.97, 120, 25.9807};

  int existingToolNum{2};

  void CheckToolValidity();
  void UpdateTool();
  void OptimizeTool();

};

#endif // SpineCTRegistration_h
