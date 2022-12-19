#include "lancetSpatialFittingPelvicVerifyDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE


PelvisVerifyDirector::PelvisVerifyDirector() :PelvisRegisterDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);

	this->convertMatrix = mitk::AffineTransform3D::New();
}

PelvisVerifyDirector::~PelvisVerifyDirector()
{
	
}
mitk::NavigationDataSource::Pointer PelvisVerifyDirector::GetNdiNavigationDataSource() const
{
	return this->ndiNavigationDataSource;
}

void PelvisVerifyDirector::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{

	this->ndiNavigationDataSource = navigationDataSource;
}
mitk::NavigationDataSource::Pointer PelvisVerifyDirector::GetRobotNavigationDataSource() const
{
	return this->roboticsNavigationDataSource;
}
void PelvisVerifyDirector::SetRobotNavigationDataSource(mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->roboticsNavigationDataSource = navigationDataSource;
}
bool PelvisVerifyDirector::Builder()
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
		pipelineBuilder->BuilderNavigationToolToSpaceFilter(1, this->GetConvertMatrix());
		pipelineBuilder->BuilderNavigationToolCollector(2, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("name-01");
		pipelineBuilder->GetOutput()->FindFilter(1)->SetName("name-02");
		pipelineBuilder->GetOutput()->FindFilter(2)->SetName("ProbeRF2Image_ToolCollector");

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
void PelvisVerifyDirector::SetConvertMatrix(mitk::AffineTransform3D::Pointer matrix)
{
	this->convertMatrix = matrix;
}

mitk::AffineTransform3D::Pointer PelvisVerifyDirector::GetConvertMatrix() const
{
	return this->convertMatrix;
}
END_SPATIAL_FITTING_NAMESPACE
