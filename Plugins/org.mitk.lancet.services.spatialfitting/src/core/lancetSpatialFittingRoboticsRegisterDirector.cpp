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

mitk::NavigationDataSource::Pointer RoboticsRegisterDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void RoboticsRegisterDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}

bool RoboticsRegisterDirector::Builder()
{
	PipelineBuilder::Pointer pipelineBuilder = dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

	if (pipelineBuilder.IsNull() || this->GetNdiNavigationDataSource().IsNull())
	{
		return false;
	}
    int robotMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("");
	pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,  
		this->GetNdiNavigationDataSource()->GetOutput(robotMarkerIndex));
	//pipelineBuilder->BuilderNavigationToolToSpaceFilter(1, nullptr);
	pipelineBuilder->BuilderNavigationToolCollector(1, 10);

	pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
	//pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-02");
	pipelineBuilder->GetOutput()->FindFilter(1)->SetName("RobotEndRF2RobotBaseRF_ToolCollector");
	return true;
}

END_SPATIAL_FITTING_NAMESPACE
