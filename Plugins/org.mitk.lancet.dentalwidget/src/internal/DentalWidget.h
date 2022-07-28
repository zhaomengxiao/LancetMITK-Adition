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
  

	// Steelball center extraction
  bool GetLooseSteelballCenters(double steelballVoxel); // very loose condition
  void EnhancedGetSteelballCenters(); // stricter condition
  void RemoveRedundantBalls(); // remove redundant centers
  void GetSteelballCenters();
  double GetPointDistance(const mitk::Point3D p0,const mitk::Point3D p1);

  double allBallFingerPrint[42]
  {
	  11.28, 20.77, 16.2, 15.65, 19.96, 10.95,
	  11.28, 16.1, 13.1, 23.54, 28.156, 20.03,
	  20.77, 16.1, 4.71, 22.7, 29.04, 29.86,
	  16.2, 13.1, 4.71, 19.097, 25.65, 25.5,
	  15.56, 23.54, 22.7, 19.097, 7.91, 16.76,
	  19.96, 28.156, 29.04, 25.65, 7.91, 16.386,
	  10.95, 20.03, 29.86, 25.5, 16.76, 16.386
  };

  double stdCenters[21]
  {
	  0, 0, 0,
	  10, -1.5, -5,
	  15.5, 13.5, -3,
	  11.8, 11, -1.5,
	  -7, 14, 0,
	  -13.5, 14, -4.5,
	  -10, -2, -4
  };

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
  void CheckPrecision();

	// Extract planning start & end points
  void ExtractAllPlans();
  void ExtractPlan(vtkSmartPointer<vtkPolyData> implant_polydata, vtkSmartPointer<vtkPolyData> teeth_polydata, 
	  double startPoint[3], double endPoint[3]);

};

#endif // DentalWidget_h
