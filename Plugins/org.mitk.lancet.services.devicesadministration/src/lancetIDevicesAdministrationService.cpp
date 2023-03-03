#include "lancetIDevicesAdministrationService.h"
#include <internal/lancetTrackingDeviceManage.h>
namespace lancet
{
  struct IDevicesAdministrationService::IDevicesAdministrationServicePrivateImp
  {
		static berry::SmartPointer<IDevicesAdministrationService> staticInstance;

		berry::SmartPointer<TrackingDeviceManage> TrackingDeviceManage;
  };
	berry::SmartPointer<IDevicesAdministrationService>
		IDevicesAdministrationService::IDevicesAdministrationServicePrivateImp::
		staticInstance = berry::SmartPointer<IDevicesAdministrationService>();

	IDevicesAdministrationService::IDevicesAdministrationService()
		: imp(std::make_shared<IDevicesAdministrationServicePrivateImp>())
	{
		this->imp->staticInstance = berry::SmartPointer(this);
	}
	berry::SmartPointer<IDevicesAdministrationService>
		IDevicesAdministrationService::GetService()
	{
		return IDevicesAdministrationServicePrivateImp::staticInstance;
	}
	void IDevicesAdministrationService::SetConnector(
		berry::SmartPointer<TrackingDeviceManage> obj)
	{
		this->DisConnectToConnector(this->GetConnector());
		this->imp->TrackingDeviceManage = obj;
		this->ConnectToConnector(this->GetConnector());
	}
	berry::SmartPointer<TrackingDeviceManage>
		IDevicesAdministrationService::GetConnector() const
	{
		return this->imp->TrackingDeviceManage;
	}
	void IDevicesAdministrationService::Slot_IDevicesGetStatus(std::string name, lancet::TrackingDeviceManage::TrackingDeviceState State)
	{
		emit this->TrackingDeviceStateChange(name, State);
	}
	void IDevicesAdministrationService::ConnectToConnector(
		const berry::SmartPointer<TrackingDeviceManage>& sender) const
	{
		if (sender.IsNotNull())
		{
			TrackingDeviceManage* o = sender.GetPointer();
			QObject::connect(o, &TrackingDeviceManage::TrackingDeviceStateChange,
				this, &IDevicesAdministrationService::Slot_IDevicesGetStatus);
		}
	}
	void IDevicesAdministrationService::DisConnectToConnector(
		const berry::SmartPointer<TrackingDeviceManage>& sender) const
	{
		if (sender.IsNotNull())
		{
			TrackingDeviceManage* o = sender.GetPointer();
			QObject::disconnect(o, &TrackingDeviceManage::TrackingDeviceStateChange,
				this, &IDevicesAdministrationService::Slot_IDevicesGetStatus);
		}
	}
}
