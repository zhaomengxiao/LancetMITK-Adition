#include "lancetSpatialFittingRoboticsRegisterModel.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPipelineManager.h"
#include "core/lancetSpatialFittingRoboticsRegisterDirector.h"
#include "core/lancetSpatialFittingRoboticsVerifyDirector.h"

#include <robotRegistration.h>

#include <QTimer>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct RoboticsRegisterModel::PrivateImp
{
	QTimer tm;
	RobotRegistration roboticsRegisterModel;
	//RoboticsRegisterPointSet roboticsRegisterPointSet;

	PipelineManager::Pointer pipelineRoboticsRegisterManager;
	PipelineManager::Pointer pipelineRoboticsAccuracyManager;
};

RoboticsRegisterModel::RoboticsRegisterModel(const QString& serialNumber)
	: AbstractModel(serialNumber)
	, imp(std::make_shared<PrivateImp>())
{
	connect(&this->imp->tm, &QTimer::timeout, this, [=]()
	{
		if (this->GetRegisterNavigationPipeline().IsNotNull())
		{
			this->GetRegisterNavigationPipeline()->UpdateFilter();
		}
		if (this->GetAccutacyVerifyPipeline().IsNotNull())
		{
			this->GetAccutacyVerifyPipeline()->UpdateFilter();
		}
	});
}

bool RoboticsRegisterModel::Initialize()
{
	this->imp->tm.setInterval(20);

	if (this->GetNdiNavigationDataSource().IsNull() ||
		this->GetRoboticsNavigationDataSource().IsNull())
	{
		return false;
	}
	RoboticsRegisterDirector::Pointer roboticsRegisterDirector = RoboticsRegisterDirector::New();
	roboticsRegisterDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	if (false == roboticsRegisterDirector->Builder())
	{
		return false;
	}
	this->SetRegisterNavigationPipeline(roboticsRegisterDirector->GetBuilder()->GetOutput());

	RoboticsVerifyDirector::Pointer roboticsVerifyDirector = RoboticsVerifyDirector::New();
	roboticsVerifyDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	if (false == roboticsVerifyDirector->Builder())
	{
		return false;
	}
	this->SetAccutacyVerifyPipeline(roboticsVerifyDirector->GetBuilder()->GetOutput());

	return true;
}

bool RoboticsRegisterModel::Uninitialize()
{
	this->SetAccutacyVerifyPipeline(nullptr);
	this->SetRegisterNavigationPipeline(nullptr);
	this->SetNdiNavigationDataSource(nullptr);
	this->SetRoboticsNavigationDataSource(nullptr);
	return true;
}

bool RoboticsRegisterModel::isInitialize() const
{
	return this->GetNdiNavigationDataSource().IsNotNull() && 
		this->GetRoboticsNavigationDataSource().IsNotNull() &&
		this->GetAccutacyVerifyPipeline().IsNotNull() && 
		this->GetRegisterNavigationPipeline().IsNotNull();
}

bool RoboticsRegisterModel::StartWorking()
{
	this->imp->tm.start();
	return this->IsWorking();
}

bool RoboticsRegisterModel::StopWorking()
{
	this->imp->tm.stop();
	return false == this->IsWorking();
}

bool RoboticsRegisterModel::IsWorking() const
{
	return this->imp->tm.isActive();
}

RobotRegistration& RoboticsRegisterModel::GetRegisterModel()
{
	return this->imp->roboticsRegisterModel;
}

//RoboticsRegisterModel::RoboticsRegisterPointSet& RoboticsRegisterModel::GetRegisterPointSet()
//{
//	return this->imp->roboticsRegisterPointSet;
//}

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

END_SPATIAL_FITTING_NAMESPACE