#include <QTest>
#include <QFile>
#include <QMainWindow>
#include <QToolButton>
#include <QPluginLoader>

#include <ctkPluginActivator.h>

#include <lancetIScxmlStateMachineService.h>
#include <internal/lancetScxmlStateMachineService.h>
#include <internal/lancetScxmlStateMachineLoader.h>
#include <internal/lancetScxmlStateMachineLoader.h>
#include <internal/QLancetThaWorkbenchMenuBar.h>

#include "tst_thaWorkbenchWindow.h"

class tst_thaWorkbenchMenuBar 
  : public QObject 
{
  Q_OBJECT
public:
	
private slots:
  void initTestCase()
  {

  }

  void test_loaderErrorDataCase_data()
  {

  }

  void test_loaderErrorDataCase()
	{
  }

  void test_workbenchMenuBarGui()
  {
		// Make StateMachine
		auto stateMachineService = new lancet::ScxmlStateMachineService();
		stateMachineService->Initialize();
		QDir workDir(QDir::currentPath());
		qDebug() << workDir;
		workDir.cd("..");
		workDir.cd("..");
		workDir.cd("..");
		workDir.cd("..");
		workDir.cd("bin");
		workDir.cd("resources");
		workDir.cd("stateMachine");
		workDir.cd("tha");
		stateMachineService->CreateScxmlStateMachine(
			workDir.absoluteFilePath("org_mitk_lancet_tha_application.scxml"),
			workDir.absoluteFilePath("application_subsidiary.json"),
			lancet::IScxmlStateMachineState::StateType::Admin,
			lancet::IScxmlStateMachineState::Pointer(nullptr), "org_mitk_lancet_tha_application");

		// Make Window GUi
    tst_thaWorkbenchWindow win;
		win.setMinimumSize(1920, 1080);

		QLancetThaWorkbenchMenuBar* menuBar = new QLancetThaWorkbenchMenuBar();
    menuBar->SetStateMachineService(stateMachineService->GetScxmlStateMachineService());
		menuBar->setMovable(false);
		menuBar->setMinimumHeight(48);
		menuBar->setOrientation(Qt::Horizontal);
		menuBar->setObjectName("topMainActionsToolBar");
		menuBar->setContextMenuPolicy(Qt::PreventContextMenu);
		menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		menuBar->InitializeStateMachineForUi();
		

		// Update Menu style sheet
		QFile qss(":/qlancetthaworkbenchmenubar.qss");
		QCOMPARE(qss.open(QIODevice::ReadOnly), true);
		menuBar->setStyleSheet(qss.readAll());


		QToolButton* testToolBtn1 = new QToolButton();
		testToolBtn1->setMinimumWidth(100);
		testToolBtn1->setText("_________________testToolBtn1");
		testToolBtn1->setObjectName("testToolBtn1");
		testToolBtn1->setPopupMode(QToolButton::InstantPopup);
		testToolBtn1->setToolButtonStyle(Qt::ToolButtonFollowStyle);

		QMenu* menu = new QMenu();
		menu->addAction("SSSSS");
		menu->addAction(QString::fromLocal8Bit("中文按钮测试1"));
		menu->addAction(QString::fromLocal8Bit("中文按钮测试2"));
		menu->addAction(QString::fromLocal8Bit("中文按钮测试3"));
		testToolBtn1->setMenu(menu);

		menuBar->GetApplicationToolBar()->addWidget(testToolBtn1);


		win.setMenuWidget(menuBar);

    win.show();
    win.exec();
  }
};

//Q_DECLARE_METATYPE(tst_thaWorkbenchMenuBar::TestDataItem)
QTEST_MAIN(tst_thaWorkbenchMenuBar)
#include "tst_thaWorkbenchMenuBar.moc"