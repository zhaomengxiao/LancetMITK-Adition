#include "lancetSpatialFittingAbstractDirector.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct AbstractDirector::AbstractDirectorPrivateImp
{
	itk::SmartPointer<AbstractPipelineBuilder> builder;
};

AbstractDirector::AbstractDirector()
	: imp(std::make_shared<AbstractDirectorPrivateImp>())
{
}

itk::SmartPointer<AbstractPipelineBuilder> AbstractDirector::GetBuilder() const
{
	return itk::SmartPointer<AbstractPipelineBuilder>();
}

void AbstractDirector::SetBuilder(itk::SmartPointer<AbstractPipelineBuilder> builder)
{
}


END_SPATIAL_FITTING_NAMESPACE

