#include "lancetSpatialFittingService.h"
#include "lancetSpatialFittingGlobal.h"
#include "core/lancetSpatialFittingPipelineManager.h"
#include "internal/lancetSpatialFittingRoboticsRegisterModel.h"
#include "internal/lancetSpatialFittingProbeCheckPointModel.h"
#include "internal/lancetSpatialFittingPelvisCheckPointModel.h"
#include "internal/lancetSpatialFittingPelvicRoughRegistrationsModel.h"
#include <lancetIDevicesAdministrationService.h>
#include <internal/lancetTrackingDeviceManage.h>
namespace lancet
{
	SpatialFittingService::SpatialFittingService()
	{
		this->Initialize();
		connect(
			this->GetDeviceService()->GetConnector().GetPointer(),  // sender
			SIGNAL(TrackingDeviceStateChange(std::string, lancet::TrackingDeviceManage::TrackingDeviceState)),
			this, // recv
			SLOT(onDeviceConnectState_change(std::string, lancet::TrackingDeviceManage::TrackingDeviceState)));
	}

	void SpatialFittingService::Initialize()
	{
		this->InitializeRoboticsRegisterModel();
		this->InitializeProbeCheckPointModel();
		this->InitializePelvisCheckPointModel();
		this->InitializePelvisRoughRegistrationsModel();
	}

	void SpatialFittingService::InitializeRoboticsRegisterModel()
	{
		MITK_DEBUG << "log";
		if (this->GetRoboticsRegisterModel().IsNull())
		{
			lancet::spatial_fitting::RoboticsRegisterModel::Pointer model
				= lancet::spatial_fitting::RoboticsRegisterModel::Pointer(new lancet::spatial_fitting::RoboticsRegisterModel);

			this->SetRoboticsRegisterModel(model);
		}

		auto toolDataStorage = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Vega");
		if (toolDataStorage.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage->GetTool(index)->GetToolName();
			}
		}

		auto toolDataStorage2 = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Kuka");
		if (toolDataStorage2.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage2->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage2->GetTool(index)->GetToolName();
			}
		}
		auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
		this->GetRoboticsRegisterModel()->SetNdiNavigationDataSource(ndidatasource);

		auto robotdatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Kuka");
		this->GetRoboticsRegisterModel()->SetRoboticsNavigationDataSource(robotdatasource);

		this->GetRoboticsRegisterModel()->ConfigureRegisterPipeline();

		this->GetRoboticsRegisterModel()->ConfigureVerifyPipeline();
	}

	void SpatialFittingService::InitializeProbeCheckPointModel()
	{
		MITK_DEBUG << "log";
		if (this->GetProbeCheckPointModel().IsNull())
		{
			using ProbeCheckPointModel = lancet::spatial_fitting::ProbeCheckPointModel;
			ProbeCheckPointModel::Pointer model = ProbeCheckPointModel::Pointer(
			new ProbeCheckPointModel);

			this->SetProbeCheckPointModel(model);
		}

		auto toolDataStorage = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Vega");
		if (toolDataStorage.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage->GetTool(index)->GetToolName();
			}
		}

		auto toolDataStorage2 = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Kuka");
		if (toolDataStorage2.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage2->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage2->GetTool(index)->GetToolName();
			}
		}
		auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
		this->GetProbeCheckPointModel()->SetNdiNavigationDataSource(ndidatasource);

		auto robotdatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Kuka");
		this->GetProbeCheckPointModel()->SetRoboticsNavigationDataSource(robotdatasource);

		this->GetProbeCheckPointModel()->ConfigureVerifyPipeline();
	}

	void SpatialFittingService::InitializePelvisCheckPointModel()
	{
		MITK_DEBUG << "log";
		if (this->GetPelvisCheckPointModel().IsNull())
		{
			using PelvisCheckPointModel = lancet::spatial_fitting::PelvisCheckPointModel;
			PelvisCheckPointModel::Pointer model = PelvisCheckPointModel::Pointer(
				new PelvisCheckPointModel);

			this->SetPelvisCheckPointModel(model);
		}

		auto toolDataStorage = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Vega");
		if (toolDataStorage.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage->GetTool(index)->GetToolName();
			}
		}

		auto toolDataStorage2 = this->GetDeviceService()->GetConnector()->GetNavigationToolStorage("Kuka");
		if (toolDataStorage2.IsNotNull())
		{
			for (int index = 0; index < toolDataStorage2->GetToolCount(); ++index)
			{
				MITK_ERROR << "read tool data storage [" << index << "]: " << toolDataStorage2->GetTool(index)->GetToolName();
			}
		}
		auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
		this->GetPelvisCheckPointModel()->SetNdiNavigationDataSource(ndidatasource);

		auto robotdatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Kuka");
		this->GetPelvisCheckPointModel()->SetRoboticsNavigationDataSource(robotdatasource);

		this->GetPelvisCheckPointModel()->ConfigureGetCheckPointPipeline();
	}

	void SpatialFittingService::InitializePelvisRoughRegistrationsModel()
	{
		if (this->GetPelvicRoughRegistrationsModel().IsNull())
		{
			using PelvicRoughRegistrationsModel = lancet::spatial_fitting::PelvicRoughRegistrationsModel;
			PelvicRoughRegistrationsModel::Pointer model = PelvicRoughRegistrationsModel::Pointer(
				new PelvicRoughRegistrationsModel);

			this->SetPelvicRoughRegistrationsModel(model);
		}
		auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
		this->GetPelvicRoughRegistrationsModel()->SetNdiNavigationDataSource(ndidatasource);

		this->GetPelvicRoughRegistrationsModel()->ConfigureRegistrationsPipeline();
	}

	void SpatialFittingService::onDeviceConnectState_change(std::string name, lancet::TrackingDeviceManage::TrackingDeviceState state)
	{
	  if (state == lancet::TrackingDeviceManage::UnInstall || state & lancet::TrackingDeviceManage::Tracking)
	  {
		  this->Initialize();
	  }
	}
}


