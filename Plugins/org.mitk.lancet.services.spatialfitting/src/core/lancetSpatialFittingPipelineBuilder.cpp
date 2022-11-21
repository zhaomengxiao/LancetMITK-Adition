#include "lancetSpatialFittingPipelineBuilder.h"

#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PipelineBuilder::PipelineBuilderPrivateImp
{
	itk::SmartPointer<PipelineManager> pipelineManager = PipelineManager::New();
};

PipelineBuilder::PipelineBuilder()
	: imp(std::make_shared<PipelineBuilderPrivateImp>())
{
}

PipelineBuilder::~PipelineBuilder()
{
}

void PipelineBuilder::Reset()
{
}

itk::SmartPointer<PipelineManager> PipelineBuilder::GetOutput()
{
	return this->imp->pipelineManager;
}

void PipelineBuilder::BuilderNavigationToolToNavigationToolFilter(int index, 
	mitk::NavigationData* ucsTool)
{
}

void PipelineBuilder::BuilderNavigationToolToSpaceFilter(int index, 
	vtkMatrix4x4* convert)
{
}

void PipelineBuilder::BuilderNavigationToolCollector(int index, 
	unsigned int number, long interval, double accuracyRange)
{
}


END_SPATIAL_FITTING_NAMESPACE
