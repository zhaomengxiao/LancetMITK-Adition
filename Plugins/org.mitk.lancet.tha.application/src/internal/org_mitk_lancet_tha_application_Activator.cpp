#include "org_mitk_lancet_tha_application_Activator.h"

// tha
#include "QLancetThaApplication.h"
#include "QLancetThaApplicationPerspective.h"

ctkPluginContext* org_mitk_lancet_tha_application_Activator::staticPluginContext = nullptr;

void org_mitk_lancet_tha_application_Activator::start(ctkPluginContext* context)
{
	org_mitk_lancet_tha_application_Activator::staticPluginContext = context;
	BERRY_REGISTER_EXTENSION_CLASS(QLancetThaApplication, context)
	BERRY_REGISTER_EXTENSION_CLASS(QLancetThaApplicationPerspective, context)
}

void org_mitk_lancet_tha_application_Activator::stop(ctkPluginContext* context)
{
	org_mitk_lancet_tha_application_Activator::staticPluginContext = nullptr;
}

ctkPluginContext* org_mitk_lancet_tha_application_Activator::GetPluginContext()
{
	return org_mitk_lancet_tha_application_Activator::staticPluginContext;
}
