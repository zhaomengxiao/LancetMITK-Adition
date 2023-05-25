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
#include <mitkImage.h>
#include <mitkTrackingDeviceSource.h>
#include "QRoboticsRegistrationsAccuracy.h"

// Qt
#include <QDir>
#include <QMessageBox>

#include <lancetSpatialFittingAbstractService.h>

#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingPointAccuracyDate.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <internal/moduls/lancetSpatialFittingProbeCheckPointModel.h>

#include <lancetIDevicesAdministrationService.h>

#include "org_mitk_lancet_roboticsregistrationsaccuracy_Activator.h"

const std::string QRoboticsRegistrationsAccuracy::VIEW_ID = "org.mitk.views.qroboticsregistrationsaccuracy";

using NavigationToolCollector = lancet::spatial_fitting::NavigationToolCollector;

lancet::IDevicesAdministrationService* GetDeviceService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	auto service = context->getService<lancet::IDevicesAdministrationService>(serviceRef);

	return service;
}

void QRoboticsRegistrationsAccuracy::SetFocus()
{
}

void QRoboticsRegistrationsAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent); 
  auto test_dir = QDir(":/org.mitk.lancet.roboticsregistrationsaccuracy/");
  QFile qss(test_dir.absoluteFilePath("roboticsregistrationsaccuracy.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  MITK_WARN << ":" << "error load file "
		  << test_dir.absoluteFilePath("roboticsregistrationsaccuracy.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  this->ConnectToQtWidget();
  this->UpdateUiForService();
  this->InitializeTrackingToolsWidget();
}

void QRoboticsRegistrationsAccuracy::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QRoboticsRegistrationsAccuracy::DoImageProcessing()
{
  
}

void QRoboticsRegistrationsAccuracy::ConnectToQtWidget()
{
  connect(this->m_Controls.pushButtonProbeCheckPoint, SIGNAL(clicked()),
   this, SLOT(on_pushButtonProbeCheckPoint_clicked()));

	using namespace lancet::spatial_fitting;
	if (this->GetServiceModel().IsNull())
	{
		MITK_WARN << "Input ProbeCheckPointModel is nullptr, Ignore this request!";
		return;
	}
	// Collector
  // Pull the server's data resources.
	PipelineManager::Pointer pipelineManager =
		this->GetServiceModel()->GetVerifyPipeline();
	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("ProbeRF2RobotEndRF_ToolCollector").GetPointer());
  if (toolCollector.IsNotNull())
	{
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Fail,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_fail);
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Step,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_step);
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Complete,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_complete);
	}
}

void QRoboticsRegistrationsAccuracy::UpdateUiForService()
{
   if (this->GetServiceModel().IsNull())
	 {
	 	MITK_INFO << "Failed to request service resources spatial fitting, ignoring this processing";
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

	// Update the activation status of the [Probe check point] button.
	this->m_Controls.pushButtonProbeCheckPoint->setEnabled(notTrackingDeviceTips.isEmpty());
}

void QRoboticsRegistrationsAccuracy::InitializeTrackingToolsWidget()
{
	using TrackingTools = lancet::DeviceTrackingWidget::Tools;
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VProbe);
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VRobotEndRF);

	lancet::IDevicesAdministrationService* sender = GetDeviceService();
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

berry::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel> 
QRoboticsRegistrationsAccuracy::GetServiceModel() const
{
	using namespace lancet::spatial_fitting;
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::AbstractService>();
	auto service = context->getService<lancet::AbstractService>(serviceRef);

	if (service)
	{
		return service->GetModel(ModulsFactor::Items::PROBE_REGISTER_MODEL)
			.Cast<ProbeCheckPointModel>();
	}

	return lancet::spatial_fitting::ProbeCheckPointModel::Pointer(nullptr);
}

void QRoboticsRegistrationsAccuracy::on_toolCollector_fail(int step)
{
  this->m_Controls.progressBarProbeCheckpoint->setValue(0);
}

void QRoboticsRegistrationsAccuracy::on_toolCollector_complete(mitk::NavigationData* data)
{
	MITK_DEBUG << "log.parame.data " << data;
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
		MITK_WARN << "Input ProbeCheckPointModel is nullptr, Ignore this request!";
		return;
	}

	if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonProbeCheckPoint_clicked")
	{
		mitk::NavigationData* verifyEndMarker = this->GetServiceModel()->GetVerifyEndMarkerData();
		if (nullptr == verifyEndMarker)
		{
			MITK_WARN << "Not found verify marker data for ndi device.";
			return;
		}

		lancet::spatial_fitting::PointAccuracyDate ptAccuracy;
		ptAccuracy.SetSourcePoint(data->GetPosition());
		ptAccuracy.SetTargetPoint(verifyEndMarker->GetPosition());
		this->m_Controls.lcdNumberProbeCheckPoint->display(ptAccuracy.Compute());

		// update check box state.
		this->m_Controls.checkBoxProbeCheckPoint->setChecked(data->IsDataValid());

		// Upload service.
		mitk::NavigationData::Pointer property_data = mitk::NavigationData::New();
		property_data->Graft(data);

		mitk::GenericProperty<mitk::NavigationData::Pointer>::Pointer property =
			mitk::GenericProperty<mitk::NavigationData::Pointer>::New(property_data);

		this->GetServiceModel()->SetProperty("PROBE_REGISTER_POINT", property);
	}
}

void QRoboticsRegistrationsAccuracy::on_toolCollector_step(int step, mitk::NavigationData*)
{
  this->m_Controls.progressBarProbeCheckpoint->setValue(step);
}

void QRoboticsRegistrationsAccuracy::on_pushButtonProbeCheckPoint_clicked()
{
	MITK_DEBUG << "log.pushButtonProbeCheckPoint.clicked";

	if (this->GetServiceModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
		return;
	}
	using namespace lancet::spatial_fitting;
	
	// Pull the server's data resources.
	PipelineManager::Pointer pipelineManager = this->GetServiceModel()->GetVerifyPipeline();

	pipelineManager->UpdateFilter();

	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("ProbeRF2RobotEndRF_ToolCollector").GetPointer());

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
