/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include "org_mitk_lancet_femurmarkerpoint_Activator.h"

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QFemurMarkerPoint.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <mitkTrackingDeviceSource.h>

#include <lancetIDevicesAdministrationService.h>

const std::string QFemurMarkerPoint::VIEW_ID = "org.mitk.views.qfemurmarkerpoint";

lancet::IDevicesAdministrationService* GetDeviceService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	auto service = context->getService<lancet::IDevicesAdministrationService>(serviceRef);

	return service;
}

void QFemurMarkerPoint::SetFocus()
{
}

void QFemurMarkerPoint::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.femurmarkerpoint/");
  QFile qss(test_dir.absoluteFilePath("femurmarkerpoint.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  MITK_WARN << "error load file "
		  << test_dir.absoluteFilePath("femurmarkerpoint.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  this->InitializeTrackingToolsWidget();
}

void QFemurMarkerPoint::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  
}

void QFemurMarkerPoint::DoImageProcessing()
{
  
}
void QFemurMarkerPoint::InitializeTrackingToolsWidget()
{
	using TrackingTools = lancet::DeviceTrackingWidget::Tools;
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VProbe);
	this->m_Controls.widgetTrackingTools->InitializeTrackingToolVisible(TrackingTools::VPelvis);

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