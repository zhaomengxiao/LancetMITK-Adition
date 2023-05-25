#ifndef LancetPluginActivator_H
#define LancetPluginActivator_H

#include <ctkPluginActivator.h>

#include <berrySmartPointer.h>

#include <usServiceEvent.h>

#include <lancetSpatialFittingGlobal.h>

namespace us
{
	class ModuleContext;
}

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN 
	org_mitk_lancet_services_SpatialFitting_ex : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org_mitk_lancet_services_spatialfitting_ex")
	Q_INTERFACES(ctkPluginActivator)
public:
	org_mitk_lancet_services_SpatialFitting_ex();
public:
	static const std::string PLUGIN_ID;

	virtual ~org_mitk_lancet_services_SpatialFitting_ex() override;

	void start(ctkPluginContext* context) override;

	void stop(ctkPluginContext* context) override;

	static ctkPluginContext* GetPluginContext();
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

}
typedef lancet::org_mitk_lancet_services_SpatialFitting_ex PluginActivator;

#endif // !LancetPluginActivator_H