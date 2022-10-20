#ifndef LancetMedicalRecordsAdministrationService_H
#define LancetMedicalRecordsAdministrationService_H

#include <lancetIMedicalRecordsAdministrationService.h>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_medicalrecordsadministration_Export.h"

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN 
	MedicalRecordsAdministrationService
	: public IMedicalRecordsAdministrationService
{
	Q_OBJECT
	Q_INTERFACES(lancet::IMedicalRecordsAdministrationService)
public:
	berryObjectMacro(lancet::IMedicalRecordsProperty);
	MedicalRecordsAdministrationService();
public:
	virtual void Start() override;
	virtual void Stop() override;
};
}

#endif // !LancetMedicalRecordsAdministrationService_H
