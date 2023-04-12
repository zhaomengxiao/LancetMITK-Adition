#include "QMedicalRecordLoaderDialog.h"
#include "ui_QMedicalRecordLoaderDialog.h"

#include <QThread>

#include <mitkIOUtil.h>
#include <mitkProgressBar.h>
#include <mitkProgressBarImplementation.h>

class mitkFileProgressBar : public mitk::ProgressBarImplementation
{
public:
	//##Documentation
	//## @brief Constructor
	mitkFileProgressBar() 
	{
		loaderDialog = nullptr;
	};

	//##Documentation
	//## @brief Destructor
	virtual ~mitkFileProgressBar() 
	{
		loaderDialog = nullptr;
	};

	//##Documentation
	//## @brief Sets whether the current progress value is displayed.
	virtual void SetPercentageVisible(bool visible) override
	{
		MITK_WARN << "log.visible " << visible;
	}

	//##Documentation
	//## @brief Explicitly reset progress bar.
	virtual void Reset() override
	{
		m_count = 0;
		m_totalSteps = 0;
	}

	//##Documentation
	//## @brief Adds steps to totalSteps.
	virtual void AddStepsToDo(unsigned int steps) override
	{
		MITK_INFO << "log.steps " << steps;
		m_totalSteps += steps;
		if (nullptr != this->loaderDialog)
		{
			emit this->loaderDialog->updateProgressBar();
		}
	}

	//##Documentation
	//## @brief Sets the current amount of progress to current progress + steps.
	//## @param steps the number of steps done since last Progress(int steps) call.
	virtual void Progress(unsigned int steps) override
	{
		++m_count;
		if (nullptr != this->loaderDialog)
		{
			emit this->loaderDialog->updateProgressBar();
		}
	}

	int m_count = 0;
	int m_totalSteps = 0;

	QMedicalRecordLoaderDialog* loaderDialog;
};

struct QMedicalRecordLoaderDialog::QMedicalRecordLoaderDialogPrivateImp
{
	mitkFileProgressBar progressBar;

	mitk::DataStorage::Pointer dataStorage;
	QMedicalRecordsProperty medicalRecordsProperty;
};

QMedicalRecordLoaderDialog::QMedicalRecordLoaderDialog(QWidget* parent)
	: QDialog(parent)
	, m_Controls(new Ui::QMedicalRecordLoaderDialogFrom)
	, imp(std::make_shared<QMedicalRecordLoaderDialogPrivateImp>())
{
	m_Controls->setupUi(this);

	this->imp->progressBar.loaderDialog = this;
	connect(this, SIGNAL(updateProgressBar()), this, SLOT(on_updateProgressBar()));
	connect(this, &QMedicalRecordLoaderDialog::closeLoaderDialog, this, [=]() {
		this->close();
	});
	mitk::ProgressBar::GetInstance()->RegisterImplementationInstance(&this->imp->progressBar);
}

QMedicalRecordLoaderDialog::~QMedicalRecordLoaderDialog()
{
	mitk::ProgressBar::GetInstance()->UnregisterImplementationInstance(&this->imp->progressBar);
}

void QMedicalRecordLoaderDialog::on_updateProgressBar()
{
	this->m_Controls->progressBar->setMaximum(this->imp->progressBar.m_totalSteps);
	this->m_Controls->progressBar->setValue(this->imp->progressBar.m_count);
}

void QMedicalRecordLoaderDialog::SetMedicalRecordProperty(const QMedicalRecordsProperty& property)
{
	this->imp->medicalRecordsProperty = property;
}

QMedicalRecordsProperty QMedicalRecordLoaderDialog::GetMedicalRecordProperty() const
{
	return this->imp->medicalRecordsProperty;
}

void QMedicalRecordLoaderDialog::SetDataStorage(mitk::DataStorage::Pointer dataStorage)
{
	this->imp->dataStorage = dataStorage;
}

mitk::DataStorage::Pointer QMedicalRecordLoaderDialog::GetDataStorage() const
{
	return this->imp->dataStorage;
}

int QMedicalRecordLoaderDialog::exec()
{
	auto callback = std::function<void()>([&]()
	{
		try
		{
			if (this->GetDataStorage().IsNull() || this->GetMedicalRecordProperty().isNull())
			{
				throw std::exception("Input data is nullptr!");
			}

			this->imp->progressBar.Reset();

			auto mitkFilePath = this->GetMedicalRecordProperty()->GetKeyValue(QPropertyKeys::DataAddress);
			MITK_INFO << "try load file:///" << mitkFilePath.toString().toStdString();
			mitk::IOUtil::Load(mitkFilePath.toString().toStdString(),
				*this->GetDataStorage().GetPointer());
			// emit close event
			emit this->closeLoaderDialog();
		}
		catch (...)
		{
			auto mitkFilePath = this->GetMedicalRecordProperty()->GetKeyValue(QPropertyKeys::DataAddress);
			MITK_ERROR << "Failed to load *.mitk file! see file:///" << mitkFilePath.toString().toStdString();
		}
	});

	auto thread_callback = QThread::create(callback);
	if (thread_callback)
	{
		thread_callback->start();
	}

	return __super::exec();
}