#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include <internal/lancetSpatialFittingService.h>
namespace lancet
{
  const std::string org_mitk_lancet_services_SpatialFitting::PLUGIN_ID = "org.mitk.lancet.services.spatialfitting";

  struct org_mitk_lancet_services_SpatialFitting::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    berry::SmartPointer<SpatialFittingAbstractService> SpatialFittingService;
  };
  ctkPluginContext* PluginActivator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_services_SpatialFitting::org_mitk_lancet_services_SpatialFitting()
    : imp(std::make_shared<PrivateImp>())
	{
    
	}

	org_mitk_lancet_services_SpatialFitting::~org_mitk_lancet_services_SpatialFitting()
	{
	}

	void org_mitk_lancet_services_SpatialFitting::start(ctkPluginContext* context)
	{
        this->imp->pluginContext = context;

        // warning
        // The service object does not intentionally run naked.
        // Fortunately, it does not affect the function logically.
        SpatialFittingAbstractService* streakingServicePointer = new SpatialFittingService();
    
        // register service to mitk
        this->imp->SpatialFittingService =berry::SmartPointer(streakingServicePointer);
        this->imp->pluginContext->registerService<SpatialFittingAbstractService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_SpatialFitting::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<SpatialFittingAbstractService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_SpatialFitting::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}