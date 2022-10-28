/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Define case attribute data class.
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \version V1.0.0
 * \data 2022-10-11 10:45:02
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-10-11 10:45:02
 * \remark Nothing
 */
#ifndef LancetIMedicalRecordsProperty_H
#define LancetIMedicalRecordsProperty_H

// c or c++
#include <memory>

// berry
#include <berryObject.h>

// Qt
#include <QSharedPointer>

// ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
#include "org_mitk_lancet_services_medicalrecordsadministration_Export.h"

class QString;
class QVariant;
namespace lancet
{
/**
 * \ingroup org_mitk_lancet_services_medicalrecordsadministration
 * \brief This is a complete case attribute data \c abstract class.
 *
 * This property data class is interpreted by PropertyKeys. If you need additional 
 * properties based on this, you can customize them freely by the derived class, 
 * because the manager of the property field is \c QMap.In addition, you need to 
 * pay attention to void initializeKeys() = 0 to realize the desire to extend 
 * attributes.
 * 
 * \note 
 * Please note that this data class provides a feature recognition mechanism for 
 * whether fields are modified, which is a very useful function.
 * 
 * \author Sun
 */
class ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
	IMedicalRecordsProperty : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::IMedicalRecordsProperty);
	IMedicalRecordsProperty();

	/**
	 * \brief struct PropertyKeys
	 * This data structure can be abstracted into enumeration.
	 */
	struct ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
		PropertyKeys
	{
		using PropertyKey = const char* const;

		static PropertyKey Key;			///> Unique value, an unordered string consisting of {[0-9] [a-z] [A-Z]}
		static PropertyKey Name;		///> Patient Name
		static PropertyKey Id;			///> Patient ID
		static PropertyKey Sex;			///> Gender
		static PropertyKey Age;			///> Age
		static PropertyKey Valid;		///> Is it valid? False is invalid. The system does not recognize it
		static PropertyKey OperatingSurgeonName;	///> Name of chief surgeon
		static PropertyKey SurgicalArea;					///> Surgical area
		static PropertyKey Type;		///> Type of product
		static PropertyKey CreateTime;			///> Creation time
		static PropertyKey LastModifyTime;	///> Last modified

		static PropertyKey DataAddress;			///> Data address
		static PropertyKey DrPreviewAddress;			///> DR Preview Data Address
		static PropertyKey DrPositiveDicomAddress;	///> DR Dicom file address of positive bit slice
		static PropertyKey DrLateralDicomAddress;		///> DR Dicom file address

		/**
		 * \brief Returns a dictionary in Qt format
		 *
		 * \return Qt style List dictionary
		 */
		
		static QList<QString> ToQtList();
	};
public:
	virtual QString GetDirectory() const;
	virtual void SetDirectory(const QString&);

	/**
	 * \brief Returns whether the target attribute exists.
	 * 
	 * \see PropertyKeys
	 * \return True exists, otherwise it does not exist.
	 */
	virtual bool HasKey(const QString&) const;

	/**
	 * \brief Returns whether the target attribute has been modified.
	 *
	 * \params var
	 *              Attribute key value.
	 * \see PropertyKeys
	 * \return True exists, otherwise it does not exist.
	 */
	virtual bool HasModifyOf(const QString&) const;

	/**
	 * \brief Returns whether the properties in the target dictionary have been 
	 * modified.
	 *
	 * \params list
	 *              Qt style attribute key value dictionary.
	 * 
	 * \see HasModifyOf(), PropertyKeys
	 * \return Only when all attributes in the dictionary have not been modified 
	 * can true be returned, otherwise false.
	 */
	virtual bool HasModifyOf(const QList<QString>&) const;

	/**
	 * \brief Synchronize property values of target data objects.
	 *
	 * If the attribute values are inconsistent, the modified status of the 
	 * corresponding attribute values will be updated.
	 *
	 * \note The function of this method is different from that of the assignment 
	 * operator.
	 *
	 * \params data
	 *              data object.
	 * \see operator=(IMedicalRecordsProperty*), operator=(IMedicalRecordsProperty::Pointer)
	 * , GetModifyKeys()
	 */
	virtual void ModifyOf(const IMedicalRecordsProperty*);
	//@see ModifyOf(const IMedicalRecordsProperty*)
	virtual void ModifyOf(const IMedicalRecordsProperty::Pointer&);

	/**
	 * \brief Return the dictionary property value list.
	 *
	 * \return Qt style List dictionary.
	 */
	virtual QList<QString> GetKeys() const;

	/**
	 * \brief Returns the property key value of the modified property activated.
	 *
	 * \return Qt style List dictionary.
	 * 
	 * \see ModifyOf(const IMedicalRecordsProperty*)
	 */
	virtual QList<QString> GetModifyKeys() const;

	/**
	 * \brief Returns the attribute data corresponding to the key value.
	 *
	 * \params key
	 *              Key value of attribute data.
	 * \return data
	 * \see PropertyKeys
	 */
	virtual QVariant GetKeyValue(const QString&) const;
	virtual void SetKeyValue(const QString&, const QVariant&);
	
	/**
	 * \brief Reset the activation status of all modified attributes.
	 */
	virtual void ResetPropertyOfModify();
	virtual bool ResetPropertyOfModify(const QString&);
	virtual bool ResetPropertyOfModify(const QStringList&);
public:
	virtual void operator=(const IMedicalRecordsProperty&);
	virtual void operator=(IMedicalRecordsProperty*);
	virtual void operator=(IMedicalRecordsProperty::Pointer);

	virtual bool operator==(const IMedicalRecordsProperty&) = 0;
	virtual bool operator==(IMedicalRecordsProperty*);
	virtual bool operator==(IMedicalRecordsProperty::Pointer);
public:
	/**
	 * \brief Initialize the key attribute dictionary
	 *
	 * This method should be implemented by a derived class. Ideally, this interface 
	 * specifies all data formats and styles of the attribute data class.
	 *
	 * \see PropertyKeys
	 */
	virtual void initializeKeys() = 0;
	
	// \brief Returns the converted string of the attribute data class.
	virtual QString ToString() const = 0;

	/**
	 * \brief Convert the data field of the data attribute class to a text 
	 * string in JSON format.
	 *
	 * \return A text string in JSON format.
	 */
	virtual QString ToJsonString() const = 0;
private:
	struct IMedicalRecordsPropertyPrivateImp;
	std::shared_ptr<IMedicalRecordsPropertyPrivateImp> imp;
};
}
// \see lancet::IMedicalRecordsProperty::PropertyKeys
using QPropertyKeys = lancet::IMedicalRecordsProperty::PropertyKeys;
using QMedicalRecordsProperty = QSharedPointer<lancet::IMedicalRecordsProperty>;
using QMedicalRecordsPropertyArray = QVector<QMedicalRecordsProperty>;

#endif // !LancetIMedicalRecordsProperty_H
