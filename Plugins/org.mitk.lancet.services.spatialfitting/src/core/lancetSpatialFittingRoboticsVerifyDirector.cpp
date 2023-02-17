#include "lancetSpatialFittingRoboticsVerifyDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


RoboticsVerifyDirector::RoboticsVerifyDirector() :RoboticsRegisterDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);

	this->robotMatrix = mitk::AffineTransform3D::New();
}

RoboticsVerifyDirector::~RoboticsVerifyDirector()
{
	
}
mitk::NavigationDataSource::Pointer RoboticsVerifyDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void RoboticsVerifyDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer RoboticsVerifyDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void RoboticsVerifyDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool RoboticsVerifyDirector::Builder()
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
		int robotMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("RobotBaseRF");
		int robotEndMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("RobotEndRF");
		if (robotMarkerIndex == -1)
		{
			throw std::exception("No mechanical arm trolley marker tool found.");
		}

		pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,
			this->GetNdiNavigationDataSource()->GetOutput(robotMarkerIndex));
		pipelineBuilder->BuilderNavigationToolToSpaceFilter(1, this->GetRoboticsMatrix());
		pipelineBuilder->BuilderNavigationToolCollector(2, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
		pipelineBuilder->GetOutput()->FindFilter(2)->SetName("RobotEndRF2RobotBaseRF_ToolCollector");

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

void RoboticsVerifyDirector::SetRoboticsMatrix(mitk::AffineTransform3D::Pointer matrix)
{
	this->robotMatrix = matrix;
}

mitk::AffineTransform3D::Pointer RoboticsVerifyDirector::GetRoboticsMatrix() const
{
	return this->robotMatrix;
}

END_SPATIAL_FITTING_NAMESPACE
