#include "lancetMedicalRecordsProperty.h"

#include <QUuid>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "lancetMedicalRecordsLoader.h"

namespace lancet
{
	MedicalRecordsProperty::MedicalRecordsProperty()
	{
		this->initializeKeys();
	}

	MedicalRecordsProperty::MedicalRecordsProperty(const QString& jsonByteArray)
	{
		this->initializeKeys();

		MedicalRecordsLoader loader;
		//QVariant tmpKey = this->GetKeyValue(PropertyKeys::Key);
		this->IMedicalRecordsProperty::operator=(loader.AnalysisTargetFile(jsonByteArray.toLatin1()));
		//this->SetKeyValue(PropertyKeys::Key, tmpKey);
	}

	MedicalRecordsProperty::MedicalRecordsProperty(const MedicalRecordsProperty& o)
	{
		this->operator=(o);
	}

	MedicalRecordsProperty::MedicalRecordsProperty(const MedicalRecordsProperty* o)
	{
		this->IMedicalRecordsProperty::operator=(*o);
	}

	MedicalRecordsProperty::MedicalRecordsProperty(MedicalRecordsProperty::Pointer o)
	{
		this->IMedicalRecordsProperty::operator=(o);
	}

	QString MedicalRecordsProperty::ToString() const
	{
		QString info;

		for (auto key : this->GetKeys())
		{
			info += QString::asprintf("%.20s: %s:%s\n", key.toLatin1().constData(),
				(this->HasModifyOf(key) ? "true" : "false"),
				this->GetKeyValue(key).toString().toLatin1().data());
		}
		return info;
	}

	QString MedicalRecordsProperty::ToJsonString() const
	{
		QJsonObject __obj;
		QJsonObject __parameters_obj;

		for (auto key : PropertyKeys::ToQtList())
		{
			__parameters_obj.insert(key, QJsonValue(this->GetKeyValue(key).toString()));
		}

		__obj.insert("properties", __parameters_obj);
		return QJsonDocument::fromJson(QJsonDocument(__obj).toJson()).toJson();
	}

	void MedicalRecordsProperty::initializeKeys()
	{
		this->SetKeyValue(PropertyKeys::Key, QUuid::createUuid().toString());
		this->SetKeyValue(PropertyKeys::Name, "");
		this->SetKeyValue(PropertyKeys::Id, "");
		this->SetKeyValue(PropertyKeys::Sex, "");
		this->SetKeyValue(PropertyKeys::Age, "");
		this->SetKeyValue(PropertyKeys::Valid, "");
		this->SetKeyValue(PropertyKeys::OperatingSurgeonName, "");
		this->SetKeyValue(PropertyKeys::SurgicalArea, "");
		this->SetKeyValue(PropertyKeys::Type, "");
		this->SetKeyValue(PropertyKeys::CreateTime, "");
		this->SetKeyValue(PropertyKeys::LastModifyTime, "");
		this->SetKeyValue(PropertyKeys::DataAddress, "");
		this->SetKeyValue(PropertyKeys::DrPreviewAddress, "");
		this->SetKeyValue(PropertyKeys::DrPositiveDicomAddress, "");
		this->SetKeyValue(PropertyKeys::DrLateralDicomAddress, "");
	}
	void MedicalRecordsProperty::operator=(MedicalRecordsProperty* p)
	{
		return this->IMedicalRecordsProperty::operator=(p);
	}
	void MedicalRecordsProperty::operator=(MedicalRecordsProperty::Pointer po)
	{
		return this->IMedicalRecordsProperty::operator=(po);
	}
	bool MedicalRecordsProperty::operator==(MedicalRecordsProperty* p)
	{
		return this->IMedicalRecordsProperty::operator==(p);
	}
	bool MedicalRecordsProperty::operator==(MedicalRecordsProperty::Pointer po)
	{
		return this->IMedicalRecordsProperty::operator==(po.GetPointer());
	}
	bool MedicalRecordsProperty::operator==(const IMedicalRecordsProperty& o)
	{
		bool retval = true;

		for (auto item : this->GetKeys())
		{
			retval &= this->GetKeyValue(item) == o.GetKeyValue(item);
		}
		for (auto item : this->GetModifyKeys())
		{
			retval &= this->HasModifyOf(item) == o.HasModifyOf(item);
		}
		return retval;
	}
}