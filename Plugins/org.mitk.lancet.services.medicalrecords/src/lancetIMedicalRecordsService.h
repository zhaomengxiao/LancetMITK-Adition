/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare abstract object type of patient data service.
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \version V1.0.0
 * \data 2022-10-11 10:39:19
 * 
 * \par Modify History
 *   -# Create Initialize Version
 *
 * \author Sun
 * \data 2022-10-11 10:39:19
 * \remark Nothing
 */
#ifndef LancetIMedicalRecordsAdministrationService_H
#define LancetIMedicalRecordsAdministrationService_H

// C++
#include <memory>

// berry
#include <berryObject.h>

// Qt
#include <QObject>

#include "lancetIMedicalRecordsProperty.h"

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
#include "org_mitk_lancet_services_medicalrecords_Export.h"

namespace lancet
{
class IMedicalRecordsScanner;
/**
	* \ingroup org_mitk_lancet_services_medicalrecords
	* \brief This is the abstract object type of patient data service.
	*
	* Such data service objects will be registered in the \c Mitk microservice 
	* framework for easy access by external or followers.
	*
	* \author Sun
	* \remark Nothing
	*
	* Contact: sh4a01@163.com
	*
	*/
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
	IMedicalRecordsService: public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IMedicalRecordsProperty);
	IMedicalRecordsService();

	/**
	 * \brief Return the instance object of the service class.
	 * 
	 * This is the standard interface of the service class.
	 */
	static berry::SmartPointer<IMedicalRecordsService> GetService();
Q_SIGNALS:
	/**
	 * \brief When the currently selected data item changes, this signal will be 
	 * sent to notify the followers of the latest situation.
	 *
	 * This signal is triggered by the \c SetSelect() interface. Although the 
	 * followers may not need to use this signal to achieve the target function, 
	 * it is recommended to use this interface to maintain the unity of style.
	 *
	 * \param lancet::IMedicalRecordsProperty
	 *              Patient Data Instance Object.
	 */
	void MedicalRecordsPropertySelect(lancet::IMedicalRecordsProperty*);
	/**
	 * \brief When patient data is found, this signal is sent to inform the followers
	 * of the latest situation.
	 * 
	 * \see lancet::IMedicalRecordsScanner::MedicalRecordsPropertyTrace
	 */
	void MedicalRecordsPropertyTrace(int, lancet::IMedicalRecordsProperty*, bool);
	/**
	 * \brief When the patient data is detected to be modified, this signal is sent
	 * to notify the followers of the latest situation.
	 *
	 * \see lancet::IMedicalRecordsScanner::MedicalRecordsPropertyModify
	 */
	void MedicalRecordsPropertyModify(int, lancet::IMedicalRecordsProperty*, bool);
	/**
	 * \brief When the patient data is detected to be deleted, this signal is sent to
	 * notify the followers of the latest situation.
	 *
	 * \see lancet::IMedicalRecordsScanner::MedicalRecordsPropertyDelete
	 */
	void MedicalRecordsPropertyDelete(int, lancet::IMedicalRecordsProperty*, bool);
public:
	/**
	 * \brief Start patient data service.
	 */
	virtual void Start() = 0;

	/**
	 * \brief Stop patient data service.
	 */
	virtual void Stop() = 0;

	/**
	 * \brief Setup Scanner Components.
	 *
	 * \param berry::SmartPointer<IMedicalRecordsScanner>
	 *              Patient Data Scanner Object.
	 * \see lancet::IMedicalRecordsScanner, berry::SmartPointer
	 */
	virtual void SetScanner(berry::SmartPointer<IMedicalRecordsScanner>);
	virtual berry::SmartPointer<IMedicalRecordsScanner> GetScanner() const;

	/**
	 * \brief Set the currently selected patient data object.
	 *
	 * This behavior will send a MedicalRecordsPropertySelect signal to inform the 
	 * followers of the latest selected patient data object.
	 *
	 * \param QMedicalRecordsProperty
	 *              Patient data object.
	 */
	virtual void SetSelect(const QMedicalRecordsProperty&);
	virtual QMedicalRecordsProperty GetSelect() const;
protected Q_SLOTS:
	void Slot_MedicalRecordsPropertyTrace(int, lancet::IMedicalRecordsProperty*, bool);
	void Slot_MedicalRecordsPropertyModify(int, lancet::IMedicalRecordsProperty*, bool);
	void Slot_MedicalRecordsPropertyDelete(int, lancet::IMedicalRecordsProperty*, bool);
private:
	virtual void ConnectToScanner(const berry::SmartPointer<IMedicalRecordsScanner>&) const;
	virtual void DisConnectToScanner(const berry::SmartPointer<IMedicalRecordsScanner>&) const;
private:
	struct IMedicalRecordsServicePrivateImp;
	std::shared_ptr<IMedicalRecordsServicePrivateImp> imp;
};
}
Q_DECLARE_INTERFACE(lancet::IMedicalRecordsService, "org.mitk.lancet.services.IMedicalRecordsService")
#endif // !LancetIMedicalRecordsAdministrationService_H
