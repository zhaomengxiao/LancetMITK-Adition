#ifndef LancetPluginActivator_H
#define LancetPluginActivator_H

#include <ctkPluginActivator.h>

#include <berrySmartPointer.h>

#include <usServiceEvent.h>

#include "org_mitk_lancet_services_spatialfitting_Export.h"

namespace us
{
	class ModuleContext;
}

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN 
	org_mitk_lancet_services_SpatialFitting : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org_mitk_lancet_services_spatialfitting")
	Q_INTERFACES(ctkPluginActivator)
public:
	org_mitk_lancet_services_SpatialFitting();
public:
	static const std::string PLUGIN_ID;

	virtual ~org_mitk_lancet_services_SpatialFitting() override;

	void start(ctkPluginContext* context) override;

	void stop(ctkPluginContext* context) override;

	static ctkPluginContext* GetPluginContext();
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

	typedef org_mitk_lancet_services_SpatialFitting PluginActivator;
}

#endif // !LancetPluginActivator_H