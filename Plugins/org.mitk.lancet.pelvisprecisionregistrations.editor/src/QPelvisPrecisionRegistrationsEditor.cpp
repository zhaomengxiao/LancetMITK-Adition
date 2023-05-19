#include "QPelvisPrecisionRegistrationsEditor.h"
#include "ui_QPelvisPrecisionRegistrationsEditor.h"
#include "internal/org_mitk_lancet_pelvisprecisionregistrations_editor_Activator.h"

// Include header files for ctk.
#include <ctkServiceTracker.h>
#include <mitkIDataStorageService.h>

// Include header files for berry.
#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>

// Include header files for mitk.
#include <mitkILinkedRenderWindowPart.h>

// mitk qt widgets module
#include <QmitkInteractionSchemeToolBar.h>
#include <QmitkLevelWindowWidget.h>
#include <QmitkRenderWindowWidget.h>
#include <QmitkStdMultiWidget.h>

// Include header files for lancet device.
#include <lancetIDevicesAdministrationService.h>

// Include header file for lancet tracking data.
#include <lancetSpatialFittingAbstractService.h>
#include <lancetNavigationObjectVisualizationFilter.h>
#include <internal/lancetSpatialFittingPelvicRegistrationsModel.h>

#include <mitkDataStorageEditorInput.h>

static const char* const DEBUG_REGISTRATION_POINTS = "registration_points_left";
static const char* const DEBUG_REGISTRATION_VERIFY_POINTS = "VerifyCOR_left";
static const char* const DEBUG_PELVIS_SURFACE_LEFT = "pelvis_clipped_left";

struct QPelvisPrecisionRegistrationsEditor::PrivateImp
{
	PrivateImp() :
		prefServiceTracker(PluginActivator::GetPluginContext())
	{}

  QTimer updateProbeTm;
  QStringList listDataStorageNodes;

  QSharedPointer<QWidget> qtWidget;
  // berry::Smat
  typedef berry::IPreferencesService berryIPreferencesService;
  ctkServiceTracker<berryIPreferencesService*> prefServiceTracker;

  berry::IBerryPreferences::Pointer prefs;

	Ui::QPelvisPrecisionRegistrationsEditor m_Controls;
};
//PluginActivator
QPelvisPrecisionRegistrationsEditor::QPelvisPrecisionRegistrationsEditor()
  : berry::EditorPart()
	, imp(std::make_shared<PrivateImp>())
{
	this->imp->updateProbeTm.setInterval(100);
	connect(&this->imp->updateProbeTm, &QTimer::timeout, this, [=]()
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

QPelvisPrecisionRegistrationsEditor::~QPelvisPrecisionRegistrationsEditor()
{
	this->UnInitializeMitkMultiWidget();
	this->GetSite()->GetPage()->RemovePartListener(this);
}

void QPelvisPrecisionRegistrationsEditor::CreatePartControl(QWidget* parent)
{
	this->imp->m_Controls.setupUi(parent);

	this->InitializeMitkMultiWidget();

	this->InitializeDataStorageForService();
	this->InitializeMitkMultiWidgetOnCollectModel();
}

void QPelvisPrecisionRegistrationsEditor::SetFocus()
{
  if (this->imp->m_Controls.widget)
	{
		this->imp->m_Controls.widget->setFocus();
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
	berry::IPreferencesService* prefService = this->imp->prefServiceTracker.getService();
	if (prefService != nullptr)
	{
		return prefService->GetSystemPreferences()->Node(this->GetSite()->GetId());
	}
	return berry::IPreferences::Pointer(nullptr);
}

void QPelvisPrecisionRegistrationsEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
	if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
		throw berry::PartInitException("Invalid Input: Must be mitk::DataStorageEditorInput");

	this->SetSite(site);
	this->SetInput(input);

	this->imp->prefs = this->GetPreferences().Cast<berry::IBerryPreferences>();
	if (this->imp->prefs.IsNotNull())
	{
		//this->imp->prefs->OnChanged.AddListener(berry::MessageDelegate1<QPelvisPrecisionRegistrationsEditor, const berry::IBerryPreferences*>
		//	(this, &QPelvisPrecisionRegistrationsEditor::OnPreferencesChanged));
	}
}

berry::IPartListener::Events::Types QPelvisPrecisionRegistrationsEditor::GetPartEventTypes() const
{
	return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

int QPelvisPrecisionRegistrationsEditor::GetRegisterPointSize() const
{
	return 0;
}

void QPelvisPrecisionRegistrationsEditor::onCollectModelOfCapturePoint(int index)
{
	int registerPointSize = this->GetRegisterPointSize();
	for (int t_index = 0; t_index < registerPointSize; ++t_index)
	{
		if (t_index == index)
		{
			this->UpdateSelectedPoint(t_index, true);
			continue;
		}
		this->UpdateSelectedPoint(t_index, false, 4.0f);
	}
}

void QPelvisPrecisionRegistrationsEditor::InitializeDataStorageForService()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeDataStorageForService()
{
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

	auto defaultLayoutDesign = QmitkRenderWindowMenu::LayoutDesign::ONE_TOP_3D_BOTTOM;
	this->imp->m_Controls.widget->SetDataStorage(this->GetDataStorage().GetPointer());
	this->imp->m_Controls.widget->InitializeMultiWidget();
	this->imp->m_Controls.widget->AddPlanesToDataStorage();
	this->imp->m_Controls.widget->GetRenderWindow4()->LayoutDesignChanged(defaultLayoutDesign);
	this->imp->m_Controls.widget->ResetCrosshair();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeMitkMultiWidget()
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeQtEventOnService()
{
	if (this->GetServiceModel().IsNull())
	{
		return;
	}

	using PelvicRegistrationsModel = lancet::spatial_fitting::PelvicRegistrationsModel;

	//connect(this->GetServiceModel().GetPointer(), &PelvicRegistrationsModel::VegaPointChange,
	//	this, &QPelvisPrecisionRegistrationsEditor::OnVegaPointChange);
	//connect(this->GetServiceModel().GetPointer(), &PelvicRegistrationsModel::RenderModelChange,
	//	this, &QPelvisPrecisionRegistrationsEditor::OnRenderModelChange);
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeQtEventOnService()
{
}

void QPelvisPrecisionRegistrationsEditor::BackupDataStorageNodes(const itk::SmartPointer<mitk::DataStorage>&)
{
	if (this->GetDataStorage().IsNull())
	{
		return;
	}

	// Clear the data node snapshot list.
	auto dataNodes = this->GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		std::string __nodename;
		if (!(*item)->GetName(__nodename))
		{
			continue;
		}
		this->imp->listDataStorageNodes.push_back(__nodename.c_str());
	}
}

void QPelvisPrecisionRegistrationsEditor::RecoverDataStorageNodes(const itk::SmartPointer<mitk::DataStorage>& ds)
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeMitkMultiWidgetOnCollectModel()
{
	mitk::DataStorage::Pointer ds = this->GetDataStorage();
	if (ds.IsNull())
	{
		MITK_WARN << "*Data storage is nullptr!";
	}

	auto registerPointSet = ds->GetNamedObject<mitk::PointSet>(DEBUG_REGISTRATION_POINTS);
	if (registerPointSet)
	{
		for (int index = 0; index < registerPointSet->GetSize(); ++index)
		{
			std::string pointName = QString("register_temp_point_%1")
				.arg(index, 2, 10, QChar('0')).toStdString();

			MITK_INFO << "pointName.value " << pointName;

			// Security check: Does the node data that generates the target already 
			// exist in the DS? If so, delete it.
			auto allNodes = ds->GetAll();
			for (auto item = allNodes->begin(); item != allNodes->end(); ++item)
			{
				std::string __nodename;
				if (!(*item)->GetName(__nodename))
				{
					continue;
				}
				if (__nodename == pointName)
				{
					ds->Remove(*item);
					break;
				}
			}

			// Insert newly generated registration point data node.
			mitk::PointSet::Pointer tmpRegisterPointSet = mitk::PointSet::New();
			tmpRegisterPointSet->SetPoint(0, registerPointSet->GetPoint(index));
			mitk::DataNode::Pointer tmpNode = mitk::DataNode::New();
			tmpNode->SetVisibility(true);
			tmpNode->SetData(tmpRegisterPointSet);
			tmpNode->SetName(pointName);
			tmpNode->SetProperty("pointsize", mitk::FloatProperty::New(4.0f));
			tmpNode->SetProperty("color", mitk::ColorProperty::New(1.0f, 1.0f, 1.0f));
			tmpNode->SetProperty("selectedcolor", mitk::ColorProperty::New(0.0f, 0.0f, 1.0f));
			tmpNode->SetProperty("contourcolor", mitk::ColorProperty::New(1.0f, 0.0f, 0.0f));
			this->GetDataStorage()->Add(tmpNode);
		}

		this->UpdateSelectedPoint(0, true);

		mitk::DataNode::Pointer peviseNode = ds->GetNamedNode(DEBUG_PELVIS_SURFACE_LEFT);
		if (peviseNode.IsNotNull())
		{
			peviseNode->SetVisibility(true);
		}
	}
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeMitkMultiWidgetOnCollectModel()
{
}

void QPelvisPrecisionRegistrationsEditor::InitializeMitkMultiWidgetOnVerifyModel()
{
}

void QPelvisPrecisionRegistrationsEditor::UnInitializeMitkMultiWidgetOnVerifyModel()
{
}

void QPelvisPrecisionRegistrationsEditor::UpdateSelectedPoint(int index, bool selected, float size)
{
	mitk::DataStorage::Pointer ds = this->GetDataStorage();
	if (ds.IsNull())
	{
		MITK_WARN << "*Data storage is nullptr!";
	}

	std::string pointName = QString("register_temp_point_%1")
		.arg(index, 2, 10, QChar('0')).toStdString();

	mitk::PointSet::Pointer readyPointNode = ds->GetNamedObject<mitk::PointSet>(pointName);

	if (readyPointNode.IsNotNull())
	{
		readyPointNode->SetSelectInfo(0, selected);
		readyPointNode->SetProperty("pointsize", mitk::FloatProperty::New(size));
	}
}

lancet::IDevicesAdministrationService*
  QPelvisPrecisionRegistrationsEditor::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

itk::SmartPointer<lancet::spatial_fitting::PelvicRegistrationsModel> 
  QPelvisPrecisionRegistrationsEditor::GetServiceModel()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::SpatialFittingAbstractService>();
	auto service = context->getService<lancet::SpatialFittingAbstractService>(serviceRef);
	if (service)
	{
		return service->GetPelvicRegistrationsModel();
	}

	return lancet::spatial_fitting::PelvicRegistrationsModel::Pointer();
}

itk::SmartPointer<mitk::DataStorage> QPelvisPrecisionRegistrationsEditor::GetDataStorage()
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
