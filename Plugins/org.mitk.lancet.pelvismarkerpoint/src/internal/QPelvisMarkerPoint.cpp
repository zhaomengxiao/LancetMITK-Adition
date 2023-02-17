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
#include <mitkNavigationData.h>

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <lancetSpatialFittingAbstractService.h>

#include <core/lancetSpatialFittingPipelineManager.h>
//#include <core/lancetSpatialFittingPointAccuracyDate.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <internal/lancetSpatialFittingPelvisCheckPointModel.h>

#include <lancetIDevicesAdministrationService.h>
#include "org_mitk_lancet_pelvismarkerpoint_Activator.h"

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

void QPelvisMarkerPoint::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.pelvismarkerpoint/");
  QFile qss(test_dir.absoluteFilePath("pelvismarkerpoint.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("pelvismarkerpoint.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  this->ConnectQtWidgets();
  this->UpdateUiForService();
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

itk::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel> QPelvisMarkerPoint::GetServiceModel() const
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::SpatialFittingAbstractService>();
	auto service = context->getService<lancet::SpatialFittingAbstractService>(serviceRef);
	if (service)
	{
		return service->GetPelvisCheckPointModel();
	}

	return lancet::spatial_fitting::PelvisCheckPointModel::Pointer();
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

		// Upload service.
		this->GetServiceModel()->SetCheckPoint(data);
		MITK_INFO << "collect point " << data->GetPosition();
	}
}

void QPelvisMarkerPoint::on_toolCollector_step(int step, mitk::NavigationData*)
{
  this->m_Controls.progressBarPelvisCheckpoint->setValue(step);
}

void QPelvisMarkerPoint::on_pushButtonPelvisCheckpoint_clicked()
{
	MITK_DEBUG << "log.pushButtonAddPoint.clicked";

	if (this->GetServiceModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
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
