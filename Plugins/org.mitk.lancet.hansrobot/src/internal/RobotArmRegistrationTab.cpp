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

void RobotArmRegistrationTab::AboslutePositionTest()
{
	// 立方体的边长，单位：mm
	double cube_length = 100;

	// 定义点的名称
	const char* pointNames[] = { "a", "b", "c", "d", "e", "f", "g", "h" };

	// 选择要移动到的点
	switch (countNum)
	{
	case 0:
		m_Robot->Translate(cube_length, 0, 0); // 初始点
		std::cout << "移动到初始点: " << pointNames[0] << std::endl; // 打印点名称
		break;
	case 1:
		m_Robot->Translate(0, cube_length, 0); // 移动到顶点 1
		std::cout << "移动到点: " << pointNames[1] << std::endl; // 打印点名称
		break;
	case 2:
		m_Robot->Translate(-cube_length, 0, 0); // 移动到顶点 2
		std::cout << "移动到点: " << pointNames[2] << std::endl; // 打印点名称
		break;
	case 3:
		m_Robot->Translate(0, 0, cube_length); // 移动到顶点 3
		std::cout << "移动到点: " << pointNames[3] << std::endl; // 打印点名称
		break;
	case 4:
		m_Robot->Translate(0, -cube_length, 0); // 移动到顶点 4
		std::cout << "移动到点: " << pointNames[4] << std::endl; // 打印点名称
		break;
	case 5:
		m_Robot->Translate(cube_length, 0, 0); // 移动到顶点 5
		std::cout << "移动到点: " << pointNames[5] << std::endl; // 打印点名称
		break;
	case 6:
		m_Robot->Translate(0, cube_length, 0); // 移动到顶点 6
		std::cout << "移动到点: " << pointNames[6] << std::endl; // 打印点名称
		break;
	case 7:
		m_Robot->Translate(-cube_length, 0, 0); // 移动到顶点 7
		std::cout << "移动到点: " << pointNames[7] << std::endl; // 打印点名称
		break;
	default:
		// 如果 countNum 超过 7，重置为 0
		countNum = 0; 
		return; 
	}

	countNum++; // 增加 countNum，准备下一次移动
}





