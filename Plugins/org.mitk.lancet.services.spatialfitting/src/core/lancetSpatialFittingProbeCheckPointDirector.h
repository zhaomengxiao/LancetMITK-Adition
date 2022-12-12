/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file of the decision-maker for the registration and 
 *        construction of the robot arm.
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:20:17
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:20:17.
 *
 * \author Sun
 * \date 2022-11-18 10:20:17
 * \remark Non
 */
#ifndef LancetSpatialFittingProbeCheckPointDirector_H
#define LancetSpatialFittingProbeCheckPointDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"

#include <mitkNavigationDataSource.h>

#include <mitkTrackingDeviceSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup GroupName
 * \brief This is the decision-maker class of robot arm registration construction.
 *
 * This will determine the processing manager object for the robot arm to ProbeCheckPoint 
 * navigation data.
 * 
 * <b>Data stream:</b><br>
 * \image html ProbeCheckPointDataStream.svg
 *
 * <b>Example of use:</b><br>
 * \code
 * ProbeCheckPointDirector::Pointer ProbeCheckPointDirector = 
 *   ProbeCheckPointDirector::New();
 * 
 * if(ProbeCheckPointDirector->Builder())
 * {
 *   auto ProbeCheckPointManage = ProbeCheckPointDirector->GetBuilder()->GetOutput();
 * }
 * \endcode
 *
 * \author Sun
 * \version V1.0.0
 * \date 2022-11-21 16:06:37
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	ProbeCheckPointDirector : public AbstractDirector
{
public:
	mitkClassMacroItkParent(ProbeCheckPointDirector, AbstractDirector);

	itkNewMacro(ProbeCheckPointDirector)

	ProbeCheckPointDirector();
	virtual ~ProbeCheckPointDirector();

	virtual mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	virtual void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual mitk::NavigationDataSource::Pointer GetRobotNavigationDataSource() const;
	virtual void SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource);
	virtual bool Builder() override;

	virtual void SetRoboticsMatrix(mitk::AffineTransform3D::Pointer);
	virtual mitk::AffineTransform3D::Pointer GetRoboticsMatrix() const;
private:
	mitk::AffineTransform3D::Pointer robotMatrix;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingProbeCheckPointDirector_H
