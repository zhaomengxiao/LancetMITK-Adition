/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QPelvisMarkerPoint.h"
#include "org_mitk_lancet_pelvismarkerpoint_Activator.h"

#include <mitkNavigationData.h>

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <mitkTrackingDeviceSource.h>

#include <lancetSpatialFittingAbstractService.h>

#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <internal/moduls/lancetSpatialFittingPelvisCheckPointModel.h>

#include <lancetIDevicesAdministrationService.h>
#include <internal/lancetTrackingDeviceManage.h>

const std::string QPelvisMarkerPoint::VIEW_ID = "org.mitk.views.qpelvismarkerpoint";

lancet::IDevicesAdministrationService* GetDeviceService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	auto service = context->getService<lancet::IDevicesAdministrationService>(serviceRef);

	return service;
}

void QPelvisMarkerPoint::SetFocus()
{
}

QPelvisMarkerPoint::QPelvisMarkerPoint()
{
	if (this->GetServiceModel().IsNotNull())
	{
		this->GetServiceModel()->StartWorking();
	}
}

QPelvisMarkerPoint::~QPelvisMarkerPoint()
{
	if (this->GetServiceModel().IsNotNull())
	{
		this->GetServiceModel()->StopWorking();
	}
}

void QPelvisMarkerPoint::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.pelvismarkerpoint/");
  QFile qss(test_dir.absoluteFilePath("pelvismarkerpoint.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  MITK_WARN << "error load file "
		  << test_dir.absoluteFilePath("pelvismarkerpoint.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  this->ConnectQtWidgets();
  this->UpdateUiForService();
  this->InitializeTrackingToolsWidget();
}

void QPelvisMarkerPoint::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QPelvisMarkerPoint::DoImageProcessing()
{
 
}

void QPelvisMarkerPoint::ConnectQtWidgets()
{
	connect(this->m_Controls.pushButtonPelvisCheckpoint, &QPushButton::clicked,
		this, &QPelvisMarkerPoint::on_pushButtonPelvisCheckpoint_clicked);


	if (this->GetServiceModel().IsNull())
	{
		return;
	}
	using namespace lancet::spatial_fitting;
	// Collector
	// Pull the server's data resources.
	PipelineManager::Pointer pipelineManager =
		this->GetServiceModel()->GetCheckPointPipeline();
	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("ProbeRF2PelvisRF_ToolCollector").GetPointer());

	if (toolCollector.IsNotNull())
	{
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Fail,
			this, &QPelvisMarkerPoint::on_toolCollector_fail);
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Step,
			this, &QPelvisMarkerPoint::on_toolCollector_step);
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Complete,
			this, &QPelvisMarkerPoint::on_toolCollector_complete);
	}
}

void QPelvisMarkerPoint::UpdateUiForService()
{
   if (this->GetServiceModel().IsNull())
	 {
	 	MITK_WARN << "Failed to request service resources spatial fitting, ignoring this processing";
	 	return;
	 }

	 if (nullptr == GetDeviceService())
	 {
	 	MITK_WARN << "Failed to request service resources of device, ignoring this processing";
	 	return;
	 }
	
	bool isTrackingNDIDevice = GetDeviceService()->GetConnector()->IsTrackingDeviceConnected("Vega");
	bool isTrackingRoboticsDevice = GetDeviceService()->GetConnector()->IsTrackingDeviceConnected("Kuka");

	QString notTrackingDeviceTips = "";

	if (!isTrackingNDIDevice || !isTrackingRoboticsDevice)
	{
		if (false == isTrackingNDIDevice)
		{
			notTrackingDeviceTips += "Vega";
		}
		if (false == isTrackingRoboticsDevice)
		{
			if (false == notTrackingDeviceTips.isEmpty())
			{
				notTrackingDeviceTips += ", ";
			}
			notTrackingDeviceTips += "Kuka";
		}
		this->m_Controls.labelTips->setText(QString("Device [%1] not tracking!")
			.arg(notTrackingDeviceTips));
	}

	// Update the activation status of the [Pelvis check point] button.
	this->m_Controls.pushButtonPelvisCheckpoint->setEnabled(notTrackingDeviceTips.isEmpty());
}

berry::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel>
  QPelvisMarkerPoint::GetServiceModel() const
{
	using namespace lancet::spatial_fitting;
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::AbstractService>();
	auto service = context->getService<lancet::AbstractService>(serviceRef);
	if (service)
	{
		return service->GetModel(ModulsFactor::Items::PELVIS_CHECKPOINT_MODEL).
			Cast<PelvisCheckPointModel>();
	}

	return PelvisCheckPointModel::Pointer();
}

lancet::IDevicesAdministrationService* QPelvisMarkerPoint::GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void QPelvisMarkerPoint::on_toolCollector_fail(int)
{
  this->m_Controls.progressBarPelvisCheckpoint->setValue(0);
}

void QPelvisMarkerPoint::on_toolCollector_complete(mitk::NavigationData* data)
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
	if (this->GetServiceModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
		return;
	}
	
	if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonPelvisCheckpoint_clicked")
	{
		// update ui for widget
		this->m_Controls.checkBoxPelvisCheckpoint->setChecked(data->IsDataValid());

		mitk::NavigationData::Pointer collectNavigationData = mitk::NavigationData::New();
		collectNavigationData->Graft(data);

		// Upload service.
		this->GetServiceModel()->SetProperty("PelvisMarkerPoint", 
			mitk::GenericProperty<mitk::NavigationData::Pointer>::New(collectNavigationData));
		MITK_INFO << "collect point " << data->GetPosition();
	}
}

void QPelvisMarkerPoint::on_toolCollector_step(int step, mitk::NavigationData*)
{
  this->m_Controls.progressBarPelvisCheckpoint->setValue(step);
}

void QPelvisMarkerPoint::InitializeTrackingToolsWidget()
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

void QPelvisMarkerPoint::on_pushButtonPelvisCheckpoint_clicked()
{
	MITK_DEBUG << "log.pushButtonAddPoint.clicked";

	if (this->GetServiceModel().IsNull())
	{
		MITK_WARN << "Input Pelvis check point model is nullptr, Ignore this request!";
		return;
	}
	using namespace lancet::spatial_fitting;

	// Pull the server's data resources.
	PipelineManager::Pointer pipelineManager =
		this->GetServiceModel()->GetCheckPointPipeline();

	pipelineManager->UpdateFilter();

	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("ProbeRF2PelvisRF_ToolCollector").GetPointer());

	if (toolCollector.IsNull())
	{
		MITK_WARN << "Failed to pull the tool collector, Ignore this request!";
		return;
	}

	// When the tool collector is working, force the current request as the 
	// highest level of permission.
	if (toolCollector->IsRunning())
	{
		toolCollector->Stop();
	}

	toolCollector->SetPermissionIdentificationArea(__func__);
	toolCollector->Start();
}
