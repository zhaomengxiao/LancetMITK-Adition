#include "lancetMedicalRecordsScanner.h"

#include <QDir>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QDirIterator>
#include <QReadWriteLock>
#include <QSharedPointer>

#include <memory>

#include <lancetIMedicalRecordsProperty.h>
#include <internal/lancetMedicalRecordsLoader.h>
#include <internal/lancetMedicalRecordsProperty.h>

namespace lancet
{
	struct MedicalRecordsScanner::MedicalRecordsScannerPrivateImp
	{
		QSharedPointer<QTimer> timer;
		QReadWriteLock mutex;
		
		QMedicalRecordsPropertyArray tempVectorValidPropertyArray;
		QMedicalRecordsPropertyArray tempVectorInValidPropertyArray;
	};

	MedicalRecordsScanner::MedicalRecordsScanner()
		: imp(std::make_shared<MedicalRecordsScannerPrivateImp>())
	{
		this->SetInterval(100);
		this->SetRunTimeMode(IMedicalRecordsScanner::Thread);

		this->imp->timer.reset(new QTimer);
		connect(this->imp->timer.data(), &QTimer::timeout, this, &MedicalRecordsScanner::onTimerTimeout);
	}
	void lancet::MedicalRecordsScanner::Stop()
	{
		if (this->GetRunTimeMode() == RunTimeMode::Timer)
		{
			this->imp->timer->stop();
			this->SetState(IMedicalRecordsScanner::Downtime);
		}
		this->SetRunning(false);
		while (this->GetState() != IMedicalRecordsScanner::State::Downtime)
		{
			QThread::msleep(1);
		}
	}
	void MedicalRecordsScanner::Start()
	{
		if (false == this->GetQuoteValidPropertyArray().isEmpty())
		{
			auto validPropertyArraySize = this->GetQuoteValidPropertyArray().size();
			for (auto index = 0; index < validPropertyArraySize; ++index)
			{
				emit this->MedicalRecordsPropertyTrace(index,
					this->GetQuoteValidPropertyArray()[index].data(), true);
			}
		}
		switch (this->GetRunTimeMode())
		{
		case IMedicalRecordsScanner::RunTimeMode::Thread:
		{
			auto qtthread = QThread::create(&MedicalRecordsScanner::onThreadHandle, this);
			//auto qtthreadDataSync = QThread::create(&MedicalRecordsScanner::onThreadDataSyncHandle, this);
			if (qtthread)
			{
				qtthread->start(QThread::HighestPriority);
				//if (qtthreadDataSync)
				//{
				//	qtthreadDataSync->start(QThread::NormalPriority);
				//}
				goto UPDATES_PROPERTIES;
			}
		}
		break;
		case IMedicalRecordsScanner::RunTimeMode::Timer:
		{
			this->imp->timer->start(this->GetInterval());
			goto UPDATES_PROPERTIES;
		}
		break;
		}
		return;
	UPDATES_PROPERTIES:
		this->SetRunning(true);
		this->SetState(IMedicalRecordsScanner::Initialize);
	}
	void MedicalRecordsScanner::onTimerTimeout()
	{
		this->RunningScanner(this->GetDirectory().absolutePath(), { "*.json" });
		this->UpdateProperty();

		this->imp->tempVectorValidPropertyArray.clear();
		this->imp->tempVectorInValidPropertyArray.clear();
	}
	bool MedicalRecordsScanner::TryAnalysisTargetFile(const QString& file)
	{
		static MedicalRecordsLoader loader;
		loader.Reset();
		loader.SetFileName(file);
		IMedicalRecordsProperty::Pointer property = loader.GetOutput();
		if (loader.GetJsonParseError() != 0)
		{
			return false;
		}
		//property->ResetPropertyOfModify();
		
		property->SetDirectory(QFileInfo(loader.GetFileName()).absoluteDir().path());
		// Push element into cache.
		if (this->GetRunTimeMode() == IMedicalRecordsScanner::Thread)
		{
			this->imp->mutex.lockForWrite();
		}

		QSharedPointer<MedicalRecordsProperty> ptr(
			new MedicalRecordsProperty(property.Cast<MedicalRecordsProperty>()));

		if (ptr->GetKeyValue(MedicalRecordsProperty::PropertyKeys::Valid).toBool())
		{
			this->imp->tempVectorValidPropertyArray.push_back(ptr);
		}
		else
		{
			this->imp->tempVectorInValidPropertyArray.push_back(ptr);
		}
		if (this->GetRunTimeMode() == IMedicalRecordsScanner::Thread)
		{
			this->imp->mutex.unlock();
		}
		return true;
	}
	void MedicalRecordsScanner::RunningScanner(const QString& dir, const QStringList& suffix)
	{
		QDir __directory(dir);

		if (false == __directory.exists())
		{
			return;
		}

		QDirIterator dirIterator(__directory.absolutePath(),
			QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

		while (dirIterator.hasNext())
		{
			QString dirAbsolutePath = dirIterator.next();

			// file
			if (QFileInfo(dirAbsolutePath).isFile())
			{
				this->TryAnalysisTargetFile(dirAbsolutePath);
			}
		}
	}
	void MedicalRecordsScanner::onThreadHandle(MedicalRecordsScanner* __this)
	{
		if (nullptr == __this)
		{
			return;
		}

		__this->RunningScanner(__this->GetDirectory().absolutePath(), {"*.json"});
		__this->SetState(IMedicalRecordsScanner::Patroling);
		__this->UpdateProperty();

		while (__this->IsRunning())
		{
			QThread::msleep(__this->GetInterval());
			__this->RunningScanner(__this->GetDirectory().absolutePath(), { "*.json" });
			__this->UpdateProperty();
		}
		__this->SetState(Downtime);
	}

	void MedicalRecordsScanner::onThreadDataSyncHandle(MedicalRecordsScanner* __this)
	{
		if (nullptr == __this)
		{
			return;
		}

		while (__this->IsRunning())
		{
			if (__this->imp->tempVectorValidPropertyArray.isEmpty() &&
				__this->imp->tempVectorInValidPropertyArray.isEmpty())
			{
				QThread::msleep(10);
			}

			__this->UpdateProperty();
		}
	}
	void MedicalRecordsScanner::UpdateProperty()
	{
		using PropertyKeys = IMedicalRecordsProperty::PropertyKeys;

		QMedicalRecordsPropertyArray tempValidVectorPropertyArray(
			this->imp->tempVectorValidPropertyArray.size());
		QMedicalRecordsPropertyArray tempInvalidVectorPropertyArray(
			this->imp->tempVectorInValidPropertyArray.size());

		// copy datas of buffer
		{
			if (this->GetRunTimeMode() == IMedicalRecordsScanner::Thread)
			{
				this->imp->mutex.lockForWrite();
			}
			tempValidVectorPropertyArray = this->imp->tempVectorValidPropertyArray;
			tempInvalidVectorPropertyArray = this->imp->tempVectorInValidPropertyArray;
			this->imp->tempVectorValidPropertyArray.clear();
			this->imp->tempVectorInValidPropertyArray.clear();

			if (this->GetRunTimeMode() == IMedicalRecordsScanner::Thread)
			{
				this->imp->mutex.unlock();
			}
		}

		this->UpdateValidProperty(tempValidVectorPropertyArray);
		this->UpdateInvalidProperty(tempInvalidVectorPropertyArray);
		tempValidVectorPropertyArray.clear();
		tempInvalidVectorPropertyArray.clear();
	}
	void MedicalRecordsScanner::UpdateValidProperty(const QMedicalRecordsPropertyArray& propertyArray)
	{
		using PropertyKeys = IMedicalRecordsProperty::PropertyKeys;

		for (auto& item : propertyArray)
		{
			auto copyQuoteValidItem = this->GetQuoteValidProperty(item->
				GetKeyValue(PropertyKeys::Key).toString());
			auto copyQuoteInvalidItem = this->GetQuoteInvalidProperty(item->
				GetKeyValue(PropertyKeys::Key).toString());

			bool isInvalidModifyToValid = false;
			if (this->IsErrorProperty(copyQuoteValidItem.second))
			{
				// New
				isInvalidModifyToValid = true;
				this->GetQuoteValidPropertyArray().push_back(item);
				emit this->MedicalRecordsPropertyTrace(this->GetQuoteValidPropertyArray().size() - 1,
					this->GetQuoteValidPropertyArray().back().data(), true);
			}
			else
			{
				copyQuoteValidItem.second->ModifyOf(item.data());
				if (copyQuoteValidItem.second->HasModifyOf(PropertyKeys::ToQtList()))
				{
					// Modify
					emit this->MedicalRecordsPropertyModify(copyQuoteValidItem.first, 
						copyQuoteValidItem.second.data(), true);
				}
			}

			if (false == this->IsErrorProperty(copyQuoteInvalidItem.second))
			{
				int removeIndex = copyQuoteInvalidItem.first;//this->GetQuoteInvalidPropertyArray().indexOf(copyQuoteInvalidItem);
				// Remove $removeIndex$
				if (-1 != removeIndex)
				{
					if (isInvalidModifyToValid)
					{
						emit this->MedicalRecordsPropertyDelete(copyQuoteInvalidItem.first,
							this->GetQuoteValidPropertyArray().back().data(), false);
					}
					this->GetQuoteInvalidPropertyArray().remove(removeIndex);
				}
			}
		}
	}
	void MedicalRecordsScanner::UpdateInvalidProperty(const QMedicalRecordsPropertyArray& propertyArray)
	{
		using PropertyKeys = IMedicalRecordsProperty::PropertyKeys;

		for (auto& item : propertyArray)
		{
			auto copyQuoteValidItem = this->GetQuoteValidProperty(item->
				GetKeyValue(PropertyKeys::Key).toString());
			auto copyQuoteInvalidItem = this->GetQuoteInvalidProperty(item->
				GetKeyValue(PropertyKeys::Key).toString());

			bool isValidModifyToInvalid = false;
			if (this->IsErrorProperty(copyQuoteInvalidItem.second))
			{
				// New
				isValidModifyToInvalid = true;
				this->GetQuoteInvalidPropertyArray().push_back(item);
				emit this->MedicalRecordsPropertyTrace(this->GetQuoteInvalidPropertyArray().size() - 1,
					this->GetQuoteInvalidPropertyArray().back().data(), false);
			}
			else
			{
				copyQuoteInvalidItem.second->ModifyOf(item.data());
				if (copyQuoteInvalidItem.second->HasModifyOf(PropertyKeys::ToQtList()))
				{
					// Modify
					emit this->MedicalRecordsPropertyModify(copyQuoteInvalidItem.first,
						copyQuoteInvalidItem.second.data(), false);
				}
			}

			if (false == this->IsErrorProperty(copyQuoteValidItem.second))
			{
				int removeIndex = copyQuoteValidItem.first; //this->GetQuoteValidPropertyArray().indexOf(copyQuoteValidItem);
				// Remove $removeIndex$
				if (-1 != removeIndex)
				{
					if (isValidModifyToInvalid)
					{
						emit this->MedicalRecordsPropertyDelete(removeIndex,
							this->GetQuoteInvalidPropertyArray().back().data(), true);
					}

					this->GetQuoteValidPropertyArray().remove(removeIndex);
				}
			}
		}
	}
}