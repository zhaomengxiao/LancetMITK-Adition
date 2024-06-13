/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef MoveData_h
#define MoveData_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "QmitkDataStorageTreeModel.h"
#include "QmitkSingleNodeSelectionWidget.h"
#include "ui_MoveDataControls.h"
#include "mitkGizmo.h"
#include "mitkPointSet.h"
#include "vtkSelectEnclosedPoints.h"

/**
  \brief MoveData

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class MoveData : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
                                  const QList<mitk::DataNode::Pointer> &nodes) override;

  Ui::MoveDataControls m_Controls;


  // Variables

  QmitkDataStorageTreeModel* m_NodetreeModel{ nullptr };
  mitk::BaseData* m_baseDataToMove{ nullptr };
  mitk::DataNode* m_currentSelectedNode{ nullptr };


  mitk::DataNode* m_DirectionPointset{ nullptr };
  mitk::DataNode* m_LandmarkSourcePointset{ nullptr };
  mitk::DataNode* m_LandmarkTargetPointset{ nullptr };
  mitk::DataNode* m_IcpTargetPointset{ nullptr };
  mitk::DataNode* m_IcpSourceSurface{ nullptr };
  mitk::DataNode* m_MovingObject{ nullptr };
  Eigen::Matrix4d m_eigenMatrixTmpRegistrationResult;

	// slots
  void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitNodeSelector(QmitkSingleNodeSelectionWidget* widget);
  void PointSetDirectionChanged(QmitkSingleNodeSelectionWidget::NodeList);
  void PointSetLandmarkSourceChanged(QmitkSingleNodeSelectionWidget::NodeList);
  void PointSetLandmarkTargetChanged(QmitkSingleNodeSelectionWidget::NodeList);
  void PointSetIcpTargetChanged(QmitkSingleNodeSelectionWidget::NodeList);
  void SurfaceIcpSourceChanged(QmitkSingleNodeSelectionWidget::NodeList);
  void MovingObjectChanged(QmitkSingleNodeSelectionWidget::NodeList);

	// Toolset 1 (Intuitive)
  void Translate(double direction[3], double length, mitk::BaseData* data);
  void Rotate(double center[3], double direction[3], double counterclockwiseDegree, mitk::BaseData* data);
  void TranslatePlusX();
  void TranslatePlusY();
  void TranslatePlusZ();
  void TranslateMinusX();
  void TranslateMinusY();
  void TranslateMinusZ();
  void RotatePlusX();
  void RotatePlusY();
  void RotatePlusZ();
  void RotateMinusX();
  void RotateMinusY();
  void RotateMinusZ();

  void TranslatePlus();
  void TranslateMinus();
  void RotatePlus();
  void RotateMinus();


  // Toolset 2 (Matrix)
  void ClearMatrixContent();
  void OverwriteOffsetMatrix();
  void AppendOffsetMatrix();
  vtkMatrix4x4* ObtainVtkMatrixFromUi();
  void RealignImage(); //Realign the image and the rendering window coordinate systems (keep the spacing)
  void GetObjectGeometryWithSpacing();
  void GetObjectGeometryWithoutSpacing();



  // Toolset 3 (SurfaceRegistration)
  void UpdateUiRegistrationMatrix();
  void LandmarkRegistration();
  void IcpRegistration();
  vtkMatrix4x4* ObtainVtkMatrixFromRegistrationUi();
  void AppendRegistrationMatrix();

  void SurfaceToSurfaceIcp();

  void on_pushButton_hardenData_clicked();

  // Test cutting
  mitk::DataNode::Pointer m_growingCutterNode{nullptr};
  void TestCut();

  void TestCut2();

  void TestCut3();

  void on_pushButton_ApplyStencil_clicked();

  void on_pushButton_implantStencil_clicked();

  void on_pushButton_level_clicked();

  void on_pushButton_combine_clicked();

  void on_pushButton_addGizmo_clicked();

  void on_pushButton_testCrosshair_clicked();

  void horizontalSlider_testCrosshair_value_changed();

  void on_pushButton_testCPR_clicked();

  void on_pushButton_getCameraAngle_clicked();

  void on_pushButton_testStencil_clicked();

  void on_pushButton_gen5planes_clicked();

  void on_pushButton_testIntersect_clicked();
	// Generate a plane vtkPolyData centered at the Pset's center with the given size
  bool GeneratePlaneWithPset(mitk::PointSet::Pointer ptsOnPlane, double planeSize, vtkSmartPointer<vtkPolyData> generatedPlane, double planeNormal[3]);

  void on_pushButto_interpolation_clicked();

  void on_pushButton_testClip_clicked();

  void on_pushButton_vtkBool_clicked();

  void on_pushButton_inverNormal_clicked();

  void on_pushButton_fixhole_clicked();

  void on_pushButton_smooth_clicked();

  void on_pushButton_warp_clicked();

  void on_pushButton_intersect_clicked();

  void on_pushButton_union_clicked();

  int on_pushButton_diff_clicked(); // 0: has result ; 1: no contact ; 2: input missing ; 3: other vtkbool error

  int on_pushButton_implicitClip_clicked(); // 0: has contact and clipped something ; 1: has no contact ; 2: input missing

  void on_pushButton_initTHAcutting_clicked();

  void on_pushButton_testCut_clicked();

  void on_pushButton_gen3Region_clicked();

  void on_pushButton_testCutV2_clicked();

  void on_pushButton_colorPolyData_clicked(); 

  void on_pushButton_debugRenderer_clicked();

};

#endif // MoveData_h
