#include "lancetSpatialFittingRoboticsRegisterModel.h"
#include "core/lancetSpatialFittingPipelineManager.h"
#include "core/lancetSpatialFittingRoboticsRegisterDirector.h"
#include "core/lancetSpatialFittingRoboticsVerifyDirector.h"
#include <robotRegistration.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct RoboticsRegisterModel::RoboticsRegisterModelPrivateImp
{
	RobotRegistration roboticsRegisterModel;

	PipelineManager::Pointer pipelineRoboticsRegisterManager;
	PipelineManager::Pointer pipelineRoboticsAccuracyManager;

	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;
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

mitk::NavigationDataSource::Pointer RoboticsRegisterModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void RoboticsRegisterModel::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

mitk::NavigationDataSource::Pointer RoboticsRegisterModel::GetRoboticsNavigationDataSource() const
{
	return this->imp->roboticsNavigationDataSource;
}

void RoboticsRegisterModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->roboticsNavigationDataSource = source;
}

void RoboticsRegisterModel::ConfigureRegisterPipeline()
{
	lancet::spatial_fitting::RoboticsRegisterDirector::Pointer roboticsRegisterDirector
		= lancet::spatial_fitting::RoboticsRegisterDirector::New();
	roboticsRegisterDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	roboticsRegisterDirector->Builder();
}

void RoboticsRegisterModel::ConfigureVerifyPipeline()
{
	lancet::spatial_fitting::RoboticsVerifyDirector::Pointer roboticsVerifyDirector
		= lancet::spatial_fitting::RoboticsVerifyDirector::New();
	roboticsVerifyDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	roboticsVerifyDirector->Builder();

}
END_SPATIAL_FITTING_NAMESPACE
