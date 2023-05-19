#include <robotRegistration.h>
#include "lancetSpatialFittingAbstractService.h"
#include "lancetSpatialFittingAbstractModel.h"
#include <lancetIDevicesAdministrationService.h>

#include "internal/lancetPluginActivator.h"

#include <QMap>

namespace lancet
{
  struct AbstractService::PrivateImp
  {
	  static berry::SmartPointer<AbstractService> staticInstance;

	  QMap<QString, berry::SmartPointer<lancet::spatial_fitting::AbstractModel>> mapModels;
  };
	AbstractService::Pointer AbstractService::PrivateImp::staticInstance 
		= AbstractService::Pointer();

  AbstractService::AbstractService()
	: imp(std::make_shared<PrivateImp>())
  {
		this->imp->staticInstance = AbstractService::Pointer(this);
  }

	AbstractService::~AbstractService()
	{
		this->imp->staticInstance = AbstractService::Pointer();
	}

  IDevicesAdministrationService *AbstractService::GetDeviceService() const
  {
		auto context = PluginActivator::GetPluginContext();
		auto serviceRef = context->getServiceReference<IDevicesAdministrationService>();
		return context->getService<IDevicesAdministrationService>(serviceRef);
  }

	berry::SmartPointer<AbstractService> AbstractService::GetService()
	{
		return PrivateImp::staticInstance;
	}

  bool AbstractService::HasModel(const QString& enum_model) const
  {
    return this->imp->mapModels.find(enum_model) != this->imp->mapModels.end();
  }

  lancet::spatial_fitting::AbstractModel::Pointer AbstractService::GetModel(const QString& enum_model) const
  {
    if(this->HasModel(enum_model))
    {
      return this->imp->mapModels.value(enum_model);
    }
    return lancet::spatial_fitting::AbstractModel::Pointer();
  }

  void AbstractService::SetModel(const QString& enum_model, berry::SmartPointer<lancet::spatial_fitting::AbstractModel> obj)
  {
	  this->imp->mapModels[enum_model] = obj;
  }

  void AbstractService::InsertModel(lancet::spatial_fitting::AbstractModel::Pointer  obj)
  {
    if(obj.IsNotNull())
    {
      this->SetModel(obj->GetSerialNumber(), obj);
    }
  }
}