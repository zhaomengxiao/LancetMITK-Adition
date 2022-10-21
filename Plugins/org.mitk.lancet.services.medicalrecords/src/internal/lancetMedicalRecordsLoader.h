/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief $Document description$
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \version V1.0.0
 * \data 2022-10-11 17:15:01
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 17:15:01
 * \remark $Modify Description$
 */
#ifndef LancetMedicalRecordsLoader_H
#define LancetMedicalRecordsLoader_H

#include <lancetIMedicalRecordsLoader.h>

 // c or c++
#include <memory>

// berry
#include <berryObject.h>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
#include "org_mitk_lancet_services_medicalrecords_Export.h"

namespace lancet
{
/**
	* \class MedicalRecordsLoader
	* \ingroup org_mitk_lancet_services_medicalrecords
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
	* \date 2022-10-12 10:42:25
	* \remark todo: insert comments
	*
	* Contact: sh4a01@163.com
	*
	*/
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
	MedicalRecordsLoader : public IMedicalRecordsLoader
{
public:
	berryObjectMacro(lancet::MedicalRecordsLoader);
	MedicalRecordsLoader();

public:
	virtual QString GetErrorString() const override;
	virtual int GetJsonParseError() const override;

	virtual bool IsFileError() const override;
	virtual bool IsJsonError() const override;

	virtual berry::SmartPointer<IMedicalRecordsProperty> GetOutput() override;

	virtual berry::SmartPointer<IMedicalRecordsProperty>
		AnalysisTargetFile(const QByteArray&) const;

	virtual void Reset();
protected:
	virtual bool CheckInputFile(const QString&) const;
	virtual QString GetFileError() const;
private:
	struct MedicalRecordsLoaderPrivateImp;
	std::shared_ptr<MedicalRecordsLoaderPrivateImp> imp;
};
}
#endif
