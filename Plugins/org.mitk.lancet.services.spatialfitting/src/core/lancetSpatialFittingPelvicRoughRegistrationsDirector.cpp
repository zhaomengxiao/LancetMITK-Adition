#include "lancetSpatialFittingPelvicRoughRegistrationsDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsDirector::PelvicRoughRegistrationsDirectorPrivateImp
{
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
};

PelvicRoughRegistrationsDirector::PelvicRoughRegistrationsDirector()
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);
}

PelvicRoughRegistrationsDirector::~PelvicRoughRegistrationsDirector()
{
}

bool PelvicRoughRegistrationsDirector::Builder()
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
		int probeMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("ProbeTHA");
		int baseMarkerIndex = this->GetNdiNavigationDataSource()->GetOutputIndex("PelvisRF");
		if (baseMarkerIndex == -1)
		{
			throw std::exception("No mechanical arm trolley marker tool found.");
		}

		pipelineBuilder->BuilderNavigationToolToNavigationToolFilter(0,
			this->GetNdiNavigationDataSource()->GetOutput(baseMarkerIndex));
		pipelineBuilder->BuilderNavigationToolCollector(1, 10, 20);

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("ProbeRF2PelvisRF");
		pipelineBuilder->GetOutput()->FindFilter(1)->SetName("ProbeRF2PelvisRF_ToolCollector");

		pipelineBuilder->GetOutput()->ConnectTo(this->GetNdiNavigationDataSource());
		pipelineBuilder->GetOutput()->SetInput(this->GetNdiNavigationDataSource()->GetOutput(probeMarkerIndex));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

mitk::NavigationDataSource::Pointer PelvicRoughRegistrationsDirector::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvicRoughRegistrationsDirector::SetNdiNavigationDataSource(
	mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->imp->ndiNavigationDataSource = navigationDataSource;
}

END_SPATIAL_FITTING_NAMESPACE
