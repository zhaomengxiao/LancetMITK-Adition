#include "lancetIMedicalRecordsAdministrationService.h"

#include <lancetIMedicalRecordsScanner.h>

namespace lancet
{
  struct IMedicalRecordsAdministrationService::IMedicalRecordsAdministrationServicePrivateImp
  {
		
		static berry::SmartPointer<IMedicalRecordsAdministrationService> staticInstance;

		QMedicalRecordsProperty selectMedicalRecordsItem;
		berry::SmartPointer<IMedicalRecordsScanner> medicalRecordsScanner;
  };
	berry::SmartPointer<IMedicalRecordsAdministrationService>
		IMedicalRecordsAdministrationService::IMedicalRecordsAdministrationServicePrivateImp::
		staticInstance = berry::SmartPointer<IMedicalRecordsAdministrationService>();

	IMedicalRecordsAdministrationService::IMedicalRecordsAdministrationService()
		: imp(std::make_shared<IMedicalRecordsAdministrationServicePrivateImp>())
	{
		this->imp->staticInstance = berry::SmartPointer(this);
	}
	berry::SmartPointer<IMedicalRecordsAdministrationService>
		IMedicalRecordsAdministrationService::GetService()
	{
		return IMedicalRecordsAdministrationServicePrivateImp::staticInstance;
	}

	void IMedicalRecordsAdministrationService::SetScanner(
		berry::SmartPointer<IMedicalRecordsScanner> obj)
	{
		this->DisConnectToScanner(this->GetScanner());
		this->imp->medicalRecordsScanner = obj;
		this->ConnectToScanner(this->GetScanner());
	}
	berry::SmartPointer<IMedicalRecordsScanner> 
		IMedicalRecordsAdministrationService::GetScanner() const
	{
		return this->imp->medicalRecordsScanner;
	}
	void IMedicalRecordsAdministrationService::SetSelect(const QMedicalRecordsProperty& item)
	{
		this->imp->selectMedicalRecordsItem = item;
		emit this->MedicalRecordsPropertySelect(this->GetSelect().data());
	}
	QMedicalRecordsProperty IMedicalRecordsAdministrationService::GetSelect() const
	{
		return this->imp->selectMedicalRecordsItem;
	}
	void IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyTrace(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyTrace(index, data, valid);
	}
	void IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyModify(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyModify(index, data, valid);
	}
	void IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyDelete(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyDelete(index, data, valid);
	}
	void IMedicalRecordsAdministrationService::ConnectToScanner(
		const berry::SmartPointer<IMedicalRecordsScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IMedicalRecordsScanner* o = sender.GetPointer();
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyTrace);
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyModify,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyModify);
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyDelete,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyDelete);
		}
	}
	void IMedicalRecordsAdministrationService::DisConnectToScanner(
		const berry::SmartPointer<IMedicalRecordsScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IMedicalRecordsScanner* o = sender.GetPointer();
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyTrace);
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyModify,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyModify);
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyDelete,
				this, &IMedicalRecordsAdministrationService::Slot_MedicalRecordsPropertyDelete);
		}
	}
}