#include "lancetSpatialFittingProbeCheckPointDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


ProbeCheckPointDirector::ProbeCheckPointDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);

	this->robotMatrix = mitk::AffineTransform3D::New();
}

ProbeCheckPointDirector::~ProbeCheckPointDirector()
{
	
}
mitk::NavigationDataSource::Pointer ProbeCheckPointDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void ProbeCheckPointDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer ProbeCheckPointDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void ProbeCheckPointDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool ProbeCheckPointDirector::Builder()
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
		int probeMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool1");
		int baseMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool2");
		if (baseMarkerIndex == -1)
		{
			throw std::exception("No mechanical arm trolley marker tool found.");
		}

		pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,
			this->GetNdiNavigationDataSource()->GetOutput(baseMarkerIndex));
		pipelineBuilder->BuilderNavigationToolCollector(1, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
		pipelineBuilder->GetOutput()->FindFilter(1)->SetName("ProbeRF2RobotEndRF_ToolCollector");

		pipelineBuilder->GetOutput()->ConnectTo(this->GetNdiNavigationDataSource());
		pipelineBuilder->GetOutput()->SetInput(this->GetNdiNavigationDataSource()->GetOutput(probeMarkerIndex));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void ProbeCheckPointDirector::SetRoboticsMatrix(mitk::AffineTransform3D::Pointer matrix)
{
	this->robotMatrix = matrix;
}

mitk::AffineTransform3D::Pointer ProbeCheckPointDirector::GetRoboticsMatrix() const
{
	return this->robotMatrix;
}

END_SPATIAL_FITTING_NAMESPACE
