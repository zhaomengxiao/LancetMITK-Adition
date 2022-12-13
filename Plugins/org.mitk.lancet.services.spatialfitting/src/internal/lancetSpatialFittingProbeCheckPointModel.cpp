#include "lancetSpatialFittingProbeCheckPointModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingProbeCheckPointDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct ProbeCheckPointModel::ProbeCheckPointModelPrivateImp
{
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	PipelineManager::Pointer verifyPipeline;

	mitk::NavigationData::Pointer checkPoint;
};

ProbeCheckPointModel::ProbeCheckPointModel()
	: imp(std::make_shared<ProbeCheckPointModelPrivateImp>())
{
}

itk::SmartPointer<PipelineManager> ProbeCheckPointModel::GetVerifyPipeline() const
{
	return this->imp->verifyPipeline;
}

void ProbeCheckPointModel::SetVerifyPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->verifyPipeline = pipeline;
}

mitk::NavigationDataSource::Pointer ProbeCheckPointModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void ProbeCheckPointModel::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

mitk::NavigationDataSource::Pointer ProbeCheckPointModel::GetRoboticsNavigationDataSource() const
{
	return this->imp->roboticsNavigationDataSource;
}

void ProbeCheckPointModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->roboticsNavigationDataSource = source;
}

void ProbeCheckPointModel::ConfigureVerifyPipeline()
{
	using namespace lancet::spatial_fitting;

	ProbeCheckPointDirector::Pointer probeCheckPointDirector
		= ProbeCheckPointDirector::New();
	probeCheckPointDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	probeCheckPointDirector->Builder();

	this->SetVerifyPipeline(probeCheckPointDirector->GetBuilder()->GetOutput());
}

mitk::NavigationData* ProbeCheckPointModel::GetVerifyEndMarkerData() const
{
	if (this->GetNdiNavigationDataSource().IsNotNull())
	{
		return this->GetNdiNavigationDataSource()->GetOutput("VirtualTool3");
	}
	return nullptr;
}

void ProbeCheckPointModel::SetCheckPoint(mitk::NavigationData::Pointer pt)
{
	this->imp->checkPoint = pt;
}

mitk::NavigationData::Pointer ProbeCheckPointModel::GetCheckPoint() const
{
	return this->imp->checkPoint;
}


END_SPATIAL_FITTING_NAMESPACE