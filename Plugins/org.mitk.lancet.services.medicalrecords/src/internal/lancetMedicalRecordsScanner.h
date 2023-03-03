#ifndef LancetMedicalRecordsScanner_H
#define LancetMedicalRecordsScanner_H

#define MITK_BUILD_TESTTING
#include <lancetIMedicalRecordsScanner.h>

namespace lancet
{
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
	MedicalRecordsScanner: public IMedicalRecordsScanner
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::MedicalRecordsScanner);
	MedicalRecordsScanner();
public:
	virtual void Stop() override;
	virtual void Start() override;
public Q_SLOTS:
	void onTimerTimeout();
#ifdef MITK_BUILD_TESTTING
public:
#else
protected:
#endif // !MITK_BUILD_TESTTING
	virtual bool TryAnalysisTargetFile(const QString&);
	virtual void RunningScanner(const QString&, const QStringList&);
	static void onThreadHandle(MedicalRecordsScanner*);
	static void onThreadDataSyncHandle(MedicalRecordsScanner*);
//private:
	void UpdateProperty();
	void UpdateValidProperty(const QMedicalRecordsPropertyArray&);
	void UpdateInvalidProperty(const QMedicalRecordsPropertyArray&);
private:
	struct MedicalRecordsScannerPrivateImp;
	std::shared_ptr<MedicalRecordsScannerPrivateImp> imp;
};
}

#endif // !LancetMedicalRecordsScanner_H