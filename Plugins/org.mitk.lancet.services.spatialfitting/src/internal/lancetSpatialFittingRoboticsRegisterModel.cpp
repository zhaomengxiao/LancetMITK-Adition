#include "lancetSpatialFittingRoboticsRegisterModel.h"
#include "core/lancetSpatialFittingPipelineManager.h"
#include <robotRegistration.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct RoboticsRegisterModel::RoboticsRegisterModelPrivateImp
{
	RobotRegistration roboticsRegisterModel;

	PipelineManager::Pointer pipelineRoboticsRegisterManager;
	PipelineManager::Pointer pipelineRoboticsAccuracyManager;

	mitk::NavigationDataSource* ndiNavigationDataSource = nullptr;
	mitk::NavigationDataSource* roboticsNavigationDataSource = nullptr;
};

RoboticsRegisterModel::RoboticsRegisterModel()
	: imp(std::make_shared<RoboticsRegisterModelPrivateImp>())
{
}

RoboticsRegisterModel::~RoboticsRegisterModel()
{
}

RobotRegistration& RoboticsRegisterModel::GetRegisterModel()
{
	return this->imp->roboticsRegisterModel;
}

itk::SmartPointer<PipelineManager> RoboticsRegisterModel::GetRegisterNavigationPipeline() const
{
	return itk::SmartPointer<PipelineManager>();
}

void RoboticsRegisterModel::SetRegisterNavigationPipeline(itk::SmartPointer<PipelineManager> manager)
{
}

void RoboticsRegisterModel::SetAccutacyVerifyPipeline(itk::SmartPointer<PipelineManager> manager)
{
}

itk::SmartPointer<PipelineManager> RoboticsRegisterModel::GetAccutacyVerifyPipeline() const
{
	return itk::SmartPointer<PipelineManager>();
}

mitk::NavigationDataSource* RoboticsRegisterModel::GetNdiNavigationDataSource() const
{
	return nullptr;
}

void RoboticsRegisterModel::SetNdiNavigationDataSource(mitk::NavigationDataSource* source)
{
}

mitk::NavigationDataSource* RoboticsRegisterModel::GetRoboticsNavigationDataSource() const
{
	return nullptr;
}

void RoboticsRegisterModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource* source)
{
}



END_SPATIAL_FITTING_NAMESPACE
