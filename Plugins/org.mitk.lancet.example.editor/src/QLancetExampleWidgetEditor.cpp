/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QLancetExampleWidgetEditor.h"
#include "internal/org_mitk_lancet_example_editor_Activator.h"

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

#include "internal/QLancetExampleEditorControl.h"

const QString QLancetExampleWidgetEditor::EDITOR_ID = "org.mitk.lancet.example.editor";


//////////////////////////////////////////////////////////////////////////
// QLancetExampleWidgetEditor
//////////////////////////////////////////////////////////////////////////
QLancetExampleWidgetEditor::QLancetExampleWidgetEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(org_mitk_lancet_example_editor_Activator::GetContext())
{
  // nothing here
	m_PrefServiceTracker.open();
}

QLancetExampleWidgetEditor::~QLancetExampleWidgetEditor()
{
	GetSite()->GetPage()->RemovePartListener(this);
}

void QLancetExampleWidgetEditor::CreatePartControl(QWidget* parent)
{
	berry::IBerryPreferences* preferences = dynamic_cast<berry::IBerryPreferences*>(GetPreferences().GetPointer());

	QHBoxLayout* layout = new QHBoxLayout(parent);
	layout->setContentsMargins(0, 0, 0, 0);

	widgetInstace.reset(new QWidget(parent));
	widgetInstace->setStyleSheet("background-color:rgb(255,0,0)");
	layout->addWidget(widgetInstace.data());

	// shell->GetContol();
	//QHBoxLayout* layoutWidgetInstace = new QHBoxLayout(widgetInstace.data());
	QWidget* exampleEditorContol = new QLancetExampleEditorControl(widgetInstace.data());
	//layoutWidgetInstace->addWidget(exampleEditorContol);

	GetSite()->GetPage()->AddPartListener(this);

}

void QLancetExampleWidgetEditor::SetFocus()
{
	if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QLancetExampleWidgetEditor::DoSave()
{
}

void QLancetExampleWidgetEditor::DoSaveAs()
{
}

bool QLancetExampleWidgetEditor::IsDirty() const
{
	return false;
}

bool QLancetExampleWidgetEditor::IsSaveAsAllowed() const
{
	return false;
}

berry::IPreferences::Pointer QLancetExampleWidgetEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QLancetExampleWidgetEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
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
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QLancetExampleWidgetEditor, const berry::IBerryPreferences*>
			(this, &QLancetExampleWidgetEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QLancetExampleWidgetEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QLancetExampleWidgetEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}
