/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \version V1.0.0
 * \data 2022-10-11 10:39:19
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 10:39:19
 * \remark $Modify Description$
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

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_medicalrecordsadministration_Export.h"

namespace lancet
{
/**
	* \class IMedicalRecordsAdministrationService
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
	* \date 2022-10-11 10:39:35
	* \remark todo: insert comments
	*
	* Contact: sh4a01@163.com
	*
	*/
class IMedicalRecordsScanner;
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
	IMedicalRecordsAdministrationService: public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IMedicalRecordsProperty);
	IMedicalRecordsAdministrationService();

	static berry::SmartPointer<IMedicalRecordsAdministrationService> GetService();
Q_SIGNALS:
	void MedicalRecordsPropertySelect(lancet::IMedicalRecordsProperty*);
	void MedicalRecordsPropertyTrace(int, lancet::IMedicalRecordsProperty*, bool);
	void MedicalRecordsPropertyModify(int, lancet::IMedicalRecordsProperty*, bool);
	void MedicalRecordsPropertyDelete(int, lancet::IMedicalRecordsProperty*, bool);
public:
	virtual void Start() = 0;
	virtual void Stop() = 0;

	virtual void SetScanner(berry::SmartPointer<IMedicalRecordsScanner>);
	virtual berry::SmartPointer<IMedicalRecordsScanner> GetScanner() const;

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
	struct IMedicalRecordsAdministrationServicePrivateImp;
	std::shared_ptr<IMedicalRecordsAdministrationServicePrivateImp> imp;
};
}
Q_DECLARE_INTERFACE(lancet::IMedicalRecordsAdministrationService, "org.mitk.lancet.services.IMedicalRecordsAdministrationService")
#endif // !LancetIMedicalRecordsAdministrationService_H
