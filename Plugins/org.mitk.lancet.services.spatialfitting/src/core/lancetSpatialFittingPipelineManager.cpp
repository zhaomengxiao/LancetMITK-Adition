#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PipelineManager::PipelineManagerPrivateImp
{
	std::vector<mitk::NavigationDataToNavigationDataFilter::Pointer> vecFilterManage;
};

PipelineManager::PipelineManager()
	: imp(std::make_shared<PipelineManagerPrivateImp>())
{
}

PipelineManager::~PipelineManager()
{
}

bool PipelineManager::AddFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer)
{
	return false;
}

bool PipelineManager::RemoveFilter(int index)
{
	return false;
}

bool PipelineManager::RemoveFilter(const std::string&)
{
	return false;
}

bool PipelineManager::ModifyFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer)
{
	return false;
}

bool PipelineManager::ModifyFilter(const std::string& name, mitk::NavigationDataToNavigationDataFilter::Pointer)
{
	return false;
}

mitk::NavigationDataToNavigationDataFilter::Pointer PipelineManager::FindFilter(int index)
{
	return mitk::NavigationDataToNavigationDataFilter::Pointer();
}

mitk::NavigationDataToNavigationDataFilter::Pointer PipelineManager::FindFilter(const std::string& name)
{
	return mitk::NavigationDataToNavigationDataFilter::Pointer();
}

int PipelineManager::GetIndexByName(const std::string& name)
{
	return 0;
}

bool PipelineManager::IsEmpty() const
{
	return this->imp->vecFilterManage.empty();
}

int PipelineManager::GetSize() const
{
	return this->imp->vecFilterManage.size();
}

void PipelineManager::GenerateData()
{
}


END_SPATIAL_FITTING_NAMESPACE
