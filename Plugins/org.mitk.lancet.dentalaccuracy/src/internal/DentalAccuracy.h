/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef DentalAccuracy_h
#define DentalAccuracy_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <vtkPolyData.h>

#include "mitkPointSet.h"
#include "ui_DentalAccuracyControls.h"

/**
  \brief DentalAccuracy

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class DentalAccuracy : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

public:
  static const std::string VIEW_ID;

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  Ui::DentalAccuracyControls m_Controls;

	// Slots

  void on_pushButton_planeAdjust_clicked(); // Use Gizmo

  void on_pushButton_splineAndPanorama_clicked();

  void on_pushButton_viewPano_clicked();

  void on_pushButton_viewCursiveMPR_clicked();

  void valueChanged_horizontalSlider();

  void on_pushButton_CBCTreconstruct_clicked();

  void on_pushButton_iosCBCT_clicked();

  void on_pushButton_setCrown_clicked();

  void on_pushButton_setImplant_clicked();

  void on_pushButton_steelballExtract_clicked();

	// Functions

  void TurnOffAllNodesVisibility();

  void ResetView();

  void ClipTeeth();

  vtkSmartPointer<vtkPolyData> SweepLine_2Sides(vtkPolyData* line, double direction[3],
	  double distance, unsigned int cols);

  vtkSmartPointer<vtkPolyData> ExpandSpline(vtkPolyData* line, int divisionNum,
	  double stepSize);



  void UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters);
  double GetPointDistance(const mitk::Point3D p0, const mitk::Point3D p1);
  bool GetCoarseSteelballCenters(double steelballVoxel);
  void IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7]);
  void ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7]);
  void RemoveRedundantCenters();
  void RearrangeSteelballs(int stdNeighborNum, int foundIDs[7]);


	// Variables
  double allBallFingerPrint[42]
  {
	  0
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

  double midStdCenters[21]
  {
	  -69.07, 27, -32.73,
	-66.59, 19, -36.62,
	-57.75, 13.5, -25.66,
	-60.23, 17.50, -26.02,
	-59.17, 31, -22.83,
	-56.69, 38.50, -26.72,
	-67.65, 33, -36.98
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




};

#endif // DentalAccuracy_h
