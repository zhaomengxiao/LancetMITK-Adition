#ifndef LancetlancetDevicesAdministrationService_H
#define LancetlancetDevicesAdministrationService_H

#include <lancetIDevicesAdministrationService.h>

// ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_devicesadministration_Export.h"

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_DEVICESADMINISTRATION_PLUGIN 
	DevicesAdministrationService
	: public IDevicesAdministrationService
{
	Q_OBJECT
	Q_INTERFACES(lancet::IDevicesAdministrationService)
public:
	berryObjectMacro(lancet::DevicesAdministrationService);
	DevicesAdministrationService();
public:
	virtual void Start() override;
	virtual void Stop() override;
};
}

#endif // !LancetlancetDevicesAdministrationService_H
