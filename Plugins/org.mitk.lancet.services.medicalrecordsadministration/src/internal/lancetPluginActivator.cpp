#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include <internal/lancetMedicalRecordsAdministrationService.h>

namespace lancet
{
  const std::string org_mitk_lancet_services_MedicalRecordsAdministration::PLUGIN_ID = "org.mitk.lancet.services.medicalrecordsadministration";

  struct org_mitk_lancet_services_MedicalRecordsAdministration::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    berry::SmartPointer<IMedicalRecordsAdministrationService> medicalRecordsAdministrationService;
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
    IMedicalRecordsAdministrationService* streakingServicePointer 
      = new MedicalRecordsAdministrationService();
    
    // register service to mitk
    this->imp->medicalRecordsAdministrationService = 
      berry::SmartPointer(streakingServicePointer);
    this->imp->pluginContext->registerService<
      lancet::IMedicalRecordsAdministrationService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_MedicalRecordsAdministration::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<lancet::IMedicalRecordsAdministrationService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_MedicalRecordsAdministration::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}