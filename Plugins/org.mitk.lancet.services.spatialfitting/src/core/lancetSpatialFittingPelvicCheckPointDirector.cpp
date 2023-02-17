#include "lancetSpatialFittingPelvicCheckPointDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


PelvicCheckPointDirector::PelvicCheckPointDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);

	this->robotMatrix = mitk::AffineTransform3D::New();
}

PelvicCheckPointDirector::~PelvicCheckPointDirector()
{
	
}
mitk::NavigationDataSource::Pointer PelvicCheckPointDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void PelvicCheckPointDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer PelvicCheckPointDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void PelvicCheckPointDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool PelvicCheckPointDirector::Builder()
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
		int baseMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("RobotBaseRF");
		int probeMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("RobotEndRF");
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

void PelvicCheckPointDirector::SetRoboticsMatrix(mitk::AffineTransform3D::Pointer matrix)
{
	this->robotMatrix = matrix;
}

mitk::AffineTransform3D::Pointer PelvicCheckPointDirector::GetRoboticsMatrix() const
{
	return this->robotMatrix;
}

END_SPATIAL_FITTING_NAMESPACE
