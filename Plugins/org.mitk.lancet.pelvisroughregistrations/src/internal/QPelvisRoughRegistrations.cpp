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

void QPelvisRoughRegistrations::on_toolCollector_fail(int)
{
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
		// update ui for widget
		QVector<QRadioButton*> listRadioButtons = {
			this->m_Controls.radioButton_posteriorAcetabulum,
			this->m_Controls.radioButton_anteriorAcetabulum,
			this->m_Controls.radioButton_superiorAcetabulum
		};

		for (int index = 0; index < listRadioButtons.size(); ++index)
		{
			if (this->imp->vegaPointArray[index] == mitk::Point3D())
			{
				this->imp->vegaPointArray[index] = data->GetPosition();
				break;
			}
		}
		
		if (this->GetServiceModel().IsNotNull())
		{
			for (int index = 0; index < listRadioButtons.size(); ++index)
			{
				bool isChecked = this->imp->vegaPointArray[index] != mitk::Point3D();
				listRadioButtons[index]->setChecked(isChecked);
			}			
		}

		// Upload service.
		this->GetServiceModel()->SetVegaPointArray(this->imp->vegaPointArray);
		MITK_INFO << "collect point " << data->GetPosition();
	}
}

void QPelvisRoughRegistrations::on_toolCollector_step(int, mitk::NavigationData*)
{
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
	this->InitializeCollectStateForQtWidget();
}

void QPelvisRoughRegistrations::InitializeQtWidgetEventAtOnly()
{
	connect(this->m_Controls.pushButtonCapturePelvis_Landmark, SIGNAL(clicked()), 
		this, SLOT(on_pushButtonCapturePelvis_Landmark_clicked()));
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
		bool posteriorAcetabulumEnabled = this->GetServiceModel()->GetImagePoint(0) == mitk::Point3D();
		this->m_Controls.radioButton_posteriorAcetabulum->setEnabled(posteriorAcetabulumEnabled);

		bool anteriorAcetabulumEnabled = this->GetServiceModel()->GetImagePoint(1) == mitk::Point3D();
		this->m_Controls.radioButton_anteriorAcetabulum->setEnabled(anteriorAcetabulumEnabled);

		bool superiorAcetabulumEnabled = this->GetServiceModel()->GetImagePoint(2) == mitk::Point3D();
		this->m_Controls.radioButton_superiorAcetabulum->setEnabled(superiorAcetabulumEnabled);
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
