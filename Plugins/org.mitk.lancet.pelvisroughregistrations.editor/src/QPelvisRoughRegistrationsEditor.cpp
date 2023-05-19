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

#include <lancetIDevicesAdministrationService.h>
#include <lancetSpatialFittingAbstractService.h>
#include <internal/lancetTrackingDeviceManage.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <lancetNavigationObjectVisualizationFilter.h>
#include <internal/lancetSpatialFittingPelvicRoughRegistrationsModel.h>


const QString 
QPelvisRoughRegistrationsEditor::EDITOR_ID = "org.mitk.lancet.pelvisroughregistrations.editor";

static const char* const DEBUG_ANTERIOR_LANDMARK_LEFT = "anterior_landmark_left";
static const char* const DEBUG_SUPERIOR_LANDMARK_LEFT = "superior_landmark_left";
static const char* const DEBUG_POSTERIOR_LANDMARK_LEFT = "posterior_landmark_left";
static const char* const DEBUG_PELVIS_SURFACE_LEFT = "pelvis_clipped_left";
static const char* const DEBUG_TEMP_VERIFY_POINTSET = "sssss";

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

	this->updateProbeTm.setInterval(100);
	connect(&updateProbeTm, &QTimer::timeout, this, [=]() 
		{
			if (nullptr == this->GetDevicesService() || this->GetDevicesService()->GetConnector().IsNull())
			{
				return;
			}

			auto trackingManager = this->GetDevicesService()->GetConnector();
			auto visualizer = trackingManager->GetNavigationDataToNavigationDataFilter("Vega");

			if (visualizer.IsNotNull())
			{ 
				visualizer->Update();
				mitk::RenderingManager::GetInstance()->RequestUpdateAll();
			}
		});
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
	this->InitializeQtEventOnService();
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

void QPelvisRoughRegistrationsEditor::OnRenderModelChange()
{
	using Model = lancet::spatial_fitting::PelvicRoughRegistrationsModel::Model;
	Model model = Model::Collect;
	if (this->GetServiceModel().IsNotNull())
	{
		model = this->GetServiceModel()->GetModel();
	}

	switch (model)
	{
	case Model::Collect:
		this->InitializeMitkMultiWidgetOnCollectModel();
		break;
	case Model::Verify:
		this->InitializeMitkMultiWidgetOnVerifyModel();
		break;
	}
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

itk::SmartPointer<QPelvisRoughRegistrationsEditor::PelvicRoughRegistrationsModel> 
  QPelvisRoughRegistrationsEditor::GetServiceModel() const
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::SpatialFittingAbstractService>();
	auto service = context->getService<lancet::SpatialFittingAbstractService>(serviceRef);
	if (service)
	{
		return service->GetPelvicRoughRegistrationsModel();
	}

	return PelvicRoughRegistrationsModel::Pointer();
}

lancet::IDevicesAdministrationService* QPelvisRoughRegistrationsEditor::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void QPelvisRoughRegistrationsEditor::InitializeDataStorageForService()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	QStringList listImageArray = {
		DEBUG_POSTERIOR_LANDMARK_LEFT,
		DEBUG_ANTERIOR_LANDMARK_LEFT,
		DEBUG_SUPERIOR_LANDMARK_LEFT
	};
	for (int index = 0; index < listImageArray.size(); ++index)
	{
		auto node = this->GetDataStorage()->GetNamedNode(listImageArray[index].toStdString());
		if (node)
		{
			auto tmpPointSet = dynamic_cast<mitk::PointSet*>(node->GetData());
			if (tmpPointSet)
			{
				MITK_INFO << "image point " << tmpPointSet->GetPoint(0);
				this->GetServiceModel()->SetImagePointArray(index, tmpPointSet->GetPoint(0));
			}
		}
	}

}

void QPelvisRoughRegistrationsEditor::InitializeMitkMultiWidget()
{
	if (this->GetDataStorage().IsNotNull())
	{
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
			}
		}
		//mitk::SceneIO::Pointer sceneIO = mitk::SceneIO::New();
		//sceneIO->LoadScene("C:/Users/sun/Desktop/lancethipnew2021.mitk", this->GetDataStorage().GetPointer());
	}

	auto defaultLayoutDesign = QmitkRenderWindowMenu::LayoutDesign::ONE_TOP_3D_BOTTOM;
	m_Controls.widget->SetDataStorage(this->GetDataStorage().GetPointer());
	m_Controls.widget->InitializeMultiWidget();
	m_Controls.widget->AddPlanesToDataStorage();
	m_Controls.widget->GetRenderWindow4()->LayoutDesignChanged(defaultLayoutDesign);
	m_Controls.widget->ResetCrosshair();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	this->OnRenderModelChange();
	this->InitializeDataStorageForService();
}

void QPelvisRoughRegistrationsEditor::UnInitializeMitkMultiWidget()
{
	//if (this->GetDataStorage().IsNotNull())
	//{
	//	for (QString& nodeName : this->listDataStorageNodes)
	//	{
	//		auto nodeValue = this->GetDataStorage()->GetNamedNode(nodeName.toStdString());
	//		if (nodeValue)
	//		{
	//			this->GetDataStorage()->Remove(nodeValue);
	//		}
	//	}
	//}
	//mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	//this->m_Controls.widget->SetDataStorage(nullptr);
}

void QPelvisRoughRegistrationsEditor::InitializeQtEventOnService()
{
	if (this->GetServiceModel().IsNull())
	{
		return;
	}
	
	connect(this->GetServiceModel().GetPointer(), &PelvicRoughRegistrationsModel::VegaPointChange,
		this, &QPelvisRoughRegistrationsEditor::OnVegaPointChange);
	connect(this->GetServiceModel().GetPointer(), &PelvicRoughRegistrationsModel::RenderModelChange,
		this, &QPelvisRoughRegistrationsEditor::OnRenderModelChange);
}

void QPelvisRoughRegistrationsEditor::UnInitializeQtEventOnService()
{
}

void QPelvisRoughRegistrationsEditor::InitializeMitkMultiWidgetOnCollectModel()
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

		(*item)->SetVisibility(false);
		for (auto& visibilityNodeName : listVisibilityNodes)
		{
			// TODO: Debug
			if (__nodename == DEBUG_PELVIS_SURFACE_LEFT)
			{
				(*item)->SetProperty("opacity", mitk::FloatProperty::New(0.5));
			}

			if (visibilityNodeName.toStdString() == __nodename)
			{
				(*item)->SetVisibility(true);
				break;
			}
		}
	}

	this->OnVegaPointChange();
}

void QPelvisRoughRegistrationsEditor::InitializeMitkMultiWidgetOnVerifyModel()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	// set visible true of image point.
	QStringList listImageArray = {
	DEBUG_POSTERIOR_LANDMARK_LEFT,
	DEBUG_ANTERIOR_LANDMARK_LEFT,
	DEBUG_SUPERIOR_LANDMARK_LEFT
	};
	for (int index = 0; index < listImageArray.size(); ++index)
	{
		auto node = this->GetDataStorage()->GetNamedNode(listImageArray[index].toStdString());
		if (node)
		{
			node->SetVisibility(true);
		}
	}

	auto visiblePointSet = this->GetDataStorage()->GetNamedObject
		<mitk::PointSet>(DEBUG_TEMP_VERIFY_POINTSET);
	if (nullptr == visiblePointSet)
	{
		mitk::PointSet::Pointer tmpVisiblePointSet = mitk::PointSet::New();

		if (this->GetServiceModel()->GetRegistrationVerifyPipeline().IsNull())
		{
			this->GetServiceModel()->ConfigureRegistrationsVerifyPipeline();
		}
		auto pipeline = this->GetServiceModel()->GetRegistrationVerifyPipeline();

		for (int index = 0; index < 3; ++index)
		{
			mitk::NavigationData::Pointer data = mitk::NavigationData::New();
			data->SetPosition(this->GetServiceModel()->GetVegaPoint(index));
			data->SetDataValid(true);

			pipeline->SetInput(data);
			pipeline->UpdateFilter();
			auto translateFilter = pipeline->FindFilter(0);
			if (translateFilter.IsNotNull())
			{
				tmpVisiblePointSet->SetPoint(index, translateFilter->GetOutput()->GetPosition());
			}
		}

		mitk::DataNode::Pointer tmpNode = mitk::DataNode::New();
		tmpNode->SetVisibility(true);
		tmpNode->SetData(tmpVisiblePointSet);
		tmpNode->SetName(DEBUG_TEMP_VERIFY_POINTSET);
		tmpNode->SetProperty("pointsize", mitk::FloatProperty::New(4.0f));
		tmpNode->SetProperty("contourcolor", mitk::ColorProperty::New(1.0f, 0.0f, 0.0f));
		this->GetDataStorage()->Add(tmpNode);

		if (this->GetDataStorage()->GetNamedNode("ProbeTHA"))
		{
			this->GetDataStorage()->GetNamedNode("ProbeTHA")->SetVisibility(true);
		}

		this->updateProbeTm.start();
	}
}

void QPelvisRoughRegistrationsEditor::OnVegaPointChange()
{
	using Model = lancet::spatial_fitting::PelvicRoughRegistrationsModel::Model;
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	QStringList listImageArray = {
		DEBUG_POSTERIOR_LANDMARK_LEFT,
		DEBUG_ANTERIOR_LANDMARK_LEFT,
		DEBUG_SUPERIOR_LANDMARK_LEFT
	};
	for (int index = 0; index < listImageArray.size(); ++index)
	{
		bool isVisible = this->GetServiceModel()->GetVegaPoint(index) == mitk::Point3D();
		auto node = this->GetDataStorage()->GetNamedNode(listImageArray[index].toStdString());
		if (node)
		{
			node->SetVisibility(isVisible);
		}
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}