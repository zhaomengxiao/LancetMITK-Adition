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
	return this->imp->pipelineRoboticsRegisterManager;
}

void RoboticsRegisterModel::SetRegisterNavigationPipeline(itk::SmartPointer<PipelineManager> manager)
{ 
	this->imp->pipelineRoboticsRegisterManager = manager;
}

void RoboticsRegisterModel::SetAccutacyVerifyPipeline(itk::SmartPointer<PipelineManager> manager)
{
	this->imp->pipelineRoboticsAccuracyManager = manager;
}

itk::SmartPointer<PipelineManager> RoboticsRegisterModel::GetAccutacyVerifyPipeline() const
{
	return this->imp->pipelineRoboticsAccuracyManager;
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
