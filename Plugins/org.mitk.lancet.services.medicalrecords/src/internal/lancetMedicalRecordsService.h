#ifndef LancetMedicalRecordsAdministrationService_H
#define LancetMedicalRecordsAdministrationService_H

#include <lancetIMedicalRecordsService.h>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
#include "org_mitk_lancet_services_medicalrecords_Export.h"

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN 
	MedicalRecordsService
	: public IMedicalRecordsService
{
	Q_OBJECT
	Q_INTERFACES(lancet::IMedicalRecordsService)
public:
	berryObjectMacro(lancet::IMedicalRecordsProperty);
	MedicalRecordsService();
public:
	virtual void Start() override;
	virtual void Stop() override;
};
}

#endif // !LancetMedicalRecordsAdministrationService_H
