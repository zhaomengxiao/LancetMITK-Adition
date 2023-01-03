#include <robotRegistration.h>
#include "lancetSpatialFittingAbstractService.h"
#include "internal/lancetSpatialFittingProbeCheckPointModel.h"
#include "internal/lancetSpatialFittingPelvisCheckPointModel.h"
#include "internal/lancetSpatialFittingRoboticsRegisterModel.h"
#include "internal/lancetSpatialFittingPelvisRegisterModel.h"
#include "internal/lancetSpatialFittingPelvisMarkPointModel.h"
#include <lancetIDevicesAdministrationService.h>

#include "internal/lancetPluginActivator.h"
namespace lancet
{
  struct SpatialFittingAbstractService::SpatialFittingAbstractServicePrivateImp
  {
	  static berry::SmartPointer<SpatialFittingAbstractService> staticInstance; 
	  
	  lancet::spatial_fitting::RoboticsRegisterModel::Pointer RoboticsRegisterModel;

	  lancet::spatial_fitting::ProbeCheckPointModel::Pointer probeCheckPointModel;

	  lancet::spatial_fitting::PelvisCheckPointModel::Pointer pelvisCheckPointModel;

	  lancet::spatial_fitting::PelvisRegisterModel::Pointer pelvisRegisterModel;

	  lancet::spatial_fitting::PelvisMarkPointModel::Pointer pelvisMarkPointModel;
	    
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
	itk::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel> 
		SpatialFittingAbstractService::GetProbeCheckPointModel() const
	{
		return this->imp->probeCheckPointModel;
	}
	void SpatialFittingAbstractService::SetProbeCheckPointModel(
		itk::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel> model)
	{
		this->imp->probeCheckPointModel = model;
	}
	itk::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel> 
		SpatialFittingAbstractService::GetPelvisCheckPointModel() const
	{
		return this->imp->pelvisCheckPointModel;
	}
	void SpatialFittingAbstractService::SetPelvisCheckPointModel(
		itk::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel> model)
	{
		this->imp->pelvisCheckPointModel = model;
	}
	itk::SmartPointer<lancet::spatial_fitting::PelvisRegisterModel>
		SpatialFittingAbstractService::GetPelvisRegisterModel() const
	{
		return this->imp->pelvisRegisterModel;
	}
	void SpatialFittingAbstractService::SetPelvisRegisterModel(
		itk::SmartPointer<lancet::spatial_fitting::PelvisRegisterModel> model)
	{
		this->imp->pelvisRegisterModel = model;
	}
	itk::SmartPointer<lancet::spatial_fitting::PelvisMarkPointModel>
		SpatialFittingAbstractService::GetPelvisMarkPointModel() const
	{
		return this->imp->pelvisMarkPointModel;
	}
	void SpatialFittingAbstractService::SetPelvisMarkPointModel(
		itk::SmartPointer<lancet::spatial_fitting::PelvisMarkPointModel> model)
	{
		this->imp->pelvisMarkPointModel = model;
	}
}