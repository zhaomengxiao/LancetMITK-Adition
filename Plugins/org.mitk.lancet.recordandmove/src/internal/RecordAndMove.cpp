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
#include "RecordAndMove.h"

// Qt
#include <QMessageBox>
#include <QObject>

// mitk image
#include <mitkAffineTransform3D.h>
#include <mitkImage.h>
#include <mitkMatrixConvert.h>

//igt
#include <kukaRobotDevice.h>
#include <lancetApplyDeviceRegistratioinFilter.h>
#include <lancetPathPoint.h>
#include <lancetVegaTrackingDevice.h>
#include <mitkNavigationDataToPointSetFilter.h>

#include <QtWidgets\qfiledialog.h>
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "mitkNavigationToolStorageDeserializer.h"

#include "lancetTreeCoords.h"
#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
#include "QmitkIGTCommonHelper.h"

//image registration needed
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "QmitkDataStorageTreeModel.h"
#include <QmitkSingleNodeSelectionWidget.h>
#include <QTimer>
#include <vtkImplicitPolyDataDistance.h>

#include "lancetTreeCoords.h"

//udp
#include <QTimer>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include "kukaRobotAPI/robotInfoProtocol.h"

#include "ui_RecordAndMoveControls.h"

#include "kukaRobotAPI/defaultProtocol.h"
const std::string RecordAndMove::VIEW_ID = "org.mitk.views.recordandmove";

//=====================================  Helper Function =========================================
mitk::NavigationData::Pointer RecordAndMove::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}
//=====================================  Helper Function =========================================


void RecordAndMove::SetFocus()
{
  m_Controls.pushButton_recordPosition->setFocus();
}

void RecordAndMove::CreateQtPartControl(QWidget *parent)
{
	m_Controls.setupUi(parent);
  // create GUI widgets from the Qt Designer's .ui file
	InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis);
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src);


	InitCTSteelballCenterSelector(m_Controls.mitkNodeSelectWidget_CTSteelballCenterInImage);
	connect(m_Controls.pushButton_assembleNavigationObject, &QPushButton::clicked, this, &RecordAndMove::SetupNavigatedImage);
	//InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_metaImageNode);
	//InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetPoint);
	/*InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine);
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_ImageCheckPoint); m_Controls.setupUi(parent);*/

  connect(m_Controls.pushButton_getCTSteelballCenter, &QPushButton::clicked, this, &RecordAndMove::GetCTSteelballCenterInImage);
  connect(m_Controls.pushButton_collectLandmark, &QPushButton::clicked, this, &RecordAndMove::CollectLandmarkProbe);
  connect(m_Controls.pushButton_collectIcp, &QPushButton::clicked, this, &RecordAndMove::CollectIcpProbe);
  connect(m_Controls.pushButton_applyRegistration, &QPushButton::clicked, this, &RecordAndMove::ApplySurfaceRegistration);

  connect(m_Controls.pushButton_connectKuka, &QPushButton::clicked, this, &RecordAndMove::UseKuka);
  connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &RecordAndMove::UseVega);
  connect(m_Controls.pushButton_recordPosition, &QPushButton::clicked, this, &RecordAndMove::ThreadRecord);
  connect(m_Controls.pushButton_handDrive, &QPushButton::clicked, this, &RecordAndMove::ThreadHandDrive);
  connect(m_Controls.pushButton_setAsTarget, &QPushButton::clicked, this, &RecordAndMove::SetAsTarget);
  connect(m_Controls.pushButton_moveToTarget, &QPushButton::clicked, this, &RecordAndMove::MoveToTarget);
  connect(m_Controls.pushButton_stopHandDrive, &QPushButton::clicked, this, &RecordAndMove::StopHandDrive);
  connect(m_Controls.pushButton_capturePose, &QPushButton::clicked, this, &RecordAndMove::OnRobotCapture);
  connect(m_Controls.pushButton_moveToHomePosition, &QPushButton::clicked, this, &RecordAndMove::MoveToHomePosition);
}



bool RecordAndMove::GetCTSteelballCenterInImage()
{
	auto CTSteelballCenterPositionInImage = m_Controls.mitkNodeSelectWidget_CTSteelballCenterInImage->GetSelectedNode();
	if (CTSteelballCenterPositionInImage == nullptr)
	{
		cout << "Source surface or source landmarks is not ready!" << endl;
		return false;
	}
	cout<<"******************************"<<endl;
	cout << CTSteelballCenterPositionInImage->GetData()->GetGeometry()->GetCenter() << endl;
	cout << "_______________________________" << endl;
	cout << *dynamic_cast<mitk::PointSet*>(CTSteelballCenterPositionInImage->GetData()) << endl;
	cout << "******************************" << endl;

	return true;
}


void RecordAndMove::InitCTSteelballCenterSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

void RecordAndMove::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
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

void RecordAndMove::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

bool RecordAndMove::SetupNavigatedImage()
{
	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = m_Controls.mitkNodeSelectWidget_surface_regis->GetSelectedNode();
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src->GetSelectedNode();

	if (surfaceNode == nullptr || landmarkSrcNode == nullptr)
	{
		m_Controls.textBrowser->append("Source surface or source landmarks is not ready!");
		return false;
	}

	navigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if (matrix->IsIdentity() == false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
		cout << "Warning: the initial surface has a non - identity offset matrix; the matrix has been reset to identity!" << endl;
	}

	navigatedImage->SetDataNode(surfaceNode);

	navigatedImage->SetLandmarks(dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData()));;
	navigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- navigatedImage has been set up ---");

	return true;
}

bool RecordAndMove::CollectLandmarkProbe()
{
	if (navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	int landmark_surfaceNum = navigatedImage->GetLandmarks()->GetSize();
	int landmark_rmNum = navigatedImage->GetLandmarks_probe()->GetSize();

	if (landmark_surfaceNum == landmark_rmNum)
	{
		m_Controls.textBrowser->append("--- Enough landmarks have been collected ----");
		return true;
	}

	auto pointSet_probeLandmark = navigatedImage->GetLandmarks_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("ProbeTHA");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeLandmark->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added landmark: " + QString::number(probeTipPointUnderRf[0]) +
		"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}

bool RecordAndMove::CollectIcpProbe()
{
	if (navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}
	auto pointSet_probeIcp = navigatedImage->GetIcpPoints_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("ProbeTHA");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeIcp->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added icp point: " + QString::number(probeTipPointUnderRf[0]) +
		"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}

bool RecordAndMove::ApplySurfaceRegistration()
{
	//build ApplyDeviceRegistrationFilter
	m_surfaceRegistrationFilter = lancet::ApplySurfaceRegistratioinFilter::New();
	m_surfaceRegistrationFilter->ConnectTo(m_VegaSource);
	m_surfaceRegistrationFilter->SetnavigationImage(navigatedImage);
	m_imageRegistrationMatrix = mitk::AffineTransform3D::New();
	navigatedImage->UpdateObjectToRfMatrix();
	mitk::TransferVtkMatrixToItkTransform(navigatedImage->GetT_Object2ReferenceFrame(), m_imageRegistrationMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(m_imageRegistrationMatrix);

	m_surfaceRegistrationFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());

	auto indexOfObjectRF = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput(indexOfObjectRF));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationFilter);
	m_VegaVisualizeTimer->start();

	return true;
}






















void RecordAndMove::UseKuka()
{
	m_Controls.pushButton_connectKuka->setText("Kuka Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_KukaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_KukaToolStorage->SetName(filename.toStdString());

	m_KukaTrackingDevice = lancet::KukaRobotDevice_New::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_KukaToolStorage, m_KukaTrackingDevice);

	m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);
	// m_KukaSource->RegisterAsMicroservice();
	if (m_KukaSource.IsNotNull())
	{
		m_KukaSource->Connect();
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
	//Begin:  void SurgicalSimulate::StartTracking()
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
		//ShowToolStatus_Kuka();
		m_KukaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps
	}
	else
	{
		MITK_ERROR << "Tracking not start,Device State:" << m_KukaTrackingDevice->GetState();
	}
	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	MITK_INFO << "m_KukaSource connected!";
	m_Controls.pushButton_connectKuka->setText("Kuka Connected Successfully!");
}

void RecordAndMove::UseVega()
{
	m_Controls.pushButton_connectVega->setText("Vega Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk  ????????????????????????????????????????????????????????????
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());

	MITK_INFO << "Vega tracking";
	//QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
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
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnVegaVisualizeTimer()));
	//connect the timer to the method OnTimer()
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
	//connect the timer to the method OnTimer()
	ShowToolStatus_Vega();
	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	//use navigation scene filter
	cout << "Vega Connected Successfully!" << endl;
	m_Controls.pushButton_connectVega->setText("Vega Connected Successfully!");
}

void RecordAndMove::OnKukaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.
	if (m_KukaVisualizer.IsNotNull())
	{
		m_KukaVisualizer->Update(); //todo Crash When close plugin
		this->RequestRenderWindowUpdate();
	}


	if (m_KukaTrackingDevice->m_RobotApi.GetNumberOfCommandResult() > 0)
	{
		ResultProtocol reply = m_KukaTrackingDevice->m_RobotApi.GetCommandResult();
		cout << "This is reply: " << reply.ToString() << endl;
		//m_Controls.textBrowser->append(QString::fromStdString(reply.ToString()));
	}
}

void RecordAndMove::OnVegaVisualizeTimer()
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

		//update probe pose
		auto  probe = m_VegaSource->GetOutput("ProbeTHA")->GetAffineTransform3D();
		// m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
		// m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", probe, m_ProbeRealTimePose);
		  //mitk::AffineTransform3D::Pointer byhand = mitk::AffineTransform3D::New();
		auto registrationMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
		probe->Compose(m_VegaSource->GetOutput("RobotBaseRF")->GetInverse()->GetAffineTransform3D());
		probe->Compose(mitk::NavigationData::New(registrationMatrix)->GetInverse()->GetAffineTransform3D());
		m_ProbeRealTimePose = probe;
	}
}

void RecordAndMove::UpdateToolStatusWidget()
{
	/*m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
	m_Controls.m_StatusWidgetKukaToolToShow->Refresh();*/
}

void RecordAndMove::ShowToolStatus_Vega()
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
	}
	//initialize widget
	/*m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
	m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();*/
}

void RecordAndMove::ThreadHandDrive()
{
	if (m_ThreadHandDrive_Flag == false)
	{
		m_ThreadHandDrive_Flag = true;
		m_ThreadHandDrive_Handler = std::thread(&RecordAndMove::HandDrive, this);
		//QString handDriveFlag_text = QString::number(m_ThreadHandDrive_Flag);
		m_Controls.label_handDriveFlag->setText("Hand Drive ON");
	}
}

void RecordAndMove::ThreadRecord()
{
	m_ThreadRecord_Flag = true;
	m_ThreadRecord_Handler = std::thread(&RecordAndMove::Record, this);
}

void RecordAndMove::Record()
{
	while (m_ThreadRecord_Flag)
	{
		auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
		//MITK_INFO << frame.name;
		std::array<double, 6> p = frame.position;
		QString position_text = "x: "+ QString::number(p[0]) + "    y: " + QString::number(p[1]) + "    z: " + QString::number(p[2]) + "    a: " + QString::number(p[3]) + "    b: " + QString::number(p[4]) + "    c: " + QString::number(p[5]);
		m_Controls.label_position->setText(position_text);
	}
}

void RecordAndMove::HandDrive()
{
	while (m_ThreadHandDrive_Flag)
	{
		ResultProtocol reply;
		if (m_KukaTrackingDevice->m_RobotApi.GetNumberOfCommandResult() > 0)
		{
			reply = m_KukaTrackingDevice->m_RobotApi.GetCommandResult();
			MITK_INFO << "Receive Reply: " << QString::fromStdString(reply.operateType);
		}

		if (QString::fromStdString(reply.operateType) == "non_precious")
		{
			m_handGuidingOn = false;
		}
		//start handguiding
		if (QString::fromStdString(reply.operateType) == "precious")
		{
			if (preciousHandGuiding_select == true) {
				m_KukaTrackingDevice->m_RobotApi.SendCommandNoPara("HandGuiding");
			}
			else {
				m_KukaTrackingDevice->m_RobotApi.SendCommandNoPara("Test");
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void RecordAndMove::StopHandDrive()
{
	if (m_ThreadHandDrive_Flag == true)
	{
		m_ThreadHandDrive_Flag = false;
		m_ThreadHandDrive_Handler.join();
		QString handDriveFlag_text = QString::number(m_ThreadHandDrive_Flag);
		m_Controls.label_handDriveFlag->setText("Hand Drive OFF");
	}
}

void RecordAndMove::SetAsTarget()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	//MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	m_Target = p;
	QString position_text = "Target:    x: " + QString::number(p[0]) + "    y: " + QString::number(p[1]) + "    z: " + QString::number(p[2]) + "    a: " + QString::number(p[3]) + "    b: " + QString::number(p[4]) + "    c: " + QString::number(p[5]);
	m_Controls.label_target->setText(position_text);
}

void RecordAndMove::MoveToTarget()
{
	StopHandDrive();
	bool isGetTarget = m_KukaTrackingDevice->m_RobotApi.MovePTP(m_Target);
}

void RecordAndMove::OnAutoMove()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	cout << "m_IndexOfRobotCapture: " << m_IndexOfRobotCapture << endl;
	switch (m_IndexOfRobotCapture)
	{
	case 1: //x-50mm	  
		p[0] -= 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;
	case 2: //z-50mm
		p[2] -= 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 3: //x+50mm
		p[0] += 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 4: //y+50
		p[1] += 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 5: //x rotate 10 degree x -25
		p[0] -= 25;
		p[3] += (10.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 6: //x rotate -20 degree y -25
		p[1] -= 25;
		p[3] -= (20.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 7: //y rotate 10 degree y +25
		p[1] -= 25;
		p[4] += (10.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 8: //y rotate -20 degree x +25
		p[0] += 25;
		p[4] -= (20.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 9: //z rotate 20 degree x -25
		p[0] -= 25;
		p[5] += (20.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	default:
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;
	}
}

void::RecordAndMove::MoveToHomePosition()
{
	StopHandDrive();
	bool isGetHome = m_KukaTrackingDevice->m_RobotApi.MovePTP(m_HomePosition);
}

void RecordAndMove::CapturePose(bool translationOnly)
{
	OnAutoMove();
	//Output sequence is the same as AddTool sequence
	//get navigation data of flange in robot coords,
	mitk::NavigationData::Pointer nd_robot2flange = m_KukaSource->GetOutput(0);
	//get navigation data of RobotEndRF in ndi coords,
	auto RobotEndRF = m_VegaToolStorage->GetToolIndexByName("RobotEndRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotEndRF = m_VegaSource->GetOutput(RobotEndRF);
	//get navigation data of RobotBaseRF in ndi coords,
	auto RobotBaseRF = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotBaseRF = m_VegaSource->GetOutput(RobotBaseRF);
	//get navigation data RobotEndRF in reference frame RobotBaseRF
	mitk::NavigationData::Pointer nd_RobotBaseRF2RobotEndRF = GetNavigationDataInRef(
		nd_Ndi2RobotEndRF, nd_Ndi2RobotBaseRF);

	//add nd to registration module
	m_RobotRegistration.AddPose(nd_robot2flange, nd_RobotBaseRF2RobotEndRF, translationOnly);

	//MITK_INFO << nd_robot2flange;
	//MITK_INFO << nd_RobotBaseRF2RobotEndRF;
	cout << "nd_robot2flange: " << nd_robot2flange << endl;
	cout << "nd_RobotBaseRF2RobotEndRF: " << nd_RobotBaseRF2RobotEndRF << endl;
}

void RecordAndMove::OnRobotCapture()
{
	if (m_IndexOfRobotCapture < 5) //The first five translations, 
	{
		CapturePose(true);
		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture<<"  (Translation: "<< m_IndexOfRobotCapture<<"/5)";
		m_Controls.pushButton_capturePose->setText("Capturing "+ QString::number(m_IndexOfRobotCapture)+"/10");
	}
	else if (m_IndexOfRobotCapture < 10) //the last five rotations
	{
		CapturePose(false);
		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture << "  (Rotation: " << m_IndexOfRobotCapture-5 << "/5)";
		m_Controls.pushButton_capturePose->setText("Capturing " + QString::number(m_IndexOfRobotCapture-5) + "/10");
	}
	if (m_IndexOfRobotCapture==10)
	{
		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
		m_Controls.pushButton_capturePose->setText("OnRobotCapture finish, Calculating registration matrix....");
		vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
		m_RobotRegistration.GetRegistraionMatrix(matrix4x4);  //RobotBase to RobotBaseRF
		m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();   //RobotBase to RobotBaseRF
		mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());   //RobotBase to RobotBaseRF
		//save robot registration matrix into reference tool
		m_VegaToolStorage->GetToolByName("RobotBaseRF")->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);
		MITK_INFO << "******************Robot Registration Matrix : ****************************";
		MITK_INFO << m_RobotRegistrationMatrix;

		//build ApplyDeviceRegistrationFilter
		//m_KukaSource:基于RoboBase
		//m_KukaApplyRegistrationFilter转到NDI坐标系下
		m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
		m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
		m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
		m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("RobotBaseRF"));//must make sure NavigationDataOfRF update somewhere else.
		//m_KukaVisualizeTimer->stop();
		m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
		//m_KukaVisualizeTimer->start();
		//tcp

		std::array<double, 6> tcp{};
		m_RobotRegistration.GetTCP(tcp);
		cout << "TCP: " << tcp[0] << " " << tcp[1] << " " << tcp[2] << " " << tcp[3] << " " << tcp[4] << " " << tcp[5] << endl;
		
		//set tcp
		m_KukaTrackingDevice->m_RobotApi.AddFrame("RobotEndRF_robot", tcp);

		//save tcp into robot tool
		m_KukaToolStorage->GetToolByName("RobotEndRF_robot")->SetTCP(tcp.data());
		cout << "tcp.date: " << tcp.data() << endl;
		m_Controls.pushButton_capturePose->setText("OnRobotCapture finish! Calculate finish!");
	}
}


