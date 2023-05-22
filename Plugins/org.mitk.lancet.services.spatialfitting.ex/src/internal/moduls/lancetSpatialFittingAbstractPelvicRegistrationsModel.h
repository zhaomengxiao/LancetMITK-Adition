#ifndef lancetSpatialFittingAbstractPelvicRegistrationsModel_H
#define lancetSpatialFittingAbstractPelvicRegistrationsModel_H

#include <lancetSpatialFittingGlobal.h>

#include <lancetSpatialFittingAbstractModel.h>
#include <internal/lancetSpatiaFittingModulsFactor.h>

// Include header files for lancet model.
#include <surfaceregistraion.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	AbstractPelvicRegistrationsModel : public AbstractModel
{
public:
	/**
	 * \brief WorkingStream
	 *
	 * - Collector
	 *				Collect registration point mode, mainly collecting registration space 
	 *				point data of Vega equipment.
	 * - VerifyAccuracy
	 *				Verify the registration accuracy mode, mainly verifying the registration 
	 *				error of the spatial data of the registration set.
	 */
	enum WorkingStream
	{
		Collector,
		VerifyAccuracy
	};
public:
	berryObjectMacro(lancet::spatial_fitting::AbstractPelvicRegistrationsModel)

	AbstractPelvicRegistrationsModel(const QString& );

	virtual mitk::SurfaceRegistration::Pointer GetSurfaceRegistration() const final;

	[[deprecated("This interface is not recommended to be enabled, and this behavior\
		is not recommended to be dropped onto derived class objects")]]
	virtual void SetSurfaceRegistration(const mitk::SurfaceRegistration::Pointer&) final;

	virtual bool ComputeLandMarkResult(mitk::PointSet::Pointer,
		mitk::PointSet::Pointer, mitk::Surface::Pointer) = 0;

	virtual WorkingStream GetWorkingStream() const;
	virtual void SetWorkingStream(const WorkingStream&);
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};
END_SPATIAL_FITTING_NAMESPACE
#endif // !lancetSpatialFittingAbstractPelvicRegistrationsModel_H
