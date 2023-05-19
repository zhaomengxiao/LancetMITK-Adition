#include "lancetSpatialFittingService.h"

#include "lancetSpatiaFittingModulsFactor.h"
#include "lancetSpatialFittingAbstractModel.h"

// devices
#include <lancetIDevicesAdministrationService.h>

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
		spatial_fitting::ModulsFactor::Items::PELVIS_CHECKPOINT_MODEL,
	};

	for (auto item_key : listModuls)
	{
		this->SetModel(item_key, factor.CreateModel(item_key));
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