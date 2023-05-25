#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include <internal/lancetSpatialFittingService.h>

namespace lancet
{
  const std::string org_mitk_lancet_services_SpatialFitting_ex::PLUGIN_ID = "org.mitk.lancet.services.spatialfitting.ex";

  struct org_mitk_lancet_services_SpatialFitting_ex::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    berry::SmartPointer<AbstractService> service;
  };
  ctkPluginContext* PluginActivator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_services_SpatialFitting_ex::org_mitk_lancet_services_SpatialFitting_ex()
    : imp(std::make_shared<PrivateImp>())
	{
    
	}

	org_mitk_lancet_services_SpatialFitting_ex::~org_mitk_lancet_services_SpatialFitting_ex()
	{
	}

	void org_mitk_lancet_services_SpatialFitting_ex::start(ctkPluginContext* context)
	{
        this->imp->pluginContext = context;
				 
        // warning
        // The service object does not intentionally run naked.
        // Fortunately, it does not affect the function logically.
        AbstractService* streakingServicePointer = new Service();
    
        // register service to mitk
        this->imp->service = berry::SmartPointer(streakingServicePointer);
        this->imp->pluginContext->registerService<AbstractService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_SpatialFitting_ex::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<AbstractService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_SpatialFitting_ex::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}