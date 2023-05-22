#include "lancetSpatialFittingAbstractPelvicRegistrationsModel.h"


BEGIN_SPATIAL_FITTING_NAMESPACE

struct AbstractPelvicRegistrationsModel::PrivateImp
{
	WorkingStream workingStream;
	static mitk::SurfaceRegistration::Pointer surfaceRegistration;
};

// Initialize the static registration model of the pelvis.
mitk::SurfaceRegistration::Pointer
	AbstractPelvicRegistrationsModel::PrivateImp::surfaceRegistration =
	mitk::SurfaceRegistration::New();


AbstractPelvicRegistrationsModel::AbstractPelvicRegistrationsModel(const QString& serialNumber)
	: AbstractModel(serialNumber)
	, imp(std::make_shared<PrivateImp>())
{
}

mitk::SurfaceRegistration::Pointer 
  AbstractPelvicRegistrationsModel::GetSurfaceRegistration() const
{
	return PrivateImp::surfaceRegistration;
}

void AbstractPelvicRegistrationsModel::SetSurfaceRegistration(
	const mitk::SurfaceRegistration::Pointer& surfaceRegistration)
{
	PrivateImp::surfaceRegistration = surfaceRegistration;
}

AbstractPelvicRegistrationsModel::WorkingStream 
	AbstractPelvicRegistrationsModel::GetWorkingStream() const
{
	return this->imp->workingStream;
}

void AbstractPelvicRegistrationsModel::SetWorkingStream(const WorkingStream& v)
{
	this->imp->workingStream = v;
}


END_SPATIAL_FITTING_NAMESPACE