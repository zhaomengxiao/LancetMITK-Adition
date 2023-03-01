#include "lancetSpatialFittingPelvisCheckPointModel.h"

#include <QTimer>

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicCheckPointDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvisCheckPointModel::PelvisCheckPointModelPrivateImp
{
	QTimer tm;

	PipelineManager::Pointer checkPointPipeline;

	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	mitk::NavigationData::Pointer checkPoint;
};

PelvisCheckPointModel::PelvisCheckPointModel()
	: imp(std::make_shared<PelvisCheckPointModelPrivateImp>())
{
	this->imp->tm.setInterval(20);
	connect(&this->imp->tm, &QTimer::timeout, this, [=]() {
		if (this->GetCheckPointPipeline().IsNotNull())
		{
			this->GetCheckPointPipeline()->UpdateFilter();
		}
		});
}

void PelvisCheckPointModel::Start()
{
	this->imp->tm.start();
}

void PelvisCheckPointModel::Stop()
{
	this->imp->tm.stop();
}

itk::SmartPointer<PipelineManager> PelvisCheckPointModel::GetCheckPointPipeline() const
{
	return this->imp->checkPointPipeline;
}

void PelvisCheckPointModel::SetCheckPointPipelinePipeline(const itk::SmartPointer<PipelineManager>& pipeline)
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

	PelvicCheckPointDirector::Pointer roboticsRegisterDirector = PelvicCheckPointDirector::New();
	roboticsRegisterDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	roboticsRegisterDirector->Builder();

	this->SetCheckPointPipelinePipeline(roboticsRegisterDirector->GetBuilder()->GetOutput());
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
