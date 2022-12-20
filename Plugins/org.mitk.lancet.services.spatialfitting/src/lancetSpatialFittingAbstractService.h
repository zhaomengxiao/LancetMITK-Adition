#ifndef LancetSpatialFittingAbstractService_H
#define LancetSpatialFittingAbstractService_H

// C++
#include <memory>

// berry
#include <berryObject.h>

// Qt
#include <QObject>

#include "lancetSpatialFittingGlobal.h"

#include "org_mitk_lancet_services_spatialfitting_Export.h"

BEGIN_SPATIAL_FITTING_NAMESPACE
class ProbeCheckPointModel;
class RoboticsRegisterModel;
class PelvisCheckPointModel;
END_SPATIAL_FITTING_NAMESPACE

namespace lancet
{
class IDevicesAdministrationService;
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN 
	SpatialFittingAbstractService : public QObject, public berry::Object
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::SpatialFittingAbstractService);
	SpatialFittingAbstractService();

	static berry::SmartPointer<SpatialFittingAbstractService> GetService();

public:
	IDevicesAdministrationService* GetDeviceService() const;
	virtual void SetRoboticsRegisterModel(itk::SmartPointer<lancet::spatial_fitting::RoboticsRegisterModel>);
	virtual itk::SmartPointer<lancet::spatial_fitting::RoboticsRegisterModel> GetRoboticsRegisterModel() const;

	virtual itk::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel> 
		GetProbeCheckPointModel() const;
	virtual void SetProbeCheckPointModel(itk::SmartPointer<lancet::spatial_fitting::ProbeCheckPointModel>);

	virtual itk::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel> 
		GetPelvisCheckPointModel() const;
	virtual void SetPelvisCheckPointModel(itk::SmartPointer<lancet::spatial_fitting::PelvisCheckPointModel>);


	virtual void Initialize() = 0;
private:struct SpatialFittingAbstractServicePrivateImp;
	std::shared_ptr<SpatialFittingAbstractServicePrivateImp> imp;
};

}
Q_DECLARE_INTERFACE(lancet::SpatialFittingAbstractService, "org.mitk.lancet.services.SpatialFittingAbstractService")
#endif // !LancetSpatialFittingAbstractService_H