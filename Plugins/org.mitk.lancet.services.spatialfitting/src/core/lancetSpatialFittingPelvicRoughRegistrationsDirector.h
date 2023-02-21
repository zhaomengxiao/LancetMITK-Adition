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
#ifndef LancetSpatialFittingPelvicRoughRegistrationsDirector_H
#define LancetSpatialFittingPelvicRoughRegistrationsDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"

#include <mitkNavigationDataSource.h>
#include <mitkTrackingDeviceSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PelvicRoughRegistrationsDirector : public AbstractDirector
{
public:
	mitkClassMacroItkParent(PelvicRoughRegistrationsDirector, AbstractDirector);

	itkNewMacro(PelvicRoughRegistrationsDirector)

	PelvicRoughRegistrationsDirector();
	virtual ~PelvicRoughRegistrationsDirector();
	
	virtual bool Builder() override;
public:
	virtual mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	virtual void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
private:
	struct PelvicRoughRegistrationsDirectorPrivateImp;
	std::shared_ptr<PelvicRoughRegistrationsDirectorPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvicRoughRegistrationsDirector_H