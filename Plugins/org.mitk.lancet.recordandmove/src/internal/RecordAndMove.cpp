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

//udp
#include <QTimer>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include "kukaRobotAPI/robotInfoProtocol.h"

#include "ui_RecordAndMoveControls.h"

#include "kukaRobotAPI/defaultProtocol.h"
const std::string RecordAndMove::VIEW_ID = "org.mitk.views.recordandmove";

void RecordAndMove::SetFocus()
{
  m_Controls.pushButton_recordPosition->setFocus();
}


//继续看了解IGT filter
//VirtualDevice 和Visualizer是什么
//geo是什么
//RenderingManager是什么
//void RecordAndMove::OnVirtualDevice2VisualizeTimer()
//{
//	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
//	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
//	//will be adapted.
//	if (m_VirtualDevice2Visualizer.IsNotNull())
//	{
//		m_VirtualDevice2Visualizer->Update();
//		auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
//		mitk::RenderingManager::GetInstance()->InitializeViews(geo);
//		this->RequestRenderWindowUpdate();
//	}
//}


// “Probe” 是什么？
//void RecordAndMove::threadUpdateFriTransform()
//{
//	while (m_KeepUpdateFriTransform)
//	{
//		//calculate matrix
//		auto robotEndRFDestInNDI = m_VegaSource->GetOutput("Probe")->GetAffineTransform3D();
//		robotEndRFDestInNDI->Compose(T_probe2robotEndRF, true);
//		//MITK_WARN << "robotEndRFDestInNDI" << robotEndRFDestInNDI;
//		mitk::AffineTransform3D::Pointer robotDest = mitk::AffineTransform3D::New();
//		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", robotEndRFDestInNDI, robotDest);//都是什么？？
//		m_KukaTrackingDevice->m_RobotApi.SetFriDynamicFrameTransform(robotDest);//??????FriDynamic是什么？？？
//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
//	}
//}

void RecordAndMove::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.pushButton_connect, &QPushButton::clicked, this, &RecordAndMove::Record);
  connect(m_Controls.pushButton_recordPosition, &QPushButton::clicked, this, &RecordAndMove::Record);
}

void RecordAndMove::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
	const QList<mitk::DataNode::Pointer>& nodes)
{
}

void RecordAndMove::UseKuka()
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
	m_KukaTrackingDevice = lancet::KukaRobotDevice_New::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_KukaToolStorage, m_KukaTrackingDevice);

	m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);
	// m_KukaSource->RegisterAsMicroservice();
	if (m_KukaSource.IsNotNull())
	{
		m_KukaSource->Connect();
	}

	//use navigation scene filter
	m_NavigationSceneFilter->AddTrackingDevice(m_KukaSource, "RobotBaseRF");
	m_NavigationSceneFilter->GetNavigationScene()->Tranversal();
}


void RecordAndMove::Record()
{
	QTimer* timer = new QTimer(this);
	timer->start(100);
	connect(timer, &QTimer::timeout, this, &RecordAndMove::RecordOnce);
}

void RecordAndMove::RecordOnce()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	cout << "frame.position" << p[0] << endl;
	cout << "frame.position" << p[1] << endl;
	cout << "frame.position" << p[2] << endl;
	cout << "frame.position" << p[3] << endl;
	cout << "frame.position" << p[4] << endl;
	cout << "frame.position" << p[5] << endl;
	cout << "***************" << endl;
}


void RecordAndMove::OnAutoMove()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	cout << "frame.position" << p[0] << endl;
	cout << "frame.position" << p[1] << endl;
	cout << "frame.position" << p[2] << endl;
	cout << "frame.position" << p[3] << endl;
	cout << "frame.position" << p[4] << endl;
	cout << "frame.position" << p[5] << endl;	  
	p[0] -= 100;
	p[1] -= 100;
	p[2] -= 100;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}


