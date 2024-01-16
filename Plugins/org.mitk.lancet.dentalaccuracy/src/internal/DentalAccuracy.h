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

  DentalAccuracy();

protected:
  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override;

  Ui::DentalAccuracyControls m_Controls;

	// Slots

  void on_pushButton_testMoveImplant_clicked();
  bool ObtainImplantExitEntryPts(mitk::Surface::Pointer implantSurface, mitk::PointSet::Pointer exitEntryPts);
  bool CutPolyDataWithPlane(vtkSmartPointer<vtkPolyData> dataToCut,
	  vtkSmartPointer<vtkPolyData> largerSubPart,
	  vtkSmartPointer<vtkPolyData> smallerSubPart,
	  double planeOrigin[3], double planeNormal[3]);

  void on_pushButton_followAbutment_clicked();
  void on_pushButton_followCrown_clicked();
  void on_pushButton_implantTipExtract_clicked();
  void on_pushButton_implantToCrown_clicked();
  void on_pushButton_abutmentToImplant_clicked();

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

  void on_comboBox_plate_changed(int index);

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

  // Project the implant head and tail points onto the "Panorama"
  void ProjectImplantOntoPanorama();

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
  void IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs);
  void ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs);
  void RemoveRedundantCenters();
  void RearrangeSteelballs(int stdNeighborNum, std::vector<int>& foundIDs);


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


  // SteelBall extraction and dental splint movement 
  std::vector<double> allBallFingerPrint;
 
  std::vector<double> stdCenters;
  std::vector<double> stdCentersA
  {
	  11.48, 5.06, 1.70,
	  4.98, 5.03, 5.70,
	  -1.52, 5.01, 0.70,
	  -0.28, 13.93, 0.70,
	  17.67, 13.39, 1.70,
	  8.22, 24.49, 2.70,
	  17, 31.87, 0.70,
	  36.57, 23.60, 1.70,
	  32.02, 32.03, 5.70,
	  27.38, 40.61, 0.70
  };

  std::vector<double> stdCentersB
  {
	  11.50, 0.70, -5.02,
	  5, 5.70, -5.02,
	  -1.50, 1.70, -5.02,
	  10.23, 0.70, -13.93,
	  -7.72, 1.70, -13.33,
	  1.70, 2.70, -24.46,
	  -7.10, 0.70, -31.81,
	  -17.52, 0.70, -40.51,
	  -22.12, 5.70, -31.92,
	  -26.65, 1.70, -23.47
  };
  std::vector<double> stdCentersC
  {
	  16.77, 2.30, -3.12,  //p1
	  29.96, 3.30, -12.73, //p13
	  5.91, 9.30, -17.44,  //p7
	  1.02, 2.80, -9.76,   //p4
	  -1.33, 4.30, -3.29,  //p3
	  25.99, 2.30, -28.94, //p17
	  8.16, 10.30, -3.61,  //p2
	  32.33, 10.30, -21.52,//p18
	  32.04, 3.30, -29.88, //p19
	  12.56, 1.30, -23.12, //p10
	  18.95, 9.30, -26.61  //p11
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
  

  int realballnumber = 10;
  int edgenumber = realballnumber * (realballnumber - 1);


};

#endif // DentalAccuracy_h
