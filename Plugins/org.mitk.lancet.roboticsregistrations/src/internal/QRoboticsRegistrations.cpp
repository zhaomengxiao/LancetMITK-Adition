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

#include <core/lancetSpatialFittingPipelineBuilder.h>
#include <core/lancetSpatialFittingPointAccuracyDate.h>
#include <core/lancetSpatialFittingRoboticsRegisterDirector.h>
#include <core/lancetSpatialFittingRoboticsVerifyDirector.h>

const std::string QRoboticsRegistrations::VIEW_ID = "org.mitk.views.qroboticsregistrations";

using PointAccuracyDate = lancet::spatial_fitting::PointAccuracyDate;
using PipelineManager = lancet::spatial_fitting::PipelineManager;
using NavigationToolCollector = lancet::spatial_fitting::NavigationToolCollector;

using RoboticsVerifyDirector = lancet::spatial_fitting::RoboticsVerifyDirector;
using RoboticsRegisterDirector = lancet::spatial_fitting::RoboticsRegisterDirector;

void QRoboticsRegistrations::SetFocus()
{
}

QRoboticsRegistrations::~QRoboticsRegistrations()
{
  this->DisConnectToQtWidget();
}

void QRoboticsRegistrations::TestInitService()
{
  RoboticsRegisterDirector director;
  director.Builder();

  RoboticsVerifyDirector directorV;
  directorV.Builder();

	this->GetServiceRoboticsModel()->SetRegisterNavigationPipeline(director.GetBuilder()->GetOutput());
	this->GetServiceRoboticsModel()->SetAccutacyVerifyPipeline(directorV.GetBuilder()->GetOutput());
}

void QRoboticsRegistrations::CreateQtPartControl(QWidget *parent)
{
  this->TestInitService();

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

  this->ConnectToQtWidget();
	this->UpdateUiForService();
}

void QRoboticsRegistrations::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
}

void QRoboticsRegistrations::DoImageProcessing()
{
	
}

void QRoboticsRegistrations::ConnectToQtWidget()
{
  connect(this->m_Controls.pushButtonAddPoint, &QPushButton::clicked,
		this, &QRoboticsRegistrations::on_pushButtonAddPoint_clicked);
	connect(this->m_Controls.pushButtonCalResult, &QPushButton::clicked,
		this, &QRoboticsRegistrations::on_pushButtonCalResult_clicked);
	connect(this->m_Controls.pushButtonRobotVerify, &QPushButton::clicked,
		this, &QRoboticsRegistrations::on_pushButtonRobotVerify_clicked);

  // Collector
		// Pull the server's data resources.
	PipelineManager::Pointer pipelineManager =
		this->GetServiceRoboticsModel()->GetRegisterNavigationPipeline();
	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("RobotEndRF2RobotBaseRF_ToolCollector").GetPointer());

	if (toolCollector.IsNotNull())
	{
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Fail,
			this, &QRoboticsRegistrations::on_toolCollector_fail);
		//connect(toolCollector.GetPointer(), &NavigationToolCollector::Step,
		//	this, &QRoboticsRegistrations::on_toolCollector_step);
		connect(toolCollector.GetPointer(), &NavigationToolCollector::Complete,
			this, &QRoboticsRegistrations::on_toolCollector_complete);
	}
}

void QRoboticsRegistrations::DisConnectToQtWidget()
{
	//disconnect(this->m_Controls.pushButtonAddPoint, &QPushButton::clicked,
	//	this, &QRoboticsRegistrations::on_pushButtonAddPoint_clicked);
 // disconnect(this->m_Controls.pushButtonCalResult, &QPushButton::clicked,
	//	this, &QRoboticsRegistrations::on_pushButtonCalResult_clicked);
 // disconnect(this->m_Controls.pushButtonRobotVerify, &QPushButton::clicked,
	//	this, &QRoboticsRegistrations::on_pushButtonRobotVerify_clicked);
}

void QRoboticsRegistrations::UpdateWidgetOfService()
{
  if (this->GetServiceRoboticsModel().IsNotNull())
  {
    int step = this->GetServiceRoboticsModel()->GetRegisterModel().PoseCount();
    this->m_Controls.rioProgressBar->setValue(step);
  }

	return this->UpdateUiForService();
}

void QRoboticsRegistrations::UpdateUiForService()
{
	if (this->GetServiceRoboticsModel().IsNull())
	{
		MITK_INFO << "Failed to request service resources, ignoring this processing";
		return;
	}
	bool isTrackingNDIDevice = false;
	bool isTrackingRoboticsDevice = false;

	QString notTrackingDeviceTips = "";

	if (!isTrackingNDIDevice || !isTrackingRoboticsDevice)
	{
		if (false == isTrackingNDIDevice)
		{
			notTrackingDeviceTips += "NDI";
		}
		if (false == isTrackingRoboticsDevice)
		{
			if (false == notTrackingDeviceTips.isEmpty())
			{
				notTrackingDeviceTips += ", ";
			}
			notTrackingDeviceTips += "Robot";
		}
		this->m_Controls.labelTips->setText(QString("Device [%1] not tracking!")
			.arg(notTrackingDeviceTips));
	}
	RobotRegistration& roboticsModel = 
		this->GetServiceRoboticsModel()->GetRegisterModel();

	this->m_Controls.rioProgressBar->setValue(roboticsModel.PoseCount());

	// 20: Number of space registration units required.
	bool btnEnabled = roboticsModel.PoseCount() >= 20;

	// Update the activation status of the [Calculate Registration Result] button.
	this->m_Controls.pushButtonCalResult->setEnabled(btnEnabled);
}

lancet::spatial_fitting::RoboticsRegisterModelPtr 
QRoboticsRegistrations::GetServiceRoboticsModel() const
{
  static lancet::spatial_fitting::RoboticsRegisterModelPtr robot =
    lancet::spatial_fitting::RoboticsRegisterModel::New();
  return robot;
}

void QRoboticsRegistrations::on_pushButtonRobotVerify_clicked()
{
  MITK_DEBUG << __FUNCTION__ << "log.pushButtonRobotVerify.clicked";
	if (this->GetServiceRoboticsModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
		return;
	}

	// Pull the server's data resources.
	PipelineManager::Pointer pipelineManager =
		this->GetServiceRoboticsModel()->GetAccutacyVerifyPipeline();
	NavigationToolCollector::Pointer toolCollector =
		dynamic_cast<NavigationToolCollector*>(pipelineManager->FindFilter("NRT2NRRCollector").GetPointer());

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

void QRoboticsRegistrations::on_pushButtonCalResult_clicked()
{
	MITK_DEBUG << __FUNCTION__ << "log.pushButtonCalResult.clicked";

	if (this->GetServiceRoboticsModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
		return;
	}

	if (this->GetServiceRoboticsModel()->GetRegisterModel().Regist())
	{
		// The calculation is successful and the output is obtained.
	}
	else
	{
		// At least four translation only pose needed to regist.
	}
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
    dynamic_cast<NavigationToolCollector*>(pipelineManager->
			FindFilter("RobotEndRF2RobotBaseRF_ToolCollector").GetPointer());

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

void QRoboticsRegistrations::on_toolCollector_fail(int step)
{
	MITK_DEBUG << "log.parame.isNull ";
}

void QRoboticsRegistrations::on_toolCollector_complete(mitk::NavigationData* data)
{
	NavigationToolCollector* toolCollector
		= dynamic_cast<NavigationToolCollector*>(sender());

	if (nullptr == toolCollector || nullptr == data)
	{
		// network security: The data flow is abnormal and may be attacked by intruders.
		MITK_WARN << "The data transmitter is abnormal. This request will be discarded!";
		return;
	}
	if (this->GetServiceRoboticsModel().IsNull())
	{
		MITK_WARN << "Input RoboticsRegisterModel is nullptr, Ignore this request!";
		return;
	}

	if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonAddPoint_clicked")
	{
		// Pull the server's data resources.

		mitk::NavigationDataSource* roboticsNavigationSource =
			this->GetServiceRoboticsModel()->GetRoboticsNavigationDataSource();
		int roboticsNavigationToolIndex = roboticsNavigationSource->GetOutputIndex("");
		mitk::NavigationData::Pointer roboticsPose =
			roboticsNavigationSource->GetOutput(roboticsNavigationToolIndex);

		bool translationOnly =
			this->GetServiceRoboticsModel()->GetRegisterModel().PoseCount() < 10;

		// Push the registration data of the mechanical arm to the server.
		this->GetServiceRoboticsModel()->GetRegisterModel().AddPose(data,
			roboticsPose,
			translationOnly);

		// Update interface mapping of server data.
		this->UpdateWidgetOfService();
	}
	else if (toolCollector->GetPermissionIdentificationArea() == "on_pushButtonCalResult_clicked")
	{
		// Pull the server's data resources.

		mitk::NavigationDataSource* ndiNavigationSource =
			this->GetServiceRoboticsModel()->GetNdiNavigationDataSource();

		int roboticsAccuracyToolIndex = ndiNavigationSource->GetOutputIndex("");
		mitk::NavigationData* roboticsAccuracyTool = 
			ndiNavigationSource->GetOutput(roboticsAccuracyToolIndex);

		if (roboticsAccuracyTool)
		{
			PointAccuracyDate roboticsAccuracyPoint;
			roboticsAccuracyPoint.SetSourcePoint(data->GetPosition());
			roboticsAccuracyPoint.SetTargetPoint(roboticsAccuracyTool->GetPosition());
			this->m_Controls.lcdNumber_RIO->display(roboticsAccuracyPoint.Compute());
		}
	}
}
