/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the header file of the robot arm registration data filter manager
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:19:56
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:19:56.
 *
 * \author Sun
 * \date 2022-11-18 10:19:56
 * \remark Non
 */
#ifndef LancetSpatialFittingRoboticsRegisterModel_H
#define LancetSpatialFittingRoboticsRegisterModel_H

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataSource.h>

class RobotRegistration;
BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	RoboticsRegisterModel : public itk::Object
{
public:
	mitkClassMacroItkParent(RoboticsRegisterModel, itk::Object)

	itkNewMacro(RoboticsRegisterModel)

	RoboticsRegisterModel();
	virtual ~RoboticsRegisterModel();
public:
	RobotRegistration& GetRegisterModel();

	itk::SmartPointer<PipelineManager> GetRegisterNavigationPipeline() const;
	void SetRegisterNavigationPipeline(itk::SmartPointer<PipelineManager> manager);

	void SetAccutacyVerifyPipeline(itk::SmartPointer<PipelineManager> manager);
	itk::SmartPointer<PipelineManager> GetAccutacyVerifyPipeline() const;

	mitk::NavigationDataSource* GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource* source);

	mitk::NavigationDataSource* GetRoboticsNavigationDataSource() const;
	void SetRoboticsNavigationDataSource(mitk::NavigationDataSource* source);
private:
	struct RoboticsRegisterModelPrivateImp;
	std::shared_ptr<RoboticsRegisterModelPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingRoboticsRegisterModel_H
