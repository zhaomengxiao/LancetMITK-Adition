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

#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "mitkPointSet.h"
#include "mitkTrackingDeviceSource.h"
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

  void on_pushButton_connectVega_clicked();

  void on_pushButton_imageRegis_clicked();

  void on_pushButton_calibrateDrill_clicked();

  void on_pushButton_genSplineAndAppend_clicked();

  void on_pushButton_GenSeeds_clicked();

  void on_pushButton_collectDitch_clicked();

  void on_pushButton_imageRegisNew_clicked();

  void on_pushButton_resetImageRegis_clicked();

  void on_pushButton_implantFocus_clicked();


	// U: up
	// D: down
	// R: right
	// L: left
	// clock: clockwise rotation
	// counter: counterclockwise rotation 
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
	// Functions

  double CalImplantToAlveolarNerve();

  mitk::NavigationData::Pointer DentalAccuracy::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	  mitk::NavigationData::Pointer nd_ref);

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


  void OnVegaVisualizeTimer();
  void ShowToolStatus_Vega();

	// Variables

  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  QTimer* m_VegaVisualizeTimer{ nullptr };
  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;

  // Image registration
  lancet::NavigationObject::Pointer m_NavigatedImage;
  vtkNew<vtkMatrix4x4> m_ImageRegistrationMatrix; // image(surface) to ObjectRf matrix
  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer m_SurfaceRegistrationStaticImageFilter;

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
	  -23.930450476, -87.4997006055, 10.9484740283,
	  -34.990049118, -89.0005671575, 9.31059563815,
	  -38.350507198, -74.0005629566, 4.51922953965,
	  -34.458882368, -76.5002777601, 5.4117977894,
	  -18.196807484, -73.4994482961, 14.9632296697,
	  -15.451925984, -73.4996695078, 22.3771091697,
	  -18.031253332, -89.4997450515, 19.9595375081
	  // 0, 0, 0,
	  // 10, -1.5, -5,
	  // 15.5, 13.5, -3,
	  // 11.8, 11, -1.5,
	  // -7, 14, 0,
	  // -13.5, 14, -4.5,
	  // -10, -2, -4
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

  



  // Rewrite the image registration and navigation part without using the MITK IGT pipeline

  mitk::PointSet::Pointer m_probeDitchPset_cmm; // probe ditch points from CMM
  mitk::PointSet::Pointer m_probeDitchPset_image; // probe ditch points under image frame
  mitk::PointSet::Pointer m_probeDitchPset_rf; // probe ditch points under patientRF

  double CalculateDistance(const mitk::Point3D& point1, const mitk::Point3D& point2);
  mitk::Point3D CalculateMassCenter(const mitk::PointSet::Pointer& pointSet);
  bool ComparePointsByDistance(const mitk::Point3D& massCenter, const mitk::Point3D& point1, const mitk::Point3D& point2);
  void SortPointSetByDistance(mitk::PointSet::Pointer inputPointSet, mitk::PointSet::Pointer outputPointSet);
  void GenerateCombinations(int m, int n, int index, std::vector<int>& currentCombination, std::vector<std::vector<int>>& result);
  std::vector<std::vector<int>> GenerateAllCombinations(int m, int n);


  void on_pushButton_startNavi_clicked();
  void UpdateDrillVisual();

  // Navigation data from the camera
  double m_T_cameraToHandpieceRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
  bool m_Stat_cameraToHandpieceRF{false};
  double m_T_cameraToPatientRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
  bool m_Stat_cameraToPatientRF{false};
  double m_T_cameraToCalibratorRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
  bool m_Stat_cameraToCalibratorRF{ false };

  // Image registration matrix and tool calibration matrix
  bool m_Stat_calibratorRFtoDrill{ true };
  double m_T_calibratorRFtoDrill[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // from hardware design
  bool m_Stat_handpieceRFtoDrill{ false };
  double m_T_handpieceRFtoDrill[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // Perform handpiece calibration to acquire
  bool m_Stat_patientRFtoImage{ false };
  double m_T_patientRFtoImage[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // Perform image registration to acquire  
  


};

#endif // DentalAccuracy_h
