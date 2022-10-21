/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare case loader abstract object.
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \version V1.0.0
 * \data 2022-10-11 10:40:55
 * 
 * \par Modify History
 *   -# Create Initialize Version
 *
 * \author Sun
 * \data 2022-10-11 10:40:55
 * \remark Nothing
 */
#ifndef LancetIMedicalRecordsLoader_H
#define LancetIMedicalRecordsLoader_H

 // c or c++
#include <memory>

// berry
#include <berryObject.h>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
#include "org_mitk_lancet_services_medicalrecords_Export.h"

namespace lancet
{
class IMedicalRecordsProperty;

/**
 * \ingroup org_mitk_lancet_services_medicalrecords
 * \brief This is a case loader abstract object.
 *
 * The case is embodied in the file system, and the function of the loader is to 
 * map the case data in the file system to the memory data object.
 * 
 * Because the function of this object is composed of multiple sub functions, it 
 * does not show too many additional dependencies on the surface, that is, error 
 * handling may be slightly inadequate or ambiguous. Therefore, IsFileError() and 
 * IsJsonError() interfaces must be used to assist in identifying detailed error 
 * types.
 * 
 * Maybe you need to pay attention to \c lancet::IMedicalRecordsProperty class, 
 * because they are very close collaboration.
 * \author Sun
 */
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDS_PLUGIN
	IMedicalRecordsLoader
{
public:
	berryObjectMacro(lancet::IMedicalRecordsLoader);
	IMedicalRecordsLoader();

	/**
	 * \brief Get Output Data Object.
	 *
	 * Produce the target data object according to the input parameters.
	 *
	 * \warning This method has a high probability of outputting an empty data object.
	 * 
	 * \return data object.
	 * \see lancet::IMedicalRecordsProperty, berry::SmartPointer
	 */
	virtual berry::SmartPointer<IMedicalRecordsProperty> GetOutput() = 0;
public:
	virtual QString GetFileName() const;
	virtual void SetFileName(const QString&);

	/**
	 * \brief Return detailed error information.
	 *
	 * Internal functions are jointly completed by multiple external functions, so 
	 * this method may return error messages of multiple external functions.
	 *
	 * \warning The error information output by this interface is recommended to be 
	 * used for the \c debugging work of developers, not directly applied to the 
	 * application interface.
	 * 
	 */
	virtual QString GetErrorString() const = 0;

	/**
	 * \brief Return error code
	 *
	 * This interface is the same as GetErrorString()
	 *
	 * \return No error returned 0.
	 * \see GetErrorString()
	 */
	virtual int GetJsonParseError() const = 0;

	virtual bool IsFileError() const = 0;
	virtual bool IsJsonError() const = 0;
private:
	struct IMedicalRecordsLoaderPrivateImp;
	std::shared_ptr<IMedicalRecordsLoaderPrivateImp> imp;
};
}
#endif // !LancetIMedicalRecordsLoader_H

