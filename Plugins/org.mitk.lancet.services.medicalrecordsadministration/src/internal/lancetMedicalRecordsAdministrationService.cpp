#include "lancetMedicalRecordsAdministrationService.h"

#include <internal/lancetMedicalRecordsScanner.h>

namespace lancet
{
	MedicalRecordsAdministrationService::MedicalRecordsAdministrationService()
	{
		MedicalRecordsScanner::Pointer scannar
			= MedicalRecordsScanner::Pointer(new MedicalRecordsScanner);

		this->SetScanner(scannar);
	}

	void MedicalRecordsAdministrationService::Start()
	{
		if (this->GetScanner().IsNotNull())
		{
			this->GetScanner()->Start();
		}
	}

	void MedicalRecordsAdministrationService::Stop()
	{
		if (this->GetScanner().IsNotNull())
		{
			this->GetScanner()->Stop();
		}
	}
}


