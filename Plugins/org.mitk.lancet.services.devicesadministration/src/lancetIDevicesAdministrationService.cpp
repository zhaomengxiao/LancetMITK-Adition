#include "lancetIDevicesAdministrationService.h"
#include <lancetIDevicesScanner.h>
namespace lancet
{
  struct IDevicesAdministrationService::IDevicesAdministrationServicePrivateImp
  {
		
		static berry::SmartPointer<IDevicesAdministrationService> staticInstance;

		berry::SmartPointer<IDevicesScanner> devicesScanner;

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
	void IDevicesAdministrationService::SetScanner(
		berry::SmartPointer<IDevicesScanner> obj)
	{
		this->DisConnectToScanner(this->GetScanner());
		this->imp->devicesScanner = obj;
		this->ConnectToScanner(this->GetScanner());
	}
	berry::SmartPointer<IDevicesScanner>
		IDevicesAdministrationService::GetScanner() const
	{
		return this->imp->devicesScanner;
	}
	void IDevicesAdministrationService::Slot_IDevicesGetStatus()
	{
		emit this->IDevicesGetStatus();
	}
	void IDevicesAdministrationService::ConnectToScanner(
		const berry::SmartPointer<IDevicesScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IDevicesScanner* o = sender.GetPointer();
			QObject::connect(o, &IDevicesScanner::IDevicesGetStatus,
				this, &IDevicesAdministrationService::Slot_IDevicesGetStatus);
		}
	}
	void IDevicesAdministrationService::DisConnectToScanner(
		const berry::SmartPointer<IDevicesScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IDevicesScanner* o = sender.GetPointer();
			QObject::disconnect(o, &IDevicesScanner::IDevicesGetStatus,
				this, &IDevicesAdministrationService::Slot_IDevicesGetStatus);
		}
	}
}
