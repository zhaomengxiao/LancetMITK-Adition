#include "lancetSpatialFittingRoboticsVerifyDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


RoboticsVerifyDirector::RoboticsVerifyDirector() :RoboticsRegisterDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);
}

RoboticsVerifyDirector::~RoboticsVerifyDirector()
{
	
}

bool RoboticsVerifyDirector::Builder()
{
	PipelineBuilder::Pointer pipelineBuilder = dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

	if (pipelineBuilder.IsNull())
	{
		return false;
	}

	pipelineBuilder->BuilderNavigationToolToSpaceFilter(0, nullptr);
	pipelineBuilder->BuilderNavigationToolToSpaceFilter(1, nullptr);

	pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(2, nullptr);
	pipelineBuilder->GetOutput()->FindFilter(0)->SetName("Verify-01");
	pipelineBuilder->GetOutput()->FindFilter(1)->SetName("Verify-02");
	return true;
}

END_SPATIAL_FITTING_NAMESPACE
