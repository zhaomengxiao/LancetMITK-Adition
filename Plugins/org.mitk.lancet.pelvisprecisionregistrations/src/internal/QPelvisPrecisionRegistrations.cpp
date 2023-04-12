/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include "org_mitk_lancet_pelvisprecisionregistrations_Activator.h"

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QPelvisPrecisionRegistrations.h"

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

const std::string QPelvisPrecisionRegistrations::VIEW_ID 
  = "org.mitk.views.qpelvisprecisionregistrations";

static const char* const DEBUG_REGISTRATION_POINTS = "registration_points_left";
static const char* const DEBUG_REGISTRATION_VERIFY_POINTS = "VerifyCOR_left";
static const char* const DEBUG_PELVIS_SURFACE_LEFT = "pelvis_clipped_left";

struct QPelvisPrecisionRegistrations::PrivateImp
{
	std::array<mitk::Point3D, 40> imagePointArray;
	std::array<mitk::Point3D, 40> vegaPointArray;
};

QPelvisPrecisionRegistrations::QPelvisPrecisionRegistrations()
	: imp(std::make_shared<PrivateImp>())
{
}

void QPelvisPrecisionRegistrations::SetFocus()
{
}

void QPelvisPrecisionRegistrations::on_toolCollector_fail(int)
{

}

QPelvisPrecisionRegistrations::~QPelvisPrecisionRegistrations()
{
}

void QPelvisPrecisionRegistrations::on_toolCollector_complete(mitk::NavigationData*)
{
}

void QPelvisPrecisionRegistrations::on_toolCollector_step(int, mitk::NavigationData* data)
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

void QPelvisPrecisionRegistrations::on_pushButtonCapturePelvis_Registration_clicked()
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

void QPelvisPrecisionRegistrations::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
	auto qss_file = 
		":/org.mitk.lancet.pelvisprecisionregistrations/pelvisprecisionregistrations.qss";
	this->UpdateQtPartControlStyleSheet(qss_file);
	this->InitializeTrackingToolsWidget();
	this->InitializeCollectStateForQtWidget();
}

void QPelvisPrecisionRegistrations::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QPelvisPrecisionRegistrations::DoImageProcessing()
{
}

itk::SmartPointer<QPelvisPrecisionRegistrations::PelvicRoughRegistrationsModel> 
  QPelvisPrecisionRegistrations::GetServiceModel() const
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

void QPelvisPrecisionRegistrations::Initialize()
{
}

void QPelvisPrecisionRegistrations::InitializeTrackingToolsWidget()
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

void QPelvisPrecisionRegistrations::InitializeQtWidgetEventAtOnly()
{
	connect(this->m_Controls.pushButtonCapturePelvis_Registration, SIGNAL(clicked()),
		this, SLOT(on_pushButtonCapturePelvis_Registration_clicked()));
	//connect(this->m_Controls.pushButtonClearOne_Landmark, SIGNAL(clicked()),
	//	this, SLOT(on_pushButtonClearOne_Landmark_clicked()));
}

void QPelvisPrecisionRegistrations::InitializeQtEventToService()
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
					this, &QPelvisPrecisionRegistrations::on_toolCollector_fail);
				connect(toolCollector.GetPointer(), &NavigationToolCollector::Step,
					this, &QPelvisPrecisionRegistrations::on_toolCollector_step);
				connect(toolCollector.GetPointer(), &NavigationToolCollector::Complete,
					this, &QPelvisPrecisionRegistrations::on_toolCollector_complete);
			}
		}
	}
}

void QPelvisPrecisionRegistrations::InitializeCollectStateForQtWidget()
{
	if (lancet::IDevicesAdministrationService::GetService().IsNotNull())
	{
		auto deviceService = lancet::IDevicesAdministrationService::GetService();

		if (deviceService->GetConnector().IsNotNull())
		{
			bool isWidgetEnable = true;
			isWidgetEnable &= deviceService->GetConnector()->IsStartTrackingDevice("Vega");
			isWidgetEnable &= deviceService->GetConnector()->IsStartTrackingDevice("Kuka");

			//this->m_Controls.pushButtonCapturePelvis_Landmark->setEnabled(isWidgetEnable);
			//this->m_Controls.pushButtonClearOne_Landmark->setEnabled(isWidgetEnable);
		}
	}
}

bool QPelvisPrecisionRegistrations::UpdateQtPartControlStyleSheet(const QString& qssFileName)
{
	QFile qss(qssFileName);

	if (!qss.open(QIODevice::ReadOnly))
	{
		MITK_WARN << "Not open file. see " << qssFileName;
		return false;
	}

	this->m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	return true;
}

void QPelvisPrecisionRegistrations::AppendVegaPointOnBack(const mitk::Point3D& pt)
{
	if (this->GetServiceModel().IsNotNull())
	{
		int appendIndex = this->GetServiceModel()->GetVegaICPPointVaildIndex();
		if (40 > appendIndex && appendIndex >= 0)
		{
			this->GetServiceModel()->SetVegaICPPointArray(appendIndex, pt);
			this->InitializeCollectStateForQtWidget();
		}

		// is verify ?
		if (this->GetServiceModel()->GetVegaICPPointVaildIndex() == 39)
		{
			this->VerifyImageRegistor();
		}
	}
}

void QPelvisPrecisionRegistrations::RemoveVegaPointOnBack()
{
}

void QPelvisPrecisionRegistrations::VerifyImageRegistor()
{
}

lancet::IDevicesAdministrationService* QPelvisPrecisionRegistrations::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}
