#include "RobotArmRegistrationTab.h"/// <summary>

/// Robots the arm registration tab.
/// </summary>
/// <param name="ui">The UI.</param>
/// <param name="aDataStorage">a data storage.</param>
/// <param name="aRobot">a robot.</param>
/// <param name="aCamera">a camera.</param>
/// <param name="parent">The parent.</param>
/// <returns></returns>
RobotArmRegistrationTab::RobotArmRegistrationTab(Ui::HansRobotControls ui, mitk::DataStorage* aDataStorage, AbstractRobot* aRobot, AbstractCamera* aCamera, QWidget* parent)
{
	m_ui = ui;
	m_Robot = dynamic_cast<DianaRobot*>(aRobot);
	m_Camera = aCamera;
	m_dataStorage = aDataStorage;
	if (!m_dataStorage)
	{
		std::cout << "m_dataStorage is nullptr" << std::endl;
	}
	//移动
	m_LancetRobotRegistration = new LancetRobotRegistration(aRobot, aCamera);

	connect(m_ui.pushButton_xp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->xp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_yp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->yp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_zp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->zp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_xm_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->xm();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_ym_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->ym();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_zm_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->zm();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_rxp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->rxp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_ryp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->ryp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_rzp_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->rzp();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_rxm_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->rxm();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_rym_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->rym();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_rzm_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->rzm();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	//配准
	connect(m_ui.pushButton_setTCPToFlange_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->setTCPToFlange();
		m_ui.textBrowser_hans->append(QString("SetTCPToFlange"));
		});
	connect(m_ui.pushButton_setInitialPoint_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->recordInitialPos();
		m_ui.textBrowser_hans->append(QString("RecordInitialPos"));
		});
	connect(m_ui.pushButton_goToInitial_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->goToInitialPos();
		m_ui.textBrowser_hans->append(QString("GoToInitialPos"));
		});
	connect(m_ui.pushButton_captureRobot_2, &QPushButton::clicked, this, [this]() {
		m_ui.textBrowser_hans->append(QString("captureRobot"));
		m_LancetRobotRegistration->captureRobot();
		//m_ui.lineEdit_collectedRoboPose_2->setText(QString::number(m_LancetRobotRegistration->captureRobot()));
		});
	connect(m_ui.pushButton_replaceRegistration_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->replaceRegistration();
		m_ui.textBrowser_hans->append(QString("Replace Registration"));
		m_ui.lineEdit_collectedRoboPose_2->setText(QString::number(0));
		});
	connect(m_ui.pushButton_saveArmMatrix_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->saveArmMatrix();
		m_ui.textBrowser_hans->append(QString("Save ArmMatrix"));
		});
	connect(m_ui.pushButton_reuseArmMatrix_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->reuseArmMatrix();
		m_ui.textBrowser_hans->append(QString("Reuse ArmMatrix"));
		m_ui.lineEdit_collectedRoboPose_2->setText(QString::number(10));
		});
	connect(m_ui.pushButton_AutoMoveJ_2, &QPushButton::clicked, this, [this]() {
		m_LancetRobotRegistration->autoCollection();
		m_ui.textBrowser_hans->append(QString("Auto Collection"));
		//m_ui.lineEdit_collectedRoboPose_2->setText(QString::number(m_LancetRobotRegistration->countPose()));
		});
	connect(m_LancetRobotRegistration, &LancetRobotRegistration::countPose,
		this, &RobotArmRegistrationTab::updateUiCaputreCount);

	connect(m_ui.pushButton_RepeatPositionTest, &QPushButton::clicked, this, &RobotArmRegistrationTab::RepeatPositionTest);
	connect(m_ui.pushButton_AboslutePositionTest, &QPushButton::clicked, this, &RobotArmRegistrationTab::AboslutePositionTest);
}

void RobotArmRegistrationTab::updateUiCaputreCount(int cnt)
{ 
	m_ui.lineEdit_collectedRoboPose_2->setText(QString::number(cnt));
}

void RobotArmRegistrationTab::RepeatPositionTest()
{
	m_Robot->RecordInitialPos();
	m_Robot->Translate(50, 0, 0);//xyz
	m_Robot->GoToInitialPos();
}
int a;
void RobotArmRegistrationTab::AboslutePositionTest()
{
	double cube_length = 100; // 立方体边长，单位：mm
	double moveMent[][3] = {
		{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, -1, 0}, {0,0,1}
	};
	// 沿 X 轴移动
	m_Robot->Translate(cube_length, 0, 0); // 从 (0, 0, 0) 移动到 (300, 0, 0)

	// 沿 Y 轴移动
	m_Robot->Translate(0, cube_length, 0); // 从 (300, 0, 0) 移动到 (300, 300, 0)

	// 沿 Z 轴移动
	m_Robot->Translate(0, 0, cube_length); // 从 (300, 300, 0) 移动到 (300, 300, 300)

	// 沿 -X 轴移动
	m_Robot->Translate(-cube_length, 0, 0); // 从 (300, 300, 300) 移动到 (0, 300, 300)

	// 沿 -Y 轴移动
	m_Robot->Translate(0, -cube_length, 0); // 从 (0, 300, 300) 移动到 (0, 0, 300)

	// 沿 -Z 轴移动
	m_Robot->Translate(0, 0, -cube_length); // 从 (0, 0, 300) 移动到 (0, 0, 0)
}





