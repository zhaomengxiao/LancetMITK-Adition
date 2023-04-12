#ifndef QMedicalRecordLoaderDialog_H
#define QMedicalRecordLoaderDialog_H

#include <QDialog>
#include <mitkDataStorage.h>
#include <lancetIMedicalRecordsProperty.h>

namespace Ui
{
	class QMedicalRecordLoaderDialogFrom;
}

class QMedicalRecordLoaderDialog : public QDialog
{
	Q_OBJECT
public:
	QMedicalRecordLoaderDialog(QWidget* = nullptr);
	virtual ~QMedicalRecordLoaderDialog();

Q_SIGNALS:
	void updateProgressBar();
	void closeLoaderDialog();
public Q_SLOTS:
	void on_updateProgressBar();
public:
	virtual void SetMedicalRecordProperty(const QMedicalRecordsProperty&);
	virtual QMedicalRecordsProperty GetMedicalRecordProperty() const;

	virtual void SetDataStorage(mitk::DataStorage::Pointer);
	virtual mitk::DataStorage::Pointer GetDataStorage() const;
public slots:
	virtual int exec() override;
private:
	Ui::QMedicalRecordLoaderDialogFrom* m_Controls;

	struct QMedicalRecordLoaderDialogPrivateImp;
	std::shared_ptr<QMedicalRecordLoaderDialogPrivateImp> imp;
};

#endif // !QMedicalRecordLoaderDialog_H
