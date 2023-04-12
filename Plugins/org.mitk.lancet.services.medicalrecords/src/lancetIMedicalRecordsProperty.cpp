#include "lancetIMedicalRecordsProperty.h"

#include <QMap>
#include <QPair> 
#include <QVariant>

namespace lancet
{
  struct IMedicalRecordsProperty::IMedicalRecordsPropertyPrivateImp
  {
		// | key | [property, isModify] |
		QMap<QString, QPair<QVariant, bool>> mapProperties;

		QString directory;
  };
	using LancetPropertyKeys = IMedicalRecordsProperty::PropertyKeys;
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Key = "Key";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Name = "Name";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Id = "Id";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Sex = "Sex";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Age = "Age";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Valid = "Valid";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::OperatingSurgeonName = "OperatingSurgeonName";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::SurgicalArea = "SurgicalArea";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::Type = "Type";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::CreateTime = "CreateTime";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::LastModifyTime = "LastModifyTime";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::DataAddress = "DataAddress";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::DrPreviewAddress = "DrPreviewAddress";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::DrPositiveDicomAddress = "DrPositiveDicomAddress";
	LancetPropertyKeys::PropertyKey LancetPropertyKeys::DrLateralDicomAddress = "DrLateralDicomAddress";

	IMedicalRecordsProperty::IMedicalRecordsProperty()
		: imp(std::make_shared<IMedicalRecordsPropertyPrivateImp>())
	{
	}
	bool IMedicalRecordsProperty::HasKey(const QString& key) const
	{
		return this->imp->mapProperties.find(key) != this->imp->mapProperties.end();
	}
	bool IMedicalRecordsProperty::HasModifyOf(const QString& key) const
	{
		if (this->HasKey(key))
		{
			return this->GetModifyKeys().contains(key);
		}
		return false;
	}
	bool IMedicalRecordsProperty::HasModifyOf(const QList<QString>& keys) const
	{
		if (keys.isEmpty())
		{
			return false;
		}

		for (auto& item : keys)
		{
			if (true == this->HasModifyOf(item))
			{
				return true;
			}
		}	

		return false;
	}
	void IMedicalRecordsProperty::ModifyOf(const IMedicalRecordsProperty* o)
	{
		if (!o)
		{
			return;
		}

		for (auto& key : o->GetKeys())
		{
			if (key == PropertyKeys::Key)
			{
				continue;
			}
			if (this->HasKey(key))
			{
				if (this->GetKeyValue(key) != o->GetKeyValue(key))
				{
					this->SetKeyValue(key, o->GetKeyValue(key));
				}
			}
			else
			{
				this->SetKeyValue(key, o->GetKeyValue(key));
			}
		}
	}
	bool IMedicalRecordsProperty::ResetPropertyOfModify(const QString& key)
	{
		if (this->HasKey(key))
		{
			this->imp->mapProperties[key].second = false;
			return true;
		}
		return false;
	}
	bool IMedicalRecordsProperty::ResetPropertyOfModify(const QStringList& list)
	{
		bool retval = true;
		for (auto& key : list)
		{
			retval &= this->ResetPropertyOfModify(key);
		}
		return retval;
	}
	void IMedicalRecordsProperty::ModifyOf(const IMedicalRecordsProperty::Pointer& o)
	{ 
		return this->ModifyOf(o.GetPointer());
	}
	QList<QString> IMedicalRecordsProperty::GetKeys() const
	{
		return this->imp->mapProperties.keys();
	}
	QList<QString> IMedicalRecordsProperty::GetModifyKeys() const
	{
		QList<QString> listModify;

		for (auto& iterator = this->imp->mapProperties.begin();
			iterator != this->imp->mapProperties.end(); ++iterator)
		{
			if (iterator.value().second)
			{
				listModify.push_back(iterator.key());
			}
		}
		return listModify;
	}
	QVariant IMedicalRecordsProperty::GetKeyValue(const QString& key) const
	{
		if (this->HasKey(key))
		{
			return this->imp->mapProperties[key].first;
		}
		return QVariant();
	}
	void IMedicalRecordsProperty::SetKeyValue(const QString& key, const QVariant& value)
	{
		this->imp->mapProperties[key] = QPair<QVariant, bool>(value, true);
	}
	void IMedicalRecordsProperty::ResetPropertyOfModify()
	{
		for (auto& item : this->imp->mapProperties)
		{
			item.second = false;
		}
	}

	void IMedicalRecordsProperty::operator=(const IMedicalRecordsProperty& o)
	{
		this->imp = o.imp;
	}
	void IMedicalRecordsProperty::operator=(IMedicalRecordsProperty* o)
	{
		if (o)
		{
			this->imp = o->imp;
		}
	}
	void IMedicalRecordsProperty::operator=(IMedicalRecordsProperty::Pointer po)
	{
		if (po.IsNotNull())
		{
			this->imp = po->imp;
		}
	}
	bool IMedicalRecordsProperty::operator==(IMedicalRecordsProperty* po)
	{
		if (po)
		{
			return this->imp == po->imp;
		}
		return false;
	}
	bool IMedicalRecordsProperty::operator==(IMedicalRecordsProperty::Pointer po)
	{
		return this->operator==(po.GetPointer());
	}
	QList<QString> IMedicalRecordsProperty::PropertyKeys::ToQtList()
	{
		return QList<QString>() 
			<< LancetPropertyKeys::Key
			<< LancetPropertyKeys::Name
			<< LancetPropertyKeys::Id
			<< LancetPropertyKeys::Sex
			<< LancetPropertyKeys::Age
			<< LancetPropertyKeys::Valid
			<< LancetPropertyKeys::OperatingSurgeonName
			<< LancetPropertyKeys::SurgicalArea
			<< LancetPropertyKeys::Type
			<< LancetPropertyKeys::CreateTime
			<< LancetPropertyKeys::LastModifyTime
			<< LancetPropertyKeys::DataAddress
			<< LancetPropertyKeys::DrPreviewAddress
			<< LancetPropertyKeys::DrPositiveDicomAddress
			<< LancetPropertyKeys::DrLateralDicomAddress;
	}
	QString IMedicalRecordsProperty::GetDirectory() const
	{
		return this->imp->directory;
	}
	void IMedicalRecordsProperty::SetDirectory(const QString& dir)
	{
		this->imp->directory = dir;
	}
}