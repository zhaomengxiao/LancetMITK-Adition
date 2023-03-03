#include "lancetIMedicalRecordsService.h"

#include <lancetIMedicalRecordsScanner.h>

namespace lancet
{
  struct IMedicalRecordsService::IMedicalRecordsServicePrivateImp
  {
		
		static berry::SmartPointer<IMedicalRecordsService> staticInstance;

		QMedicalRecordsProperty selectMedicalRecordsItem;
		berry::SmartPointer<IMedicalRecordsScanner> medicalRecordsScanner;
  };
	berry::SmartPointer<IMedicalRecordsService>
		IMedicalRecordsService::IMedicalRecordsServicePrivateImp::
		staticInstance = berry::SmartPointer<IMedicalRecordsService>();

	IMedicalRecordsService::IMedicalRecordsService()
		: imp(std::make_shared<IMedicalRecordsServicePrivateImp>())
	{
		this->imp->staticInstance = berry::SmartPointer(this);
	}
	berry::SmartPointer<IMedicalRecordsService>
		IMedicalRecordsService::GetService()
	{
		return IMedicalRecordsServicePrivateImp::staticInstance;
	}

	void IMedicalRecordsService::SetScanner(
		berry::SmartPointer<IMedicalRecordsScanner> obj)
	{
		this->DisConnectToScanner(this->GetScanner());
		this->imp->medicalRecordsScanner = obj;
		this->ConnectToScanner(this->GetScanner());
	}
	berry::SmartPointer<IMedicalRecordsScanner> 
		IMedicalRecordsService::GetScanner() const
	{
		return this->imp->medicalRecordsScanner;
	}
	void IMedicalRecordsService::SetSelect(const QMedicalRecordsProperty& item)
	{
		this->imp->selectMedicalRecordsItem = item;
		emit this->MedicalRecordsPropertySelect(this->GetSelect().data());
	}
	QMedicalRecordsProperty IMedicalRecordsService::GetSelect() const
	{
		return this->imp->selectMedicalRecordsItem;
	}
	void IMedicalRecordsService::Slot_MedicalRecordsPropertyTrace(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyTrace(index, data, valid);
	}
	void IMedicalRecordsService::Slot_MedicalRecordsPropertyModify(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyModify(index, data, valid);
	}
	void IMedicalRecordsService::Slot_MedicalRecordsPropertyDelete(
		int index, lancet::IMedicalRecordsProperty* data, bool valid)
	{
		emit this->MedicalRecordsPropertyDelete(index, data, valid);
	}
	void IMedicalRecordsService::ConnectToScanner(
		const berry::SmartPointer<IMedicalRecordsScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IMedicalRecordsScanner* o = sender.GetPointer();
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyTrace);
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyModify,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyModify);
			QObject::connect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyDelete,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyDelete);
		}
	}
	void IMedicalRecordsService::DisConnectToScanner(
		const berry::SmartPointer<IMedicalRecordsScanner>& sender) const
	{
		if (sender.IsNotNull())
		{
			IMedicalRecordsScanner* o = sender.GetPointer();
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyTrace,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyTrace);
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyModify,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyModify);
			QObject::disconnect(o, &IMedicalRecordsScanner::MedicalRecordsPropertyDelete,
				this, &IMedicalRecordsService::Slot_MedicalRecordsPropertyDelete);
		}
	}
}