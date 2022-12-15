#ifndef LancetlancetDevicesAdministrationService_H
#define LancetlancetDevicesAdministrationService_H

#include <lancetSpatialFittingAbstractService.h>
#include <internal/lancetTrackingDeviceManage.h>
#include "org_mitk_lancet_services_spatialfitting_Export.h"

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	SpatialFittingService
	: public SpatialFittingAbstractService
{
	Q_OBJECT
	Q_INTERFACES(lancet::SpatialFittingAbstractService)
public:
	berryObjectMacro(lancet::SpatialFittingService);
	SpatialFittingService();

	virtual void Initialize() override;

private:
	virtual void InitializeRoboticsRegisterModel();

	virtual void InitializeProbeCheckPointModel();

	virtual void InitializePelvisCheckPointModel();

	virtual void InitializePelvisRegisterModel();

	virtual void InitializePelvisMarkPointModel();

protected slots:
	void onDeviceConnectState_change(std::string, lancet::TrackingDeviceManage::TrackingDeviceState);
};
}

#endif // !LancetlancetDevicesAdministrationService_H
