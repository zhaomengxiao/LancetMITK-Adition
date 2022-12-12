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

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QPelvisMarkerPoint::VIEW_ID = "org.mitk.views.qpelvismarkerpoint";

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
}

void QPelvisMarkerPoint::UpdateUiForService()
{
  // if (this->GetServiceRoboticsModel().IsNull())
	// {
	// 	MITK_WARN << "Failed to request service resources spatial fitting, ignoring this processing";
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

	// Update the activation status of the [Pelvis check point] button.
	this->m_Controls.pushButtonPelvisCheckpoint->setEnabled(notTrackingDeviceTips.isEmpty());
}

void QPelvisMarkerPoint::on_toolCollector_fail(int)
{
  this->m_Controls.progressBarPelvisCheckpoint->setValue(0);
}

void QPelvisMarkerPoint::on_toolCollector_complete(mitk::NavigationData* data)
{
  if(nullptr == data || false == data->IsDataVaild())
  {
    MITK_ERROR << "Collected data error. Input data is nullptr or invaild!";
    this->m_Controls.checkBoxPelvisCheckpoint->setChecked(data->IsDataVaild());
    return;
  }

  // update ui for widget
  this->m_Controls.checkBoxPelvisCheckpoint->setChecked(data->IsDataVaild());

  // Upload service.

}

void QPelvisMarkerPoint::on_toolCollector_step(int step, mitk::NavigationData*)
{
  this->m_Controls.progressBarPelvisCheckpoint->setValue(step);
}

void QPelvisMarkerPoint::on_pushButtonPelvisCheckpoint_clicked()
{
  MITK_INFO << "log";
}
