#ifndef LancetSpatialFittingAbstractService_H
#define LancetSpatialFittingAbstractService_H

// C++
#include <memory>

// berry
#include <berryObject.h>

// Qt
#include <QObject>

#include "lancetSpatialFittingGlobal.h"

// Pre declaration registration function module object's underlying type.
namespace lancet::spatial_fitting { class AbstractModel; }


namespace lancet
{
class IDevicesAdministrationService;

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \brief Service class of spatial registration module.
 * 
 * This class will provide the only interface of functions externally, and is 
 * mainly responsible for memory management of the registration model. All the 
 * interpretation rights of specific functions belong to the registration model 
 * instance object.
 * 
 * Classes that provide functionality externally will inherit this abstract class 
 * to derive and implement specific behaviors.
 * 
 * <b>Example of use:</b>
 * \code
 * 
 * class MyService : public lancet::AbstractService
 * {
 * public:
 *   MyService() {}
 *   ~MyService() {}
 * 
 * public:
 *   void initMyService() 
 *   {
 *      lancet::spatial_fitting::AbstractModel::Pointer myModel = MyModel::New();
 * 		myModel->SetSerialNumber("myModel");
 *      this->SetModel(myModel->GetSerialNumber(), myModel);
 *      this->GetModel("myModel")->Initialize();
 * 
 *      // this->SetModel("myModel", myModel);
 *      // this->GetModel("myModel")->Initialize();
 *   }
 * 
 *   void uninitMyService()
 *   {
 *      this->GetModel("myModel")->Uninitialize();
 *      this->SetModel("myModel", nullptr);
 * 
 *      // this->GetModel("myModel")->Uninitialize();
 *      // this->SetModel("myModel", nullptr);
 *   }
 * }
 * 
 * \endcond
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN 
	AbstractService : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::AbstractService);
	AbstractService();
	~AbstractService();

	// Return device service object.
	// \see \c plugin org.mitk.lancet.services.devicesadministration
	IDevicesAdministrationService* GetDeviceService() const;
	static berry::SmartPointer<AbstractService> GetService();

public:
	/**
	 * \brief Detect the presence of target model objects.
	 * 
	 * \see void AbstractModel#SetSerialNumber(const QString&), QString AbstractModel#GetSerialNumber()
	 */
	virtual bool HasModel(const QString&) const;

	virtual berry::SmartPointer<lancet::spatial_fitting::AbstractModel> GetModel(const QString&) const;
	virtual void SetModel(const QString&, berry::SmartPointer<lancet::spatial_fitting::AbstractModel>);

	/**
	 * \brief Insert a new model into the manager.
	 * 
	 * \note This interface will default to extending the sequence number of the 
	 *       AbstractModel object and store it in the container.
	 * 
	 * \see void AbstractService#SetModel(const QString&, berry::SmartPointer<lancet::spatial_fitting::AbstractModel>)
	 */
	virtual void InsertModel(berry::SmartPointer<lancet::spatial_fitting::AbstractModel> );
private:
struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

}
Q_DECLARE_INTERFACE(lancet::AbstractService, "org.mitk.lancet.services.AbstractServiceEx")
#endif // !LancetSpatialFittingAbstractService_H