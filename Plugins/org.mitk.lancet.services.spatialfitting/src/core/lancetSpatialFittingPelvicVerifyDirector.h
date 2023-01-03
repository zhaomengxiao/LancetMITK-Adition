#ifndef LancetSpatialFittingPelvisVerifyDirector_H
#define LancetSpatialFittingPelvisVerifyDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"
#include "lancetSpatialFittingPelvicRegisterDirector.h"

#include <mitkNavigationDataSource.h>

#include <mitkTrackingDeviceSource.h>
BEGIN_SPATIAL_FITTING_NAMESPACE

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PelvisVerifyDirector : public PelvisRegisterDirector
{
public:
	mitkClassMacroItkParent(PelvisVerifyDirector, PelvisRegisterDirector);

	itkNewMacro(PelvisVerifyDirector)

	PelvisVerifyDirector();
	virtual ~PelvisVerifyDirector();

	virtual mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	virtual void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual mitk::NavigationDataSource::Pointer GetRobotNavigationDataSource() const;
	virtual void SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual bool Builder() override;
	virtual void SetConvertMatrix(mitk::AffineTransform3D::Pointer);
	virtual mitk::AffineTransform3D::Pointer GetConvertMatrix() const;
private:
	mitk::AffineTransform3D::Pointer convertMatrix;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisVerifyDirector_H
