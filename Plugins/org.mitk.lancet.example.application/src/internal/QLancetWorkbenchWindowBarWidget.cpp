#include "QLancetWorkbenchWindowBarWidget.h"

#include "ui_lancetworkbenchwindowbarfrom.h"
#include "org_mitk_lancet_example_application_Activator.h"

#include <berryPlatformUI.h>
#include <berryIWorkbenchPage.h>
#include <berryWorkbenchPlugin.h>
#include <berryIWorkbenchWindow.h>
#include <berryIWorkbenchWindowConfigurer.h>

#include <mitkIDataStorageService.h>
#include <mitkDataStorageEditorInput.h>

#include <QMainWindow>
#include <QDebug>

QLancetWorkbenchWindowBarWidget::QLancetWorkbenchWindowBarWidget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::LancetWorkbenchWindowBarFrom)
{
	ui->setupUi(this);
}

void QLancetWorkbenchWindowBarWidget::on_pushbtnExample1_clicked()
{
	berry::IWorkbenchWindow::Pointer window = berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow();
	QMainWindow* mainWindow = qobject_cast<QMainWindow*> (window->GetShell()->GetControl());
	
	if (window->GetActivePage().IsNull())
	{
		QString defaultPerspId = window->GetWorkbench()->GetPerspectiveRegistry()->GetDefaultPerspective();
		window->GetWorkbench()->ShowPerspective(defaultPerspId, window);
	}

	//if (window->GetActivePage()->GetActiveEditor().IsNotNull())
	//{
	//	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) 
	//		<< QString("close Editor %1").arg(window->GetActivePage()->GetActiveEditor()->GetPartName()) << "\033[0m";
	//	window->GetActivePage()->CloseEditor(window->GetActivePage()->GetActiveEditor(), false);
	//}

	{
		QString editorId = /*"org.blueberry.editors.help"*/"org.mitk.editors.stdmultiwidget";
		ctkPluginContext* context = org_mitk_lancet_example_application_Activator::GetPluginContext();

		ctkServiceReference serviceRef = context->getServiceReference<mitk::IDataStorageService>();
		if (serviceRef)
		{
			mitk::IDataStorageService* dsService = context->getService<mitk::IDataStorageService>(serviceRef);
			if (dsService)
			{
				mitk::IDataStorageReference::Pointer dsRef = dsService->GetDataStorage();
				berry::IEditorInput::Pointer editorInput(new mitk::DataStorageEditorInput(dsRef));


				if (berry::WorkbenchPlugin::GetDefault()->GetEditorRegistry()->FindEditor(editorId).IsNotNull())
				{
					window->GetActivePage()->OpenEditor(editorInput, editorId, true, berry::IWorkbenchPage::MATCH_ID);
				}
				else
				{
					qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)
						<< "Not find target plugin " << editorId;
				}

			}
		}
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) 
			<< QString("open %1").arg(editorId) << "\033[0m";
	}
}

void QLancetWorkbenchWindowBarWidget::on_pushbtnExample2_clicked()
{
	berry::IWorkbenchWindow::Pointer window = berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow();
	QMainWindow* mainWindow = qobject_cast<QMainWindow*> (window->GetShell()->GetControl());

	if (window->GetActivePage().IsNull())
	{
		QString defaultPerspId = window->GetWorkbench()->GetPerspectiveRegistry()->GetDefaultPerspective();
		window->GetWorkbench()->ShowPerspective(defaultPerspId, window);
	}

	//if (window->GetActivePage()->GetActiveEditor().IsNotNull())
	//{
	//	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)
	//		<< QString("close Editor %1").arg(window->GetActivePage()->GetActiveEditor()->GetPartName()) << "\033[0m";
	//	window->GetActivePage()->CloseEditor(window->GetActivePage()->GetActiveEditor(), false);
	//}

	{
		QString editorId = /*"org.blueberry.editors.help"*/"org.mitk.lancet.example.editor";
		ctkPluginContext* context = org_mitk_lancet_example_application_Activator::GetPluginContext();

		ctkServiceReference serviceRef = context->getServiceReference<mitk::IDataStorageService>();
		if (serviceRef)
		{
			mitk::IDataStorageService* dsService = context->getService<mitk::IDataStorageService>(serviceRef);
			if (dsService)
			{
				mitk::IDataStorageReference::Pointer dsRef = dsService->GetDataStorage();
				berry::IEditorInput::Pointer editorInput(new mitk::DataStorageEditorInput(dsRef));


				if (berry::WorkbenchPlugin::GetDefault()->GetEditorRegistry()->FindEditor(editorId).IsNotNull())
				{
					window->GetActivePage()->OpenEditor(editorInput, editorId, true, berry::IWorkbenchPage::MATCH_ID);
				}
				else
				{
					qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)
						<< "Not find target plugin " << editorId;
				}

			}
		}
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)
			<< QString("open %1").arg(editorId) << "\033[0m";
	}
}

//void QLancetWorkbenchWindowBarWidget::on_pushbtnExample3_clicked()
//{
//	berry::IWorkbenchWindow::Pointer window = berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow();
//	QMainWindow* mainWindow = qobject_cast<QMainWindow*> (window->GetShell()->GetControl());
//
//	if (window->GetActivePage().IsNull())
//	{
//		QString defaultPerspId = window->GetWorkbench()->GetPerspectiveRegistry()->GetDefaultPerspective();
//		window->GetWorkbench()->ShowPerspective(defaultPerspId, window);
//	}
//
//	window->GetActivePage()->ShowView("org.mitk.views.acetabularpreparationwidget");
//}