/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "SurgicalSimulate.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <mitkAffineTransform3D.h>
#include <mitkMatrixConvert.h>

//igt
#include <lancetVegaTrackingDevice.h>
#include <kukaRobotDevice.h>
#include <lancetApplyDeviceRegistratioinFilter.h>
#include <mitkNavigationDataToPointSetFilter.h>
#include <lancetPathPoint.h>
#include <vtkQuaternion.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include <QtWidgets\qfiledialog.h>

#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
#include "QmitkIGTCommonHelper.h"
#include "lancetTreeCoords.h"
const std::string SurgicalSimulate::VIEW_ID = "org.mitk.views.surgicalsimulate";

void SurgicalSimulate::SetFocus()
{
  m_Controls.pushButton_connectKuka->setFocus();
}


void SurgicalSimulate::OnVirtualDevice2VisualizeTimer()
{
  //Here we call the Update() method from the Visualization Filter. Internally the filter checks if
  //new NavigationData is available. If we have a new NavigationData the cone position and orientation
  //will be adapted.
  if (m_VirtualDevice2Visualizer.IsNotNull())
  {
    m_VirtualDevice2Visualizer->Update();
    auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
    mitk::RenderingManager::GetInstance()->InitializeViews(geo);
    this->RequestRenderWindowUpdate();
  }
}

void SurgicalSimulate::CreateQtPartControl(QWidget* parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_metaImageNode);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src);
  // InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_ImageCheckPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetPlane);

  m_imageRegistrationMatrix = mitk::AffineTransform3D::New();

  connect(m_Controls.pushButton_connectKuka, &QPushButton::clicked, this, &SurgicalSimulate::UseKuka);
  //connect(m_Controls.pushButton_connectKuka, &QPushButton::clicked, this, &SurgicalSimulate::UseVirtualDevice2);
  connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &SurgicalSimulate::UseVega);
  //connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &SurgicalSimulate::UseVirtualDevice1);
  connect(m_Controls.pushButton_captureRobot, &QPushButton::clicked, this, &SurgicalSimulate::OnRobotCapture);
  connect(m_Controls.pushButton_automove, &QPushButton::clicked, this, &SurgicalSimulate::OnAutoMove);
  connect(m_Controls.pushButton_selfcheck, &QPushButton::clicked, this, &SurgicalSimulate::OnSelfCheck);
  connect(m_Controls.pushButton_resetRobotReg, &QPushButton::clicked, this, &SurgicalSimulate::OnResetRobotRegistration);
  connect(m_Controls.pushButton_assembleNavigationObject, &QPushButton::clicked, this, &SurgicalSimulate::SetupNavigatedImage);
  connect(m_Controls.pushButton_collectLandmark, &QPushButton::clicked, this, &SurgicalSimulate::CollectLandmarkProbe);
  connect(m_Controls.pushButton_applyRegistration, &QPushButton::clicked, this, &SurgicalSimulate::ApplySurfaceRegistration);
  connect(m_Controls.pushButton_collectIcp, &QPushButton::clicked, this, &SurgicalSimulate::CollectIcpProbe);
  connect(m_Controls.pushButton_startTracking, &QPushButton::clicked, this, &SurgicalSimulate::StartTracking);
  // connect(m_Controls.pushButton_captureSurgicalPlane, &QPushButton::clicked, this, &SurgicalSimulate::OnCaptureProbeAsSurgicalPlane);
  connect(m_Controls.pushButton_startAutoPosition, &QPushButton::clicked, this, &SurgicalSimulate::OnAutoPositionStart);
  connect(m_Controls.pushButton_gotoPlaneEdge, &QPushButton::clicked, this, &SurgicalSimulate::OnAutoPositionStart);
  connect(m_Controls.pushButton_saveRobotRegist, &QPushButton::clicked, this, &SurgicalSimulate::OnSaveRobotRegistraion);
  connect(m_Controls.pushButton_usePreRobotRegit, &QPushButton::clicked, this, &SurgicalSimulate::OnUsePreRobotRegitration);

  connect(m_Controls.pushButton_applyPreImageRegistration, &QPushButton::clicked, this, &SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration);
  connect(m_Controls.pushButton_applyPreImageRegistrationNew, &QPushButton::clicked, this, &SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration_staticImage);
  connect(m_Controls.pushButton_applyRegistrationNew, &QPushButton::clicked, this, &SurgicalSimulate::ApplySurfaceRegistration_staticImage);

  connect(m_Controls.pushButton_saveNdiTools, &QPushButton::clicked, this, &SurgicalSimulate::OnSaveRobotRegistraion);

  connect(m_Controls.pushButton_confirmImageTargetLine, &QPushButton::clicked, this, &SurgicalSimulate::InterpretImageLine);
  connect(m_Controls.pushButton_probeCheckPoint, &QPushButton::clicked, this, &SurgicalSimulate::ProbeImageCheckPoint);

  // connect(m_Controls.pushButton_touchP1, &QPushButton::clicked, this, &SurgicalSimulate::TouchProbeCalibrationPoint1);
  // connect(m_Controls.pushButton_touchP2, &QPushButton::clicked, this, &SurgicalSimulate::TouchProbeCalibrationPoint2);
  // connect(m_Controls.pushButton_touchP3, &QPushButton::clicked, this, &SurgicalSimulate::TouchProbeCalibrationPoint3);

  connect(m_Controls.pushButton_probeSurfaceDistance, &QPushButton::clicked, this, &SurgicalSimulate::ProbeSurface);
  connect(m_Controls.pushButton_InitializeTcp, &QPushButton::clicked, this, &SurgicalSimulate::ResetRobotTcp);
  connect(m_Controls.pushButton_setTcpPrecisionTest, &QPushButton::clicked, this, &SurgicalSimulate::SetPrecisionTestTcp);
  connect(m_Controls.pushButton_setPlanePrecisionTestTcp, &QPushButton::clicked, this, &SurgicalSimulate::SetPlanePrecisionTestTcp);
  connect(m_Controls.pushButton_recordInitial, &QPushButton::clicked, this, &SurgicalSimulate::RecordInitial);
  connect(m_Controls.pushButton_goToInitial, &QPushButton::clicked, this, &SurgicalSimulate::GoToInitial);

  connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &SurgicalSimulate::TranslateX_minus);
  connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &SurgicalSimulate::TranslateX_plus);
  connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &SurgicalSimulate::TranslateY_minus);
  connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &SurgicalSimulate::TranslateY_plus);
  connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &SurgicalSimulate::TranslateZ_minus);
  connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &SurgicalSimulate::TranslateZ_plus);

  connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &SurgicalSimulate::RotateX_minus);
  connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &SurgicalSimulate::RotateX_plus);
  connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &SurgicalSimulate::RotateY_minus);
  connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &SurgicalSimulate::RotateY_plus);
  connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &SurgicalSimulate::RotateZ_minus);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &SurgicalSimulate::RotateZ_plus);
  connect(m_Controls.pushButton_setFlangeTcp, &QPushButton::clicked, this, &SurgicalSimulate::ResetRobotTcp);

  connect(m_Controls.pushButton_confirmImageTargetPlane, &QPushButton::clicked, this, &SurgicalSimulate::InterpretImagePlane);
  connect(m_Controls.pushButton_goToFakePlane, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_goToFakePlane_clicked);

  connect(m_Controls.pushButton_updateRobotSimuPose, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_updateRobotSimuPose_clicked);

  connect(m_Controls.pushButton_a1_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a1_p_clicked);
  connect(m_Controls.pushButton_a1_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a1_m_clicked);
  connect(m_Controls.pushButton_a2_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a2_p_clicked);
  connect(m_Controls.pushButton_a2_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a2_m_clicked);
  connect(m_Controls.pushButton_a3_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a3_p_clicked);
  connect(m_Controls.pushButton_a3_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a3_m_clicked);
  connect(m_Controls.pushButton_a4_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a4_p_clicked);
  connect(m_Controls.pushButton_a4_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a4_m_clicked);
  connect(m_Controls.pushButton_a5_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a5_p_clicked);
  connect(m_Controls.pushButton_a5_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a5_m_clicked);
  connect(m_Controls.pushButton_a6_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a6_p_clicked);
  connect(m_Controls.pushButton_a6_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a6_m_clicked);
  connect(m_Controls.pushButton_a7_p, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a7_p_clicked);
  connect(m_Controls.pushButton_a7_m, &QPushButton::clicked, this, &SurgicalSimulate::On_pushButton_a7_m_clicked);

  connect(m_Controls.pushButton_endToolPower, &QPushButton::clicked, this, &SurgicalSimulate::StartTrackingWithPowerControl);


}

void SurgicalSimulate::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                          const QList<mitk::DataNode::Pointer>& nodes)
{
  // iterate all selected objects, adjust warning visibility
  // foreach (mitk::DataNode::Pointer node, nodes)
  // {
  //   if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
  //   {
  //     m_Controls.labelWarning->setVisible(false);
  //     m_Controls.buttonPerformImageProcessing->setEnabled(true);
  //     return;
  //   }
  // }
  //
  // m_Controls.labelWarning->setVisible(true);
  // m_Controls.buttonPerformImageProcessing->setEnabled(false);
}

void SurgicalSimulate::UseVega()
{
  //read in filename
  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
                                                  tr("Tool Storage Files (*.IGTToolStorage)"));
  if (filename.isNull()) return;

  //read tool storage from disk
  std::string errorMessage = "";
  mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
    GetDataStorage());
  m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
  m_VegaToolStorage->SetName(filename.toStdString());

  //! [UseKuka 1]
  //Here we want to use the Kuka robot as a tracking device. Therefore we instantiate a object of the class
  //KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
  MITK_INFO << "Vega tracking";
  //QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
  lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

  //Create Navigation Data Source with the factory class, and the visualize filter.
  lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
    lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

  m_VegaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
  m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
  m_VegaSource->Connect();

  m_VegaSource->StartTracking();

  //update visualize filter by timer
  if (m_VegaVisualizeTimer == nullptr)
  {
    m_VegaVisualizeTimer = new QTimer(this); //create a new timer
  }
  connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnVegaVisualizeTimer()));
  //connect the timer to the method OnTimer()
  connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
  //connect the timer to the method OnTimer()
  ShowToolStatus_Vega();
  m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

  auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
  mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void SurgicalSimulate::GeneratePoses()
{
  //Use current pose as reference to generate new poses
  mitk::NavigationData::Pointer nd_robot2flange = m_KukaSource->GetOutput(0)->Clone();

  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();
  mitk::TransferItkTransformToVtkMatrix(nd_robot2flange->GetAffineTransform3D().GetPointer(), vtkMatrix);


  //
}

void SurgicalSimulate::CapturePose(bool translationOnly)
{
  //Output sequence is the same as AddTool sequence
  //get navigation data of flange in robot coords,
  mitk::NavigationData::Pointer nd_robot2flange = m_KukaSource->GetOutput(0);

  //get navigation data of RobotEndRF in ndi coords,
  //auto RobotEndRF = m_VegaToolStorage->GetToolIndexByName("RobotEndRF");
  mitk::NavigationData::Pointer nd_Ndi2RobotEndRF = m_VegaSource->GetOutput("RobotEndRF");
  //get navigation data of RobotBaseRF in ndi coords,
  //auto RobotBaseRF = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
  mitk::NavigationData::Pointer nd_Ndi2RobotBaseRF = m_VegaSource->GetOutput("RobotBaseRF");
  //get navigation data RobotEndRF in reference frame RobotBaseRF
  mitk::NavigationData::Pointer nd_RobotBaseRF2RobotEndRF = GetNavigationDataInRef(
    nd_Ndi2RobotEndRF, nd_Ndi2RobotBaseRF);

  //add nd to registration module
  //m_RobotRegistration.AddPose(nd_robot2flange, nd_RobotBaseRF2RobotEndRF, translationOnly);

  // add vtkMatrix as poses to the registration module
  // Average the NavigationData from the NDI camera
  double ndiToRoboEndArrayAvg[16];
  double ndiToBaseRFarrayAvg[16];
  AverageNavigationData(nd_Ndi2RobotEndRF, 30, 20, ndiToRoboEndArrayAvg);
  AverageNavigationData(nd_Ndi2RobotBaseRF, 30, 20, ndiToBaseRFarrayAvg);

  vtkNew<vtkMatrix4x4> vtkNdiToRoboEndMatrix;
  vtkNew<vtkMatrix4x4> vtkBaseRFToNdiMatrix;
  vtkNdiToRoboEndMatrix->DeepCopy(ndiToRoboEndArrayAvg);
  vtkBaseRFToNdiMatrix->DeepCopy(ndiToBaseRFarrayAvg);
  vtkBaseRFToNdiMatrix->Invert();
  auto vtkRoboBaseToFlangeMatrix = getVtkMatrix4x4(nd_robot2flange);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(ndiToRoboEndArrayAvg);
	tmpTransform->Concatenate(vtkBaseRFToNdiMatrix);
	tmpTransform->Update();
	auto vtkBaseRFtoRoboEndMatrix = tmpTransform->GetMatrix();

	m_RobotRegistration.AddPoseWithVtkMatrix(vtkRoboBaseToFlangeMatrix, vtkBaseRFtoRoboEndMatrix, translationOnly);

  MITK_INFO << nd_robot2flange;
  MITK_INFO << nd_RobotBaseRF2RobotEndRF;
}

mitk::NavigationData::Pointer SurgicalSimulate::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
                                                                       mitk::NavigationData::Pointer nd_ref)
{
  mitk::NavigationData::Pointer res = mitk::NavigationData::New();
  res->Graft(nd);
  res->Compose(nd_ref->GetInverse());
  return res;
}

SurgicalSimulate::~SurgicalSimulate()
{
  if (m_KukaVisualizeTimer != nullptr)
  {
    m_KukaVisualizeTimer->stop();
  }
  if (m_VegaVisualizeTimer != nullptr)
  {
    m_VegaVisualizeTimer->stop();
  }
}

void SurgicalSimulate::UseKuka()
{
  //read in filename
  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
                                                  tr("Tool Storage Files (*.IGTToolStorage)"));
  if (filename.isNull()) return;

  //read tool storage from disk
  std::string errorMessage = "";
  mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
    GetDataStorage());
  m_KukaToolStorage = myDeserializer->Deserialize(filename.toStdString());
  m_KukaToolStorage->SetName(filename.toStdString());

  //! [UseKuka 1]
  //Here we want to use the Kuka robot as a tracking device. Therefore we instantiate a object of the class
  //KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
  MITK_INFO << "Kuka tracking";
  //QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
  m_KukaTrackingDevice = lancet::KukaRobotDevice::New(); //instantiate

  //Create Navigation Data Source with the factory class, and the visualize filter.
  lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
    lancet::TrackingDeviceSourceConfiguratorLancet::New(m_KukaToolStorage, m_KukaTrackingDevice);

  m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);
  // m_KukaSource->RegisterAsMicroservice(); 

  m_KukaSource->Connect();
}



void SurgicalSimulate::OnCheckPowerStatusTimer()
{
	double limit{ 50 };
	double current{ 1000 };

	limit = m_Controls.lineEdit_powerThres->text().toDouble();


	double targetPoint[3]{ m_T_robot->GetOffset()[0] ,m_T_robot->GetOffset()[1] ,m_T_robot->GetOffset()[2] };
	double currentPoint[3];

	currentPoint[0] = m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[0];
	currentPoint[1] = m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[1];
	currentPoint[2] = m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[2];

	current = sqrt(
		pow(targetPoint[0] - currentPoint[0], 2) +
		pow(targetPoint[1] - currentPoint[1], 2) +
		pow(targetPoint[2] - currentPoint[2], 2)
	);

	m_Controls.lineEdit_distanceToTarget->setText(QString::number(current));



	if (current < limit && m_PowerStatus == 0)
	{
		// Switch On
		m_KukaTrackingDevice->RequestExecOperate("setio", { "1","1" });
		m_PowerStatus = 1;
	}

	if (current > limit && m_PowerStatus == 1)
	{
		// Switch Off
		m_KukaTrackingDevice->RequestExecOperate("setio", { "1","0" });
		m_PowerStatus = 0;
	}


}

// Todo: Monitor endTool powerStatus
void SurgicalSimulate::StartTrackingWithPowerControl()
{
	if(m_T_robot != nullptr)
	{
		disconnect(m_CheckPowerStatusTimer, SIGNAL(timeout()), this, SLOT(OnCheckPowerStatusTimer()));

		if (m_CheckPowerStatusTimer == nullptr)
		{
			m_CheckPowerStatusTimer = new QTimer(this); //create a new timer
		}
		
		connect(m_CheckPowerStatusTimer, SIGNAL(timeout()), this, SLOT(OnCheckPowerStatusTimer()));
		// connect the timer to the method OnTimer()
		// connect(m_KukaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
		// connect the timer to the method OnTimer()
		// ShowToolStatus_Kuka();
		m_CheckPowerStatusTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

		m_Controls.textBrowser->append("Start endTool power control");
	}

}


void SurgicalSimulate::StartTracking()
{
  if (m_KukaTrackingDevice->GetState() == 1) //ready
  {
    m_KukaSource->StartTracking();

    //update visualize filter by timer
    if (m_KukaVisualizeTimer == nullptr)
    {
      m_KukaVisualizeTimer = new QTimer(this); //create a new timer
    }
    connect(m_KukaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnKukaVisualizeTimer()));
    //connect the timer to the method OnTimer()
    connect(m_KukaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
    //connect the timer to the method OnTimer()
    ShowToolStatus_Kuka();
    m_KukaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps
  }
  else
  {
    MITK_ERROR << "Tracking not start,Device State:" << m_KukaTrackingDevice->GetState();
  }
  auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
  mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void SurgicalSimulate::OnKukaVisualizeTimer()
{
  //Here we call the Update() method from the Visualization Filter. Internally the filter checks if
  //new NavigationData is available. If we have a new NavigationData the cone position and orientation
  //will be adapted.
  if (m_KukaVisualizer.IsNotNull())
  {
    m_KukaVisualizer->Update(); //todo Crash When close plugin
    this->RequestRenderWindowUpdate();
  }
}

void SurgicalSimulate::OnSelfCheck()
{
  // if (m_KukaTrackingDevice.IsNotNull() && m_KukaTrackingDevice->GetState()!=0)
  // {
  m_KukaTrackingDevice->RequestExecOperate(/*"Robot",*/ "setio", {"20", "20"});
  // }
  // else
  // {
  //   MITK_ERROR << "robot not connect";
  // }
}

void SurgicalSimulate::OnVegaVisualizeTimer()
{
  //Here we call the Update() method from the Visualization Filter. Internally the filter checks if
  //new NavigationData is available. If we have a new NavigationData the cone position and orientation
  //will be adapted.
  if (m_VegaVisualizer.IsNotNull())
  {
    m_VegaVisualizer->Update();
    // auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
    // mitk::RenderingManager::GetInstance()->InitializeViews(geo);
    this->RequestRenderWindowUpdate();
  }
}

void SurgicalSimulate::OnRobotCapture()
{
  if (m_IndexOfRobotCapture < 5) //The first five translations, 
  {
    CapturePose(true);
    //Increase the count each time you click the button
    m_IndexOfRobotCapture++;
	m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));

    MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
  }
  else if (m_IndexOfRobotCapture < 10) //the last five rotations
  {
    CapturePose(false);
    //Increase the count each time you click the button
    m_IndexOfRobotCapture++;
	m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));
    MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
  }
  else
  {
	MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
    vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
    m_RobotRegistration.GetRegistraionMatrix(matrix4x4);

	vtkNew<vtkMatrix4x4> robotEndToFlangeMatrix;
	m_RobotRegistration.GetTCPmatrix(robotEndToFlangeMatrix);
	robotEndToFlangeMatrix->Invert();

	m_Controls.textBrowser->append("Registration RMS: "+QString::number(m_RobotRegistration.RMS()));

    //For Test Use ,4L tka device registration result ,you can skip registration workflow by using it, Only if the RobotBase Reference Frame not moved!
    /*vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
    matrix4x4->SetElement(0, 0, -0.48); matrix4x4->SetElement(0, 1, -0.19); matrix4x4->SetElement(0, 2, -0.86);
    matrix4x4->SetElement(1, 0, -0.01); matrix4x4->SetElement(1, 1, -0.97); matrix4x4->SetElement(1, 2, 0.22);
    matrix4x4->SetElement(2, 0, -0.88); matrix4x4->SetElement(2, 1, 0.11); matrix4x4->SetElement(2, 2, 0.46);
    matrix4x4->SetElement(0, 3, 162.37);
    matrix4x4->SetElement(1, 3, -530.45);
    matrix4x4->SetElement(2, 3, -255.62);*/

    m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();

	auto affineRobotEndRFtoFlangeMatrix = mitk::AffineTransform3D::New();

    mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());

	mitk::TransferVtkMatrixToItkTransform(robotEndToFlangeMatrix, affineRobotEndRFtoFlangeMatrix.GetPointer());

    //save robot registration matrix into reference tool
    m_VegaToolStorage->GetToolByName("RobotBaseRF")->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);
	m_VegaToolStorage->GetToolByName("RobotEndRF")->SetToolRegistrationMatrix(affineRobotEndRFtoFlangeMatrix);


	MITK_INFO << "Robot Registration Matrix";
	MITK_INFO << m_RobotRegistrationMatrix;
    //build ApplyDeviceRegistrationFilter
    m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
    m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
    m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
    m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("RobotBaseRF"));//must make sure NavigationDataOfRF update somewhere else.

    m_KukaVisualizeTimer->stop();
    m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
    m_KukaVisualizeTimer->start();
	//tcp

	std::array<double, 6> tcp{};
	m_RobotRegistration.GetTCP(tcp);


	//For Test Use ,4L tka device registration result ,you can skip registration workflow by using it, Only if the RobotBase Reference Frame not moved!
	/*tcp[0] = 69.162;
	tcp[1] = -22.335;
	tcp[2] = -82.3293;
	tcp[3] =0.2433;
	tcp[4] = -3.089;
	tcp[5] = -0.019;*/
	
	//For Test Use ,7L tka device registration result ,you can skip registration workflow by using it, Only if the RobotBase Reference Frame not moved!
	/*tcp[0] = 23.80;
	tcp[1] = 47.49;
	tcp[2] = 95;
	tcp[3] =1.575;
	tcp[4] = -3.141;
	tcp[5] = 0;*/

	//For Test Use, regard ball 2 as the TCP, the pose is the same as the flange
	// https://gn1phhht53.feishu.cn/wiki/wikcnxxvosvrccWKPux0Bjd4j6g
	// tcp[0] = 0;
	// tcp[1] = 100;
	// tcp[2] = 138;
	// tcp[3] = 0;
	// tcp[4] = 0;
	// tcp[5] = 0;

	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{QString::number( tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
  }

}

void SurgicalSimulate::OnAutoMove()
{
  //Use current pose as reference to generate new poses
  mitk::NavigationData::Pointer nd_robot2flange = m_KukaSource->GetOutput(0);
  MITK_INFO << "nd_robot2flange";
  MITK_INFO << nd_robot2flange;
  mitk::AffineTransform3D::Pointer affine = mitk::AffineTransform3D::New();
  affine = nd_robot2flange->GetAffineTransform3D()->Clone();
  double axisx[3]{1, 0, 0};
  double axisy[3]{0, 1, 0};
  double axisz[3]{0, 0, 1};
  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();

  double trans1[3]{0, 0, 50};
  double trans2[3]{0, 50, 0};
  double trans3[3]{50, 0, 0};
  double trans4[3]{0, 0, -25};
  double trans5[3]{-25, 0, 0};
  double trans6[3]{0, -25, 0};
  double trans7[3]{0, -25, 0};
  double trans8[3]{25, 0, 0};
  double trans9[3]{-25, 0, 0};

  double plusY[3]{ 0, 75, 0 };
  double plusZ[3]{ 0,0,75 };
  double plusX[3]{75,0,0};
  double minusX[3]{ -150,0,0 };

  vtkNew<vtkTransform> tmpTrans;
  double tcp[6]{ 0 };

  switch (m_IndexOfRobotCapture)
  {
  case 1: //y+75

	  // ensure flange TCP is applied when the robot starts to move
	  
	  MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	  //set tcp to robot
		//set tcp
	  QThread::msleep(1000);
	  m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	  QThread::msleep(1000);
	  m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	  QThread::msleep(1000);
	  m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

	  // record the initial position into m_initial_robotBaseToFlange
  	RecordInitial();

    affine->Translate(plusY);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 2: //y - 75
    // affine->Translate(plusY);
    //
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
	  tmpTrans->PostMultiply();
	  tmpTrans->Translate(0, -75, 0);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

    m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
    break;

  case 3:
    // affine->Translate(plusX);
    //
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
	  tmpTrans->PostMultiply();
	  tmpTrans->Translate(0, 0, 75);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;


    break;

  case 4: 
    // affine->Translate(plusX);
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);

	  tmpTrans->PostMultiply();
	  tmpTrans->Translate(0, 0, -75);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
    break;

  case 5: 
    // affine->Rotate3D(axisx, 0.174);
    // affine->Translate(minusX);
    //
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
    // break;
	  tmpTrans->PostMultiply();
	  tmpTrans->RotateX(15);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;


  case 6: 
    // affine->Rotate3D(axisx, -0.174 * 2);
    // affine->Translate(trans6);
    //
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
    // break;
	  tmpTrans->PostMultiply();
	  tmpTrans->RotateX(-15);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;

  case 7:
    // affine->Rotate3D(axisy, 0.174);
    // affine->Translate(trans7);
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
    // break;
	  tmpTrans->PostMultiply();
	  tmpTrans->RotateX(11);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;

  case 8: 
    // affine->Rotate3D(axisy, -0.174 * 2);
    // affine->Translate(trans8);
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
    // break;
	  tmpTrans->PostMultiply();
	  tmpTrans->RotateZ(10);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;


  case 9: //z rotate 20 degree x -25
    // affine->Rotate3D(axisz, 0.174 * 2);
    // affine->Translate(trans9);
    // mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
    //
    // m_KukaTrackingDevice->RobotMove(vtkMatrix);
    // break;
	  tmpTrans->PostMultiply();
	  tmpTrans->RotateZ(-10);
	  tmpTrans->Concatenate(m_initial_robotBaseToFlange);
	  tmpTrans->Update();

	  m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	  break;

  default:


    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;
  }
}

bool SurgicalSimulate::RecordInitial()
{
	m_initial_robotBaseToFlange = vtkMatrix4x4::New();

	mitk::NavigationData::Pointer nd_robotBaseToFlange = m_KukaSource->GetOutput(0)->Clone();

	mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), m_initial_robotBaseToFlange);

	return true;
}

bool SurgicalSimulate::GoToInitial()
{
	m_KukaTrackingDevice->RobotMove(m_initial_robotBaseToFlange);

	return true;
}

bool SurgicalSimulate::InterpretMovementAsInBaseSpace(vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase)
{
	int index = m_Controls.comboBox_robotMoveType->currentIndex();
	if (index == 1)
	{
		vtkNew<vtkMatrix4x4> matrix_robotBaseToFlange;

		mitk::NavigationData::Pointer nd_robotBaseToFlange = m_KukaSource->GetOutput(0)->Clone();

		mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), matrix_robotBaseToFlange);

		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(matrix_robotBaseToFlange);
		tmpTransform->Concatenate(rawMovementMatrix);
		tmpTransform->Update();

		movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());

		m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
			"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}

	if (index == 0)
	{
		vtkNew<vtkMatrix4x4> matrix_robotBaseToFlange;

		mitk::NavigationData::Pointer nd_robotBaseToFlange = m_KukaSource->GetOutput(0)->Clone();

		mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), matrix_robotBaseToFlange);

		vtkNew<vtkMatrix4x4> matrix_flangeSpaceToMovementSpace;
		// matrix_flangeSpaceToMovementSpace->DeepCopy(m_matrix_flangeSpaceToMovementSpace);
		matrix_flangeSpaceToMovementSpace->Identity();

		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(rawMovementMatrix);
		tmpTransform->Concatenate(matrix_flangeSpaceToMovementSpace);
		tmpTransform->Concatenate(matrix_robotBaseToFlange);
		tmpTransform->Update();

		movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());

		m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
			"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}

	return false;
}

bool SurgicalSimulate::TranslateX_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 1,0,0 };
	axis[0] = axis[0] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::TranslateX_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 1,0,0 };
	axis[0] = -axis[0] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::TranslateY_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,1,0 };
	axis[1] = axis[1] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::TranslateY_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,1,0 };
	axis[1] = -axis[1] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::TranslateZ_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,0,1 };
	axis[2] = axis[2] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::TranslateZ_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,0,1 };
	axis[2] = -axis[2] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateX_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 1,0,0 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateX_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 1,0,0 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateY_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,1,0 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateY_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,1,0 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateZ_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,0,1 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool SurgicalSimulate::RotateZ_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,0,1 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

void SurgicalSimulate::OnResetRobotRegistration()
{
  m_RobotRegistration.RemoveAllPose();
  m_IndexOfRobotCapture = 0;
  m_Controls.lineEdit_collectedRoboPose->setText(QString::number(0));
}

void SurgicalSimulate::OnSaveRobotRegistraion()
{
  if (m_VegaToolStorage.IsNotNull())
  {
    QFileDialog* fileDialog = new QFileDialog;
    fileDialog->setDefaultSuffix("IGTToolStorage");
    QString suffix = "IGT Tool Storage (*.IGTToolStorage)";
    // Set default file name to LastFileSavePath + storage name
    QString defaultFileName = QmitkIGTCommonHelper::GetLastFileSavePath() + "/" + QString::fromStdString(m_VegaToolStorage->GetName());
    QString filename = fileDialog->getSaveFileName(nullptr, tr("Save Navigation Tool Storage"), defaultFileName, suffix, &suffix);

    if (filename.isEmpty()) return; //canceled by the user

    // check file suffix
    QFileInfo file(filename);
    if (file.suffix().isEmpty()) filename += ".IGTToolStorage";

    //serialize tool storage
    mitk::NavigationToolStorageSerializer::Pointer mySerializer = mitk::NavigationToolStorageSerializer::New();

    try
    {
      mySerializer->Serialize(filename.toStdString(), m_VegaToolStorage);
    }
    catch (const mitk::IGTIOException & e)
    {
      m_Controls.textBrowser->append(QString::fromStdString("Error: " + std::string(e.GetDescription())));
      return;
    }
    m_Controls.textBrowser->append(QString::fromStdString(m_VegaToolStorage->GetName()+" saved"));
  }
}

void SurgicalSimulate::OnUsePreRobotRegitration()
{
  //build ApplyDeviceRegistrationFilter
  m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
  m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
  m_RobotRegistrationMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
  m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
  m_VegaSource->Update();
  m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("RobotBaseRF"));//must make sure NavigationDataOfRF update somewhere else.

  //TODO: store tcp into toolstorage
  //For Test Use ,7L tka device registration result ,you can skip registration workflow by using it, Only if the RobotBase Reference Frame not moved!
  std::array<double, 6> tcp;
	//design
  /*tcp[0] = 23.80;
	tcp[1] = 47.49;
	tcp[2] = 95;
	tcp[3] =1.575;
	tcp[4] = -3.141;
	tcp[5] = 0;*/
	//algo
	tcp[0] = 22.5281;
	tcp[1] = 45.9194;
	tcp[2] = 93.4865;
	tcp[3] = 1.575;
	tcp[4] = -3.141;
	tcp[5] = 0;

	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

  m_KukaVisualizeTimer->stop();
  m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
  m_KukaVisualizeTimer->start();


  




  //For Test Use, regard ball 2 as the TCP, the pose is the same as the flange
  // https://gn1phhht53.feishu.cn/wiki/wikcnxxvosvrccWKPux0Bjd4j6g
  
  tcp[0] = 0;
  tcp[1] = 100;
  tcp[2] = 138;
  tcp[3] = 0;
  tcp[4] = 0;
  tcp[5] = 0;
  MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
  //set tcp to robot
	//set tcp
  QThread::msleep(1000);
  m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
  QThread::msleep(1000);
  m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
  QThread::msleep(1000);
  m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
}


// void SurgicalSimulate::OnCaptureProbeAsSurgicalPlane()
// {
//   //create NavigationDataToPointSetFilter to get a point3D by probe in NDI coordinates
//   mitk::NavigationDataToPointSetFilter::Pointer probePoint = mitk::NavigationDataToPointSetFilter::New();
//   //auto probeToolIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
//   probePoint->SetInput(m_VegaSource->GetOutput("Probe"));
//   probePoint->SetOperationMode(mitk::NavigationDataToPointSetFilter::Mode3DMean);
//   probePoint->SetNumberForMean(10);
//   //run the filter
//   probePoint->Update();
//   //get output
//   mitk::PointSet::Pointer target = probePoint->GetOutput(0);
//
//   // //create a Surgical plan
//   // m_SurgicalPlan = lancet::PointPath::New();
//   //convert to robot coordinates
//   mitk::AffineTransform3D::Pointer targetMatrix = mitk::AffineTransform3D::New();
//   targetMatrix->SetOffset(target->GetPoint(0).GetDataPointer());
//   MITK_INFO << "Captured Point: " << targetMatrix;
//
//   m_T_robot = mitk::AffineTransform3D::New();
//   m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
//   m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", targetMatrix, m_T_robot);
//
//   //========
//   //convert from ndi to robot use navigationTree
//   //========
//     //build the tree
//   //NavigationTree::Pointer tree = NavigationTree::New();
//
//   //NavigationNode::Pointer ndi = NavigationNode::New();
//   //ndi->SetNavigationData(mitk::NavigationData::New());
//   //ndi->SetNodeName("ndi");
//
//   //tree->Init(ndi);
//
//   //NavigationNode::Pointer robotBaseRF = NavigationNode::New();
//   //robotBaseRF->SetNodeName("RobotBaseRF");
//   //robotBaseRF->SetNavigationData(m_VegaSource->GetOutput("RobotBaseRF"));
//
//   //tree->AddChild(robotBaseRF, ndi);
//
//   //NavigationNode::Pointer robot = NavigationNode::New();
//   //robot->SetNodeName("Robot");
//   //robot->SetNavigationData(mitk::NavigationData::New(m_RobotRegistrationMatrix));
//
//   //tree->AddChild(robot, robotBaseRF);
//
//   //  //use tree
//   //mitk::NavigationData::Pointer treeRes =  tree->GetNavigationData(mitk::NavigationData::New(targetMatrix), "ndi", "Robot");
//   //mitk::AffineTransform3D::Pointer treeResMatrix = treeRes->GetAffineTransform3D();
//
//   //m_T_robot = treeResMatrix;
//
//   //MITK_INFO << "tree res";
//   //MITK_INFO << treeResMatrix;
//   //========
//   //convert from ndi to robot use navigationTree
//   //========
//   
//   //by hand
//   //Td2e = Td2c*Tc2a*Ta2e
// 	//  = Tc2d^-1 * Ta2c^-1 *Ta2e
// 	//  = m_ndD^-1 * m_ndC^-1 *m_ndInput
//  /* mitk::NavigationData::Pointer byhand = mitk::NavigationData::New(targetMatrix);
//   byhand->Compose(m_VegaSource->GetOutput("RobotBaseRF")->GetInverse());
//   byhand->Compose(mitk::NavigationData::New(m_RobotRegistrationMatrix)->GetInverse());
//   MITK_INFO << "by hand:";
//   MITK_INFO << byhand->GetAffineTransform3D();
//
//   MITK_INFO << "correct:";
//   MITK_INFO << m_T_robot;*/
//
//
//   //use robot matrix,not change the end tool rotation,only apply the offset from probe;
//   m_T_robot->SetMatrix(m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetMatrix());
//
//   m_Controls.textBrowser->append(QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));
//
//
//   MITK_INFO << m_T_robot;
// }



bool SurgicalSimulate::InterpretImageLine()
{
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);

	// Interpret targetPoint_0 from image frame to robot (internal) base frame
	// do some average
	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };
	for (int i{ 0 }; i < 20; i++) {
		m_VegaSource->Update();
		auto ndiToObjectRfMatrix = m_VegaSource->GetOutput("ObjectRf")->GetAffineTransform3D();

		auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();

		auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
		vtkNew<vtkMatrix4x4> tmpMatrix;
		mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), tmpMatrix);
		tmpMatrix->Invert();

		mitk::TransferVtkMatrixToItkTransform(tmpMatrix, rfToSurfaceMatrix.GetPointer());

		auto ndiToTargetMatrix_0 = mitk::AffineTransform3D::New();
		ndiToTargetMatrix_0->SetOffset(targetPoint_0.GetDataPointer());
		ndiToTargetMatrix_0->Compose(rfToSurfaceMatrix);
		ndiToTargetMatrix_0->Compose(ndiToObjectRfMatrix);
		auto targetUnderBase_0 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix_0, targetUnderBase_0);
		auto targetPointUnderBase_tmp0 = targetUnderBase_0->GetOffset();

		auto ndiToTargetMatrix_1 = mitk::AffineTransform3D::New();
		ndiToTargetMatrix_1->SetOffset(targetPoint_1.GetDataPointer());
		ndiToTargetMatrix_1->Compose(rfToSurfaceMatrix);
		ndiToTargetMatrix_1->Compose(ndiToObjectRfMatrix);
		auto targetUnderBase_1 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix_1, targetUnderBase_1);
		auto targetPointUnderBase_tmp1 = targetUnderBase_1->GetOffset();

		targetPointUnderBase_0[0] += targetPointUnderBase_tmp0[0];
		targetPointUnderBase_0[1] += targetPointUnderBase_tmp0[1];
		targetPointUnderBase_0[2] += targetPointUnderBase_tmp0[2];

		targetPointUnderBase_1[0] += targetPointUnderBase_tmp1[0];
		targetPointUnderBase_1[1] += targetPointUnderBase_tmp1[1];
		targetPointUnderBase_1[2] += targetPointUnderBase_tmp1[2];

		MITK_INFO << "tmp target Point:" << targetPointUnderBase_tmp0[0];
	}
	
	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	// fine tune the direction
	// dynamic TCP will take effect when the GetOutput function of KukaSource is called;
	// By comparison, GetOutput function of VegaSource will not automatically take the registration matrix of the corresponding
	// tool into consideration
	auto currentPostureUnderBase = m_KukaSource->GetOutput(0)->GetAffineTransform3D();
	vtkNew<vtkMatrix4x4> vtkCurrentPoseUnderBase;
	mitk::TransferItkTransformToVtkMatrix(currentPostureUnderBase.GetPointer(), vtkCurrentPoseUnderBase);

	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = vtkCurrentPoseUnderBase->GetElement(0, 0);
	currentXunderBase[1] = vtkCurrentPoseUnderBase->GetElement(1, 0);
	currentXunderBase[2] = vtkCurrentPoseUnderBase->GetElement(2, 0);
	currentXunderBase.normalize();
	MITK_INFO << "currentXunderBase" << currentXunderBase;

	// Eigen::Vector3d currentYunderBase;
	// currentYunderBase[0] = vtkCurrentPoseUnderBase->GetElement(0, 1);
	// currentYunderBase[1] = vtkCurrentPoseUnderBase->GetElement(1, 1);
	// currentYunderBase[2] = vtkCurrentPoseUnderBase->GetElement(2, 1);
	//
	// Eigen::Vector3d currentZunderBase;
	// currentZunderBase[0] = vtkCurrentPoseUnderBase->GetElement(0, 2);
	// currentZunderBase[1] = vtkCurrentPoseUnderBase->GetElement(1, 2);
	// currentZunderBase[2] = vtkCurrentPoseUnderBase->GetElement(2, 2);

	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];

	targetXunderBase.normalize();

	MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d rotationAxis;
	rotationAxis = currentXunderBase.cross(targetXunderBase);
	rotationAxis;

	double rotationAngle;
	if (currentXunderBase.dot(targetXunderBase) > 0) {
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;
	}
	else {
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}
	

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(vtkCurrentPoseUnderBase);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();

	auto targetPoseUnderBase = mitk::AffineTransform3D::New();
	mitk::TransferVtkMatrixToItkTransform(tmpTransform->GetMatrix(), targetPoseUnderBase.GetPointer());

	m_Controls.textBrowser->append("Move to this x axix:" + QString::number(testMatrix->GetElement(0,0)) + "/" + QString::number(testMatrix->GetElement(1, 0)) + "/" + QString::number(testMatrix->GetElement(2, 0)));


	// Assemble m_T_robot
	m_T_robot = mitk::AffineTransform3D::New();
	m_T_robot->SetMatrix(targetPoseUnderBase->GetMatrix());
	m_T_robot->SetOffset(targetPointUnderBase_0);

	m_Controls.textBrowser->append("result Line target point:" + QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));

	// m_Controls.textBrowser->append("Move to this x axix:" + QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));

	return true;
}

// bool SurgicalSimulate::InterpretImagePlane()
// {
// 	auto targetPlanePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetPlane->GetSelectedNode()->GetData());
// 	auto targetPoint_0 = targetPlanePoints->GetPoint(0); // TCP frame origin should move to this point
// 	auto targetPoint_1 = targetPlanePoints->GetPoint(1);
// 	auto targetPoint_2 = targetPlanePoints->GetPoint(2);
//
// 	// // Interpret targetPoint_0 & targetPoint_1 & targetPoint_2 from image frame to robot (internal) base frame
// 	// double targetPointUnderBase_0[3]{ 0 };
// 	// double targetPointUnderBase_1[3]{ 0 };
// 	// double targetPointUnderBase_2[3]{ 0 };
//
// 	// get objectRF to surface matrix 
// 	auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();
// 	auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
// 	vtkNew<vtkMatrix4x4> vtkObjectRFtoSurface;
// 	mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), vtkObjectRFtoSurface);
// 	vtkObjectRFtoSurface->Invert();
// 	// mitk::TransferVtkMatrixToItkTransform(vtkObjectRFtoSurface, rfToSurfaceMatrix.GetPointer());
//
// 	// get baseRF to objectRF matrix
// 	auto nd_ndiToObjectRF = m_VegaSource->GetOutput("ObjectRf");
// 	auto nd_ndiToBaseRF = m_VegaSource->GetOutput("RobotBaseRF");
// 	double array_ndiToObjectRF[16];
// 	double array_ndiToBaseRF[16];
//
// 	AverageNavigationData(nd_ndiToObjectRF, 10, 5, array_ndiToObjectRF);
// 	AverageNavigationData(nd_ndiToBaseRF, 10, 5, array_ndiToBaseRF);
//
// 	vtkNew<vtkMatrix4x4> vtkNdiToObjectRF;
// 	vtkNdiToObjectRF->DeepCopy(array_ndiToObjectRF);
// 	vtkNew<vtkMatrix4x4> vtkBaseRFtoNdi;
// 	vtkBaseRFtoNdi->DeepCopy(array_ndiToBaseRF);
// 	vtkBaseRFtoNdi->Invert();
//
// 	vtkNew<vtkTransform> baseRFtoObjectRFtransform;
// 	baseRFtoObjectRFtransform->PostMultiply();
// 	baseRFtoObjectRFtransform->SetMatrix(vtkNdiToObjectRF);
// 	baseRFtoObjectRFtransform->Concatenate(vtkBaseRFtoNdi);
// 	baseRFtoObjectRFtransform->Update();
// 	auto vtkBaseRFtoObjectRF = baseRFtoObjectRFtransform->GetMatrix();
//
// 	// get base to baseRF matrix
// 	auto baseRFtoBase = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
// 	vtkNew<vtkMatrix4x4> vtkBaseToBaseRF;
// 	mitk::TransferItkTransformToVtkMatrix(baseRFtoBase.GetPointer(), vtkBaseToBaseRF);
// 	vtkBaseToBaseRF->Invert();
//
// 	// get surface to target plane matrix
// 	// Eigen::Vector3d x_surfaceToPlane;
// 	// x_surfaceToPlane << targetPoint_2[0] - targetPoint_1[0],
// 	// targetPoint_2[1] - targetPoint_1[1],
// 	// targetPoint_2[2] - targetPoint_1[2];
// 	// x_surfaceToPlane.normalize();
// 	//
// 	// Eigen::Vector3d tmp_y;
// 	// tmp_y << targetPoint_0[0] - targetPoint_1[0],
// 	// 	targetPoint_0[1] - targetPoint_1[1],
// 	// 	targetPoint_0[2] - targetPoint_1[2];
// 	// tmp_y.normalize();
// 	//
// 	// Eigen::Vector3d z_surfaceToPlane = x_surfaceToPlane.cross(tmp_y);
// 	// z_surfaceToPlane.normalize();
// 	//
// 	// Eigen::Vector3d y_surfaceToPlane = z_surfaceToPlane.cross(x_surfaceToPlane);
// 	// y_surfaceToPlane.normalize();
//
// 	//---------------------- March 07, 2023 modification
// 	Eigen::Vector3d z_surfaceToPlane;
// 	z_surfaceToPlane << targetPoint_0[0] - targetPoint_1[0],
// 	targetPoint_0[1] - targetPoint_1[1],
// 	targetPoint_0[2] - targetPoint_1[2];
// 	z_surfaceToPlane.normalize();
//
// 	Eigen::Vector3d tmp_y;
// 	tmp_y << targetPoint_2[0] - targetPoint_0[0],
// 		targetPoint_2[1] - targetPoint_0[1],
// 		targetPoint_2[2] - targetPoint_0[2];
// 	tmp_y.normalize();
//
// 	Eigen::Vector3d x_surfaceToPlane = tmp_y.cross(z_surfaceToPlane);
// 	x_surfaceToPlane.normalize();
//
// 	Eigen::Vector3d y_surfaceToPlane = z_surfaceToPlane.cross(x_surfaceToPlane);
// 	y_surfaceToPlane.normalize();
//
// 	double array_surfaceToPlane[16]
// 	{
// 		x_surfaceToPlane[0], y_surfaceToPlane[0], z_surfaceToPlane[0], targetPoint_0[0],
// 		x_surfaceToPlane[1], y_surfaceToPlane[1], z_surfaceToPlane[1], targetPoint_0[1],
// 		x_surfaceToPlane[2], y_surfaceToPlane[2], z_surfaceToPlane[2], targetPoint_0[2],
// 		0,0,0,1
// 	};
// 	vtkNew<vtkMatrix4x4> vtkSurfaceToPlane;
// 	vtkSurfaceToPlane->DeepCopy(array_surfaceToPlane);
//
// 	// get base to target plane matrix
// 	vtkNew<vtkTransform> vtkBaseToTargetPlaneTransform;
// 	vtkBaseToTargetPlaneTransform->Identity();
// 	vtkBaseToTargetPlaneTransform->PostMultiply();
// 	vtkBaseToTargetPlaneTransform->SetMatrix(vtkSurfaceToPlane);
// 	vtkBaseToTargetPlaneTransform->Concatenate(vtkObjectRFtoSurface);
// 	vtkBaseToTargetPlaneTransform->Concatenate(vtkBaseRFtoObjectRF);
// 	vtkBaseToTargetPlaneTransform->Concatenate(vtkBaseToBaseRF);
// 	vtkBaseToTargetPlaneTransform->Update();
//
// 	m_T_robot = mitk::AffineTransform3D::New();
// 	mitk::TransferVtkMatrixToItkTransform(vtkBaseToTargetPlaneTransform->GetMatrix(), m_T_robot.GetPointer());
//
// 	m_Controls.textBrowser->append("result plane target point:" + QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));
//
// 	return true;
// }

bool SurgicalSimulate::InterpretImagePlane()
{
	auto targetPlanePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetPlane->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetPlanePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetPlanePoints->GetPoint(1);
	auto targetPoint_2 = targetPlanePoints->GetPoint(2);

	// // Interpret targetPoint_0 & targetPoint_1 & targetPoint_2 from image frame to robot (internal) base frame
	// double targetPointUnderBase_0[3]{ 0 };
	// double targetPointUnderBase_1[3]{ 0 };
	// double targetPointUnderBase_2[3]{ 0 };

	// get objectRF to surface matrix 
	auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();
	auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
	vtkNew<vtkMatrix4x4> vtkObjectRFtoSurface;
	mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), vtkObjectRFtoSurface);
	vtkObjectRFtoSurface->Invert();
	// mitk::TransferVtkMatrixToItkTransform(vtkObjectRFtoSurface, rfToSurfaceMatrix.GetPointer());

	// get baseRF to objectRF matrix
	auto nd_ndiToObjectRF = m_VegaSource->GetOutput("ObjectRf");
	auto nd_ndiToBaseRF = m_VegaSource->GetOutput("RobotBaseRF");
	double array_ndiToObjectRF[16];
	double array_ndiToBaseRF[16];

	AverageNavigationData(nd_ndiToObjectRF, 10, 5, array_ndiToObjectRF);
	AverageNavigationData(nd_ndiToBaseRF, 10, 5, array_ndiToBaseRF);

	vtkNew<vtkMatrix4x4> vtkNdiToObjectRF;
	vtkNdiToObjectRF->DeepCopy(array_ndiToObjectRF);
	vtkNew<vtkMatrix4x4> vtkBaseRFtoNdi;
	vtkBaseRFtoNdi->DeepCopy(array_ndiToBaseRF);
	vtkBaseRFtoNdi->Invert();

	vtkNew<vtkTransform> baseRFtoObjectRFtransform;
	baseRFtoObjectRFtransform->PostMultiply();
	baseRFtoObjectRFtransform->SetMatrix(vtkNdiToObjectRF);
	baseRFtoObjectRFtransform->Concatenate(vtkBaseRFtoNdi);
	baseRFtoObjectRFtransform->Update();
	auto vtkBaseRFtoObjectRF = baseRFtoObjectRFtransform->GetMatrix();

	// get base to baseRF matrix
	auto baseRFtoBase = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
	vtkNew<vtkMatrix4x4> vtkBaseToBaseRF;
	mitk::TransferItkTransformToVtkMatrix(baseRFtoBase.GetPointer(), vtkBaseToBaseRF);
	vtkBaseToBaseRF->Invert();

	// get surface to target plane matrix
	Eigen::Vector3d x_surfaceToPlane;
	x_surfaceToPlane << targetPoint_2[0] - targetPoint_1[0],
	targetPoint_2[1] - targetPoint_1[1],
	targetPoint_2[2] - targetPoint_1[2];
	x_surfaceToPlane.normalize();
	
	Eigen::Vector3d tmp_y;
	tmp_y << targetPoint_0[0] - targetPoint_1[0],
		targetPoint_0[1] - targetPoint_1[1],
		targetPoint_0[2] - targetPoint_1[2];
	tmp_y.normalize();
	
	Eigen::Vector3d z_surfaceToPlane = x_surfaceToPlane.cross(tmp_y);
	z_surfaceToPlane.normalize();
	
	Eigen::Vector3d y_surfaceToPlane = z_surfaceToPlane.cross(x_surfaceToPlane);
	y_surfaceToPlane.normalize();

	double array_surfaceToPlane[16]
	{
		x_surfaceToPlane[0], y_surfaceToPlane[0], z_surfaceToPlane[0], targetPoint_1[0],
		x_surfaceToPlane[1], y_surfaceToPlane[1], z_surfaceToPlane[1], targetPoint_1[1],
		x_surfaceToPlane[2], y_surfaceToPlane[2], z_surfaceToPlane[2], targetPoint_1[2],
		0,0,0,1
	};
	vtkNew<vtkMatrix4x4> vtkSurfaceToPlane;
	vtkSurfaceToPlane->DeepCopy(array_surfaceToPlane);

	// get base to target plane matrix
	vtkNew<vtkTransform> vtkBaseToTargetPlaneTransform;
	vtkBaseToTargetPlaneTransform->Identity();
	vtkBaseToTargetPlaneTransform->PostMultiply();
	vtkBaseToTargetPlaneTransform->SetMatrix(vtkSurfaceToPlane);
	vtkBaseToTargetPlaneTransform->Concatenate(vtkObjectRFtoSurface);
	vtkBaseToTargetPlaneTransform->Concatenate(vtkBaseRFtoObjectRF);
	vtkBaseToTargetPlaneTransform->Concatenate(vtkBaseToBaseRF);
	vtkBaseToTargetPlaneTransform->Update();

	m_T_robot = mitk::AffineTransform3D::New();
	mitk::TransferVtkMatrixToItkTransform(vtkBaseToTargetPlaneTransform->GetMatrix(), m_T_robot.GetPointer());

	m_Controls.textBrowser->append("result plane target point:" + QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));

	return true;
}


void SurgicalSimulate::OnAutoPositionStart()
{
	vtkMatrix4x4* t = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_T_robot.GetPointer(), t);

	m_KukaTrackingDevice->RobotMove(t);
}

void SurgicalSimulate::On_pushButton_goToFakePlane_clicked()
{
	vtkMatrix4x4* t = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_T_robot.GetPointer(), t);

	auto tmpVtkTrans = vtkTransform::New();

	auto offsetMatrix = vtkMatrix4x4::New();
	offsetMatrix->Identity();
	offsetMatrix->SetElement(1, 3, 50);

	tmpVtkTrans->PostMultiply();
	tmpVtkTrans->Identity();
	tmpVtkTrans->SetMatrix(offsetMatrix);
	tmpVtkTrans->Concatenate(t);
	tmpVtkTrans->Update();

	auto resultMatrix = tmpVtkTrans->GetMatrix();

	m_KukaTrackingDevice->RobotMove(resultMatrix);
}

void SurgicalSimulate::UseVirtualDevice1()
{
  //read in filename
  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
                                                  tr("Tool Storage Files (*.IGTToolStorage)"));
  if (filename.isNull()) return;

  //read tool storage from disk
  std::string errorMessage = "";
  mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
    GetDataStorage());
  m_VirtualDevice1ToolStorage = myDeserializer->Deserialize(filename.toStdString());
  m_VirtualDevice1ToolStorage->SetName(filename.toStdString());

  //! [UseKuka 1]
  //Here we want to use the VirtualDevice as a tracking device. Therefore we instantiate a object of the class
  //KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
  MITK_INFO << "VirtualDevice1 tracking";
  //QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
  m_VirtualDevice1 = mitk::VirtualTrackingDevice::New(); //instantiate

  //Create Navigation Data Source with the factory class, and the visualize filter.
  lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
    lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VirtualDevice1ToolStorage, m_VirtualDevice1);

  m_VirtualDevice1Source = kukaSourceFactory->CreateTrackingDeviceSource(m_VirtualDevice1Visualizer);

  m_VirtualDevice1Source->Connect();

  m_VirtualDevice1Source->StartTracking();

  //update visualize filter by timer
  if (m_VirtualDevice1Timer == nullptr)
  {
    m_VirtualDevice1Timer = new QTimer(this); //create a new timer
  }
  connect(m_VirtualDevice1Timer, SIGNAL(timeout()), this, SLOT(OnVirtualDevice1VisualizeTimer()));
  //connect the timer to the method OnTimer()
  connect(m_VirtualDevice1Timer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
  //connect the timer to the method OnTimer()
  ShowToolStatus_Vega();

  m_VirtualDevice1Timer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps
  auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
  mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void SurgicalSimulate::OnVirtualDevice1VisualizeTimer()
{
  //Here we call the Update() method from the Visualization Filter. Internally the filter checks if
  //new NavigationData is available. If we have a new NavigationData the cone position and orientation
  //will be adapted.
  if (m_VirtualDevice1Visualizer.IsNotNull())
  {
    m_VirtualDevice1Visualizer->Update();
    // auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
    // mitk::RenderingManager::GetInstance()->InitializeViews(geo);
    this->RequestRenderWindowUpdate();
  }
}

void SurgicalSimulate::UseVirtualDevice2()
{
  //read in filename
  QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
                                                  tr("Tool Storage Files (*.IGTToolStorage)"));
  if (filename.isNull()) return;

  //read tool storage from disk
  std::string errorMessage = "";
  mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
    GetDataStorage());
  m_VirtualDevice2ToolStorage = myDeserializer->Deserialize(filename.toStdString());
  m_VirtualDevice2ToolStorage->SetName(filename.toStdString());

  //! [UseKuka 1]
  //Here we want to use the VirtualDevice as a tracking device. Therefore we instantiate a object of the class
  //KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
  MITK_INFO << "VirtualDevice1 tracking";
  //QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
  m_VirtualDevice2 = mitk::VirtualTrackingDevice::New(); //instantiate

  //Create Navigation Data Source with the factory class, and the visualize filter.
  lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
    lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VirtualDevice2ToolStorage, m_VirtualDevice2);

  m_VirtualDevice2Source = kukaSourceFactory->CreateTrackingDeviceSource(m_VirtualDevice2Visualizer);

  m_VirtualDevice2Source->Connect();

  m_VirtualDevice2Source->StartTracking();

  //update visualize filter by timer
  if (m_VirtualDevice2Timer == nullptr)
  {
    m_VirtualDevice2Timer = new QTimer(this); //create a new timer
  }
  connect(m_VirtualDevice2Timer, SIGNAL(timeout()), this, SLOT(OnVirtualDevice2VisualizeTimer()));
  //connect the timer to the method OnTimer()
  connect(m_VirtualDevice2Timer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
  //connect the timer to the method OnTimer()
  ShowToolStatus_Kuka();
  m_VirtualDevice2Timer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

  auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
  mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void SurgicalSimulate::UpdateToolStatusWidget()
{
  m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
  m_Controls.m_StatusWidgetKukaToolToShow->Refresh();

  // Update the real time robot registration distance error
	if(m_KukaSource == nullptr || m_VegaSource == nullptr)
	{
		return;
	}

	auto baseRFtoBaseMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
	auto endRFtoFlangematrix = m_VegaToolStorage->GetToolByName("RobotEndRF")->GetToolRegistrationMatrix();

	mitk::NavigationData::Pointer nd_baseToFlange = m_KukaSource->GetOutput(0);
	mitk::NavigationData::Pointer nd_ndiToRobotEndRF = m_VegaSource->GetOutput("RobotEndRF");
	mitk::NavigationData::Pointer nd_ndiToBaseRF = m_VegaSource->GetOutput("RobotBaseRF");

	auto baseToFlangeMatrix = nd_baseToFlange->GetAffineTransform3D();
	auto ndiToBaseRFMatrix = nd_ndiToBaseRF->GetAffineTransform3D();

	vtkNew<vtkMatrix4x4> vtkNdiToBaseRF;
	mitk::TransferItkTransformToVtkMatrix(ndiToBaseRFMatrix.GetPointer(), vtkNdiToBaseRF);
	
	vtkNew<vtkMatrix4x4> vtkBaseRFtoBase;
	mitk::TransferItkTransformToVtkMatrix(baseRFtoBaseMatrix.GetPointer(), vtkBaseRFtoBase);

	vtkNew<vtkMatrix4x4> vtkFlangeToEndRF_backup;
	mitk::TransferItkTransformToVtkMatrix(endRFtoFlangematrix.GetPointer(), vtkFlangeToEndRF_backup);

	vtkNew<vtkMatrix4x4> vtkFlangeToEndRF;
	vtkFlangeToEndRF->DeepCopy(vtkFlangeToEndRF_backup);
	vtkFlangeToEndRF->Invert();

	vtkNew<vtkMatrix4x4> vtkBaseToFlange;
	mitk::TransferItkTransformToVtkMatrix(baseToFlangeMatrix.GetPointer(), vtkBaseToFlange);

	double endRFposition[3];
	endRFposition[0] = (nd_ndiToRobotEndRF->GetPosition())[0];
	endRFposition[1] = (nd_ndiToRobotEndRF->GetPosition())[1];
	endRFposition[2] = (nd_ndiToRobotEndRF->GetPosition())[2];

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(vtkFlangeToEndRF);
	tmpTransform->Concatenate(vtkBaseToFlange);
	tmpTransform->Concatenate(vtkBaseRFtoBase);
	tmpTransform->Concatenate(vtkNdiToBaseRF);
	tmpTransform->Update();

	auto vtkNdiToEndRFmatrix = tmpTransform->GetMatrix();

	double  endRFposition_1[3]
	{
		vtkNdiToEndRFmatrix->GetElement(0,3),vtkNdiToEndRFmatrix->GetElement(1,3),vtkNdiToEndRFmatrix->GetElement(2,3)
	};

	double error = sqrt(pow(endRFposition_1[0] - endRFposition[0],2) +
		pow(endRFposition_1[1] - endRFposition[1],2)+
		pow(endRFposition_1[2] - endRFposition[2], 2));
	m_Controls.lineEdit_roboRegistrationError->setText(QString::number(error));

	// Update the real time robot registration angle error
	auto vtkNdiToRoboEndRFmatrix_direct = getVtkMatrix4x4(nd_ndiToRobotEndRF);
	double x_direct[3]
	{
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0,0),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1,0),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2,0)
	};

	double x_indirect[3]
	{
		vtkNdiToEndRFmatrix->GetElement(0,0),
		vtkNdiToEndRFmatrix->GetElement(1,0),
		vtkNdiToEndRFmatrix->GetElement(2,0)
	};

	double y_direct[3]
	{
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0,1),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1,1),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2,1)
	};

	double y_indirect[3]
	{
		vtkNdiToEndRFmatrix->GetElement(0,1),
		vtkNdiToEndRFmatrix->GetElement(1,1),
		vtkNdiToEndRFmatrix->GetElement(2,1)
	};

	double z_direct[3]
	{
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0,2),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1,2),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2,2)
	};

	double z_indirect[3]
	{
		vtkNdiToEndRFmatrix->GetElement(0,2),
		vtkNdiToEndRFmatrix->GetElement(1,2),
		vtkNdiToEndRFmatrix->GetElement(2,2)
	};

	double x_angleError = (180 / 3.14159265359) * acos(x_direct[0]*x_indirect[0] 
		+ x_direct[1] * x_indirect[1]
		+ x_direct[2] * x_indirect[2]);

	double y_angleError = (180 / 3.14159265359) * acos(y_direct[0] * y_indirect[0]
		+ y_direct[1] * y_indirect[1]
		+ y_direct[2] * y_indirect[2]);

	double z_angleError = (180 / 3.14159265359) * acos(z_direct[0] * z_indirect[0]
		+ z_direct[1] * z_indirect[1]
		+ z_direct[2] * z_indirect[2]);

	m_Controls.lineEdit_xAngleError->setText(QString::number(x_angleError));
	m_Controls.lineEdit_yAngleError->setText(QString::number(y_angleError));
	m_Controls.lineEdit_zAngleError->setText(QString::number(z_angleError));


}

void SurgicalSimulate::ShowToolStatus_Vega()
{
  m_VegaNavigationData.clear();
  for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
  {
    m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
  }
  //initialize widget
  m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
  m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
  m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
  m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
  m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();
}

void SurgicalSimulate::ShowToolStatus_Kuka()
{
  m_KukaNavigationData.clear();
  for (std::size_t i = 0; i < m_KukaSource->GetNumberOfOutputs(); i++)
  {
    m_KukaNavigationData.push_back(m_KukaSource->GetOutput(i));
  }
  //initialize widget
  m_Controls.m_StatusWidgetKukaToolToShow->RemoveStatusLabels();
  m_Controls.m_StatusWidgetKukaToolToShow->SetShowPositions(true);
  m_Controls.m_StatusWidgetKukaToolToShow->SetTextAlignment(Qt::AlignLeft);
  m_Controls.m_StatusWidgetKukaToolToShow->SetNavigationDatas(&m_KukaNavigationData);
  m_Controls.m_StatusWidgetKukaToolToShow->ShowStatusLabels();
}


bool SurgicalSimulate::AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval, int intervalNum, double matrixArray[16])
{
	// The frame rate of Vega ST is 60 Hz, so the timeInterval should be larger than 16.7 ms

	double tmp_x[3]{ 0,0,0 };
	double tmp_y[3]{ 0,0,0 };
	double tmp_translation[3]{ 0,0,0 };

	for (int i{ 0 }; i < intervalNum; i++)
	{
		ndPtr->Update();

		auto tmpMatrix = getVtkMatrix4x4(ndPtr);

		MITK_INFO << "Averaging NavigationData and print the 1st element:" << tmpMatrix->GetElement(0, 0);

		tmp_x[0] += tmpMatrix->GetElement(0, 0);
		tmp_x[1] += tmpMatrix->GetElement(1, 0);
		tmp_x[2] += tmpMatrix->GetElement(2, 0);

		tmp_y[0] += tmpMatrix->GetElement(0, 1);
		tmp_y[1] += tmpMatrix->GetElement(1, 1);
		tmp_y[2] += tmpMatrix->GetElement(2, 1);

		tmp_translation[0] += tmpMatrix->GetElement(0, 3);
		tmp_translation[1] += tmpMatrix->GetElement(1, 3);
		tmp_translation[2] += tmpMatrix->GetElement(2, 3);

		QThread::msleep(timeInterval);
	}

	// Assemble baseRF to EndRF matrix
	Eigen::Vector3d x;
	x[0] = tmp_x[0];
	x[1] = tmp_x[1];
	x[2] = tmp_x[2];
	x.normalize();

	Eigen::Vector3d h;
	h[0] = tmp_y[0];
	h[1] = tmp_y[1];
	h[2] = tmp_y[2];
	h.normalize();

	Eigen::Vector3d z;
	z = x.cross(h);
	z.normalize();

	Eigen::Vector3d y;
	y = z.cross(x);
	y.normalize();

	tmp_translation[0] = tmp_translation[0] / intervalNum;
	tmp_translation[1] = tmp_translation[1] / intervalNum;
	tmp_translation[2] = tmp_translation[2] / intervalNum;

	double tmpArray[16]
	{
	  x[0], y[0], z[0], tmp_translation[0],
	  x[1], y[1], z[1], tmp_translation[1],
	  x[2], y[2], z[2], tmp_translation[2],
	  0,0,0,1
	};

	for (int i{ 0 }; i < 16; i++)
	{
		matrixArray[i] = tmpArray[i];
	}
	
	return true;
}

vtkMatrix4x4* SurgicalSimulate::getVtkMatrix4x4(mitk::NavigationData::Pointer nd)
{
	auto o = nd->GetOrientation();
	double R[3][3];
	double* V = { nd->GetPosition().GetDataPointer() };
	vtkQuaterniond quaterniond{ o.r(), o.x(), o.y(), o.z() };
	quaterniond.ToMatrix3x3(R);

	vtkMatrix4x4* matrix = vtkMatrix4x4::New();
	matrix->SetElement(0, 0, R[0][0]);
	matrix->SetElement(0, 1, R[0][1]);
	matrix->SetElement(0, 2, R[0][2]);
	matrix->SetElement(1, 0, R[1][0]);
	matrix->SetElement(1, 1, R[1][1]);
	matrix->SetElement(1, 2, R[1][2]);
	matrix->SetElement(2, 0, R[2][0]);
	matrix->SetElement(2, 1, R[2][1]);
	matrix->SetElement(2, 2, R[2][2]);

	matrix->SetElement(0, 3, V[0]);
	matrix->SetElement(1, 3, V[1]);
	matrix->SetElement(2, 3, V[2]);

	matrix->Print(std::cout);
	return matrix;
}

bool SurgicalSimulate::ResetRobotTcp()
{
	double tcp[6]{ 0 };
	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
	return true;
}

bool SurgicalSimulate::SetPrecisionTestTcp()
{
	// set TCP for precision test
	// For Test Use, regard ball 2 as the TCP, the pose can be seen on
	// https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	//--------------------------------------------------
	Eigen::Vector3d p2;
	p2[0] = m_Controls.lineEdit_p2_x->text().toDouble();
	p2[1] = m_Controls.lineEdit_p2_y->text().toDouble();
	p2[2] = m_Controls.lineEdit_p2_z->text().toDouble();

	Eigen::Vector3d p3;
	p3[0] = m_Controls.lineEdit_p3_x->text().toDouble();
	p3[1] = m_Controls.lineEdit_p3_y->text().toDouble();
	p3[2] = m_Controls.lineEdit_p3_z->text().toDouble();

	Eigen::Vector3d x_tcp;
	x_tcp = p3 - p2;
	// x_tcp[0] = 51.91;
	// x_tcp[1] = -55.01;
	// x_tcp[2] = 0.16;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);
	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = p2[0]; // tx
	tcp[1] = p2[1]; // ty
	tcp[2] = p2[2]; // tz
	tcp[3] = eulerAngle(0);//-0.81;// -0.813428203; // rz
	tcp[4] = eulerAngle(1); // ry
	tcp[5] = eulerAngle(2); // rx
	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

	return true;
}

// bool SurgicalSimulate::SetPlanePrecisionTestTcp()
// {
// 	// set TCP for precision test
// 	// For plane Test Use, regard ball 2 as the TCP, the pose can be seen on
// 	// https://gn1phhht53.feishu.cn/wiki/wikcnqHGHeSFDrg6pDogJH6c8pe
// 	//--------------------------------------------------
// 	Eigen::Vector3d p2;
// 	p2[0] = m_Controls.lineEdit_plane_p2_x->text().toDouble();
// 	p2[1] = m_Controls.lineEdit_plane_p2_y->text().toDouble();
// 	p2[2] = m_Controls.lineEdit_plane_p2_z->text().toDouble();
//
// 	Eigen::Vector3d p3;
// 	p3[0] = m_Controls.lineEdit_plane_p3_x->text().toDouble();
// 	p3[1] = m_Controls.lineEdit_plane_p3_y->text().toDouble();
// 	p3[2] = m_Controls.lineEdit_plane_p3_z->text().toDouble();
//
// 	Eigen::Vector3d p4;
// 	p4[0] = m_Controls.lineEdit_plane_p4_x->text().toDouble();
// 	p4[1] = m_Controls.lineEdit_plane_p4_y->text().toDouble();
// 	p4[2] = m_Controls.lineEdit_plane_p4_z->text().toDouble();
//
// 	// Eigen::Vector3d x_tcp;
// 	// x_tcp[0] = 1.0;
// 	// x_tcp[1] = 0.0;
// 	// x_tcp[2] = 0.0;
// 	// x_tcp.normalize();
// 	//
// 	// Eigen::Vector3d z_flange;
// 	// z_flange[0] = 0.0;
// 	// z_flange[1] = 0.0;
// 	// z_flange[2] = 1;
// 	//
// 	// Eigen::Vector3d y_tcp;
// 	// y_tcp = z_flange.cross(x_tcp);
// 	// y_tcp.normalize();
// 	//
// 	// Eigen::Vector3d z_tcp;
// 	// z_tcp = x_tcp.cross(y_tcp);
//
// 	Eigen::Vector3d z_tcp;
// 	z_tcp = p2 - p3;
// 	z_tcp.normalize();
//
// 	Eigen::Vector3d y_tmp;
// 	y_tmp = p4 - p2;
//
// 	Eigen::Vector3d x_tcp;
// 	x_tcp = y_tmp.cross(z_tcp);
// 	x_tcp.normalize();
//
// 	Eigen::Vector3d y_tcp;
// 	y_tcp = z_tcp.cross(x_tcp);
// 	y_tcp.normalize();
//
// 	Eigen::Matrix3d Re;
//
// 	Re << x_tcp[0], y_tcp[0], z_tcp[0],
// 		x_tcp[1], y_tcp[1], z_tcp[1],
// 		x_tcp[2], y_tcp[2], z_tcp[2];
//
// 	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
//
// 	//------------------------------------------------
// 	double tcp[6];
// 	tcp[0] = -10.17; // tx
// 	tcp[1] = 137.40; // ty
// 	tcp[2] = 114.13; // tz
// 	tcp[3] = eulerAngle(0); //-0.81;// -0.813428203; // rz
// 	tcp[4] = eulerAngle(1); // ry
// 	tcp[5] = eulerAngle(2); // rx
// 	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
// 	//set tcp to robot
// 	  //set tcp
// 	QThread::msleep(1000);
// 	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
// 	QThread::msleep(1000);
// 	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
// 	QThread::msleep(1000);
// 	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
// 	return true;
// }


bool SurgicalSimulate::SetPlanePrecisionTestTcp()
{
	// set TCP for precision test
	// For plane Test Use, regard ball 2 as the TCP, the pose can be seen on
	// https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh?table=tblk7ke86l8RyAoN&view=vew1Kd7wVv
	//--------------------------------------------------
	Eigen::Vector3d p1;
	p1[0] = m_Controls.lineEdit_plane_p2_x->text().toDouble();
	p1[1] = m_Controls.lineEdit_plane_p2_y->text().toDouble();
	p1[2] = m_Controls.lineEdit_plane_p2_z->text().toDouble();

	Eigen::Vector3d p2;
	p2[0] = m_Controls.lineEdit_plane_p3_x->text().toDouble();
	p2[1] = m_Controls.lineEdit_plane_p3_y->text().toDouble();
	p2[2] = m_Controls.lineEdit_plane_p3_z->text().toDouble();

	Eigen::Vector3d p3;
	p3[0] = m_Controls.lineEdit_plane_p4_x->text().toDouble();
	p3[1] = m_Controls.lineEdit_plane_p4_y->text().toDouble();
	p3[2] = m_Controls.lineEdit_plane_p4_z->text().toDouble();

	// Eigen::Vector3d x_tcp;
	// x_tcp[0] = 1.0;
	// x_tcp[1] = 0.0;
	// x_tcp[2] = 0.0;
	// x_tcp.normalize();
	//
	// Eigen::Vector3d z_flange;
	// z_flange[0] = 0.0;
	// z_flange[1] = 0.0;
	// z_flange[2] = 1;
	//
	// Eigen::Vector3d y_tcp;
	// y_tcp = z_flange.cross(x_tcp);
	// y_tcp.normalize();
	//
	// Eigen::Vector3d z_tcp;
	// z_tcp = x_tcp.cross(y_tcp);

	Eigen::Vector3d x_tcp;
	x_tcp = p3 - p1;
	x_tcp.normalize();

	Eigen::Vector3d y_tmp;
	y_tmp = p2 - p1;
	y_tmp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tmp);
	z_tcp.normalize();

	Eigen::Vector3d y_tcp;
	y_tcp = z_tcp.cross(x_tcp);
	y_tcp.normalize();

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = p1[0]; // tx
	tcp[1] = p1[1]; // ty
	tcp[2] = p1[2]; // tz
	tcp[3] = eulerAngle(0); //-0.81;// -0.813428203; // rz
	tcp[4] = eulerAngle(1); // ry
	tcp[5] = eulerAngle(2); // rx
	MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
	//set tcp to robot
	  //set tcp
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
	return true;
}


void SurgicalSimulate::On_pushButton_updateRobotSimuPose_clicked()
{
	// F1 --> F2
	// rotation: jt_1
	// translation: [0, 0, 0]
	auto vtkTrans_F1ToF2 = vtkTransform::New();
	vtkTrans_F1ToF2->Identity();
	vtkTrans_F1ToF2->PostMultiply();
	vtkTrans_F1ToF2->RotateZ(m_Controls.lineEdit_jt_1->text().toDouble()); // in degrees
	vtkTrans_F1ToF2->Update();
	auto vtkMatrix_F1ToF2 = vtkTrans_F1ToF2->GetMatrix();

	// F2 --> F3
	// rotation: jt_2
	// translation: [0, 0, 202.2]
	auto vtkTrans_F2ToF3 = vtkTransform::New();
	vtkTrans_F2ToF3->Identity();
	vtkTrans_F2ToF3->PostMultiply();
	vtkTrans_F2ToF3->RotateY(m_Controls.lineEdit_jt_2->text().toDouble()); // in degrees
	vtkTrans_F2ToF3->Translate(0, 0, 202.2);
	vtkTrans_F2ToF3->Update();
	auto vtkMatrix_F2ToF3 = vtkTrans_F2ToF3->GetMatrix();

	// F3 --> F4
	// rotation: jt_3
	// translation: [0, 0, 441-202.2]
	auto vtkTrans_F3ToF4 = vtkTransform::New();
	vtkTrans_F3ToF4->Identity();
	vtkTrans_F3ToF4->PostMultiply();
	vtkTrans_F3ToF4->RotateZ(m_Controls.lineEdit_jt_3->text().toDouble()); // in degrees
	vtkTrans_F3ToF4->Translate(0, 0, 441 - 202.2);
	vtkTrans_F3ToF4->Update();
	auto vtkMatrix_F3ToF4 = vtkTrans_F3ToF4->GetMatrix();

	// F4 --> F5
	// rotation: - jt_4
	// translation: [0, 0, 621.6 - 441]
	auto vtkTrans_F4ToF5 = vtkTransform::New();
	vtkTrans_F4ToF5->Identity();
	vtkTrans_F4ToF5->PostMultiply();
	vtkTrans_F4ToF5->RotateY(-m_Controls.lineEdit_jt_4->text().toDouble()); // in degrees
	vtkTrans_F4ToF5->Translate(0, 0, 621.6 - 441);
	vtkTrans_F4ToF5->Update();
	auto vtkMatrix_F4ToF5 = vtkTrans_F4ToF5->GetMatrix();

	// F5 --> F6
	// rotation: jt_5
	// translation: [0, 0, 839.5 - 621.6]
	auto vtkTrans_F5ToF6 = vtkTransform::New();
	vtkTrans_F5ToF6->Identity();
	vtkTrans_F5ToF6->PostMultiply();
	vtkTrans_F5ToF6->RotateZ(m_Controls.lineEdit_jt_5->text().toDouble()); // in degrees
	vtkTrans_F5ToF6->Translate(0, 0, 839.5 - 621.6);
	vtkTrans_F5ToF6->Update();
	auto vtkMatrix_F5ToF6 = vtkTrans_F5ToF6->GetMatrix();

	// F6 --> F7
	// rotation: jt_6
	// translation: [0, -61.2, 1021 - 839.5]
	auto vtkTrans_F6ToF7 = vtkTransform::New();
	vtkTrans_F6ToF7->Identity();
	vtkTrans_F6ToF7->PostMultiply();
	vtkTrans_F6ToF7->RotateY(m_Controls.lineEdit_jt_6->text().toDouble()); // in degrees
	vtkTrans_F6ToF7->Translate(0, -61.2, 1021 - 839.5);
	vtkTrans_F6ToF7->Update();
	auto vtkMatrix_F6ToF7 = vtkTrans_F6ToF7->GetMatrix();

	// F7 --> F8
	// rotation: jt_7
	// translation: [0, 61.2, 1113 - 1021]
	auto vtkTrans_F7ToF8 = vtkTransform::New();
	vtkTrans_F7ToF8->Identity();
	vtkTrans_F7ToF8->PostMultiply();
	vtkTrans_F7ToF8->RotateZ(m_Controls.lineEdit_jt_7->text().toDouble()); // in degrees
	vtkTrans_F7ToF8->Translate(0, 61.2, 1113 - 1021);
	vtkTrans_F7ToF8->Update();
	auto vtkMatrix_F7ToF8 = vtkTrans_F7ToF8->GetMatrix();


	// world --> F1
	auto vtkMatrix_sceneToF1 = vtkMatrix4x4::New();
	vtkMatrix_sceneToF1->Identity();

	// world --> F2
	auto vtkTrans_sceneToF2 = vtkTransform::New();
	vtkTrans_sceneToF2->Identity();
	vtkTrans_sceneToF2->PostMultiply();
	vtkTrans_sceneToF2->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF2->Concatenate(vtkMatrix_sceneToF1);

	// world --> F3
	auto vtkTrans_sceneToF3 = vtkTransform::New();
	vtkTrans_sceneToF3->Identity();
	vtkTrans_sceneToF3->PostMultiply();
	vtkTrans_sceneToF3->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF3->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF3->Concatenate(vtkMatrix_sceneToF1);

	// world --> F4
	auto vtkTrans_sceneToF4 = vtkTransform::New();
	vtkTrans_sceneToF4->Identity();
	vtkTrans_sceneToF4->PostMultiply();
	vtkTrans_sceneToF4->Concatenate(vtkMatrix_F3ToF4);
	vtkTrans_sceneToF4->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF4->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF4->Concatenate(vtkMatrix_sceneToF1);

	// world --> F5
	auto vtkTrans_sceneToF5 = vtkTransform::New();
	vtkTrans_sceneToF5->Identity();
	vtkTrans_sceneToF5->PostMultiply();
	vtkTrans_sceneToF5->Concatenate(vtkMatrix_F4ToF5);
	vtkTrans_sceneToF5->Concatenate(vtkMatrix_F3ToF4);
	vtkTrans_sceneToF5->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF5->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF5->Concatenate(vtkMatrix_sceneToF1);

	// world --> F6
	auto vtkTrans_sceneToF6 = vtkTransform::New();
	vtkTrans_sceneToF6->Identity();
	vtkTrans_sceneToF6->PostMultiply();
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_F5ToF6);
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_F4ToF5);
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_F3ToF4);
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF6->Concatenate(vtkMatrix_sceneToF1);

	// world --> F7
	auto vtkTrans_sceneToF7 = vtkTransform::New();
	vtkTrans_sceneToF7->Identity();
	vtkTrans_sceneToF7->PostMultiply();
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F6ToF7);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F5ToF6);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F4ToF5);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F3ToF4);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF7->Concatenate(vtkMatrix_sceneToF1);

	// world --> F8
	auto vtkTrans_sceneToF8 = vtkTransform::New();
	vtkTrans_sceneToF8->Identity();
	vtkTrans_sceneToF8->PostMultiply();
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F7ToF8);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F6ToF7);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F5ToF6);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F4ToF5);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F3ToF4);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F2ToF3);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_F1ToF2);
	vtkTrans_sceneToF8->Concatenate(vtkMatrix_sceneToF1);

	auto p_1 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_1");
	p_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkMatrix_sceneToF1);
	auto f_1 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_1");
	f_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkMatrix_sceneToF1);

	auto p_2 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_2");
	p_2->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF2->GetMatrix());
	auto f_2 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_2");
	f_2->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF2->GetMatrix());

	auto p_3 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_3");
	p_3->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF3->GetMatrix());
	auto f_3 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_3");
	f_3->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF3->GetMatrix());

	auto p_4 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_4");
	p_4->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF4->GetMatrix());
	auto f_4 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_4");
	f_4->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF4->GetMatrix());

	auto p_5 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_5");
	p_5->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF5->GetMatrix());
	auto f_5 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_5");
	f_5->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF5->GetMatrix());

	auto p_6 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_6");
	p_6->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF6->GetMatrix());
	auto f_6 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_6");
	f_6->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF6->GetMatrix());

	auto p_7 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_7");
	p_7->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF7->GetMatrix());
	auto f_7 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_7");
	f_7->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF7->GetMatrix());

	auto p_8 = GetDataStorage()->GetNamedObject<mitk::Surface>("p_8");
	p_8->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF8->GetMatrix());
	auto f_8 = GetDataStorage()->GetNamedObject<mitk::Surface>("f_8");
	f_8->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF8->GetMatrix());

	auto tool = GetDataStorage()->GetNamedObject<mitk::Surface>("THA_endEffector");
	tool->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkTrans_sceneToF8->GetMatrix());


	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}


void SurgicalSimulate::On_pushButton_a1_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_1->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_1->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a1_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_1->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_1->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a2_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_2->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_2->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a2_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_2->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_2->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a3_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_3->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_3->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a3_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_3->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_3->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a4_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_4->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_4->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a4_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_4->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_4->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a5_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_5->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_5->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a5_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_5->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_5->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a6_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_6->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_6->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a6_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_6->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_6->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a7_m_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_7->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle - stepSize;

	m_Controls.lineEdit_jt_7->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

void SurgicalSimulate::On_pushButton_a7_p_clicked()
{
	double currentAngle = m_Controls.lineEdit_jt_7->text().toDouble();
	double stepSize = m_Controls.lineEdit_robotMoveStep->text().toDouble();

	double newAngle = currentAngle + stepSize;

	m_Controls.lineEdit_jt_7->setText(QString::number(newAngle));

	On_pushButton_updateRobotSimuPose_clicked();
}

