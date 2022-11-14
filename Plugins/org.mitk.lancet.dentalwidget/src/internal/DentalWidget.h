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

#include "mitkPointSet.h"
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
  bool GetCoarseSteelballCenters(double steelballVoxel); // very loose condition
  void ScreenCoarseSteelballCenters(); // strictest condition, each candidate must match the fingerprint 100% to be accepted 
 
  void ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7]);
  void RemoveRedundantCenters(); // remove redundant centers
  void IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7]);
  void RearrangeSteelballs(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7]);

  void GetSteelballCenters_iosCBCT();
  void GetSteelballCenters_modelCBCT();

  double GetPointDistance(const mitk::Point3D p0,const mitk::Point3D p1);
  void UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters);
  void UpdateStdCenters();
  void ResetModelRegistration();

  void RegisterModeltoCBCT();
  void ExtractAllPlans_model();
  vtkMatrix4x4* m_ModelRegistrationResult{ nullptr };
  vtkMatrix4x4* m_ModelInitalMatrix{ nullptr };

  double allBallFingerPrint[42]
  {
	  0
  };

  double stdCenters[21]
  {
	 0
  };

  double iosStdCenters[21]
  {
	  0, 0, 0,
	  10, -1.5, -5,
	  15.5, 13.5, -3,
	  11.8, 11, -1.5,
	  -7, 14, 0,
	  -13.5, 14, -4.5,
	  -10, -2, -4
	// 25.36, -152.54, -21.55,
	// 21.88,  -155.34, -12.45,
	// 21.98, -151.21, -6.14,
	// 26.85,   -140.02, -4.47,
	// 20.11, -133.68, -25.36,
	// 17.35, -145.66, -32.41,
	// 18.93, -150.78, -29.20
  };

 
  double modelStdCenters[21]
  {
	  29.97436, -76.53379, -51.15766,
	  25.42992, -80.65884, -51.80030,
	  8.11930, -86.60384, -47.72123,
	  2.53530, -83.51462, -50.29480,
	  4.02108, -68.11822,-54.55366,
	  13.91066, -68.38903, -56.32448,
	  21.41421, -64.86712, -55.98294
  };

  // Register preop CBCT to intraop CBCT
  bool PrepareCbctImages();
  bool RetrieveRegistrationMatrix();
  double m_preOpCtToIntraOpCtMatrix[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
  bool AppendMatrix_regisCbct();
  mitk::Geometry3D::Pointer InitializeWithImageGeometry(const mitk::BaseGeometry* geometry);
  bool AssembleRegistrationPoints();


  // Intuitive move operation
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


// intraoral scan and CBCT registration: Surface registration function (landmark + icp)
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
