#include "lancetSpatialFittingRoboticsRegisterDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


RoboticsRegisterDirector::RoboticsRegisterDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);
}

RoboticsRegisterDirector::~RoboticsRegisterDirector()
{
	
}

bool RoboticsRegisterDirector::Builder()
{
	PipelineBuilder::Pointer pipelineBuilder = dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

	if (pipelineBuilder.IsNull())
	{
		return false;
	}
	pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0, nullptr);
	pipelineBuilder->BuilderNavigationToolToSpaceFilter(1, nullptr);
	pipelineBuilder->BuilderNavigationToolCollector(2, 10);

	pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
	pipelineBuilder->GetOutput()->FindFilter(1)->SetName("name-02");
	pipelineBuilder->GetOutput()->FindFilter(2)->SetName("NRT2NRRCollector");
	return true;
}

END_SPATIAL_FITTING_NAMESPACE
