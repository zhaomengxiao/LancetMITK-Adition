#include "RobotArmRegistrationTab.h"

RobotArmRegistrationTab::RobotArmRegistrationTab(Ui::HansRobotControls ui, mitk::DataStorage* aDataStorage, AbstractRobot* aRobot, AbstractCamera* aCamera, QWidget* parent)
{
	m_ui = ui;
	m_Robot = dynamic_cast<LancetHansRobot*>(aRobot);
	m_Camera = aCamera;
	m_dataStorage = aDataStorage;
	if (!m_dataStorage)
	{
		std::cout << "m_dataStorage is nullptr" << std::endl;
	}

	connect(m_ui.pushButton_xp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::xp);
	connect(m_ui.pushButton_yp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::yp);
	connect(m_ui.pushButton_zp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::zp);
	connect(m_ui.pushButton_xm_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::xm);
	connect(m_ui.pushButton_ym_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::ym);
	connect(m_ui.pushButton_zm_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::zm);
	connect(m_ui.pushButton_rxp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::rxp);
	connect(m_ui.pushButton_ryp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::ryp);
	connect(m_ui.pushButton_rzp_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::rzp);
	connect(m_ui.pushButton_rxm_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::rxm);
	connect(m_ui.pushButton_rym_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::rym);
	connect(m_ui.pushButton_rzm_2, &QPushButton::clicked, this, &RobotArmRegistrationTab::rzm);
}

void RobotArmRegistrationTab::xp()
{
	int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	double Distance = static_cast<double>(intValue);
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}
void RobotArmRegistrationTab::yp()
{
	int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	double Distance = static_cast<double>(intValue);
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Translate(templeArray, Distance);
}
void RobotArmRegistrationTab::zp()
{
	int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	double Distance = static_cast<double>(intValue);
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Translate(templeArray, Distance);
}
void RobotArmRegistrationTab::xm()
{
	 int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	 double Distance = -static_cast<double>(intValue);
	 double templeArray[3] = { 1, 0.0, 0.0 };
	 m_Robot->Translate(templeArray, Distance);
}

void RobotArmRegistrationTab::ym()
{

	 int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	 double Distance = -static_cast<double>(intValue);
	 double templeArray[3] = { 0.0, 1, 0.0 };
	 m_Robot->Translate(templeArray, Distance);
}
void RobotArmRegistrationTab::zm()
{
	 int intValue = m_ui.lineEdit_intuitiveValue_2->text().toInt();
	 double Distance = -static_cast<double>(intValue);
	 double templeArray[3] = { 0.0, 0.0, 1 };
	 m_Robot->Translate(templeArray, Distance);

}
void RobotArmRegistrationTab::rxp()
{
	int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	double Angle = static_cast<double>(intValue);
	double templeArray[3] = { 1, 0.0, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}
void RobotArmRegistrationTab::ryp()
{
	int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	double Angle = static_cast<double>(intValue);
	double templeArray[3] = { 0.0, 1, 0.0 };
	m_Robot->Rotate(templeArray, Angle);
}
void RobotArmRegistrationTab::rzp()
{
	 int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	 double Angle = static_cast<double>(intValue);
	 double templeArray[3] = { 0.0, 0.0, 1 };
	 m_Robot->Rotate(templeArray, Angle);

}
void RobotArmRegistrationTab::rxm()
{
	 int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	 double Angle = -static_cast<double>(intValue);
	 double templeArray[3] = { 1, 0.0, 0.0 };
	 m_Robot->Rotate(templeArray, Angle);

}
void RobotArmRegistrationTab::rym()
{
	 int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	 double Angle = -static_cast<double>(intValue);
	 double templeArray[3] = { 0.0, 1, 0.0 };
	 m_Robot->Rotate(templeArray, Angle);

}
void RobotArmRegistrationTab::rzm()
{
	int intValue = m_ui.lineEdit_intuitiveValue_degree_2->text().toInt();
	double Angle = -static_cast<double>(intValue);
	double templeArray[3] = { 0.0, 0.0, 1 };
	m_Robot->Rotate(templeArray, Angle);
}


