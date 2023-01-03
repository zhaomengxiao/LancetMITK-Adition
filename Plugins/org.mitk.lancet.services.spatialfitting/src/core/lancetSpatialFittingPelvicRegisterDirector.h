#ifndef LancetSpatialFittingPelvisRegisterDirector_H
#define LancetSpatialFittingPelvisRegisterDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"

#include <mitkNavigationDataSource.h>

#include <mitkTrackingDeviceSource.h>
BEGIN_SPATIAL_FITTING_NAMESPACE

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PelvisRegisterDirector : public AbstractDirector
{
public:
	mitkClassMacroItkParent(PelvisRegisterDirector, AbstractDirector);

	itkNewMacro(PelvisRegisterDirector)

	PelvisRegisterDirector();
	virtual ~PelvisRegisterDirector();

	virtual mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	virtual void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual mitk::NavigationDataSource::Pointer GetRobotNavigationDataSource() const;
	virtual void SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual bool Builder() override;
private:
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisRegisterDirector_H
