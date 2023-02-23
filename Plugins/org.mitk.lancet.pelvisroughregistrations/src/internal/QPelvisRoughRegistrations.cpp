/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include "org_mitk_lancet_pelvisroughregistrations_Activator.h"

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QPelvisRoughRegistrations.h"

// Qt
#include <QMessageBox>
#include <QDir>

// mitk image
#include <mitkImage.h>
#include <mitkTrackingDeviceSource.h>

#include <internal/lancetTrackingDeviceManage.h>
#include <lancetSpatialFittingAbstractService.h>
#include <lancetIDevicesAdministrationService.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <internal/lancetSpatialFittingPelvicRoughRegistrationsModel.h>

struct QPelvisRoughRegistrations::QPelvisRoughRegistrationsPrivateImp
{
	std::array<mitk::Point3D, 3> imagePointArray;
	std::array<mitk::Point3D, 3> vegaPointArray;
};

const std::string QPelvisRoughRegistrations::VIEW_ID = "org.mitk.views.qpelvisroughregistrations";

void QPelvisRoughRegistrations::SetFocus()
{
}

QPelvisRoughRegistrations::QPelvisRoughRegistrations()
	: imp(std::make_shared<QPelvisRoughRegistrationsPrivateImp>())
{
	if (this->GetServiceModel().IsNotNull())
	{
		this->GetServiceModel()->Start();
	}
}

QPelvisRoughRegistrations::~QPelvisRoughRegistrations()
{
	if (this->GetServiceModel().IsNotNull())
	{
		this->GetServiceModel()->Stop();
	}
}

void QPelvisRoughRegistrations::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.pelvisroughregistrations/");
  QFile qss(test_dir.absoluteFilePath("pelvisroughregistrations.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  MITK_WARN << "error load file "
		  << test_dir.absoluteFilePath("pelvisroughregistrations.qss") << "\n"
		  << "error: " << qss.errorString();
  }

  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();
	this->Initialize();
	this->InitializeQtWidgetEventAtOnly();
}

void QPelvisRoughRegistrations::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QPelvisRoughRegistrations::DoImageProcessing()
{
}

void QPelvisRoughRegistrations::on_toolCollector_fail(int step)
{
	MITK_INFO << "step.value " << step;
}

void QPelvisRoughRegistrations::on_toolCollector_complete(mitk::NavigationData* data)
{
	using namespace lancet::spatial_fitting;

	NavigationToolCollector* toolCollector
		= dynamic_cast<NavigationToolCollector*>(sender());

	if (nullptr == toolCollector || nullptr == data)
	{
		// network security: The data flow is abnormal and may be attacked by intruders.
		MITK_WARN << "The data transmitter is abnormal. This request will be discarded!";
		return;
	}

	if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonCapturePelvis_Landmark_clicked")
	{
		this->AppendVegaPointOnBack(data->GetPosition());
	}
}

void QPelvisRoughRegistrations::on_toolCollector_step(int step, mitk::NavigationData* data)
{
	MITK_INFO << "step.value " << step << (data ? data->GetPosition() : mitk::Point3D());
}

void QPelvisRoughRegistrations::on_pushButtonCapturePelvis_Landmark_clicked()
{
	if (this->GetServiceModel().IsNotNull())
	{
		if (this->GetServiceModel()->GetRegistrationPipeline().IsNotNull())
		{
			auto pipeline = this->GetServiceModel()->GetRegistrationPipeline();

			using namespace lancet::spatial_fitting;
			NavigationToolCollector::Pointer toolCollector =
				dynamic_cast<NavigationToolCollector*>(pipeline->
					FindFilter("ProbeRF2PelvisRF_ToolCollector").GetPointer());
			if (toolCollector.IsNotNull())
			{
				// When the tool collector is working, force the current request as the 
				// highest level of permission.
				if (toolCollector->IsRunning())
				{
					toolCollector->Stop();
				}

				toolCollector->SetPermissionIdentificationArea(__func__);
				toolCollector->Start();
			}
		}
	}
}

void QPelvisRoughRegistrations::on_pushButtonClearOne_Landmark_clicked()
{
	this->RemoveVegaPointOnBack();
}

itk::SmartPointer<QPelvisRoughRegistrations::PelvicRoughRegistrationsModel> 
  QPelvisRoughRegistrations::GetServiceModel() const
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

void QPelvisRoughRegistrations::Initialize()
{
	this->InitializeQtEventToService();
	this->InitializeTrackingToolsWidget();
	this->InitializeCollectStateForQtWidget();
}

void QPelvisRoughRegistrations::InitializeTrackingToolsWidget()
{
	using TrackingTools = lancet::DeviceTrackingWidget::Tools;
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VProbe);
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VPelvis);

	lancet::IDevicesAdministrationService* sender = this->GetDevicesService();
	if (sender && sender->GetConnector().IsNotNull())
	{
		auto vegaTrackSource = sender->GetConnector()->GetTrackingDeviceSource("Vega");
		auto kukaTrackSource = sender->GetConnector()->GetTrackingDeviceSource("Kuka");

		if (this->m_Controls.widgetTrackingTools->HasTrackingToolSource("Vega Tracking Source"))
		{
			this->m_Controls.widgetTrackingTools->RemoveTrackingToolSource("Vega Tracking Source");
		}
		if (this->m_Controls.widgetTrackingTools->HasTrackingToolSource("Kuka Robot Tracking Source"))
		{
			this->m_Controls.widgetTrackingTools->RemoveTrackingToolSource("Kuka Robot Tracking Source");
		}
		this->m_Controls.widgetTrackingTools->AddTrackingToolSource(vegaTrackSource);
		this->m_Controls.widgetTrackingTools->AddTrackingToolSource(kukaTrackSource);
	}
}

void QPelvisRoughRegistrations::InitializeQtWidgetEventAtOnly()
{
	connect(this->m_Controls.pushButtonCapturePelvis_Landmark, SIGNAL(clicked()), 
		this, SLOT(on_pushButtonCapturePelvis_Landmark_clicked()));
	connect(this->m_Controls.pushButtonClearOne_Landmark, SIGNAL(clicked()),
		this, SLOT(on_pushButtonClearOne_Landmark_clicked()));
}

void QPelvisRoughRegistrations::InitializeQtEventToService()
{
	if (this->GetServiceModel().IsNotNull())
	{
		if (this->GetServiceModel()->GetRegistrationPipeline().IsNotNull())
		{
			auto pipeline = this->GetServiceModel()->GetRegistrationPipeline();

			using namespace lancet::spatial_fitting;
			NavigationToolCollector::Pointer toolCollector =
				dynamic_cast<NavigationToolCollector*>(pipeline->
					FindFilter("ProbeRF2PelvisRF_ToolCollector").GetPointer());
			if (toolCollector.IsNotNull())
			{
				connect(toolCollector.GetPointer(), &NavigationToolCollector::Fail,
					this, &QPelvisRoughRegistrations::on_toolCollector_fail);
				connect(toolCollector.GetPointer(), &NavigationToolCollector::Step,
					this, &QPelvisRoughRegistrations::on_toolCollector_step);
				connect(toolCollector.GetPointer(), &NavigationToolCollector::Complete,
					this, &QPelvisRoughRegistrations::on_toolCollector_complete);
			}
		}
	}
}

void QPelvisRoughRegistrations::InitializeCollectStateForQtWidget()
{
	if (this->GetServiceModel().IsNotNull())
	{
		bool posteriorAcetabulumEnabled = this->GetServiceModel()->GetVegaPoint(0) != mitk::Point3D();
		this->m_Controls.radioButton_posteriorAcetabulum->setChecked(posteriorAcetabulumEnabled);

		bool anteriorAcetabulumEnabled = this->GetServiceModel()->GetVegaPoint(1) != mitk::Point3D();
		this->m_Controls.radioButton_anteriorAcetabulum->setChecked(anteriorAcetabulumEnabled);

		bool superiorAcetabulumEnabled = this->GetServiceModel()->GetVegaPoint(2) != mitk::Point3D();
		this->m_Controls.radioButton_superiorAcetabulum->setChecked(superiorAcetabulumEnabled);

		if (posteriorAcetabulumEnabled && anteriorAcetabulumEnabled && superiorAcetabulumEnabled)
		{
			// TODO: peizhun 

		}
	}	

	if (lancet::IDevicesAdministrationService::GetService().IsNotNull())
	{
		auto deviceService = lancet::IDevicesAdministrationService::GetService();

		if (deviceService->GetConnector().IsNotNull())
		{
			bool isWidgetEnable = true;
			isWidgetEnable &= deviceService->GetConnector()->IsStartTrackingDevice("Vega");
			isWidgetEnable &= deviceService->GetConnector()->IsStartTrackingDevice("Kuka");

			this->m_Controls.pushButtonCapturePelvis_Landmark->setEnabled(isWidgetEnable);
			this->m_Controls.pushButtonClearOne_Landmark->setEnabled(isWidgetEnable);
		}
	}
}

void QPelvisRoughRegistrations::AppendVegaPointOnBack(const mitk::Point3D& pt)
{
	if (this->GetServiceModel().IsNotNull())
	{
		for (int index = 0; index < 3; ++index)
		{
			if (this->GetServiceModel()->GetVegaPoint(index) == mitk::Point3D())
			{
				this->GetServiceModel()->SetVegaPointArray(index, pt);
				this->InitializeCollectStateForQtWidget();
				break;
			}
		}

		// is verify ?
		if (this->GetServiceModel()->GetVegaPointVaildIndex() == 3)
		{
			this->VerifyImageRegistor();
		}
	}
}

void QPelvisRoughRegistrations::RemoveVegaPointOnBack()
{
	if (this->GetServiceModel().IsNotNull())
	{
		for (int index = 2; index >= 0; --index)
		{
			if (this->GetServiceModel()->GetVegaPoint(index) != mitk::Point3D())
			{
				this->GetServiceModel()->SetVegaPointArray(index, mitk::Point3D());
				this->InitializeCollectStateForQtWidget();
				break;
			}
		}
	}
}

void QPelvisRoughRegistrations::VerifyImageRegistor()
{
	if (this->GetServiceModel().IsNull() || this->GetDataStorage().IsNull())
	{
		return;
	}

	mitk::PointSet::Pointer imagePointArray = mitk::PointSet::New();
	mitk::PointSet::Pointer vegaPointArray = mitk::PointSet::New();

	for (int index = 0; index < 3; ++index)
	{
		imagePointArray->SetPoint(index, this->GetServiceModel()->GetImagePoint(index));
		vegaPointArray->SetPoint(index, this->GetServiceModel()->GetVegaPoint(index));
	}
	mitk::Surface::Pointer pelvisSurface = this->GetDataStorage()->GetNamedObject<mitk::Surface>("pelvis_clipped_left");

	if (this->GetServiceModel()->ComputeLandMarkResult(imagePointArray, vegaPointArray, pelvisSurface))
	{
		this->GetServiceModel()->SetModel(PelvicRoughRegistrationsModel::Model::Verify);
	}
}

lancet::IDevicesAdministrationService* QPelvisRoughRegistrations::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}
