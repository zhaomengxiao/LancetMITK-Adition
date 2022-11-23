#include "lancetSpatialFittingPipelineBuilder.h"

#include "lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingNavigationToolCollector.h"
#include "lancetSpatialFittingNavigationToolToSpaceFilter.h"
#include "lancetSpatialFittingNavigationToolToNavigationToolFilter.h"
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
	this->imp->pipelineManager = PipelineManager::New();
}

itk::SmartPointer<PipelineManager> PipelineBuilder::GetOutput()
{
	return this->imp->pipelineManager;
}

void PipelineBuilder::BuilderNavigationToolToNavigationToolFilter(int index, 
	mitk::NavigationData* ucsTool)
{
	NavigationToolToNavigationToolFilter::Pointer filter = NavigationToolToNavigationToolFilter::New();
	filter->SetUCSTool(ucsTool);
	this->GetOutput()->AddFilter(index, filter);
}

void PipelineBuilder::BuilderNavigationToolToSpaceFilter(int index, 
	vtkMatrix4x4* convert)
{
	NavigationToolToSpaceFilter::Pointer filter = NavigationToolToSpaceFilter::New();
	filter->SetConvertMatrix4x4(convert);
	this->GetOutput()->AddFilter(index, filter);
}

void PipelineBuilder::BuilderNavigationToolCollector(int index, 
	unsigned int number, long interval, double accuracyRange)
{
	NavigationToolCollector::Pointer filter = NavigationToolCollector::New();
	filter->SetInterval(interval);
	filter->SetNumberForMean(number);
	filter->SetAccuracyRange(accuracyRange);
	this->GetOutput()->AddFilter(index, filter);
}


END_SPATIAL_FITTING_NAMESPACE
