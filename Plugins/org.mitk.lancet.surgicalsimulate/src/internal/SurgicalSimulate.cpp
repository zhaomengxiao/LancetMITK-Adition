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
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_metaImageNode);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetPoint);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_ImageCheckPoint);


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
  connect(m_Controls.pushButton_captureSurgicalPlane, &QPushButton::clicked, this,
          &SurgicalSimulate::OnCaptureProbeAsSurgicalPlane);
  connect(m_Controls.pushButton_startAutoPosition, &QPushButton::clicked, this, &SurgicalSimulate::OnAutoPositionStart);
  connect(m_Controls.pushButton_saveRobotRegist, &QPushButton::clicked, this, &SurgicalSimulate::OnSaveRobotRegistraion);
  connect(m_Controls.pushButton_usePreRobotRegit, &QPushButton::clicked, this, &SurgicalSimulate::OnUsePreRobotRegitration);

  connect(m_Controls.pushButton_confirmImageTarget, &QPushButton::clicked, this, &SurgicalSimulate::InterpretImagePoint);
  connect(m_Controls.pushButton_applyPreImageRegistration, &QPushButton::clicked, this, &SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration);
  connect(m_Controls.pushButton_applyPreImageRegistrationNew, &QPushButton::clicked, this, &SurgicalSimulate::ApplyPreexistingImageSurfaceRegistration_staticImage);
  connect(m_Controls.pushButton_applyRegistrationNew, &QPushButton::clicked, this, &SurgicalSimulate::ApplySurfaceRegistration_staticImage);

	connect(m_Controls.pushButton_saveNdiTools, &QPushButton::clicked, this, &SurgicalSimulate::OnSaveRobotRegistraion);


  connect(m_Controls.pushButton_setTCP, &QPushButton::clicked, this, &SurgicalSimulate::OnSetTCP);
  connect(m_Controls.pushButton_confirmImageTargetLine, &QPushButton::clicked, this, &SurgicalSimulate::InterpretImageLine);
  connect(m_Controls.pushButton_probeCheckPoint, &QPushButton::clicked, this, &SurgicalSimulate::ProbeImageCheckPoint);

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
  m_RobotRegistration.AddPose(nd_robot2flange, nd_RobotBaseRF2RobotEndRF, translationOnly);

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
    MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
  }
  else if (m_IndexOfRobotCapture < 10) //the last five rotations
  {
    CapturePose(false);
    //Increase the count each time you click the button
    m_IndexOfRobotCapture++;
    MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
  }
  else
  {
	MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
    vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
    m_RobotRegistration.GetRegistraionMatrix(matrix4x4);
    
    //For Test Use ,4L tka device registration result ,you can skip registration workflow by using it, Only if the RobotBase Reference Frame not moved!
    /*vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
    matrix4x4->SetElement(0, 0, -0.48); matrix4x4->SetElement(0, 1, -0.19); matrix4x4->SetElement(0, 2, -0.86);
    matrix4x4->SetElement(1, 0, -0.01); matrix4x4->SetElement(1, 1, -0.97); matrix4x4->SetElement(1, 2, 0.22);
    matrix4x4->SetElement(2, 0, -0.88); matrix4x4->SetElement(2, 1, 0.11); matrix4x4->SetElement(2, 2, 0.46);
    matrix4x4->SetElement(0, 3, 162.37);
    matrix4x4->SetElement(1, 3, -530.45);
    matrix4x4->SetElement(2, 3, -255.62);*/

    m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();

    mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());

    //save robot registration matrix into reference tool
    m_VegaToolStorage->GetToolByName("RobotBaseRF")->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);

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
  switch (m_IndexOfRobotCapture)
  {
  case 1: //z+50

    affine->Translate(trans1);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 2: //y+50


    affine->Translate(trans2);

    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 3: //x+50


    affine->Translate(trans3);

    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 4: //z-50


    affine->Translate(trans4);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 5: //x rotate 10 degree x -25


    affine->Rotate3D(axisx, 0.174);
    affine->Translate(trans5);

    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 6: //x rotate -20 degree y -25

    affine->Rotate3D(axisx, -0.174 * 2);
    affine->Translate(trans6);

    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 7: //y rotate 10 degree y +25

    affine->Rotate3D(axisy, 0.174);
    affine->Translate(trans7);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 8: //y rotate -20 degree x +25

    affine->Rotate3D(axisy, -0.174 * 2);
    affine->Translate(trans8);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  case 9: //z rotate 20 degree x -25

    affine->Rotate3D(axisz, 0.174 * 2);
    affine->Translate(trans9);
    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;

  default:

    mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

    m_KukaTrackingDevice->RobotMove(vtkMatrix);
    break;
  }
}

void SurgicalSimulate::OnResetRobotRegistration()
{
  m_RobotRegistration.RemoveAllPose();
  m_IndexOfRobotCapture = 0;
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

void SurgicalSimulate::OnSetTCP()
{
	QString tcpNum = m_Controls.lineEdit_tcp->text();

	m_KukaTrackingDevice->RequestExecOperate("setTcpNum", { "1", tcpNum });
	m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "0" });
}

void SurgicalSimulate::OnCaptureProbeAsSurgicalPlane()
{
  //create NavigationDataToPointSetFilter to get a point3D by probe in NDI coordinates
  mitk::NavigationDataToPointSetFilter::Pointer probePoint = mitk::NavigationDataToPointSetFilter::New();
  //auto probeToolIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
  probePoint->SetInput(m_VegaSource->GetOutput("Probe"));
  probePoint->SetOperationMode(mitk::NavigationDataToPointSetFilter::Mode3DMean);
  probePoint->SetNumberForMean(10);
  //run the filter
  probePoint->Update();
  //get output
  mitk::PointSet::Pointer target = probePoint->GetOutput(0);

  // //create Surgaical plane
  // m_SurgicalPlan = lancet::PointPath::New();
  //convert to robot coordinates
  mitk::AffineTransform3D::Pointer targetMatrix = mitk::AffineTransform3D::New();
  targetMatrix->SetOffset(target->GetPoint(0).GetDataPointer());
  MITK_INFO << "Captured Point: " << targetMatrix;

  m_T_robot = mitk::AffineTransform3D::New();
  m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
  m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", targetMatrix, m_T_robot);

  //========
  //convert from ndi to robot use navigationTree
  //========
    //build the tree
  //NavigationTree::Pointer tree = NavigationTree::New();

  //NavigationNode::Pointer ndi = NavigationNode::New();
  //ndi->SetNavigationData(mitk::NavigationData::New());
  //ndi->SetNodeName("ndi");

  //tree->Init(ndi);

  //NavigationNode::Pointer robotBaseRF = NavigationNode::New();
  //robotBaseRF->SetNodeName("RobotBaseRF");
  //robotBaseRF->SetNavigationData(m_VegaSource->GetOutput("RobotBaseRF"));

  //tree->AddChild(robotBaseRF, ndi);

  //NavigationNode::Pointer robot = NavigationNode::New();
  //robot->SetNodeName("Robot");
  //robot->SetNavigationData(mitk::NavigationData::New(m_RobotRegistrationMatrix));

  //tree->AddChild(robot, robotBaseRF);

  //  //use tree
  //mitk::NavigationData::Pointer treeRes =  tree->GetNavigationData(mitk::NavigationData::New(targetMatrix), "ndi", "Robot");
  //mitk::AffineTransform3D::Pointer treeResMatrix = treeRes->GetAffineTransform3D();

  //m_T_robot = treeResMatrix;

  //MITK_INFO << "tree res";
  //MITK_INFO << treeResMatrix;
  //========
  //convert from ndi to robot use navigationTree
  //========
  
  //by hand
  //Td2e = Td2c*Tc2a*Ta2e
	//  = Tc2d^-1 * Ta2c^-1 *Ta2e
	//  = m_ndD^-1 * m_ndC^-1 *m_ndInput
 /* mitk::NavigationData::Pointer byhand = mitk::NavigationData::New(targetMatrix);
  byhand->Compose(m_VegaSource->GetOutput("RobotBaseRF")->GetInverse());
  byhand->Compose(mitk::NavigationData::New(m_RobotRegistrationMatrix)->GetInverse());
  MITK_INFO << "by hand:";
  MITK_INFO << byhand->GetAffineTransform3D();

  MITK_INFO << "correct:";
  MITK_INFO << m_T_robot;*/


  //use robot matrix,not change the end tool rotation,only apply the offset from probe;
  m_T_robot->SetMatrix(m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetMatrix());

  m_Controls.textBrowser->append(QString::number(m_T_robot->GetOffset()[0]) + "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));


  MITK_INFO << m_T_robot;
}

bool SurgicalSimulate::InterpretImagePoint()
{
	auto targetPoint = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetPoint->GetSelectedNode()->GetData())->GetPoint(0);
	auto ndiToObjectRfMatrix = m_VegaSource->GetOutput("ObjectRf")->GetAffineTransform3D();

	//auto surfaceToRfMatrix = mitk::AffineTransform3D::New();
	auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();


	auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
	vtkNew<vtkMatrix4x4> tmpMatrix;
	mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), tmpMatrix);
	tmpMatrix->Invert();

	mitk::TransferVtkMatrixToItkTransform(tmpMatrix, rfToSurfaceMatrix.GetPointer());
	
	auto ndiToTargetMatrix = mitk::AffineTransform3D::New();
	m_T_robot = mitk::AffineTransform3D::New();

	// auto targetMatrix = mitk::AffineTransform3D::New();
	ndiToTargetMatrix->SetOffset(targetPoint.GetDataPointer());
	ndiToTargetMatrix->Compose(rfToSurfaceMatrix);
	ndiToTargetMatrix->Compose(ndiToObjectRfMatrix);
	
	m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix, m_T_robot);

	/*auto registrationMatrix_robot = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
	auto matrix_roboBaseToRoboBaseRF = mitk::AffineTransform3D::New();
	vtkNew<vtkMatrix4x4> tmpMatrix2;
	mitk::TransferItkTransformToVtkMatrix(registrationMatrix_robot.GetPointer(), tmpMatrix2);
	tmpMatrix2->Invert();

	mitk::TransferVtkMatrixToItkTransform(tmpMatrix2, matrix_roboBaseToRoboBaseRF.GetPointer());*/

	//m_T_robot->Compose(matrix_roboBaseToRoboBaseRF);

	m_Controls.textBrowser->append(QString::number(m_T_robot->GetOffset()[0])+ "/" + QString::number(m_T_robot->GetOffset()[1]) + "/" + QString::number(m_T_robot->GetOffset()[2]));

	m_T_robot->SetMatrix(m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetMatrix());

	return true;
}

bool SurgicalSimulate::InterpretImageLine()
{
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);

	// Interpret targetPoint_0 from image frame to robot (internal) base frame
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
	auto targetPointUnderBase_0 = targetUnderBase_0->GetOffset();

	auto ndiToTargetMatrix_1 = mitk::AffineTransform3D::New();
	ndiToTargetMatrix_1->SetOffset(targetPoint_1.GetDataPointer());
	ndiToTargetMatrix_1->Compose(rfToSurfaceMatrix);
	ndiToTargetMatrix_1->Compose(ndiToObjectRfMatrix);
	auto targetUnderBase_1 = mitk::AffineTransform3D::New();
	m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix_1, targetUnderBase_1);
	auto targetPointUnderBase_1 = targetUnderBase_1->GetOffset();

	
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


void SurgicalSimulate::OnAutoPositionStart()
{
   vtkMatrix4x4* t = vtkMatrix4x4::New();
   mitk::TransferItkTransformToVtkMatrix(m_T_robot.GetPointer(), t);
  
   m_KukaTrackingDevice->RobotMove(t);
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
