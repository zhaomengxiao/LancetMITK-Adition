/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QPelvisRoughRegistrationsEditor.h"
#include "internal/org_mitk_lancet_pelvisroughregistrations_editor_Activator.h"

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>
#include <berryIPreferences.h>

#include <mitkIOUtil.h>
#include <mitkSceneIO.h>
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

const QString 
QPelvisRoughRegistrationsEditor::EDITOR_ID = "org.mitk.lancet.pelvisroughregistrations.editor";

static const char* const DEBUG_ANTERIOR_LANDMARK_LEFT = "anterior_landmark_left";
static const char* const DEBUG_SUPERIOR_LANDMARK_LEFT = "superior_landmark_left";
static const char* const DEBUG_POSTERIOR_LANDMARK_LEFT = "posterior_landmark_left";
static const char* const DEBUG_PELVIS_SURFACE_LEFT = "pelvis_clipped_left";

//////////////////////////////////////////////////////////////////////////
// QPelvisRoughRegistrationsEditor
//////////////////////////////////////////////////////////////////////////
QPelvisRoughRegistrationsEditor::QPelvisRoughRegistrationsEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(PluginActivator::GetPluginContext())
{
  // nothing here
	m_PrefServiceTracker.open();
}

QPelvisRoughRegistrationsEditor::~QPelvisRoughRegistrationsEditor()
{
	this->UnInitializeMitkMultiWidget();
	this->GetSite()->GetPage()->RemovePartListener(this);
}

void QPelvisRoughRegistrationsEditor::CreatePartControl(QWidget* parent)
{
	m_Controls.setupUi(parent);
	this->InitializeMitkMultiWidget();
	//auto test_dir = QDir(":/org.mitk.lancet.pelvisroughregistrationseditor/");
	//QFile qss(test_dir.absoluteFilePath("pelvisroughregistrationseditor.qss"));
	//if (!qss.open(QIODevice::ReadOnly))
	//{
	//	MITK_WARN << "error load file "
	//		<< test_dir.absoluteFilePath("pelvisroughregistrationseditor.qss") << "\n"
	//		<< "error: " << qss.errorString();
	//}

}

void QPelvisRoughRegistrationsEditor::SetFocus()
{
if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QPelvisRoughRegistrationsEditor::DoSave()
{
}

void QPelvisRoughRegistrationsEditor::DoSaveAs()
{
}

bool QPelvisRoughRegistrationsEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QPelvisRoughRegistrationsEditor::IsDirty() const
{
	return true;
}

bool QPelvisRoughRegistrationsEditor::IsSaveAsAllowed() const
{
	return true;
}

berry::IPreferences::Pointer QPelvisRoughRegistrationsEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QPelvisRoughRegistrationsEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	m_Prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (m_Prefs.IsNotNull())
	{
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QPelvisRoughRegistrationsEditor, const berry::IBerryPreferences*>
			(this, &QPelvisRoughRegistrationsEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QPelvisRoughRegistrationsEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QPelvisRoughRegistrationsEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}

itk::SmartPointer<mitk::DataStorage> QPelvisRoughRegistrationsEditor::GetDataStorage() const
{
	auto context = PluginActivator::GetPluginContext();

	auto serviceRef = context->getServiceReference<mitk::IDataStorageService>();
	auto service = context->getService<mitk::IDataStorageService>(serviceRef);
	if (service && service->GetDataStorage().IsNotNull())
	{
		return service->GetDataStorage()->GetDataStorage();
	}

	return itk::SmartPointer<mitk::DataStorage>();
}

void QPelvisRoughRegistrationsEditor::InitializeMitkMultiWidget()
{
	if (this->GetDataStorage().IsNotNull())
	{
		QStringList listVisibilityNodes = {
			DEBUG_ANTERIOR_LANDMARK_LEFT,
			DEBUG_SUPERIOR_LANDMARK_LEFT,
			DEBUG_POSTERIOR_LANDMARK_LEFT,
			DEBUG_PELVIS_SURFACE_LEFT
		};
		auto dataNodes = this->GetDataStorage()->GetAll();
		for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
		{
			std::string __nodename;
			if (!(*item)->GetName(__nodename))
			{
				continue;
			}

			auto find_node = this->GetDataStorage()->GetNamedNode(__nodename.c_str());
			if (find_node)
			{
				listDataStorageNodes.push_back(__nodename.c_str());

				find_node->SetVisibility(false);
				for (auto& visibilityNodeName : listVisibilityNodes)
				{
					if (visibilityNodeName.toStdString() == __nodename)
					{
						find_node->SetVisibility(true);
						break;
					}
				}
			}
		}
		mitk::SceneIO::Pointer sceneIO = mitk::SceneIO::New();
		sceneIO->LoadScene("C:/Users/Sun/Desktop/lancethipnew2021.mitk", this->GetDataStorage().GetPointer());
		auto dataNodes_2 = this->GetDataStorage()->GetAll();
		for (auto item = dataNodes_2->begin(); item != dataNodes_2->end(); ++item)
		{
			std::string __nodename;
			if (!(*item)->GetName(__nodename))
			{
				continue;
			}

			(*item)->SetVisibility(false);
			for (auto& visibilityNodeName : listVisibilityNodes)
			{
				if (visibilityNodeName.toStdString() == __nodename)
				{
					(*item)->SetVisibility(true);
					break;
				}
			}
		}
	}

	auto defaultLayoutDesign = QmitkRenderWindowMenu::LayoutDesign::ONE_TOP_3D_BOTTOM;
	m_Controls.widget->SetDataStorage(this->GetDataStorage().GetPointer());
	m_Controls.widget->InitializeMultiWidget();
	m_Controls.widget->AddPlanesToDataStorage();
	m_Controls.widget->GetRenderWindow4()->LayoutDesignChanged(defaultLayoutDesign);
	m_Controls.widget->ResetCrosshair();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	//m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
}

void QPelvisRoughRegistrationsEditor::UnInitializeMitkMultiWidget()
{
	if (this->GetDataStorage().IsNotNull())
	{
		for (QString& nodeName : this->listDataStorageNodes)
		{
			auto nodeValue = this->GetDataStorage()->GetNamedNode(nodeName.toStdString());
			if (nodeValue)
			{
				this->GetDataStorage()->Remove(nodeValue);
			}
		}
	}
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	//this->m_Controls.widget->SetDataStorage(nullptr);
}
