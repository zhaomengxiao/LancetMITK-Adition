/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QFemurRoughRegistrationsEditor.h"
#include "internal/org_mitk_lancet_femurroughregistrations_editor_Activator.h"

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

const QString QFemurRoughRegistrationsEditor::EDITOR_ID = "org.mitk.lancet.femurroughregistrations.editor";


//////////////////////////////////////////////////////////////////////////
// QFemurRoughRegistrationsEditor
//////////////////////////////////////////////////////////////////////////
QFemurRoughRegistrationsEditor::QFemurRoughRegistrationsEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(PluginActivator::GetPluginContext())
{
  // nothing here
	m_PrefServiceTracker.open();
}

QFemurRoughRegistrationsEditor::~QFemurRoughRegistrationsEditor()
{
	GetSite()->GetPage()->RemovePartListener(this);
}

#include <mitkDataStorage.h>

mitk::IDataStorageService* GetDataStorageService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<mitk::IDataStorageService>();
	return context->getService<mitk::IDataStorageService>(serviceRef);
}

mitk::DataStorage::Pointer GetDataStorage() 
{
	mitk::IDataStorageService* pDsService = GetDataStorageService();
	if (nullptr != pDsService && pDsService->GetActiveDataStorage().IsNotNull())
	{
		return pDsService->GetActiveDataStorage()->GetDataStorage();
	}
	return mitk::DataStorage::Pointer();
}

void QFemurRoughRegistrationsEditor::CreatePartControl(QWidget* parent)
{
	auto pDs = GetDataStorage();
	auto dataNodeArray = pDs->GetAll();
	
	for (auto& node = dataNodeArray->Begin(); node != dataNodeArray->End(); ++node)
	{

	}

	m_Controls.setupUi(parent);
	auto test_dir = QDir(":/org.mitk.lancet.femurroughregistrationseditor/");
	QFile qss(test_dir.absoluteFilePath("femurroughregistrationseditor.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		MITK_WARN << "error load file "
			<< test_dir.absoluteFilePath("femurroughregistrationseditor.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();
}

void QFemurRoughRegistrationsEditor::SetFocus()
{
	if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QFemurRoughRegistrationsEditor::DoSave()
{
}

void QFemurRoughRegistrationsEditor::DoSaveAs()
{
}

bool QFemurRoughRegistrationsEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QFemurRoughRegistrationsEditor::IsDirty() const
{
	return true;
}

bool QFemurRoughRegistrationsEditor::IsSaveAsAllowed() const
{
	return true;
}

berry::IPreferences::Pointer QFemurRoughRegistrationsEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QFemurRoughRegistrationsEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	//editorSite = site;
	//editorInput = input;
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	m_Prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (m_Prefs.IsNotNull())
	{
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QFemurRoughRegistrationsEditor, const berry::IBerryPreferences*>
			(this, &QFemurRoughRegistrationsEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QFemurRoughRegistrationsEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QFemurRoughRegistrationsEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}
