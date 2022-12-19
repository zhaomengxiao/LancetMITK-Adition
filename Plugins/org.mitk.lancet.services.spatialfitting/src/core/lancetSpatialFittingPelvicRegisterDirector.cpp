#include "lancetSpatialFittingPelvicRegisterDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


PelvisRegisterDirector::PelvisRegisterDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);
}

PelvisRegisterDirector::~PelvisRegisterDirector()
{
	
}
mitk::NavigationDataSource::Pointer PelvisRegisterDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void PelvisRegisterDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer PelvisRegisterDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void PelvisRegisterDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool PelvisRegisterDirector::Builder()
{
	PipelineBuilder::Pointer pipelineBuilder = dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

	if (pipelineBuilder.IsNull() || this->GetNdiNavigationDataSource().IsNull())
	{
		return false;
	}

	try
	{
		// RobotBaseRF
		// RobotEndRF
		int baseMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool1");
		int probeMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool2");
		if (baseMarkerIndex == -1)
		{
			throw std::exception("No mechanical arm trolley marker tool found.");
		}

		pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,
			this->GetNdiNavigationDataSource()->GetOutput(baseMarkerIndex));
		pipelineBuilder->BuilderNavigationToolCollector(1, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
		pipelineBuilder->GetOutput()->FindFilter(1)->SetName("ProbeRF2PelvisRF_ToolCollector");

		// connect to device pipeline resourec filter.
		//this->GetNdiNavigationDataSource()->GetOutput(robotEndMarkerIndex);
		pipelineBuilder->GetOutput()->ConnectTo(this->GetNdiNavigationDataSource());
		pipelineBuilder->GetOutput()->SetInput(this->GetNdiNavigationDataSource()->GetOutput(probeMarkerIndex));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

END_SPATIAL_FITTING_NAMESPACE
