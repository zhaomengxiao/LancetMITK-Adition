/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QLinkingHardWareEditor.h"
#include "internal/org_mitk_lancet_linkinghardware_editor_Activator.h"

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>
#include <berryIPreferences.h>

#include <mitkColorProperty.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include <mitkDataStorageEditorInput.h>
#include <mitkIDataStorageService.h>

// mitk qt widgets module
#include <QmitkInteractionSchemeToolBar.h>
#include <QmitkLevelWindowWidget.h>
#include <QmitkRenderWindowWidget.h>
#include <QmitkStdMultiWidget.h>


#include <ctkServiceTracker.h>

// mitk gui qt common plugin
#include <QmitkMultiWidgetDecorationManager.h>
// Qt
#include <QMessageBox>
#include <QDir>
#include <lancetIDevicesAdministrationService.h>
#include <lancetIDevicesScanner.h>
const QString QLinkingHardWareEditor::EDITOR_ID = "org.mitk.lancet.linkinghardware.editor";


//////////////////////////////////////////////////////////////////////////
// QLinkingHardWareEditor
//////////////////////////////////////////////////////////////////////////
QLinkingHardWareEditor::QLinkingHardWareEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(org_mitk_lancet_linkinghardware_editor_Activator::GetContext())
{
  // nothing here
	m_PrefServiceTracker.open();
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

QLinkingHardWareEditor::~QLinkingHardWareEditor()
{
	GetSite()->GetPage()->RemovePartListener(this);
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QLinkingHardWareEditor::CreatePartControl(QWidget* parent)
{
	m_Controls.setupUi(parent);
	auto test_dir = QDir(":/org.mitk.lancet.linkinghardwareeditor/");
	QFile qss(test_dir.absoluteFilePath("linkinghardwareeditor.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		qWarning() << __func__ << __LINE__ << ":" << "error load file "
			<< test_dir.absoluteFilePath("linkinghardwareeditor.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	qInfo() << "log.file.pos " << qss.pos();
	m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();
	if (this->GetService())
	{
		this->ConnectToService();
	}
}

void QLinkingHardWareEditor::SetFocus()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QLinkingHardWareEditor::DoSave()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QLinkingHardWareEditor::DoSaveAs()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

bool QLinkingHardWareEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QLinkingHardWareEditor::IsDirty() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

bool QLinkingHardWareEditor::IsSaveAsAllowed() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

berry::IPreferences::Pointer QLinkingHardWareEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QLinkingHardWareEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	//editorSite = site;
	//editorInput = input;
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	m_Prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (m_Prefs.IsNotNull())
	{
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QLinkingHardWareEditor, const berry::IBerryPreferences*>
			(this, &QLinkingHardWareEditor::OnPreferencesChanged));
	}

	// open service
	if (this->GetService())
	{
		auto scanner = this->GetService()->GetScanner();
	}
}

berry::IPartListener::Events::Types QLinkingHardWareEditor::GetPartEventTypes() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QLinkingHardWareEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}
lancet::IDevicesAdministrationService* QLinkingHardWareEditor::GetService() const
{
	auto context = PluginActivator::GetContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}
void QLinkingHardWareEditor::ConnectToService()
{
	auto sender = this->GetService();
	if (sender)
	{
		lancet::IDevicesAdministrationService* o = sender;
		QObject::connect(o, &lancet::IDevicesAdministrationService::IDevicesGetStatus,
			this, &QLinkingHardWareEditor::Slot_IDevicesGetStatus);
	}
}
void QLinkingHardWareEditor::on_HardWareWidget(int staubli, int ndi)
{
	QString str = "--staubli:" + QString::number(staubli) + "--ndi:" + QString::number(ndi);
	MITK_INFO << "QLinkingHardware:" << __func__ + str << ": log";
	if (staubli == 0 && ndi == 0)
		m_Controls.widget->setStyleSheet("border-image: url(:/org.mitk.lancet.linkinghardwareeditor/registration/robot_0_NID_0.png);");
	else if (staubli == 0 && ndi == 1)
		m_Controls.widget->setStyleSheet("border-image: url(:/org.mitk.lancet.linkinghardwareeditor/registration/robot_0_NID_1.png);");
	else if (staubli == 1 && ndi == 0)
		m_Controls.widget->setStyleSheet("border-image: url(:/org.mitk.lancet.linkinghardwareeditor/registration/robot_1_NID_0.png);");
	else if (staubli == 1 && ndi == 1)	
		m_Controls.widget->setStyleSheet("border-image: url(:/org.mitk.lancet.linkinghardwareeditor/registration/robot_1_NID_1.png);");
}

void QLinkingHardWareEditor::Slot_IDevicesGetStatus()
{
	auto scanner = this->GetService()->GetScanner();
	this->on_HardWareWidget(scanner->GetRobotStatus(), scanner->GetNDIStatus());
}
