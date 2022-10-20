#include "lancetMedicalRecordsLoader.h"

#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QJsonValue> 
#include <QJsonObject> 
#include <QJsonDocument>
#include <QJsonParseError>

#include "lancetMedicalRecordsProperty.h"

namespace lancet
{
  struct MedicalRecordsLoader::MedicalRecordsLoaderPrivateImp
  {
		enum ExtendedError
		{
			NoError = 0,
			Unknown = 0x1000000,
			NotFile,
			FormatNotJson
		};

		ExtendedError errorFlag = ExtendedError::NoError;
		QFileDevice::FileError fileError = QFileDevice::FileError::NoError;
		::QJsonParseError jsonError;
  };
	MedicalRecordsLoader::MedicalRecordsLoader()
		: imp(std::make_shared<MedicalRecordsLoaderPrivateImp>())
	{
		this->imp->jsonError.error = ::QJsonParseError::NoError;
	}
	QString MedicalRecordsLoader::GetErrorString() const
	{
		return 
			this->imp->errorFlag == MedicalRecordsLoaderPrivateImp::ExtendedError::NoError ?
			this->imp->jsonError.errorString() : this->GetFileError();
	}
	int MedicalRecordsLoader::GetJsonParseError() const
	{
		return 
			this->imp->errorFlag == MedicalRecordsLoaderPrivateImp::ExtendedError::NoError ? 
			this->imp->jsonError.error : this->imp->errorFlag;
	}

	bool MedicalRecordsLoader::IsFileError() const
	{
		return this->imp->errorFlag != MedicalRecordsLoaderPrivateImp::NoError ||
			this->imp->fileError != QFileDevice::FileError::NoError;
	}

	bool MedicalRecordsLoader::IsJsonError() const
	{
		return this->imp->jsonError.error != QJsonParseError::ParseError::NoError;
	}

	bool MedicalRecordsLoader::CheckInputFile(const QString& fp) const
	{
		if(false == QFile::exists(fp))
		{
			this->imp->errorFlag = MedicalRecordsLoaderPrivateImp::NotFile;
			return false;
		}

		static QFileInfo fpInfo;
		fpInfo.setFile(fp);
		if (false == fpInfo.isFile() || fpInfo.suffix().toLower() != "json")
		{
			this->imp->errorFlag = MedicalRecordsLoaderPrivateImp::FormatNotJson;
			return false;
		}

		return true;
	}

	berry::SmartPointer<IMedicalRecordsProperty> MedicalRecordsLoader::GetOutput()
	{
		IMedicalRecordsProperty::Pointer retval;

		if (this->CheckInputFile(this->GetFileName()))
		{
			QFile fp(this->GetFileName());
			if (false == fp.open(QIODevice::ReadOnly))
			{
				this->imp->fileError = fp.error();
				fp.close();
				return retval;
			}

			retval = this->AnalysisTargetFile(fp.readAll());
			this->imp->errorFlag = MedicalRecordsLoaderPrivateImp::ExtendedError::NoError;
			fp.close();
		}

		return retval;
	}
	QString MedicalRecordsLoader::GetFileError() const
	{
		if (this->imp->errorFlag != MedicalRecordsLoaderPrivateImp::NoError)
		{
			switch (this->imp->errorFlag)
			{
			case MedicalRecordsLoaderPrivateImp::NotFile:
				return QString("Target file not found. see %1").arg(this->GetFileName());
			case MedicalRecordsLoaderPrivateImp::FormatNotJson:
				return QString("The target file is not of Json format type. see %1").arg(this->GetFileName());
			}
		}
		else if(this->imp->fileError != QFileDevice::FileError::NoError)
		{
			switch (this->imp->fileError)
			{
			case QFileDevice::FileError::ReadError:
				return "Error reading file";
			case QFileDevice::FileError::WriteError:
				return "Error writing file";
			case QFileDevice::FileError::FatalError:
				return "Fatal error occurred";
			case QFileDevice::FileError::ResourceError:
				return "Insufficient resources (such as too many open files, insufficient memory, etc.)";
			case QFileDevice::FileError::OpenError:
				return "File cannot be opened";
			case QFileDevice::FileError::AbortError:
				return "Operation aborted";
			case QFileDevice::FileError::TimeOutError:
				return "Operation timeout";
			case QFileDevice::FileError::UnspecifiedError:
				return "Unspecified error";
			case QFileDevice::FileError::RemoveError:
				return "Cannot delete file";
			case QFileDevice::FileError::RenameError:
				return "Cannot rename file";
			case QFileDevice::FileError::PositionError:
				return "The location in the file cannot be changed";
			case QFileDevice::FileError::ResizeError:
				return "Unable to resize file";
			case QFileDevice::FileError::PermissionsError:
				return "Unable to access file";
			case QFileDevice::FileError::CopyError:
				return "Cannot copy file";
			}
		}
		return "unknown error.";
	}
	berry::SmartPointer<IMedicalRecordsProperty> 
		MedicalRecordsLoader::AnalysisTargetFile(const QByteArray& bytes) const
	{
		auto tmpBytesArray = QTextCodec::codecForName("GBK")->toUnicode(bytes);
		auto jsonDocObject = QJsonDocument::fromJson(tmpBytesArray.toUtf8(), &this->imp->jsonError);
		QJsonObject obj = jsonDocObject.object();

		MedicalRecordsProperty* retval = new MedicalRecordsProperty;

		// json format error.
		if (this->imp->jsonError.error != QJsonParseError::NoError)
		{
			return berry::SmartPointer<IMedicalRecordsProperty>(retval);
		}

		// unknow json file.
		if (!obj.value("properties").isObject())
		{
			this->imp->errorFlag = MedicalRecordsLoaderPrivateImp::FormatNotJson;
			return berry::SmartPointer<IMedicalRecordsProperty>(retval);
		}

		auto obj_properties = obj["properties"].toObject();
		auto keysList = retval->GetKeys();
		for (auto& key : keysList)
		{
			retval->SetKeyValue(key, obj_properties[key].toVariant());
		}

		return berry::SmartPointer<IMedicalRecordsProperty>(retval);
	}

	void MedicalRecordsLoader::Reset()
	{
		this->imp->errorFlag = MedicalRecordsLoaderPrivateImp::NoError;
		this->imp->fileError = QFileDevice::FileError::NoError;
		this->imp->jsonError.error = QJsonParseError::NoError;
	}
}