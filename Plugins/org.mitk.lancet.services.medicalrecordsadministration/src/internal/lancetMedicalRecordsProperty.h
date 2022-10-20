#ifndef LancetMedicalRecordsProperty_H
#define LancetMedicalRecordsProperty_H

#include "lancetIMedicalRecordsProperty.h"

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
	MedicalRecordsProperty : public IMedicalRecordsProperty
{
public:
	berryObjectMacro(lancet::MedicalRecordsProperty);
	MedicalRecordsProperty();
	MedicalRecordsProperty(const QString&);
	MedicalRecordsProperty(const MedicalRecordsProperty&);
	MedicalRecordsProperty(const MedicalRecordsProperty*);
	MedicalRecordsProperty(MedicalRecordsProperty::Pointer);

public:
	virtual QString ToString() const override;
	virtual QString ToJsonString() const override;
public:
	virtual void initializeKeys() override;
	virtual void operator=(MedicalRecordsProperty*);
	virtual void operator=(MedicalRecordsProperty::Pointer);
	virtual bool operator==(MedicalRecordsProperty*);
	virtual bool operator==(MedicalRecordsProperty::Pointer);
	virtual bool operator==(const IMedicalRecordsProperty&) override;
};
}

#endif // !LancetMedicalRecordsProperty_H
