#include "lancetDevicesAdministrationService.h"

#include <internal/lancetDevicesScanner.h>

namespace lancet
{
	DevicesAdministrationService::DevicesAdministrationService()
	{
		DevicesScanner::Pointer scannar
			= DevicesScanner::Pointer(new DevicesScanner);

		this->SetScanner(scannar);

	}
	void DevicesAdministrationService::Start()
	{

	}

	void DevicesAdministrationService::Stop()
	{

	}
}


