#include "ConnectionTab.h"

ConnectionTab::ConnectionTab(Ui::HansRobotControls ui, mitk::DataStorage* aDataStorage, AbstractRobot* aRobot, AbstractCamera* aCamera, QWidget* parent)
{
	m_ui = ui;
	m_Robot = dynamic_cast<LancetHansRobot*>( aRobot);
	m_Camera = aCamera;
	m_dataStorage = aDataStorage;
	if (!m_dataStorage)
	{
		std::cout << "m_dataStorage is nullptr" << std::endl;
	}//here


	InitConnection();
}

void ConnectionTab::InitConnection()
{
	connect(m_ui.pushButton_connectArm_3, &QPushButton::clicked, this, &ConnectionTab::connectRobot);
	connect(m_ui.pushButton_powerOn_3, &QPushButton::clicked, this, &ConnectionTab::connectRobot);
	connect(m_Camera, &AimCamera::CameraUpdateClock, this, &ConnectionTab::upDateUi);
}

void ConnectionTab::connectRobot()
{
	m_Robot->Connect();
}
void ConnectionTab::powerOn()
{
	m_Robot->PowerOn();
}
void ConnectionTab::flashLable(Eigen::Vector3d tempTip,QLabel* lable)
{
	if (lable != nullptr)
	{
		QString str = "x:" + QString::number(tempTip[0]) + " "
			+ "y:" + QString::number(tempTip[1]) + " "
			+ "z:" + QString::number(tempTip[2]);
		lable->setText(str);
		lable->setStyleSheet("QLabel { color : green; }");
		/*std::cout << str << std::endl;*/
	}
}
void ConnectionTab::upDateUi()
{
	//AimCamera::GetToolMatrixByName

	flashLable(m_Camera->GetToolTipByName("HTO_RobotBaseRF"),m_ui.Spine_RobotBaseRFDataLabel_3);
	flashLable(m_Camera->GetToolTipByName("HTO_RobotEndRF"), m_ui.Spine_RobotEndRFDataLabel_3);
	flashLable(m_Camera->GetToolTipByName("HTO_PatientRF"), m_ui.Spine_PatientRFDataLabel_3);
	flashLable(m_Camera->GetToolTipByName("HTO_Probe"), m_ui.Spine_ProbeDataLabel_3);
}




void ConnectionTab::powerOff()
{
	m_Robot->PowerOff();
}

void ConnectionTab::connectCamera()
{
	m_Camera->Connect();
}
void ConnectionTab::updateData()
{
	m_Camera->Start();
}
