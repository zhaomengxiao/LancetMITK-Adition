/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QPutMolarCupProsthesisEditor.h"
#include "internal/org_mitk_lancet_putmolarcupprosthesis_editor_Activator.h"

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

const QString QPutMolarCupProsthesisEditor::EDITOR_ID = "org.mitk.lancet.putmolarcupprosthesis.editor";


//////////////////////////////////////////////////////////////////////////
// QPutMolarCupProsthesisEditor
//////////////////////////////////////////////////////////////////////////
QPutMolarCupProsthesisEditor::QPutMolarCupProsthesisEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(org_mitk_lancet_putmolarcupprosthesis_editor_Activator::GetContext())
{
  // nothing here
	m_PrefServiceTracker.open();
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

QPutMolarCupProsthesisEditor::~QPutMolarCupProsthesisEditor()
{
	GetSite()->GetPage()->RemovePartListener(this);
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QPutMolarCupProsthesisEditor::CreatePartControl(QWidget* parent)
{
	m_Controls.setupUi(parent);
	auto test_dir = QDir(":/org.mitk.lancet.putmolarcupprosthesiseditor/");
	QFile qss(test_dir.absoluteFilePath("putmolarcupprosthesiseditor.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		qWarning() << __func__ << __LINE__ << ":" << "error load file "
			<< test_dir.absoluteFilePath("putmolarcupprosthesiseditor.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	qInfo() << "log.file.pos " << qss.pos();
	m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();
}

void QPutMolarCupProsthesisEditor::SetFocus()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QPutMolarCupProsthesisEditor::DoSave()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

void QPutMolarCupProsthesisEditor::DoSaveAs()
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
}

bool QPutMolarCupProsthesisEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QPutMolarCupProsthesisEditor::IsDirty() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

bool QPutMolarCupProsthesisEditor::IsSaveAsAllowed() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return true;
}

berry::IPreferences::Pointer QPutMolarCupProsthesisEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QPutMolarCupProsthesisEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
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
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QPutMolarCupProsthesisEditor, const berry::IBerryPreferences*>
			(this, &QPutMolarCupProsthesisEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QPutMolarCupProsthesisEditor::GetPartEventTypes() const
{
	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QPutMolarCupProsthesisEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}