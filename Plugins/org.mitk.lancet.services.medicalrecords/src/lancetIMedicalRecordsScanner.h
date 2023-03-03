/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief A scanner abstract object that declares patient data.
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \version V1.0.0
 * \data 2022-10-11 10:43:35
 * 
 * \par Modify History
 *   -# Create Initialize Version
 * 
 * \author Sun
 * \data 2022-10-11 10:43:35
 * \remark Nothing.
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
// ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
#include "org_mitk_lancet_services_medicalrecords_Export.h"

class QDir;
namespace lancet
{
class IMedicalRecordsProperty;
/**
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \brief This is the scanner abstract object of patient data.
 *
 * TODO: long description
 *
 * \author Sun
 * \test tst_medicalRecordsScanner: unit testing
 */
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
	IMedicalRecordsScanner : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IMedicalRecordsScanner);
	IMedicalRecordsScanner();

	/**
	 * \enum State
	 * \brief Workflow status enumeration type inside patient data scanner.
	 */
	enum State 
	{ 
		Downtime,			///< Offline status
		Initialize,		///< Initialization status
		Patroling			///< Busy scanning patient data
	};

	/**
	 * \enum RunTimeMode
	 * \brief How the Scanner Works Enumeration Types.
	 */
	enum RunTimeMode
	{ 
		Timer,			///< The working environment of scanner patrol is in the main thread.
		Thread			///< The working environment of scanner patrol is multi-threaded.
	};

	Q_ENUM(State)
	Q_ENUM(RunTimeMode)

Q_SIGNALS:
	/**
	 * \brief When patient data is found, this signal is sent to inform the followers 
	 * of the latest situation.
	 *
	 * \warning Don't do too much operation on the data pointer object. Its memory is 
	 * managed by the scanner. What is released here is only the pointer reference 
	 * object. I hope you can do it yourself.
	 *
	 * \param int
	 *              The database ID of the patient data found, and the valid ID is not -1.
	 * \param lancet::IMedicalRecordsProperty
	 *							Patient Data pointer Object, Ontology exists in the database.
	 * \param bool
	 *							Valid patient data flag is true, otherwise false.
	 * 
	 * \see MedicalRecordsPropertyModify(), MedicalRecordsPropertyDelete()
	 */
	void MedicalRecordsPropertyTrace(int, lancet::IMedicalRecordsProperty*, bool);

	/**
	 * \brief When the patient data is detected to be modified, this signal is sent 
	 * to notify the followers of the latest situation.
	 *
	 * \warning Don't do too much operation on the data pointer object. Its memory is
	 * managed by the scanner. What is released here is only the pointer reference
	 * object. I hope you can do it yourself.
	 *
	 * \param int
	 *              The database ID of the patient data found, and the valid ID is not -1.
	 * \param lancet::IMedicalRecordsProperty
	 *							Patient Data pointer Object, Ontology exists in the database.
	 * \param bool
	 *							Valid patient data flag is true, otherwise false.
	 *
	 * \see MedicalRecordsPropertyTrace(), MedicalRecordsPropertyDelete()
	 */
	void MedicalRecordsPropertyModify(int, lancet::IMedicalRecordsProperty*, bool);

	/**
	 * \brief When the patient data is detected to be deleted, this signal is sent to 
	 * notify the followers of the latest situation.
	 *
	 * \warning Don't do too much operation on the data pointer object. Its memory is
	 * managed by the scanner. What is released here is only the pointer reference
	 * object. I hope you can do it yourself.
	 *
	 * \param int
	 *              The database ID of the patient data found, and the valid ID is not -1.
	 * \param lancet::IMedicalRecordsProperty
	 *							Patient Data pointer Object, Ontology exists in the database.
	 * \param bool
	 *							Valid patient data flag is true, otherwise false.
	 *
	 * \see MedicalRecordsPropertyTrace(), MedicalRecordsPropertyDelete()
	 */
	void MedicalRecordsPropertyDelete(int, lancet::IMedicalRecordsProperty*, bool);
public:
	virtual QDir GetDirectory() const;
	virtual void SetDirectory(const QDir&);

	/**
	 * \brief Stop the scanner.
	 *
	 * This will end the scanner's work and update its working status.
	 */
	virtual void Stop() = 0;

	/**
	 * \brief Start the scanner.
	 *
	 * This will start the scanner's work and update its working status.
	 */
	virtual void Start() = 0;

	/**
	 * \brief Set the interval of scanner patrol.
	 *
	 * \params int
	 *              in milliseconds.
	 * \see GetInterval()
	 */
	virtual void SetInterval(int);
	virtual int GetInterval() const;

	virtual State GetState() const;
	virtual RunTimeMode GetRunTimeMode() const;

	/**
	 * \brief Set Runtime Mode.
	 *
	 * According to different runtime modes, determine the working mode of the \c 
	 * scanner.
	 * 
	 * \warning This interface only takes effect before the scanner has officially 
	 * patrolled.
	 *
	 * \params RunTimeMode
	 *              Enumeration values for runtime mode.
	 * \see RunTimeMode, Start()
	 */
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
