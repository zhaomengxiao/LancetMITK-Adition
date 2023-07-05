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
#include "CameraTest.h"

// Qt
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

#include "kukaRobotAPI/defaultProtocol.h"

//#include <numeric>
//#include <vector>
//#include<iostream>
//using namespace std;
const std::string CameraTest::VIEW_ID = "org.mitk.views.cameratest";

void CameraTest::SetFocus()
{
  m_Controls.pushButton_connectVega->setFocus();
}

void CameraTest::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &CameraTest::UseVega);
  connect(m_Controls.pushButton_readPosition, &QPushButton::clicked, this, &CameraTest::ReadPosition);
  connect(m_Controls.pushButton_readEnd10fLine, &QPushButton::clicked, this, &CameraTest::ReadEnd10fLine);
  connect(m_Controls.pushButton_readEnd2OfLine, &QPushButton::clicked, this, &CameraTest::ReadEnd20fLine);
  connect(m_Controls.pushButton_calculateRepeatability, &QPushButton::clicked, this, &CameraTest::CalculateRepeatability);
  connect(m_Controls.pushButton_calculateLength, &QPushButton::clicked, this, &CameraTest::CalculateLength);
  connect(m_Controls.pushButton_calculateDeviationOfLength, &QPushButton::clicked, this, &CameraTest::CalculateDeviationOfLength);
  connect(m_Controls.pushButton_printPoint, &QPushButton::clicked, this, &CameraTest::PrintPoint);
  connect(m_Controls.pushButton_printEnd1, &QPushButton::clicked, this, &CameraTest::PrintEnd1);
  connect(m_Controls.pushButton_printEnd2, &QPushButton::clicked, this, &CameraTest::PrintEnd2);
  connect(m_Controls.pushButton_clearPoint, &QPushButton::clicked, this, &CameraTest::ClearPoint);
  connect(m_Controls.pushButton_clearEnd1, &QPushButton::clicked, this, &CameraTest::ClearEnd1);
  connect(m_Controls.pushButton_ClearEnd2, &QPushButton::clicked, this, &CameraTest::ClearEnd2);
}


void CameraTest::UseVega()
{
	m_Controls.pushButton_connectVega->setText("Vega Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

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
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(CalculatePositionAverage(mitk::AffineTransform3D::Pointer  m_ProbeRealTimePose)));

	//connect the timer to the method OnTimer()
	ShowToolStatus_Vega();
	m_VegaVisualizeTimer->start(10); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	//use navigation scene filter
	cout << "Vega Connected Successfully!" << endl;
	m_Controls.pushButton_connectVega->setText("Vega Connected Successfully!");
}

bool CameraTest::ApplySurfaceRegistration()
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

void CameraTest::OnVegaVisualizeTimer()
{
	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		this->RequestRenderWindowUpdate();
		//update probe pose
		auto  probe = m_VegaSource->GetOutput("Probe")->GetAffineTransform3D(); //F_NDI_Probe
		probe->Compose(m_VegaSource->GetOutput("ObjectRf")->GetInverse()->GetAffineTransform3D());  //F_ObjectRf_NDI
		this->CalculatePositionAverage(probe);
	}
}

void CameraTest::UpdateToolStatusWidget()
{
	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
}

void CameraTest::ShowToolStatus_Vega()
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

void CameraTest::ReadPosition()
{
	p_s = { 0,0,0 };
	p_a = { 0,0,0 };
	m_calculate_position_flag = true;
	m_nRealTimePoseCount = 0;
	m_ProbeRealTimePose = mitk::AffineTransform3D::New();
	QTimer::singleShot(1000, this, [=]() {
		m_calculate_position_flag = false;
		});
	QTimer::singleShot(1010, this, [=]() {
		ShowPosition();
		});
}

void CameraTest::ReadEnd10fLine()
{
	p_s = { 0,0,0 };
	p_a = { 0,0,0 };
	m_calculate_position_flag = true;
	m_nRealTimePoseCount = 0;
	m_ProbeRealTimePose = mitk::AffineTransform3D::New();
	QTimer::singleShot(200, this, [=]() {
		m_calculate_position_flag = false;
		});
	QTimer::singleShot(210, this, [=]() {
		ShowEnd1Position();
		});
}

void CameraTest::ReadEnd20fLine()
{
	p_s = { 0,0,0 };
	p_a = { 0,0,0 };
	m_calculate_position_flag = true;
	m_nRealTimePoseCount = 0;
	m_ProbeRealTimePose = mitk::AffineTransform3D::New();
	QTimer::singleShot(200, this, [=]() {
		m_calculate_position_flag = false;
		});
	QTimer::singleShot(210, this, [=]() {
		ShowEnd2Position();
		});
}

void CameraTest::CalculatePositionAverage(mitk::AffineTransform3D::Pointer probe)
{
	if (this->m_calculate_position_flag && probe.IsNotNull())
	{
		p_s[0] += probe->GetOffset()[0];
		p_s[1] += probe->GetOffset()[1];
		p_s[2] += probe->GetOffset()[2];
		++m_nRealTimePoseCount;
		p_a[0] = p_s[0] / m_nRealTimePoseCount;
		p_a[1] = p_s[1] / m_nRealTimePoseCount;
		p_a[2] = p_s[2] / m_nRealTimePoseCount;
	}
}

void CameraTest::ShowPosition()
{		
		m_ProbeRealTimePose_list.push_back(p_a);
		QString position_text;
		position_text += QString::number(p_a[0]) + "  " + QString::number(p_a[1]) + "  " + QString::number(p_a[2]) + "\n";
		m_Controls.textBrowserLog->append(position_text);
}

void CameraTest::ShowEnd1Position()
{
	m_End1Position_list.push_back(p_a);
	QString position_text;
	position_text += QString::number(p_a[0]) + "  " + QString::number(p_a[1]) + "  " + QString::number(p_a[2]) + "\n";
	m_Controls.textBrowser_End1OfLine->append(position_text);
}

void CameraTest::ShowEnd2Position()
{
	m_End2Position_list.push_back(p_a);
	QString position_text;
	position_text += QString::number(p_a[0]) + "  " + QString::number(p_a[1]) + "  " + QString::number(p_a[2]) + "\n";
	m_Controls.textBrowser_End2OfLine->append(position_text);
}

void CameraTest::CalculateRepeatability()
{
	double x_sum = 0;
	double y_sum = 0;
	double z_sum = 0;
	for (int i = 0; i < size(m_ProbeRealTimePose_list); i++)
	{
		x_sum += m_ProbeRealTimePose_list[i][0];
		y_sum += m_ProbeRealTimePose_list[i][1];
		z_sum += m_ProbeRealTimePose_list[i][2];
	}
	double x_average = x_sum / size(m_ProbeRealTimePose_list);
	double y_average = y_sum / size(m_ProbeRealTimePose_list);
	double z_average = z_sum / size(m_ProbeRealTimePose_list);
	std::vector<double> deviation_list;
	for (int i = 0; i < size(m_ProbeRealTimePose_list); i++)
	{
		deviation_list.push_back(sqrt(pow((x_average - m_ProbeRealTimePose_list[i][0]), 2) +
			pow((y_average - m_ProbeRealTimePose_list[i][1]), 2) +
			pow((z_average - m_ProbeRealTimePose_list[i][2]), 2)));
	}
	double deviation_average;
	deviation_average = accumulate(deviation_list.begin(), deviation_list.end(), 0) / size(deviation_list);
	double standard_deviation;
	double sum_of_deviation = 0;
	for (int i = 0; i < size(deviation_list); i++)
	{
		sum_of_deviation += pow((deviation_list[i] - deviation_average), 2);
	}
	standard_deviation = sqrt(sum_of_deviation / size(deviation_list));
	double Repeatability;
	Repeatability = deviation_average + 3 * standard_deviation;
	//QString repeatability_text =  QString::number(Repeatability);
	m_Controls.label_repeatability->setText(QString::number(Repeatability));
}

void CameraTest::CalculateLength()
{
	length_list.clear();
	if (size(m_End1Position_list) != size(m_End2Position_list))
	{
		cout << "Wrong Point Pair!" << endl;
		return;
	}
	
	for (int i = 0; i < size(m_End1Position_list); i++)
	{
		double length = sqrt(pow((m_End1Position_list[i][0] - m_End2Position_list[i][0]), 2) +
			pow((m_End1Position_list[i][1] - m_End2Position_list[i][1]), 2) +
			pow((m_End1Position_list[i][2] - m_End2Position_list[i][2]), 2));
		length_list.push_back(length);
	}
	QString position_text;
	for (int i = 0; i < size(m_End1Position_list); i++)
	{
		position_text += QString::number(length_list[i]) + "\n"+ "\n";
	}
	m_Controls.textBrowser_length->setText(position_text);
}

void CameraTest::CalculateDeviationOfLength()
{
	double real_length;
	length_deviation_list.clear();
	double length_deviation;
	double max_length_deviation = 0;
	for (int i = 0; i < size(length_list); i++)
	{
		switch (size(length_deviation_list))
		{
			cout << "************ 1 ***********" << endl;
		case 0:
			cout << "************ 1 ***********" << endl;

			real_length = 99.991001;
			cout << "0--1: 99.001001" << endl;
			break;
		case 1:
			cout << "************ 1 ***********" << endl;

			real_length = 99.998001;
			cout << "2--3: 99.998001" << endl;
			break;
		case 2:
			cout << "************ 1 ***********" << endl;

			real_length = 100.000;
			cout << "4--5: 100.000" << endl;
			break;
		case 3:
			cout << "************ 1 ***********" << endl;

			real_length = 59.996;
			cout << "0--2: 59.996" << endl;
			break;
		case 4:
			cout << "************ 1 ***********" << endl;

			real_length = 59.999001;
			cout << "1--3: 59.999001" << endl;
			break;
		case 5:
			cout << "************ 1 ***********" << endl;

			real_length = 97.076312;
			cout << "0--4: 97.076312" << endl;
			break;
		default:
			cout << "************ 1 ***********" << endl;

			break;
		}
		
		length_deviation = abs(length_list[i] - real_length);
		cout << "length_deviation: " << length_deviation << endl;
		length_deviation_list.push_back(length_deviation);
		if (length_deviation > max_length_deviation)
		{
			max_length_deviation = length_deviation;
			cout << "max_length_deviation: " << max_length_deviation << endl;
		}
		
	}
	m_Controls.label_DeviationOfLength->setText(QString::number(max_length_deviation));
}

void CameraTest::PrintPoint()
{
	cout << "**************** Point Position Start*********************" << endl;
	for (int i=0; i < size(m_ProbeRealTimePose_list); i++)
	{
		cout << m_ProbeRealTimePose_list[i][0] << "\t" << m_ProbeRealTimePose_list[i][1] << "\t" << m_ProbeRealTimePose_list[i][2] << endl;
	}
	cout << "**************** Point Position End*********************" << endl;
}

void CameraTest::PrintEnd1()
{
	cout << "**************** End1 Position Start*********************" << endl;
	for (int i=0; i < size(m_End1Position_list); i++)
	{
		cout << m_End1Position_list[i][0] << "\t" << m_End1Position_list[i][1] << "\t" << m_End1Position_list[i][2] << endl;
	}
	cout << "**************** End1 Position End*********************" << endl;
}

void CameraTest::PrintEnd2()
{
	cout << "**************** End2 Position Start*********************" << endl;
	for (int i=0; i < size(m_End2Position_list); i++)
	{
		cout << m_End2Position_list[i][0] << "\t" << m_End2Position_list[i][1] << "\t" << m_End2Position_list[i][2] << endl;
	}
	cout << "**************** End2 Position End*********************" << endl;
}

void CameraTest::ClearPoint()
{
	m_ProbeRealTimePose_list.clear();
	m_Controls.textBrowserLog->setText("");
}

void CameraTest::ClearEnd1()
{
	m_End1Position_list.clear();
	m_Controls.textBrowser_End1OfLine->setText("");
}

void CameraTest::ClearEnd2()
{
	m_End2Position_list.clear();
	m_Controls.textBrowser_End2OfLine->setText("");
}