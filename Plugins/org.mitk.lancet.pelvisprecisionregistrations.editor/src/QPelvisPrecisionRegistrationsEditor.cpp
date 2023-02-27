/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QPelvisPrecisionRegistrationsEditor.h"
#include "internal/org_mitk_lancet_pelvisprecisionregistrations_editor_Activator.h"

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
#include <lancetSpatialFittingAbstractService.h>
#include <internal/lancetTrackingDeviceManage.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <lancetNavigationObjectVisualizationFilter.h>
#include <internal/lancetSpatialFittingPelvicRoughRegistrationsModel.h>

const QString QPelvisPrecisionRegistrationsEditor::EDITOR_ID = 
  "org.mitk.lancet.pelvisprecisionregistrations.editor";

static const char* const DEBUG_REGISTRATION_POINTS = "registration_points_left";
static const char* const DEBUG_REGISTRATION_VERIFY_POINTS = "VerifyCOR_left";
static const char* const DEBUG_PELVIS_SURFACE_LEFT = "pelvis_clipped_left";

//////////////////////////////////////////////////////////////////////////
// QPelvisPrecisionRegistrationsEditor
//////////////////////////////////////////////////////////////////////////
QPelvisPrecisionRegistrationsEditor::QPelvisPrecisionRegistrationsEditor()
  : berry::EditorPart()
	, widgetInstace(nullptr)
	, m_Prefs(nullptr)
	, m_PrefServiceTracker(org_mitk_lancet_pelvisprecisionregistrations_editor_Activator::GetPluginContext())
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

	this->InitializeWidgetOnlyOne();
}

QPelvisPrecisionRegistrationsEditor::~QPelvisPrecisionRegistrationsEditor()
{
	this->UnInitializeMitkMultiWidget();
	GetSite()->GetPage()->RemovePartListener(this);
}

void QPelvisPrecisionRegistrationsEditor::CreatePartControl(QWidget* parent)
{
	m_Controls.setupUi(parent);
	this->InitializeMitkMultiWidget();
	this->InitializeQtEventOnService();
}

void QPelvisPrecisionRegistrationsEditor::SetFocus()
{
  if (widgetInstace.isNull() == false)
	{
		widgetInstace->setFocus();
	}
}

void QPelvisPrecisionRegistrationsEditor::DoSave()
{
	
}

void QPelvisPrecisionRegistrationsEditor::DoSaveAs()
{
	
}

bool QPelvisPrecisionRegistrationsEditor::IsSaveOnCloseNeeded() const
{
	return true;
}

bool QPelvisPrecisionRegistrationsEditor::IsDirty() const
{
	return true;
}

bool QPelvisPrecisionRegistrationsEditor::IsSaveAsAllowed() const
{
	return true;
}

berry::IPreferences::Pointer QPelvisPrecisionRegistrationsEditor::GetPreferences() const
{
	berry::IPreferencesService* prefService = m_PrefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QPelvisPrecisionRegistrationsEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	m_Prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (m_Prefs.IsNotNull())
	{
		m_Prefs->OnChanged.AddListener(berry::MessageDelegate1<QPelvisPrecisionRegistrationsEditor, const berry::IBerryPreferences*>
			(this, &QPelvisPrecisionRegistrationsEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QPelvisPrecisionRegistrationsEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QPelvisPrecisionRegistrationsEditor::InitializeDataStorageForService()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	int icpPointIndex = this->GetServiceModel()->GetVegaICPPointVaildIndex();
	QString nextNodeName = QString("%1_%2").arg(DEBUG_REGISTRATION_VERIFY_POINTS)
		.arg(QString::number((-1 == icpPointIndex ? 0 : icpPointIndex)));

	for (int index = 0; index < 40; ++index)
	{
		QString nodeName = QString("%1_%2").arg(DEBUG_REGISTRATION_VERIFY_POINTS)
			.arg(QString::number(index));

		auto node = this->GetDataStorage()->GetNamedNode(nodeName.toStdString());
		if (node)
		{
			if ((index >= icpPointIndex))
			{
				node->SetVisibility(true);
				node->SetProperty("pointsize", mitk::FloatProperty::New(4.0f));
				node->SetColor(1.0f, 1.0f, 1.0f);
			}
			else
			{
				node->SetVisibility(false);
			}
		}
	}

	auto nextNode = this->GetDataStorage()->GetNamedNode(nextNodeName.toStdString());
	if (nextNode)
	{
		nextNode->SetVisibility(true);
		nextNode->SetProperty("pointsize", mitk::FloatProperty::New(5.0f));
		nextNode->SetColor(0.0f, 0.0f, 1.0f);
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void QPelvisPrecisionRegistrationsEditor::InitializeMitkMultiWidget()
{
	if (this->GetDataStorage().IsNotNull())
	{
		auto dataNodes = this->GetDataStorage()->GetAll();
		for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
		{
			(*item)->SetVisibility(false);
			std::string __nodename;
			if (!(*item)->GetName(__nodename))
			{
				continue;
			}
		}
	}

	auto dataNodeSurface = this->GetDataStorage()->GetNamedNode(DEBUG_PELVIS_SURFACE_LEFT);
	if (dataNodeSurface)
	{
		dataNodeSurface->SetVisibility(true);
	}

	auto defaultLayoutDesign = QmitkRenderWindowMenu::LayoutDesign::ONE_TOP_3D_BOTTOM;
	m_Controls.widget->SetDataStorage(this->GetDataStorage().GetPointer());
	m_Controls.widget->InitializeMultiWidget();
	m_Controls.widget->AddPlanesToDataStorage();
	m_Controls.widget->GetRenderWindow4()->LayoutDesignChanged(defaultLayoutDesign);
	m_Controls.widget->ResetCrosshair();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	//this->OnRenderModelChange();
	this->InitializeDataStorageForService();
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeMitkMultiWidget()
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
}

void QPelvisPrecisionRegistrationsEditor::InitializeQtEventOnService()
{
	if (this->GetServiceModel().IsNull())
	{
		return;
	}

	//connect(this->GetServiceModel().GetPointer(), &PelvicRoughRegistrationsModel::VegaPointChange,
	//	this, &QPelvisRoughRegistrationsEditor::OnVegaPointChange);
	//connect(this->GetServiceModel().GetPointer(), &PelvicRoughRegistrationsModel::RenderModelChange,
	//	this, &QPelvisRoughRegistrationsEditor::OnRenderModelChange);
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeQtEventOnService()
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeMitkMultiWidgetOnCollectModel()
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeMitkMultiWidgetOnVerifyModel()
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeWidgetOnlyOne()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	auto node = this->GetDataStorage()->GetNamedNode(DEBUG_REGISTRATION_POINTS);
	if (node)
	{
		auto tmpPointSet = dynamic_cast<mitk::PointSet*>(node->GetData());
		if (tmpPointSet)
		{
			for (int index = 0; index < tmpPointSet->GetSize(); ++index)
			{
				MITK_INFO << "image point " << tmpPointSet->GetPoint(index);
				this->GetServiceModel()->SetImageICPPointArray(index, tmpPointSet->GetPoint(index));
			}
		}
	}

	// Init verify points
	auto dataNodeRegisterPointSet = this->GetDataStorage()->GetNamedObject
		<mitk::PointSet>(DEBUG_REGISTRATION_POINTS);
	if (dataNodeRegisterPointSet)
	{
		for (int index = 0; index < dataNodeRegisterPointSet->GetSize(); ++index)
		{
			QString nodeName = QString("%1_%2").arg(DEBUG_REGISTRATION_VERIFY_POINTS)
				.arg(QString::number(index));
			mitk::PointSet::Pointer pointset = mitk::PointSet::New();
			pointset->SetPoint(0, dataNodeRegisterPointSet->GetPoint(index));

			mitk::DataNode::Pointer tmpNode = mitk::DataNode::New();
			tmpNode->SetVisibility(true);
			tmpNode->SetData(pointset);
			tmpNode->SetName(nodeName.toStdString());
			tmpNode->SetColor(1.0f, 1.0f, 1.0f);
			tmpNode->SetProperty("pointsize", mitk::FloatProperty::New(4.0f));
			this->GetDataStorage()->Add(tmpNode);
		}

		QString nodeName = QString("%1_%2").arg(DEBUG_REGISTRATION_VERIFY_POINTS)
			.arg(QString::number(0));

		auto dataNodeRegisterPointSetShow = this->GetDataStorage()->GetNamedNode(nodeName.toStdString());
		if (dataNodeRegisterPointSetShow)
		{
			dataNodeRegisterPointSetShow->SetColor(0.0f, 0.0f, 1.0f);
		}
	}
}

itk::SmartPointer<mitk::DataStorage> QPelvisPrecisionRegistrationsEditor::GetDataStorage() const
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

itk::SmartPointer<QPelvisPrecisionRegistrationsEditor::PelvicRoughRegistrationsModel> 
  QPelvisPrecisionRegistrationsEditor::GetServiceModel() const
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

lancet::IDevicesAdministrationService* QPelvisPrecisionRegistrationsEditor::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void QPelvisPrecisionRegistrationsEditor::OnPreferencesChanged(const berry::IBerryPreferences*)
{
}
