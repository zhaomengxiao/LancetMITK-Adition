#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include "lancetScxmlStateMachineService.h"

namespace lancet
{
  const std::string org_mitk_lancet_statemachine_services_Activator::PLUGIN_ID = "org.mitk.lancet.statemachine.services";

  struct org_mitk_lancet_statemachine_services_Activator::PrivateImp
  {
    static ctkPluginContext* pluginContext;
    ScxmlStateMachineService::Pointer stateMachineService = ScxmlStateMachineService::Pointer(nullptr);
  };
  ctkPluginContext* org_mitk_lancet_statemachine_services_Activator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_statemachine_services_Activator::org_mitk_lancet_statemachine_services_Activator()
    : imp(std::make_shared<PrivateImp>())
	{
    qDebug() << __func__ << "log";
	}

	org_mitk_lancet_statemachine_services_Activator::~org_mitk_lancet_statemachine_services_Activator()
	{
	}

	void org_mitk_lancet_statemachine_services_Activator::start(ctkPluginContext* context)
	{
		qDebug() << __func__ << "log";
    this->imp->pluginContext = context;

    // warning
    // The state machine service object does not intentionally run naked.
    // Fortunately, it does not affect the function logically.
    ScxmlStateMachineService* streakingServicePointer = new ScxmlStateMachineService();
    
    // register state machine service to mitk
    this->imp->stateMachineService = ScxmlStateMachineService::Pointer(streakingServicePointer);
    this->imp->pluginContext->registerService<lancet::IScxmlStateMachineService>(streakingServicePointer);
	}

	void org_mitk_lancet_statemachine_services_Activator::stop(ctkPluginContext* context)
	{
    this->imp->pluginContext->ungetService(this->imp->pluginContext->getServiceReference<lancet::IScxmlStateMachineService>());
    this->imp->pluginContext = nullptr;
	}

  ctkPluginContext* org_mitk_lancet_statemachine_services_Activator::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}