#include "lancetSpatialFittingPelvicCheckPointVerifyDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


PelvicCheckPointVerifyDirector::PelvicCheckPointVerifyDirector() :PelvicCheckPointDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);

	this->robotMatrix = mitk::AffineTransform3D::New();
}

PelvicCheckPointVerifyDirector::~PelvicCheckPointVerifyDirector()
{
	
}
mitk::NavigationDataSource::Pointer PelvicCheckPointVerifyDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void PelvicCheckPointVerifyDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer PelvicCheckPointVerifyDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void PelvicCheckPointVerifyDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool PelvicCheckPointVerifyDirector::Builder()
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
		int robotMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool1");
		int robotEndMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("VirtualTool2");
		if (robotMarkerIndex == -1)
		{
			throw std::exception("No mechanical arm trolley marker tool found.");
		}

		pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,
			this->GetNdiNavigationDataSource()->GetOutput(robotMarkerIndex));
		pipelineBuilder->BuilderNavigationToolCollector(1, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
		pipelineBuilder->GetOutput()->FindFilter(1)->SetName("RobotEndRF2RobotBaseRF_ToolCollector");

		// connect to device pipeline resourec filter.
		//this->GetNdiNavigationDataSource()->GetOutput(robotEndMarkerIndex);
		pipelineBuilder->GetOutput()->ConnectTo(this->GetNdiNavigationDataSource());
		pipelineBuilder->GetOutput()->SetInput(this->GetNdiNavigationDataSource()->GetOutput(robotEndMarkerIndex));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void PelvicCheckPointVerifyDirector::SetRoboticsMatrix(mitk::AffineTransform3D::Pointer matrix)
{
	this->robotMatrix = matrix;
}

mitk::AffineTransform3D::Pointer PelvicCheckPointVerifyDirector::GetRoboticsMatrix() const
{
	return this->robotMatrix;
}

END_SPATIAL_FITTING_NAMESPACE
