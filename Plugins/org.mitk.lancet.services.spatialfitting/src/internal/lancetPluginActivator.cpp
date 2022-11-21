#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

namespace lancet
{
  const std::string org_mitk_lancet_services_SpatialFitting::PLUGIN_ID = "org.mitk.lancet.services.spatialfitting";

  struct org_mitk_lancet_services_SpatialFitting::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    //berry::SmartPointer<IMedicalRecordsService> medicalRecordsAdministrationService;
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
    // this->imp->pluginContext = context;

    // // warning
    // // The service object does not intentionally run naked.
    // // Fortunately, it does not affect the function logically.
    // IMedicalRecordsService* streakingServicePointer 
      // = new MedicalRecordsService();
    
    // // register service to mitk
    // this->imp->medicalRecordsAdministrationService = 
      // berry::SmartPointer(streakingServicePointer);
    // this->imp->pluginContext->registerService<
      // lancet::IMedicalRecordsService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_SpatialFitting::stop(ctkPluginContext* context)
	{
    //this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<lancet::IMedicalRecordsService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_SpatialFitting::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}