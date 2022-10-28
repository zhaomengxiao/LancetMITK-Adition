#ifndef QMedicalRecordInfoDialog_H
#define QMedicalRecordInfoDialog_H

#include <QDialog>
class QmitkStdMultiWidget;
class QMedicalRecordManagementEditorPrivate;

namespace lancet
{
	class IMedicalRecordsProperty;
	class IMedicalRecordsAdministrationService;
}
namespace Ui
{
	class QMedicalRecordInfoDialog;
}
class QMedicalRecordInfoDialog : public QDialog
{
    Q_OBJECT
public:
	QMedicalRecordInfoDialog(QWidget* parent = nullptr);
	~QMedicalRecordInfoDialog() override;
	
	void SetInfo(lancet::IMedicalRecordsProperty*);
	void clearData();
	bool createNew = false;
protected:
	QStringList GetKeys() const;
private slots:
	void SaveCaseDirectory();
	void SelectedCaseDirectory();
	void Cancel();
private:
	Ui::QMedicalRecordInfoDialog* m_Controls;
};

#endif // QMedicalRecordInfoDialog_H
