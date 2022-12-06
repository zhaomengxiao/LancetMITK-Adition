#include <robotRegistration.h>
#include "lancetSpatialFittingAbstractService.h"
#include "internal/lancetSpatialFittingRoboticsRegisterModel.h"
#include <lancetIDevicesAdministrationService.h>

#include "internal/lancetPluginActivator.h"
namespace lancet
{
  struct SpatialFittingAbstractService::SpatialFittingAbstractServicePrivateImp
  {
	  static berry::SmartPointer<SpatialFittingAbstractService> staticInstance;

	  itk::SmartPointer<lancet::spatial_fitting::RoboticsRegisterModel> RoboticsRegisterModel;
  };
	berry::SmartPointer<SpatialFittingAbstractService>
		SpatialFittingAbstractService::SpatialFittingAbstractServicePrivateImp::
		staticInstance = berry::SmartPointer<SpatialFittingAbstractService>();

	SpatialFittingAbstractService::SpatialFittingAbstractService()
		: imp(std::make_shared<SpatialFittingAbstractServicePrivateImp>())
	{
		this->imp->staticInstance = berry::SmartPointer(this);
	}
	berry::SmartPointer<SpatialFittingAbstractService>
		SpatialFittingAbstractService::GetService()
	{
		return SpatialFittingAbstractServicePrivateImp::staticInstance;
	}
	IDevicesAdministrationService* 
		SpatialFittingAbstractService::GetDeviceService() const
	{
		auto context = PluginActivator::GetPluginContext();
		auto serviceRef = context->getServiceReference<IDevicesAdministrationService>();
		return context->getService<IDevicesAdministrationService>(serviceRef);
	}
	void SpatialFittingAbstractService::SetRoboticsRegisterModel(
		itk::SmartPointer<lancet::spatial_fitting::RoboticsRegisterModel> obj)
	{
		this->imp->RoboticsRegisterModel = obj;
	}
	itk::SmartPointer<lancet::spatial_fitting::RoboticsRegisterModel>
		SpatialFittingAbstractService::GetRoboticsRegisterModel() const
	{
		return this->imp->RoboticsRegisterModel;
	}
}