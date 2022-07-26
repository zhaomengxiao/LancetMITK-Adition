/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef DentalWidget_h
#define DentalWidget_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "QmitkDataStorageTreeModel.h"
#include "QmitkSingleNodeSelectionWidget.h"
#include "ui_DentalWidgetControls.h"
#include "vtkPolyData.h"

/**
  \brief DentalWidget

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class DentalWidget : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;
  itkGetMacro(MatrixRegistrationResult, vtkMatrix4x4*);


protected:

	// Variables

	QmitkDataStorageTreeModel* m_NodetreeModel{ nullptr };
	mitk::BaseData* m_baseDataToMove{ nullptr };
	mitk::DataNode* m_currentSelectedNode{ nullptr };

	mitk::DataNode* m_LandmarkSourcePointset{ nullptr };
	mitk::DataNode* m_LandmarkTargetPointset{ nullptr };
	mitk::DataNode* m_IcpTargetPointset{ nullptr };
	mitk::DataNode* m_IcpSourceSurface{ nullptr };
	mitk::DataNode* m_MovingObject{ nullptr };
	Eigen::Matrix4d m_eigenMatrixTmpRegistrationResult;
	Eigen::Matrix4d m_eigenMatrixInitialOffset;

	vtkMatrix4x4* m_MatrixRegistrationResult{ nullptr };
	vtkMatrix4x4* m_InitialMatrix{ nullptr };


  virtual void CreateQtPartControl(QWidget *parent) override;

  /// \brief called by QmitkFunctionality when DataManager's selection has changed
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
	  const QList<mitk::DataNode::Pointer>& nodes) override;

  virtual void SetFocus() override;

  /// \brief Called when the user clicks the GUI button
  //void DoImageProcessing();

  Ui::DentalWidgetControls m_Controls;

  void InitImageSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
  void InitNodeSelector(QmitkSingleNodeSelectionWidget* widget);


  // Toolset 0: extract CT components
  void CheckUseSmoothing();
  bool ReconstructSurface();
  bool AutoReconstructSurface();
  bool GetSteelballCenters();

  // Toolset 1 (Intuitive move operation)
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


	// Surface registration function (landmark + icp)
  void RegisterIos_();
  void ClipTeeth(); // use landmark_src to obtain nearby surface of ios
  void FineTuneRegister_();
  void ResetRegistration_();


	// Extract planning start & end points
  void ExtractAllPlans();
  void ExtractPlan(vtkSmartPointer<vtkPolyData> implant_polydata, vtkSmartPointer<vtkPolyData> teeth_polydata, 
	  double startPoint[3], double endPoint[3]);

};

#endif // DentalWidget_h
