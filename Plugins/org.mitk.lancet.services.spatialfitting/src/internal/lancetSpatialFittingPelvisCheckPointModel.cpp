#include "lancetSpatialFittingPelvisCheckPointModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicCheckPointDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvisCheckPointModel::PelvisCheckPointModelPrivateImp
{
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	PipelineManager::Pointer checkPointPipeline;
	mitk::NavigationData::Pointer checkPoint;
};

PelvisCheckPointModel::PelvisCheckPointModel()
	: imp(std::make_shared<PelvisCheckPointModelPrivateImp>())
{
}

itk::SmartPointer<PipelineManager> PelvisCheckPointModel::GetCheckPointPipeline() const
{
	return this->imp->checkPointPipeline;
}

void PelvisCheckPointModel::SetCheckPointPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->checkPointPipeline = pipeline;
}

mitk::NavigationDataSource::Pointer PelvisCheckPointModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvisCheckPointModel::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

mitk::NavigationDataSource::Pointer PelvisCheckPointModel::GetRoboticsNavigationDataSource() const
{
	return this->imp->roboticsNavigationDataSource;
}

void PelvisCheckPointModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->roboticsNavigationDataSource = source;
}

void PelvisCheckPointModel::ConfigureGetCheckPointPipeline()
{
	using namespace lancet::spatial_fitting;

	PelvicCheckPointDirector::Pointer pelvicCheckPointDirector = PelvicCheckPointDirector::New();
	pelvicCheckPointDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	pelvicCheckPointDirector->Builder();

	this->SetCheckPointPipeline(pelvicCheckPointDirector->GetBuilder()->GetOutput());
}

void PelvisCheckPointModel::SetCheckPoint(mitk::NavigationData::Pointer pt)
{
	this->imp->checkPoint = pt;
}

mitk::NavigationData::Pointer PelvisCheckPointModel::GetCheckPoint() const
{
	return this->imp->checkPoint;
}

END_SPATIAL_FITTING_NAMESPACE
