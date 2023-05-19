#include "lancetSpatialFittingProbeCheckPointModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingProbeCheckPointDirector.h"

#include <QTimer>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct ProbeCheckPointModel::PrivateImp
{
	QTimer tm;
	PipelineManager::Pointer verifyCheckPointPipeline;
};

ProbeCheckPointModel::ProbeCheckPointModel(const QString& serialNumber)
	: AbstractModel(serialNumber)
	, imp(std::make_shared<PrivateImp>())
{
	connect(&this->imp->tm, &QTimer::timeout, this, [=]()
	{
		if (this->GetVerifyPipeline().IsNotNull())
		{
			this->GetVerifyPipeline()->UpdateFilter();
		}
	});
}

bool ProbeCheckPointModel::Initialize()
{
	this->imp->tm.setInterval(20);
	if (this->GetNdiNavigationDataSource().IsNull() ||
		this->GetRoboticsNavigationDataSource().IsNull())
	{
		return false;
	}

	ProbeCheckPointDirector::Pointer probeCheckPointDirector = ProbeCheckPointDirector::New();
	probeCheckPointDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	if (false == probeCheckPointDirector->Builder())
	{
		return false;
	}
	this->SetVerifyPipeline(probeCheckPointDirector->GetBuilder()->GetOutput());
	return true;
}

bool ProbeCheckPointModel::Uninitialize()
{
	this->SetNdiNavigationDataSource(nullptr);
	this->SetRoboticsNavigationDataSource(nullptr);
	this->SetVerifyPipeline(nullptr);
	return true;
}

bool ProbeCheckPointModel::isInitialize() const
{
	return this->GetVerifyPipeline().IsNotNull()
		&& this->GetNdiNavigationDataSource().IsNotNull()
		&& this->GetRoboticsNavigationDataSource().IsNotNull();
}

bool ProbeCheckPointModel::StartWorking()
{
	this->imp->tm.start();
	return this->IsWorking();
}

bool ProbeCheckPointModel::StopWorking()
{
	this->imp->tm.stop();
	return false == this->IsWorking();
}

bool ProbeCheckPointModel::IsWorking() const
{
	return this->imp->tm.isActive();
}

itk::SmartPointer<PipelineManager> ProbeCheckPointModel::GetVerifyPipeline() const
{
	return this->imp->verifyCheckPointPipeline;
}

void ProbeCheckPointModel::SetVerifyPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->verifyCheckPointPipeline = pipeline;
}

mitk::NavigationData* ProbeCheckPointModel::GetVerifyEndMarkerData() const
{
	if (this->GetNdiNavigationDataSource().IsNotNull())
	{
		return this->GetNdiNavigationDataSource()->GetOutput("VirtualTool3");
	}
	return nullptr;
}

END_SPATIAL_FITTING_NAMESPACE