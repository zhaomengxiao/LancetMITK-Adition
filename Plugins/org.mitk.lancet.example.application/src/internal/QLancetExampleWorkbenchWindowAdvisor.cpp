#include "QLancetExampleWorkbenchWindowAdvisor.h"

#include <ctkPluginContext.h>

#include <berryIWorkbenchPage.h>
#include <berryActionBarAdvisor.h>
#include <berryIActionBarConfigurer.h>
#include <berryIWorkbenchWindowConfigurer.h>

#include <QTimer>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QMainWindow>
#include <QPushbutton>
#include <QHBoxLayout>
#include <QActionGroup>
#include <QStringLiteral>
#include <QToolButton>
#include <qstring.h>
#include <mitkIDataStorageService.h>
#include <mitkDataStorageEditorInput.h>

#include "QLancetWorkbenchWindowBarWidget.h"
#include "QLancetWorkbenchWindowToolBarWidget.h"
#include "QLancetWorkbenchWindowLeftToolBarWidget.h"
 //struct QLancetExampleWorkbenchWindowAdvisor::QLancetExampleWorkbenchWindowAdvisorPrivateImp
 //{
	// QAction* appIconAction;
 //};

 QLancetExampleWorkbenchWindowAdvisor::QLancetExampleWorkbenchWindowAdvisor(berry::WorkbenchAdvisor* wbAdvisor, 
											  const berry::SmartPointer<berry::IWorkbenchWindowConfigurer>& configurer)
	 : berry::WorkbenchWindowAdvisor(configurer)
	 //, imp(nullptr)
 {
	 qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
 }

 QLancetExampleWorkbenchWindowAdvisor::~QLancetExampleWorkbenchWindowAdvisor()
 {
	 qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
 }

 // TODO Not QmitkExtActionBarAdvisor
 berry::SmartPointer<berry::ActionBarAdvisor> QLancetExampleWorkbenchWindowAdvisor::CreateActionBarAdvisor(berry::SmartPointer<berry::IActionBarConfigurer> configurer)
 {
	 qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	 berry::ActionBarAdvisor::Pointer actionBarAdvisor(nullptr);
	 return berry::WorkbenchWindowAdvisor::CreateActionBarAdvisor(configurer);
	 //berry::ActionBarAdvisor::Pointer actionBarAdvisor(new QmitkExtActionBarAdvisor(configurer));
	 //return actionBarAdvisor;
 }

 QWidget* QLancetExampleWorkbenchWindowAdvisor::CreateEmptyWindowContents(QWidget* parent)
 {
	 // do nothing
	 qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	 return nullptr;
 }

 void printQtChilderWidgets(QObject* widget, int leven = 1, QColor c = QColor())
 {
	 QString formatter = "";
	 for (auto index = 0; index < leven; ++index)
	 {
		 formatter += "    ";
	 }
	 if (widget && dynamic_cast<QWidget*>(widget))
	 {
		 dynamic_cast<QWidget*>(widget)->setStyleSheet(
			 QString("background-color:rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue()));
		 if (widget->children().size())
		 {
			 for (auto sub_widget : widget->children())
			 {
				 qDebug() << "\033[0;32m"
					 << "\n"
					 << formatter << QString("+sub_widget.childer ") << sub_widget << "\033[0m";
				 printQtChilderWidgets(sub_widget, leven + 1, c);
			 }
		 }
		 else
		 {
			 qDebug() << "\033[0;32m"
				 << "\n"
				 << formatter << QString("+sub_widget.childer ") << widget << "\033[0m";
		 }
	 }
	 else
	 {
	 }
 }

 void QLancetExampleWorkbenchWindowAdvisor::PostWindowCreate()
 {
	 qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	 berry::IWorkbenchWindow::Pointer window = this->GetWindowConfigurer()->GetWindow();
	 QMainWindow* mainWindow = qobject_cast<QMainWindow*> (window->GetShell()->GetControl());
	 mainWindow->setContextMenuPolicy(Qt::PreventContextMenu);

	 //// ==== Application menu tool bar ============================

	 ///>> Top menu tool bar
	 /**
		\bug QToolBar Unable to validate QT layout. 
		\see QLancetWorkbenchWindowToolBarWidget
	 */

	 QLancetWorkbenchWindowToolBarWidget* topMainActionsToolBar = new QLancetWorkbenchWindowToolBarWidget();
	 topMainActionsToolBar->setMovable(false);
	 topMainActionsToolBar->setMinimumHeight(60);
	 topMainActionsToolBar->setOrientation(Qt::Horizontal);
	 topMainActionsToolBar->setObjectName("topMainActionsToolBar");
	 topMainActionsToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
	 topMainActionsToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	 QLancetWorkbenchWindowBarWidget* workbenchToolBar = new QLancetWorkbenchWindowBarWidget(topMainActionsToolBar);

	 topMainActionsToolBar->setToolBarWidget(workbenchToolBar);
	 mainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, topMainActionsToolBar);

	 ///>> Left menu tool bar
	 //
	 QLancetWorkbenchWindowToolBarWidget* leftMainActionsToolBar = new QLancetWorkbenchWindowToolBarWidget();
	 leftMainActionsToolBar->setMovable(false);
	 leftMainActionsToolBar->setMinimumWidth(160);
	 leftMainActionsToolBar->setOrientation(Qt::Vertical);
	 leftMainActionsToolBar->setObjectName("leftMainActionsToolBar");
	 leftMainActionsToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

	 leftMainActionsToolBar->setToolBarWidget(new QLancetWorkbenchWindowLeftToolBarWidget(leftMainActionsToolBar));
	 mainWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, leftMainActionsToolBar);

 }
