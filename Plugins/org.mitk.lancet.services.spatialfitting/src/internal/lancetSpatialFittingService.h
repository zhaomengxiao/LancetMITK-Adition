#ifndef LancetlancetDevicesAdministrationService_H
#define LancetlancetDevicesAdministrationService_H

#include <lancetSpatialFittingAbstractService.h>
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
	berryObjectMacro(lancet::spatial_fitting::RoboticsRegisterModel);
	SpatialFittingService();
};
}

#endif // !LancetlancetDevicesAdministrationService_H
