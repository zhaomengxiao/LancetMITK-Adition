/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the role of the navigation data pipeline for the rough registration 
 *        of the pelvis.
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2023-02-20 11:09:17
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2023-02-20 11:09:17.
 *
 * \author Sun
 * \date 2023-02-20 11:09:17
 * \remark Non
 */
#ifndef LancetSpatialFittingPelvicRoughRegistrationsVerifyDirector_H
#define LancetSpatialFittingPelvicRoughRegistrationsVerifyDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"

#include <mitkNavigationDataSource.h>
#include <mitkTrackingDeviceSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  PelvicRoughRegistrationsVerifyDirector : public AbstractDirector
{
public:
	mitkClassMacroItkParent(PelvicRoughRegistrationsVerifyDirector, AbstractDirector);

	itkNewMacro(PelvicRoughRegistrationsVerifyDirector)

	PelvicRoughRegistrationsVerifyDirector();
	virtual ~PelvicRoughRegistrationsVerifyDirector();
	
	virtual bool Builder() override;
public:
	virtual mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	virtual void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);

	virtual mitk::AffineTransform3D::Pointer GetImageConvertMatrix() const;
	virtual void SetImageConvertMatrix(const mitk::AffineTransform3D::Pointer&);
private:
  struct PelvicRoughRegistrationsVerifyDirectorPrivateImp;
  std::shared_ptr<PelvicRoughRegistrationsVerifyDirectorPrivateImp> imp;
};


END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvicRoughRegistrationsVerifyDirector_H