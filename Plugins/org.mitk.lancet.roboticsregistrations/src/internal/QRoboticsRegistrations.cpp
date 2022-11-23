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
#include "QRoboticsRegistrations.h"

// Qt
#include <QDir>
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

#include <RobotRegistration.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <core/lancetSpatialFittingNavigationToolCollector.h>
#include <internal/lancetSpatialFittingRoboticsRegisterModel.h>

const std::string QRoboticsRegistrations::VIEW_ID = "org.mitk.views.qroboticsregistrations";

using PipelineManager = lancet::spatial_fitting::PipelineManager;
using NavigationToolCollector = lancet::spatial_fitting::NavigationToolCollector;

void QRoboticsRegistrations::SetFocus()
{
}

void QRoboticsRegistrations::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  auto test_dir = QDir(":/org.mitk.lancet.roboticsregistrations/");
  QFile qss(test_dir.absoluteFilePath("roboticsregistrations.qss"));
  if (!qss.open(QIODevice::ReadOnly))
  {
	  qWarning() << __func__ << __LINE__ << ":" << "error load file "
		  << test_dir.absoluteFilePath("roboticsregistrations.qss") << "\n"
		  << "error: " << qss.errorString();
  }
  // pos
  qInfo() << "log.file.pos " << qss.pos();
  m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
  qss.close();
}

void QRoboticsRegistrations::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QRoboticsRegistrations::DoImageProcessing()
{
	
}

void QRoboticsRegistrations::UpdateWidgetOfService()
{
}

lancet::spatial_fitting::RoboticsRegisterModelPtr 
QRoboticsRegistrations::GetServiceRoboticsModel() const
{
  return lancet::spatial_fitting::RoboticsRegisterModelPtr();
}

void QRoboticsRegistrations::on_pushButtonRobotVerify_clicked()
{
  MITK_DEBUG << __FUNCTION__ << "log.pushButtonRobotVerify.clicked";
}

void QRoboticsRegistrations::on_pushButtonCalResult_clicked()
{
	MITK_DEBUG << __FUNCTION__ << "log.pushButtonCalResult.clicked";
}

void QRoboticsRegistrations::on_pushButtonAddPoint_clicked()
{
	MITK_DEBUG << __FUNCTION__ << "log.pushButtonAddPoint.clicked";

  if (this->GetServiceRoboticsModel().IsNull())
  {
    MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
    return;
  }

  // Pull the server's data resources.
  PipelineManager::Pointer pipelineManager = 
    this->GetServiceRoboticsModel()->GetRegisterNavigationPipeline();
  NavigationToolCollector::Pointer toolCollector = 
    dynamic_cast<NavigationToolCollector*>(pipelineManager->FindFilter("ToolCollector").GetPointer());

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

void QRoboticsRegistrations::on_toolCollector_fail(int step, 
  mitk::NavigationData* data)
{
  NavigationToolCollector* toolCollector 
    = dynamic_cast<NavigationToolCollector*>(sender());

  if (nullptr == toolCollector)
  {
    // network security: The data flow is abnormal and may be attacked by intruders.
		MITK_WARN << "The data transmitter is abnormal. This request will be discarded!";
		return;
  }

  if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonAddPoint_clicked")
  {
		if (this->GetServiceRoboticsModel().IsNull())
		{
			MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
			return;
		}

		// Pull the server's data resources.
		PipelineManager::Pointer pipelineManager =
			this->GetServiceRoboticsModel()->GetRegisterNavigationPipeline();

    mitk::NavigationDataToNavigationDataFilter::Pointer filter = pipelineManager->FindFilter(0);

    mitk::NavigationDataSource* roboticsNavigationSource = 
      this->GetServiceRoboticsModel()->GetRoboticsNavigationDataSource();
    int roboticsNavigationToolIndex = roboticsNavigationSource->GetOutputIndex("");
    mitk::NavigationData::Pointer roboticsPose = 
      roboticsNavigationSource->GetOutput(roboticsNavigationToolIndex);

    bool translationOnly = 
      this->GetServiceRoboticsModel()->GetRegisterModel().PoseCount() < 10;

    // Push the registration data of the mechanical arm to the server.
    this->GetServiceRoboticsModel()->GetRegisterModel().AddPose(filter->GetOutput(),
        roboticsPose,
        translationOnly);

    // Update interface mapping of server data.
    this->UpdateWidgetOfService();
  }
  else if(toolCollector->GetPermissionIdentificationArea() == "on_pushButtonCalResult_clicked")
  {

  }
}

void QRoboticsRegistrations::on_toolCollector_complete(mitk::NavigationData* data)
{
}
