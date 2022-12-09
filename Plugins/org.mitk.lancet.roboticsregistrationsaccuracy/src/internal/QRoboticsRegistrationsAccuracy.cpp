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
#include "QRoboticsRegistrationsAccuracy.h"

// Qt
#include <QDir>
#include <QMessageBox>

#include <core/lancetSpatialFittingPointAccuracyDate.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>

const std::string QRoboticsRegistrationsAccuracy::VIEW_ID = "org.mitk.views.qroboticsregistrationsaccuracy";

using NavigationToolCollector = lancet::spatial_fitting::NavigationToolCollector;

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
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("roboticsregistrationsaccuracy.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();

  this->ConnectToQtWidget();
  this->UpdateUiForService();
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

  //if (toolCollector.IsNotNull())
	{
		connect(nullptr, &NavigationToolCollector::Fail,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_fail);
		connect(nullptr, &NavigationToolCollector::Step,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_step);
		connect(nullptr, &NavigationToolCollector::Complete,
			this, &QRoboticsRegistrationsAccuracy::on_toolCollector_complete);
	}
}

void QRoboticsRegistrationsAccuracy::UpdateUiForService()
{
  // if (this->GetServiceRoboticsModel().IsNull())
	// {
	// 	MITK_INFO << "Failed to request service resources spatial fitting, ignoring this processing";
	// 	return;
	// }

	// if (nullptr == GetDeviceService())
	// {
	// 	MITK_WARN << "Failed to request service resources of device, ignoring this processing";
	// 	return;
	// }
	
	//bool isTrackingNDIDevice = GetDeviceService()->GetConnector()->IsTrackingDeviceConnected("Vega");
	//bool isTrackingRoboticsDevice = GetDeviceService()->GetConnector()->IsTrackingDeviceConnected("Kuka");

  bool isTrackingNDIDevice = false;
  bool isTrackingRoboticsDevice = false;

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

void QRoboticsRegistrationsAccuracy::on_toolCollector_fail(int step)
{
  this->m_Controls.progressBarProbeCheckpoint->setValue(0);
}

void QRoboticsRegistrationsAccuracy::on_toolCollector_complete(mitk::NavigationData* data)
{
	lancet::spatial_fitting::PointAccuracyDate ptAccuracy;

	ptAccuracy.SetSourcePoint(data->GetPosition());
  this->m_Controls.lcdNumberProbeCheckPoint->display(ptAccuracy.Compute());
}

void QRoboticsRegistrationsAccuracy::on_toolCollector_step(int step, mitk::NavigationData*)
{
  this->m_Controls.progressBarProbeCheckpoint->setValue(step);
}

void QRoboticsRegistrationsAccuracy::on_pushButtonProbeCheckPoint_clicked()
{
  MITK_INFO << "log";
}
