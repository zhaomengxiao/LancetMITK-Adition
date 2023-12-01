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
#include "SpineDemo.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <QTimer>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"

const std::string SpineDemo::VIEW_ID = "org.mitk.views.spinedemo";

void SpineDemo::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void SpineDemo::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &SpineDemo::DoImageProcessing);
  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_implant_3d);
  connect(m_Controls.pushButton_connectCamera, &QPushButton::clicked, this, &SpineDemo::on_pushButton_connectCamera_clicked);
  connect(m_Controls.pushButton_initCheckPoint_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_initCheckPoint_3d_clicked);
  connect(m_Controls.pushButton_detectBalls_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_detectBalls_3d_clicked);
  connect(m_Controls.pushButton_startPlan_3d, &QPushButton::clicked, this, &SpineDemo::on_pushButton_startPlan_3d_clicked);

  connect(m_Controls.pushButton_U_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_ax_clicked);
  connect(m_Controls.pushButton_D_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_ax_clicked);
  connect(m_Controls.pushButton_R_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_ax_clicked);
  connect(m_Controls.pushButton_L_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_ax_clicked);
  connect(m_Controls.pushButton_D_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_cor_clicked);
  connect(m_Controls.pushButton_D_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_D_sag_clicked);
  connect(m_Controls.pushButton_U_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_cor_clicked);
  connect(m_Controls.pushButton_U_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_U_sag_clicked);
  connect(m_Controls.pushButton_L_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_cor_clicked);
  connect(m_Controls.pushButton_R_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_cor_clicked);
  connect(m_Controls.pushButton_L_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_L_sag_clicked);
  connect(m_Controls.pushButton_R_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_R_sag_clicked);
  connect(m_Controls.pushButton_clock_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_cor_clicked);
  connect(m_Controls.pushButton_counter_cor, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_cor_clicked);
  connect(m_Controls.pushButton_clock_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_sag_clicked);
  connect(m_Controls.pushButton_counter_sag, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_sag_clicked);
  connect(m_Controls.pushButton_clock_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_clock_ax_clicked);
  connect(m_Controls.pushButton_counter_ax, &QPushButton::clicked, this, &SpineDemo::on_pushButton_counter_ax_clicked);

}

void SpineDemo::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void SpineDemo::on_pushButton_connectCamera_clicked()
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


	MITK_INFO << "Vega tracking";
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();

	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &SpineDemo::OnVegaVisualizeTimer);

	ShowToolStatus_Vega();

	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	m_ImageRegistrationMatrix->Identity();
}


void SpineDemo::OnVegaVisualizeTimer()
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

	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();

}

void SpineDemo::ShowToolStatus_Vega()
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

// void SpineDemo::RoboticsCalibrationByFoot()
// {
// 	// Step 0: set the puncture guide TCP to the robot
// 	Eigen::Vector3d punctureAxisProximal_inFlange{-34.59,0.0,171.04};
// 	Eigen::Vector3d punctureAxisDistal_inFlange{ 69.36,0.0, 208.88};
//
// 	Eigen::Vector3d punctureVector_inFlange = punctureAxisDistal_inFlange - punctureAxisProximal_inFlange;
// 	punctureVector_inFlange.normalize();
//
// 	// Todo: fulfill the TCP matrix T_flangeToEndEffector
// 	auto T_flangeToEndEffector = vtkMatrix4x4::New();
// 	Eigen::Vector3d x_tcp = punctureVector_inFlange;
// 	Eigen::Vector3d z_std{0, 0, 1};
//
// 	Eigen::Vector3d y_tcp = z_std.cross(x_tcp);
// 	y_tcp.normalize();
//
// 	Eigen::Vector3d z_tcp = x_tcp.cross(y_tcp);
// 	z_tcp.normalize();
//
//
// 	double const r_flangeToEndEffector[3][3]{ {x_tcp[0], y_tcp[0], z_tcp[0]},
// 		{x_tcp[1],  y_tcp[1], z_tcp[1]},
// 		{x_tcp[2],  y_tcp[2], z_tcp[2] }
// 	};
//
// 	// robot TCP a, b, c
// 	std::vector<double> angleEndEffector = this->kukamatrix2angle(r_flangeToEndEffector);
// 	
// 	QString x_tcp_string = QString::number(punctureAxisDistal_inFlange[0], 'f', 6);
// 	QString y_tcp_string = QString::number(punctureAxisDistal_inFlange[1], 'f', 6);
// 	QString z_tcp_string = QString::number(punctureAxisDistal_inFlange[2], 'f', 6);
// 	QString a_tcp_string = QString::number(angleEndEffector[0], 'f', 6);
// 	QString b_tcp_string = QString::number(angleEndEffector[1], 'f', 6);
// 	QString c_tcp_string = QString::number(angleEndEffector[2], 'f', 6);
//
// 	QString param_tcp = x_tcp_string + "," + y_tcp_string + "," + z_tcp_string + "," + a_tcp_string + "," + b_tcp_string + "," + c_tcp_string;
//
// 	// Todo: sleep for some time
// 	devicePluginManager->requestExecOperate("Robot", "applyTCPValue", param_tcp.split(','));
//
// 	// Step 1: get the implant trajectory in the image frame from a point set
// 	// Todo: this point set should be replaced by the implant surface
// 	auto trajectoryPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("trajectory");
//
// 	// Step 2: calculate the trajectory and target point in the robot base frame
// 	// Todo: get the global image registration matrix: T_imageToPatientRF
// 	auto T_imageToPatientRF = vtkMatrix4x4::New();
//
// 	auto T_patientRFtoImage = vtkMatrix4x4::New();
// 	T_patientRFtoImage->DeepCopy(T_imageToPatientRF);
// 	T_patientRFtoImage->Invert();
//
// 	// Todo: get the global T_cameraToRobotBaseRF
// 	auto T_cameraToRobotBaseRF = vtkMatrix4x4::New();
//
// 	auto T_robotBaseRFtoCamera = vtkMatrix4x4::New();
// 	T_robotBaseRFtoCamera->DeepCopy(T_cameraToRobotBaseRF);
// 	T_robotBaseRFtoCamera->Invert();
//
// 	// Todo: get the global T_cameraToPatientRF
// 	auto T_cameraToPatientRF = vtkMatrix4x4::New();
//
//
// 	// Todo: get the global Robot registration matrix: T_robotBaseRFtoRobotBase
// 	auto T_robotBaseRFtoRobotBase = vtkMatrix4x4::New();
//
// 	auto T_robotBaseToRobotBaseRF = vtkMatrix4x4::New();
// 	T_robotBaseToRobotBaseRF->DeepCopy(T_robotBaseRFtoRobotBase);
// 	T_robotBaseToRobotBaseRF->Invert();
//
// 	auto trans_robotBaseToImage = vtkTransform::New();
// 	trans_robotBaseToImage->Identity();
// 	trans_robotBaseToImage->PostMultiply();
// 	trans_robotBaseToImage->SetMatrix(T_patientRFtoImage);
// 	trans_robotBaseToImage->Concatenate(T_cameraToPatientRF);
// 	trans_robotBaseToImage->Concatenate(T_robotBaseRFtoCamera);
// 	trans_robotBaseToImage->Concatenate(T_robotBaseToRobotBaseRF);
// 	trans_robotBaseToImage->Update();
//
// 	auto T_robotBaseToImage = trans_robotBaseToImage->GetMatrix();
//
// 	trajectoryPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(T_robotBaseToImage);
//
// 	auto entryPoint_inRobotBase = trajectoryPset->GetPoint(0);
//
// 	auto exitPoint_inRobotBase = trajectoryPset->GetPoint(1);
//
// 	Eigen::Vector3d punctureVector_inRobotBase{
// 		exitPoint_inRobotBase[0] - entryPoint_inRobotBase[0],
// 	exitPoint_inRobotBase[1] - entryPoint_inRobotBase[1],
// 	exitPoint_inRobotBase[2] - entryPoint_inRobotBase[2]};
//
// 	punctureVector_inRobotBase.normalize();
//
// 	double targetPoint_inRobotBase[3]{ entryPoint_inRobotBase[0] - 100 * punctureVector_inRobotBase[0],
// 		entryPoint_inRobotBase[1] - 100 * punctureVector_inRobotBase[1],
// 		entryPoint_inRobotBase[2] - 100 * punctureVector_inRobotBase[2],
// 	};
//
//
// 	// Step 3: Calculate the endEffector pose in the robot base frame: T_robotBaseToTargetPose
//
// 	// Todo: Get the global current endEffector pose in robotBase frame T_currentRobotBaseToEndEffector
// 	auto T_currentRobotBaseToEndEffector = vtkMatrix4x4::New();
//
// 	Eigen::Vector3d current_x{ T_currentRobotBaseToEndEffector->GetElement(0,0),
// 	T_currentRobotBaseToEndEffector->GetElement(1,0),
// 	T_currentRobotBaseToEndEffector->GetElement(2,0)
// 	};
//
// 	Eigen::Vector3d rotAxis = current_x.cross(punctureVector_inRobotBase);
// 	rotAxis.normalize();
//
// 	double rotAngle = 180 * acos(current_x.dot(punctureVector_inRobotBase))/3.1415926;
//
// 	auto tmpTrans = vtkTransform::New();
// 	tmpTrans->PostMultiply();
// 	tmpTrans->Identity();
// 	tmpTrans->SetMatrix(T_currentRobotBaseToEndEffector);
// 	tmpTrans->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
// 	tmpTrans->Update();
//
// 	auto tmpMatrix = tmpTrans->GetMatrix();
//
// 	auto T_robotBaseToTargetPose = vtkMatrix4x4::New();
// 	T_robotBaseToTargetPose->DeepCopy(tmpMatrix);
// 	T_robotBaseToTargetPose->SetElement(0,3, targetPoint_inRobotBase[0]);
// 	T_robotBaseToTargetPose->SetElement(1, 3, targetPoint_inRobotBase[1]);
// 	T_robotBaseToTargetPose->SetElement(2, 3, targetPoint_inRobotBase[2]);
//
//
// 	// Step 4: Move the robot 
//
// 	double const r_robotBaseToTargetPose[3][3]{ {T_robotBaseToTargetPose->GetElement(0,0), T_robotBaseToTargetPose->GetElement(0,1), T_robotBaseToTargetPose->GetElement(0,2)},
// 		{T_robotBaseToTargetPose->GetElement(1,0), T_robotBaseToTargetPose->GetElement(1,1), T_robotBaseToTargetPose->GetElement(1,2)},
// 		{T_robotBaseToTargetPose->GetElement(2,0), T_robotBaseToTargetPose->GetElement(2,1), T_robotBaseToTargetPose->GetElement(2,2)}
// 	};
//
// 	// robot TCP a, b, c
// 	std::vector<double> angleRobotBaseToTargetPose = this->kukamatrix2angle(r_robotBaseToTargetPose);
//
// 	QString str1 = QString::number(T_robotBaseToTargetPose->GetElement(0, 3), 'f', 6);
// 	QString str2 = QString::number(T_robotBaseToTargetPose->GetElement(1, 3), 'f', 6);
// 	QString str3 = QString::number(T_robotBaseToTargetPose->GetElement(2, 3), 'f', 6);
// 	QString str4 = QString::number(angleRobotBaseToTargetPose[0], 'f', 6);
// 	QString str5 = QString::number(angleRobotBaseToTargetPose[1], 'f', 6);
// 	QString str6 = QString::number(angleRobotBaseToTargetPose[2], 'f', 6);
//
// 	QString param = str1 + "," + str2 + "," + str3 + "," + str4 + "," + str5 + "," + str6;
//
// 	devicePluginManager->requestExecOperate("Robot", "movep", param.split(','));
// }
