#include "QLancetThaApplicationWorkbenchWindowAdvisor.h"

// ctk
#include <ctkPluginContext.h>

// berry
#include <berryPlatformUI.h>
#include <berryIWorkbenchPage.h>
#include <berryWorkbenchPlugin.h>

// Qt
#include <QMenuBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMainWindow>

// statemachine
#include <lancetIScxmlStateMachineService.h>

// tha
#include "QLancetThaWorkbenBarAdvisor.h"
#include "QLancetThaWorkbenchMenuBar.h"
#include "org_mitk_lancet_tha_application_Activator.h"

QLancetThaApplicationWorkbenchWindowAdvisor::QLancetThaApplicationWorkbenchWindowAdvisor(
	berry::WorkbenchAdvisor* wbAdvisor,
	const berry::SmartPointer<berry::IWorkbenchWindowConfigurer>& configurer)
	: berry::WorkbenchWindowAdvisor(configurer)
{

}

QLancetThaApplicationWorkbenchWindowAdvisor::~QLancetThaApplicationWorkbenchWindowAdvisor()
{

}

berry::SmartPointer<berry::ActionBarAdvisor> 
QLancetThaApplicationWorkbenchWindowAdvisor::CreateActionBarAdvisor(
	berry::SmartPointer<berry::IActionBarConfigurer> configurer)
{
	return berry::SmartPointer<QLancetThaWorkbenBarAdvisor>(
		new QLancetThaWorkbenBarAdvisor(configurer));
}

void QLancetThaApplicationWorkbenchWindowAdvisor::PostWindowCreate()
{
	qDebug() << "QLancetThaApplicationWorkbenchWindowAdvisor::PostWindowCreate";
	berry::IWorkbenchWindow::Pointer window = this->GetWindowConfigurer()->GetWindow();
	QMainWindow* mainWindow = qobject_cast<QMainWindow*> (window->GetShell()->GetControl());
	mainWindow->setContextMenuPolicy(Qt::PreventContextMenu);

	QVBoxLayout* layout = new QVBoxLayout();
	mainWindow->centralWidget()->setLayout(layout);
	layout->setMargin(0);

	QWidget* mainWndMenuWidget = new QWidget(mainWindow->centralWidget());
	mainWndMenuWidget->setMinimumHeight(40);
	mainWndMenuWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
	mainWndMenuWidget->setStyleSheet("background-color:rgb(0,0,255)");

	QLancetThaWorkbenchMenuBar* menuBar = new QLancetThaWorkbenchMenuBar();
	menuBar->setMovable(false);
	menuBar->setMinimumHeight(48);
	menuBar->setOrientation(Qt::Horizontal);
	menuBar->setObjectName("topMainActionsToolBar");
	menuBar->setContextMenuPolicy(Qt::PreventContextMenu);
	menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	menuBar->setStyleSheet("background-color:rgb(255,0,0)");
	menuBar->InitializeStateMachineForUi();

	menuBar->GetModuleMenuBar()->setStyleSheet(
		"\
		QMenuBar::item{\
		margin - top: 10px; \
		margin - left: 30px; \
		font - size:12px; \
		padding:6px 45px 6px 45px; \
		background: transparent; \
		border - radius:4px; \
		border - image:url(: / default.png); \
		}\
		");


	mainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, menuBar);

	if (menuBar->GetStateMachineService().IsNotNull())
	{
		menuBar->GetStateMachineService()->StartStateMachine();
	}
}

void QLancetThaApplicationWorkbenchWindowAdvisor::PostWindowOpen()
{
	__super::PostWindowOpen();

	berry::IWorkbenchWindow::Pointer window = this->GetWindowConfigurer()->GetWindow();

	if (window.IsNotNull() && window->GetActivePage().IsNotNull())
	{
		for (auto item_view : window->GetActivePage()->GetViews())
		{
			qDebug() << item_view->GetViewSite()->GetId();
			window->GetActivePage()->HideView(item_view);
		}
	}
}
