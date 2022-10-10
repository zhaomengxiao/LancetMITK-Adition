
#ifndef org_mitk_lancet_tha_application_Activator_H
#define org_mitk_lancet_tha_application_Activator_H

#include <ctkPluginActivator.h>

class org_mitk_lancet_tha_application_Activator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org_mitk_lancet_tha_application")
	Q_INTERFACES(ctkPluginActivator)

public:
	void start(ctkPluginContext* context) override;
	void stop(ctkPluginContext* context) override;

	static ctkPluginContext* GetPluginContext();
protected:
	static ctkPluginContext* staticPluginContext;
};

typedef org_mitk_lancet_tha_application_Activator PluginActivator;
#endif // org_mitk_lancet_tha_application_Activator_H