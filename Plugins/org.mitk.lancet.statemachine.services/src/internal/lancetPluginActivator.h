#ifndef LancetPluginActivator_H
#define LancetPluginActivator_H

#include <ctkPluginActivator.h>

#include <berrySmartPointer.h>

#include <usServiceEvent.h>

#include "org_mitk_lancet_statemachine_services_Export.h"

namespace us
{
	class ModuleContext;
}

namespace lancet
{
class ScxmlStateMachineService;
class LANCET_STATEMACHINE_SERVICES_PLUGIN 
	org_mitk_lancet_statemachine_services_Activator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org_mitk_lancet_statemachine_services")
	Q_INTERFACES(ctkPluginActivator)
public:
	org_mitk_lancet_statemachine_services_Activator();
public:
	static const std::string PLUGIN_ID;

	virtual ~org_mitk_lancet_statemachine_services_Activator() override;

	void start(ctkPluginContext* context) override;

	void stop(ctkPluginContext* context) override;

	static ctkPluginContext* GetPluginContext();
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

	typedef org_mitk_lancet_statemachine_services_Activator PluginActivator;
}

#endif // !LancetPluginActivator_H