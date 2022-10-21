#ifndef LancetPluginActivator_H
#define LancetPluginActivator_H

#include <ctkPluginActivator.h>

#include <berrySmartPointer.h>

#include <usServiceEvent.h>

#include "org_mitk_lancet_services_medicalrecords_Export.h"

namespace us
{
	class ModuleContext;
}

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN 
	org_mitk_lancet_services_MedicalRecordsAdministration : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org_mitk_lancet_services_medicalrecords")
	Q_INTERFACES(ctkPluginActivator)
public:
	org_mitk_lancet_services_MedicalRecordsAdministration();
public:
	static const std::string PLUGIN_ID;

	virtual ~org_mitk_lancet_services_MedicalRecordsAdministration() override;

	void start(ctkPluginContext* context) override;

	void stop(ctkPluginContext* context) override;

	static ctkPluginContext* GetPluginContext();
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

	typedef org_mitk_lancet_services_MedicalRecordsAdministration PluginActivator;
}

#endif // !LancetPluginActivator_H