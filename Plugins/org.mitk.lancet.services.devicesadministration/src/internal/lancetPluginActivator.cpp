#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include <internal/lancetDevicesAdministrationService.h>
#include <internal/lancetTrackingDeviceManage.h>

namespace lancet
{
  const std::string org_mitk_lancet_services_DevicesAdministration::PLUGIN_ID = "org.mitk.lancet.services.devicesadministration";

  struct org_mitk_lancet_services_DevicesAdministration::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    berry::SmartPointer<IDevicesAdministrationService> DevicesAdministrationService;
  };
  ctkPluginContext* PluginActivator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_services_DevicesAdministration::org_mitk_lancet_services_DevicesAdministration()
    : imp(std::make_shared<PrivateImp>())
	{
    
	}

	org_mitk_lancet_services_DevicesAdministration::~org_mitk_lancet_services_DevicesAdministration()
	{
	}

	void org_mitk_lancet_services_DevicesAdministration::start(ctkPluginContext* context)
	{
    this->imp->pluginContext = context;

    // warning
    // The service object does not intentionally run naked.
    // Fortunately, it does not affect the function logically.
    IDevicesAdministrationService* streakingServicePointer 
      = new DevicesAdministrationService();
    streakingServicePointer->SetConnector(berry::SmartPointer<lancet::TrackingDeviceManage>(new lancet::TrackingDeviceManage));

    // register service to mitk
    this->imp->DevicesAdministrationService = 
      berry::SmartPointer(streakingServicePointer);
    this->imp->pluginContext->registerService<
      lancet::IDevicesAdministrationService>(streakingServicePointer);
	}

	void org_mitk_lancet_services_DevicesAdministration::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<lancet::IDevicesAdministrationService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_services_DevicesAdministration::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}