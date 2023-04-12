#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include <internal/lancetMedicalRecordsService.h>

namespace lancet
{
  const std::string org_mitk_lancet_services_MedicalRecordsAdministration::PLUGIN_ID = "org.mitk.lancet.services.medicalrecords";

  struct org_mitk_lancet_services_MedicalRecordsAdministration::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    berry::SmartPointer<IMedicalRecordsService> medicalRecordsAdministrationService;
  };
  ctkPluginContext* PluginActivator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_services_MedicalRecordsAdministration::org_mitk_lancet_services_MedicalRecordsAdministration()
    : imp(std::make_shared<PrivateImp>())
	{
    
	}

	org_mitk_lancet_services_MedicalRecordsAdministration::~org_mitk_lancet_services_MedicalRecordsAdministration()
	{
	}

	void org_mitk_lancet_services_MedicalRecordsAdministration::start(ctkPluginContext* context)
	{
    this->imp->pluginContext = context;

    // warning
    // The service object does not intentionally run naked.
    // Fortunately, it does not affect the function logically.
    IMedicalRecordsService* streakingServicePointer 
      = new MedicalRecordsService();
    
    // register service to mitk
    this->imp->medicalRecordsAdministrationService = 
      berry::SmartPointer(streakingServicePointer);
    this->imp->pluginContext->registerService<
      lancet::IMedicalRecordsService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_MedicalRecordsAdministration::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<lancet::IMedicalRecordsService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_MedicalRecordsAdministration::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}