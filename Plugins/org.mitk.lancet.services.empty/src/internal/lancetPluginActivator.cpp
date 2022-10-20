#include "lancetPluginActivator.h"

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>

#include "lancetTest.h"

namespace lancet
{
  const std::string org_mitk_lancet_services_empty_Activator::PLUGIN_ID = "org.mitk.lancet.services.empty";

  struct org_mitk_lancet_services_empty_Activator::PrivateImp
  {
    static ctkPluginContext* pluginContext;
  };
  ctkPluginContext* org_mitk_lancet_services_empty_Activator::PrivateImp::pluginContext = nullptr;

	org_mitk_lancet_services_empty_Activator::org_mitk_lancet_services_empty_Activator()
    : imp(std::make_shared<PrivateImp>())
	{
    qDebug() << __func__ << "log";
	}

	org_mitk_lancet_services_empty_Activator::~org_mitk_lancet_services_empty_Activator()
	{
	}

	void org_mitk_lancet_services_empty_Activator::start(ctkPluginContext* context)
	{
		qDebug() << __func__ << "log";
    this->imp->pluginContext = context;

    // warning
    // The state machine service object does not intentionally run naked.
    // Fortunately, it does not affect the function logically.
    lancetTest* o = new lancetTest;
    this->imp->pluginContext->registerService<lancetTest>(o);
	}

	void org_mitk_lancet_services_empty_Activator::stop(ctkPluginContext* context)
	{
	}

  ctkPluginContext* org_mitk_lancet_services_empty_Activator::GetPluginContext()
  {
    return PrivateImp::pluginContext;
  }
}