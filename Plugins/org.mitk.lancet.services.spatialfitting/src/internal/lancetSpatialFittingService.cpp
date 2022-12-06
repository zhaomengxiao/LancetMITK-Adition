#include "lancetSpatialFittingService.h"
#include "lancetSpatialFittingGlobal.h"
#include "internal/lancetSpatialFittingRoboticsRegisterModel.h"
#include <lancetIDevicesAdministrationService.h>
namespace lancet
{
	SpatialFittingService::SpatialFittingService()
	{
		lancet::spatial_fitting::RoboticsRegisterModel::Pointer model
			= lancet::spatial_fitting::RoboticsRegisterModel::Pointer(new lancet::spatial_fitting::RoboticsRegisterModel);

		this->SetRoboticsRegisterModel(model);

		auto ndidatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Vega");
		auto robotdatasource = this->GetDeviceService()->GetConnector()->GetTrackingDeviceSource("Kuka");
		this->GetRoboticsRegisterModel()->SetNdiNavigationDataSource(ndidatasource);
		this->GetRoboticsRegisterModel()->SetRoboticsNavigationDataSource(robotdatasource);

	}

}


