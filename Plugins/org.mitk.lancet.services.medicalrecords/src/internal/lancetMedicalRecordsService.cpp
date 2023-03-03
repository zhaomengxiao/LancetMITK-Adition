#include "lancetMedicalRecordsService.h"

#include <internal/lancetMedicalRecordsScanner.h>

namespace lancet
{
	MedicalRecordsService::MedicalRecordsService()
	{
		MedicalRecordsScanner::Pointer scannar
			= MedicalRecordsScanner::Pointer(new MedicalRecordsScanner);

		this->SetScanner(scannar);
	}

	void MedicalRecordsService::Start()
	{
		if (this->GetScanner().IsNotNull())
		{
			this->GetScanner()->Start();
		}
	}

	void MedicalRecordsService::Stop()
	{
		if (this->GetScanner().IsNotNull())
		{
			this->GetScanner()->Stop();
		}
	}
}


