#include "lancetSpatialFittingService.h"

#include "lancetSpatiaFittingModulsFactor.h"
#include "lancetSpatialFittingAbstractModel.h"

// devices
#include <lancetIDevicesAdministrationService.h>

// mitk
#include <mitkTrackingDeviceSource.h>
//#include <mitkNavigationDataSource.h>

lancet::Service::Service()
	: AbstractService()
{
	connect(
		this->GetDeviceService()->GetConnector().GetPointer(),  // sender
		SIGNAL(TrackingDeviceStateChange(std::string, lancet::TrackingDeviceManage::TrackingDeviceState)),
		this, // recv
		SLOT(onDeviceConnectState_change(std::string, lancet::TrackingDeviceManage::TrackingDeviceState)));
}

bool lancet::Service::Initialize()
{
	spatial_fitting::ModulsFactor factor;

	QStringList listModuls =
	{
		spatial_fitting::ModulsFactor::Items::PROBE_REGISTER_MODEL,
		spatial_fitting::ModulsFactor::Items::ROBOTICS_REGISTER_MODEL,
		spatial_fitting::ModulsFactor::Items::PELVIS_CHECKPOINT_MODEL,
		spatial_fitting::ModulsFactor::Items::PELVIS_ROUGH_REGISTER_MODEL,
		//spatial_fitting::ModulsFactor::Items::PELVIS_PELVIC_REGISTER_MODEL,
	};

	auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
	auto robotdatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Kuka");

	for (auto item_key : listModuls)
	{
		auto model = factor.CreateModel(item_key);

		// init
		model->SetNdiNavigationDataSource(ndidatasource);
		model->SetRoboticsNavigationDataSource(robotdatasource);

		this->SetModel(item_key, model);
	}

	return true;
}

void lancet::Service::onDeviceConnectState_change(std::string name,
	lancet::TrackingDeviceManage::TrackingDeviceState state)
{
	if (state == lancet::TrackingDeviceManage::UnInstall || state & lancet::TrackingDeviceManage::Tracking)
	{
		this->Initialize();
	}
}