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
#ifndef LancetSpatialFittingRoboticsVerifyDirector_H
#define LancetSpatialFittingRoboticsVerifyDirector_H

#include "lancetSpatialFittingGlobal.h"
#include "lancetSpatialFittingAbstractDirector.h"
#include "lancetSpatialFittingRoboticsRegisterDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup GroupName
 * \brief This is the decision-maker class of robot arm registration construction.
 *
 * This will determine the processing manager object for the robot arm to RoboticsVerify 
 * navigation data.
 * 
 * <b>Data stream:</b><br>
 * \image html RoboticsVerifyDataStream.svg
 *
 * <b>Example of use:</b><br>
 * \code
 * RoboticsVerifyDirector::Pointer RoboticsVerifyDirector = 
 *   RoboticsVerifyDirector::New();
 * 
 * if(RoboticsVerifyDirector->Builder())
 * {
 *   auto RoboticsVerifyManage = RoboticsVerifyDirector->GetBuilder()->GetOutput();
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
	RoboticsVerifyDirector : public RoboticsRegisterDirector
{
public:
	mitkClassMacroItkParent(RoboticsVerifyDirector, RoboticsRegisterDirector);

	itkNewMacro(RoboticsVerifyDirector)

	RoboticsVerifyDirector();
	virtual ~RoboticsVerifyDirector();

	virtual bool Builder() override;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingRoboticsVerifyDirector_H
