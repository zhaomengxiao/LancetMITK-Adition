/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \version V1.0.0
 * \data 2022-10-11 10:43:35
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 10:43:35
 * \remark $Modify Description$
 */
#ifndef LancetIMedicalRecordsScanner_H
#define LancetIMedicalRecordsScanner_H

#include <lancetIMedicalRecordsLoader.h>

 // c or c++
#include <memory>

// berry
#include <berryObject.h>

// qt
#include <QPair>
#include <QVector>
#include <QSharedPointer>

#include "lancetIMedicalRecordsProperty.h"
// ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_medicalrecordsadministration_Export.h"

class QDir;
namespace lancet
{
/**
 * \class IMedicalRecordsScanner
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \namespace lancet
 * \brief 
 *
 * TODO: long description
 *
 * \pre $Pre description information$
 * \example $Description of use examples$
 * \code
 *	todo...
 * \endcoed
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-10-11 10:43:16
 * \remark todo: insert comments
 *
 * Contact: sh4a01@163.com
 *
 */
class IMedicalRecordsProperty;
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
	IMedicalRecordsScanner : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IMedicalRecordsScanner);
	IMedicalRecordsScanner();

	enum State { Downtime, Initialize, Patroling };
	enum RunTimeMode { Timer, Thread};

	Q_ENUM(State)
	Q_ENUM(RunTimeMode)

Q_SIGNALS:
	void MedicalRecordsPropertyTrace(int, lancet::IMedicalRecordsProperty*, bool);
	void MedicalRecordsPropertyModify(int, lancet::IMedicalRecordsProperty*, bool);
	void MedicalRecordsPropertyDelete(int, lancet::IMedicalRecordsProperty*, bool);
public:
	virtual QDir GetDirectory() const;
	virtual void SetDirectory(const QDir&);

	virtual void Stop() = 0;
	virtual void Start() = 0;

	virtual void SetInterval(int);
	virtual int GetInterval() const;

	virtual State GetState() const;
	virtual RunTimeMode GetRunTimeMode() const;
	virtual void SetRunTimeMode(const RunTimeMode&);

	virtual bool IsRunning() const;
public:
	virtual QMedicalRecordsPropertyArray GetValidPropertyArray() const;
	virtual QMedicalRecordsPropertyArray GetInvalidPropertyArray() const;
protected:
	virtual bool IsErrorProperty(const QMedicalRecordsProperty&) const;
	virtual QPair<int, QMedicalRecordsProperty&> GetVectorQuoteProperty(QMedicalRecordsPropertyArray&,
		const QString&);
	virtual QPair<int, QMedicalRecordsProperty&> GetQuoteValidProperty(const QString&);
	virtual QPair<int, QMedicalRecordsProperty&> GetQuoteInvalidProperty(const QString&);
	virtual QMedicalRecordsPropertyArray& GetQuoteValidPropertyArray();
	virtual QMedicalRecordsPropertyArray& GetQuoteInvalidPropertyArray();
protected:
	virtual void SetRunning(bool);
	virtual void SetState(const State&);
private:
	struct IMedicalRecordsScannerPrivateImp;
	std::shared_ptr<IMedicalRecordsScannerPrivateImp> imp;
};
} // namespace lancet
#endif // !LancetIMedicalRecordsScanner_H
