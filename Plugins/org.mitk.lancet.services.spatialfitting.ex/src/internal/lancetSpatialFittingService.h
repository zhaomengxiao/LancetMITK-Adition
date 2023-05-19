#ifndef LancetlancetDevicesAdministrationService_H
#define LancetlancetDevicesAdministrationService_H

#include <lancetSpatialFittingGlobal.h>
#include <lancetSpatialFittingAbstractService.h>
#include <internal/lancetTrackingDeviceManage.h>


namespace lancet
{
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	Service
	: public AbstractService
{
	Q_OBJECT
	Q_INTERFACES(lancet::AbstractService)
public:
	berryObjectMacro(lancet::Service);
	Service();

	virtual bool Initialize();
protected slots:
	void onDeviceConnectState_change(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);
};
}

#endif // !LancetlancetDevicesAdministrationService_H
