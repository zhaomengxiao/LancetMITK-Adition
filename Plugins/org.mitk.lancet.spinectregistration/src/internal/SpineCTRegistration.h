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

#include "mitkPointSet.h"
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

  // Landmark registration (pointset to pointset)
  void LandmarkRegistration();

  // ICP registration (surface to pointset)
  void IcpRegistration();

  // Update the registration result in the UI
  void UpdateRegistrationMatrixInUI();

  // Apply the calculated registration transform to the moving object
  void TransformSurface();
  void TransformImage();



 

  double* m_G_renderWindow_patientCT_matrix = nullptr; // the image must be named as "Patient_CT"
  double* m_G_patientCT_CTdrf_matrix = nullptr;

   void DEMOregistration();
  
  mitk::DataNode* m_DEMO_Pointset_extractedballs;

  mitk::DataNode* m_DEMO_Pointset_stlballs;
	   

};


// Test function for software-algorithm integration

// The transformation matrix between the following 3 coordinate systems are fixed:
	// - The MITK rendering window
	// - The CT image (contains the steelballs)
	// - The CT DRF (contains both steelballs and optical markers)
// We hope to draw any other NDI tool in the MITK rendering window using its relative location to the CT DRF 
// This function calculated the coordinate system transformation array (MITK rendering window --> any other tool)
// This calculated array can be assigned to a vtkMatrix4X4 by "DeepCopy()" function
// Note: If the "any other tool" is the tracking DRF, whose coordinate system is also fixed within the CT image frame,
// then the CT DRF can be removed after the transformation between the tracking DRF and the MITK rendering window is calculated.  
// The calculated array can be used to further get the transformation between a "new tool (e.g., a guided hand drill)" and the MITK rendering window.
double* GetMatrixAnyToolDrfToRenderingWindow(mitk::PointSet* standardSteelballCenters /*from hardware design*/, mitk::Image* inputCtImage /*should contain steelballs*/, double voxelThreshold /*steelball: 3000*/,
	double facetNumUpperBound /*1440*/, double facetNumLowerBound /*500*/, double arrayNdiCtDrf[16] /*NDI data*/,
	double arrayNdiTrackingDrf[16] /*NDI data*/);




#endif // SpineCTRegistration_h
