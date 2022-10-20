/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \version V1.0.0
 * \data 2022-10-11 10:40:55
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 10:40:55
 * \remark $Modify Description$
 */
#ifndef LancetIMedicalRecordsLoader_H
#define LancetIMedicalRecordsLoader_H

 // c or c++
#include <memory>

// berry
#include <berryObject.h>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_medicalrecordsadministration_Export.h"

namespace lancet
{
/**
 * \class IMedicalRecordsLoader
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
 * \date 2022-10-11 10:41:39
 * \remark todo: insert comments
 *
 * Contact: sh4a01@163.com
 *
 */
class IMedicalRecordsProperty;
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
	IMedicalRecordsLoader
{
public:
	berryObjectMacro(lancet::IMedicalRecordsLoader);
	IMedicalRecordsLoader();
	virtual berry::SmartPointer<IMedicalRecordsProperty> GetOutput() = 0;
public:
	virtual QString GetFileName() const;
	virtual void SetFileName(const QString&);

	virtual QString GetErrorString() const = 0;
	virtual int GetJsonParseError() const = 0;

	virtual bool IsFileError() const = 0;
	virtual bool IsJsonError() const = 0;
private:
	struct IMedicalRecordsLoaderPrivateImp;
	std::shared_ptr<IMedicalRecordsLoaderPrivateImp> imp;
};
}
#endif // !LancetIMedicalRecordsLoader_H

