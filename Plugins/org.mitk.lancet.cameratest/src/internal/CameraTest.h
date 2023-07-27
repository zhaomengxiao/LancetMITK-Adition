/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef CameraTest_h
#define CameraTest_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>


//#include "kukaRobotDevice.h"
#include "lancetApplyDeviceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinFilter.h"
#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"
//#include "lancetKukaRobotDevice.h"
#include "lancetNavigationObjectVisualizationFilter.h"
#include "lancetPathPoint.h"
#include "mitkTrackingDeviceSource.h"
#include "mitkVirtualTrackingDevice.h"
#include "robotRegistration.h"
//#include "lancetNavigationSceneFilter.h"
#include <mutex>


#include "ui_CameraTestControls.h"


class CameraTest : public QmitkAbstractView
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

public slots:
	void UseVega();
	void ShowToolStatus_Vega();
	void OnVegaVisualizeTimer();
	void UpdateToolStatusWidget();
	void ReadPosition();
	bool ApplySurfaceRegistration();
	void ReadEnd10fLine();
	void ReadEnd20fLine();
	void CalculateRepeatability();
	void ShowPosition();
	void ShowEnd1Position();
	void ShowEnd2Position();
	void CalculateLength();
	void CalculateDeviationOfLength();
	void PrintPoint();
	void PrintEnd1();
	void PrintEnd2();
	void ClearPoint();
	void ClearEnd1();
	void ClearEnd2();
	void CameraTest::TestCalculateRepeatability();


private:
	void CalculatePositionAverage(mitk::AffineTransform3D::Pointer);
protected:
  lancet::ApplySurfaceRegistratioinFilter::Pointer m_surfaceRegistrationFilter;
  mitk::AffineTransform3D::Pointer m_imageRegistrationMatrix;  // image(surface) to Rf matrix
  Ui::CameraTestControls m_Controls;
  mitk::NavigationToolStorage::Pointer m_VegaToolStorage;
  mitk::TrackingDeviceSource::Pointer m_VegaSource;
  lancet::NavigationObjectVisualizationFilter::Pointer m_VegaVisualizer;
  QTimer* m_VegaVisualizeTimer{ nullptr };
  mitk::AffineTransform3D::Pointer m_ProbeRealTimePose;
  std::vector<std::array<double, 3>> m_ProbeRealTimePose_list;
  std::vector<std::array<double, 3>> m_End1Position_list;
  std::vector<std::array<double, 3>> m_End2Position_list;
  std::vector<mitk::NavigationData::Pointer> m_VegaNavigationData;
  lancet::NavigationObject::Pointer navigatedImage;
  std::array<double, 3> p_a;
  std::array<double, 3> p_s;
  std::vector<double> length_list;
  std::vector<double> length_deviation_list;
  
  bool m_calculate_position_flag = false;
  int m_nRealTimePoseCount=0;
  bool add_sum_finish = false;

  virtual void CreateQtPartControl(QWidget *parent) override;

  virtual void SetFocus() override; 

  /// \brief Called when the user clicks the GUI button
 
};

#endif // CameraTest_h
