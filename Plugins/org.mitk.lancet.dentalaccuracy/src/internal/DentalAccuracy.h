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

  // Old style Image registration
  lancet::NavigationObject::Pointer m_NavigatedImage;
  vtkNew<vtkMatrix4x4> m_ImageRegistrationMatrix; // image(surface) to ObjectRf matrix
  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer m_SurfaceRegistrationStaticImageFilter;


  // ************* SteelBall extraction and dental splint movement Modified by Yuhan *******************
  std::vector<double> allBallFingerPrint;
  std::vector<double> stdCenters;
  int realballnumber{0};
  int edgenumber{0};

  mitk::Surface::Pointer m_splinterSurface = mitk::Surface::New();
  mitk::PointSet::Pointer m_steelBalls_cmm = mitk::PointSet::New();


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
  bool m_Stat_calibratorRFtoDrill{ false };
  double m_T_calibratorRFtoDrill[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // from hardware design, here the drill is actually the probe
 
  bool m_Stat_handpieceRFtoDrill{ false };
  double m_T_handpieceRFtoDrill[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // Perform handpiece calibration to acquire,, here the drill is actually the probe
  double m_T_handpieceRFtoInputDrill[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // consider the actual length of the selected drill

  bool m_Stat_patientRFtoImage{ false };
  double m_T_patientRFtoImage[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // Perform image registration to acquire  
  

  // Error deviation and navigation mode switch
  int m_NaviMode{ 0 }; // 0 is drilling mode; 1 is implantation mode
  double m_AngleError{ 0 };
  double m_DrillTipTotalError{ 0 };
  double m_DrillTipVertiError{ 0 };
  double m_DrillTipHoriError{ 0 };

  double m_EntryTotalError{ 0 };
  double m_EntryVertiError{ 0 };
  double m_EntryHoriError{ 0 };

  double m_ApexTotalError{ 0 };
  double m_ApexVertiError{ 0 };
  double m_ApexHoriError{ 0 };

  void CalculateDeviation();

};

#endif // DentalAccuracy_h
