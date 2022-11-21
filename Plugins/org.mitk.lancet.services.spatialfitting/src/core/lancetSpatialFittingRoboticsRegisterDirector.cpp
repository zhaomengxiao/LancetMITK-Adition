#include "lancetSpatialFittingRoboticsRegisterDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"

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
	return false;
}

END_SPATIAL_FITTING_NAMESPACE
