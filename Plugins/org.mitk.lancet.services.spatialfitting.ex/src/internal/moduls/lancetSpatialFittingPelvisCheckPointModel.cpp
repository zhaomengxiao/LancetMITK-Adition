#include "lancetSpatialFittingPelvisCheckPointModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicCheckPointDirector.h"

#include <QTimer>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvisCheckPointModel::PrivateImp
{
	QTimer tm;
	PipelineManager::Pointer checkPointPipeline;
};

PelvisCheckPointModel::PelvisCheckPointModel(const QString& serialNumber)
	: AbstractModel(serialNumber)
	, imp(std::make_shared<PrivateImp>())
{
	connect(&this->imp->tm, &QTimer::timeout, this, [=]() 
	{
		if (this->GetCheckPointPipeline().IsNotNull())
		{
			this->GetCheckPointPipeline()->UpdateFilter();
		}
	});
}

bool PelvisCheckPointModel::Initialize()
{
	this->imp->tm.setInterval(20);

	if (this->GetNdiNavigationDataSource().IsNull() ||
		this->GetRoboticsNavigationDataSource().IsNull())
	{
		return false;
	}

	PelvicCheckPointDirector::Pointer pelvicCheckPointDirector = PelvicCheckPointDirector::New();
	pelvicCheckPointDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	if (false == pelvicCheckPointDirector->Builder())
	{
		return false;
	}
	this->SetCheckPointPipelinePipeline(pelvicCheckPointDirector->GetBuilder()->GetOutput());
	return true;
}

bool PelvisCheckPointModel::Uninitialize()
{
	this->SetNdiNavigationDataSource(nullptr);
	this->SetRoboticsNavigationDataSource(nullptr);
	this->SetCheckPointPipelinePipeline(nullptr);
	return true;
}

bool PelvisCheckPointModel::isInitialize() const
{
	return this->GetCheckPointPipeline().IsNotNull() 
		&& this->GetNdiNavigationDataSource().IsNotNull() 
		&& this->GetRoboticsNavigationDataSource().IsNotNull();
}

bool PelvisCheckPointModel::StartWorking()
{
	this->imp->tm.start();
	return this->IsWorking();
}


bool PelvisCheckPointModel::StopWorking()
{
	this->imp->tm.stop();
	return false == this->IsWorking();
}

bool PelvisCheckPointModel::IsWorking() const
{
	return this->imp->tm.isActive();
}

itk::SmartPointer<PipelineManager> PelvisCheckPointModel::GetCheckPointPipeline() const
{
	return this->imp->checkPointPipeline;
}

void PelvisCheckPointModel::SetCheckPointPipelinePipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->checkPointPipeline = pipeline;
}



END_SPATIAL_FITTING_NAMESPACE


