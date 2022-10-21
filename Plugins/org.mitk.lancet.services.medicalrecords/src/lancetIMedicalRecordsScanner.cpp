#include "lancetIMedicalRecordsScanner.h"

#include <QDir>

#include "lancetIMedicalRecordsProperty.h"

namespace lancet
{
	struct IMedicalRecordsScanner::IMedicalRecordsScannerPrivateImp
	{
		QDir dir;
		State state;
		bool isRunning;
		int interval;
		RunTimeMode runtimeMode;
		QMedicalRecordsProperty error;

		QMedicalRecordsPropertyArray listValidPropertyArray;
		QMedicalRecordsPropertyArray listInvalidPropertyArray;
	};

	IMedicalRecordsScanner::IMedicalRecordsScanner()
		: imp(std::make_shared<IMedicalRecordsScannerPrivateImp>())
	{
		this->SetInterval(100);
		this->SetState(Downtime);
		this->SetRunTimeMode(Thread);
		this->SetDirectory(QDir::currentPath());
	}
	QDir IMedicalRecordsScanner::GetDirectory() const
	{
		return this->imp->dir;
	}
	void IMedicalRecordsScanner::SetDirectory(const QDir& dir)
	{
		this->imp->dir = dir;
	}
	void IMedicalRecordsScanner::SetInterval(int i)
	{
		this->imp->interval = i;
	}
	int IMedicalRecordsScanner::GetInterval() const
	{
		return this->imp->interval;
	}
	IMedicalRecordsScanner::State IMedicalRecordsScanner::GetState() const
	{
		return this->imp->state;
	}
	IMedicalRecordsScanner::RunTimeMode IMedicalRecordsScanner::GetRunTimeMode() const
	{
		return this->imp->runtimeMode;
	}
	void IMedicalRecordsScanner::SetRunTimeMode(const RunTimeMode& mode)
	{
		if (!this->IsRunning())
		{
			this->imp->runtimeMode = mode;
		}
	}
	bool IMedicalRecordsScanner::IsRunning() const
	{
		return this->imp->isRunning;
	}
	QMedicalRecordsPropertyArray
		IMedicalRecordsScanner::GetValidPropertyArray() const
	{
		return this->imp->listValidPropertyArray;
	}
	QMedicalRecordsPropertyArray
		IMedicalRecordsScanner::GetInvalidPropertyArray() const
	{
		return this->imp->listInvalidPropertyArray;
	}
	bool IMedicalRecordsScanner::IsErrorProperty(const QMedicalRecordsProperty& o) const
	{
		return o.isNull();
	}
	QPair<int, QMedicalRecordsProperty&>
		IMedicalRecordsScanner::GetVectorQuoteProperty(QMedicalRecordsPropertyArray& vec,
			const QString& key)
	{
		static size_t len = 0;
		len = vec.size();
		for (size_t index = 0; index < len; ++index)
		{
			if (vec[index]->GetKeyValue(IMedicalRecordsProperty::PropertyKeys::Key) == key)
			{
				return QPair<int, QMedicalRecordsProperty&>(index, vec[index]);
			}
		}
		return QPair<int, QMedicalRecordsProperty&>(-1, this->imp->error);
	}
	QPair<int, QMedicalRecordsProperty&>
		IMedicalRecordsScanner::GetQuoteValidProperty(const QString& key)
	{
		return this->GetVectorQuoteProperty(this->GetQuoteValidPropertyArray(), key);
	}
	QPair<int, QMedicalRecordsProperty&>
		IMedicalRecordsScanner::GetQuoteInvalidProperty(const QString& key)
	{
		return this->GetVectorQuoteProperty(this->GetQuoteInvalidPropertyArray(), key);
	}
	QMedicalRecordsPropertyArray&
		IMedicalRecordsScanner::GetQuoteValidPropertyArray()
	{
		return this->imp->listValidPropertyArray;
	}
	QMedicalRecordsPropertyArray&
		IMedicalRecordsScanner::GetQuoteInvalidPropertyArray()
	{
		return this->imp->listInvalidPropertyArray;
	}
	void IMedicalRecordsScanner::SetRunning(bool running)
	{
		this->imp->isRunning = running;
	}
	void IMedicalRecordsScanner::SetState(const State& state)
	{
		this->imp->state = state;
	}
}