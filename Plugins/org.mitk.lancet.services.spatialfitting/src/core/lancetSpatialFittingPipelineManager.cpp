#include "lancetSpatialFittingPipelineManager.h"
#include <vector>
#include <algorithm>
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

bool PipelineManager::AddFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer filter)
{
	if (filter.IsNull() || index < 0)
	{
		return false;
	}

	if (index >= this->imp->vecFilterManage.size())
	{
		this->imp->vecFilterManage.push_back(filter);
		this->UpdateConntedToFilter();
		return true;
	}
	this->imp->vecFilterManage.insert(this->imp->vecFilterManage.begin() + index, filter);
	this->UpdateConntedToFilter();
	return true;
}

bool PipelineManager::RemoveFilter(int index)
{
	if (index >= this->GetSize() || index < 0)
	{
		return false;
	}
	this->imp->vecFilterManage.erase(this->imp->vecFilterManage.begin() + index);
	this->UpdateConntedToFilter();
	return true;
}

bool PipelineManager::RemoveFilter(const std::string& name)
{
	return this->RemoveFilter(this->GetIndexByName(name));
}

bool PipelineManager::ModifyFilter(int index, mitk::NavigationDataToNavigationDataFilter::Pointer filter)
{
	if (index >= this->GetSize() || index < 0)
	{
		return false;
	}
	this->imp->vecFilterManage[index]= filter;
	this->UpdateConntedToFilter();
	return true;
}

bool PipelineManager::ModifyFilter(const std::string& name, mitk::NavigationDataToNavigationDataFilter::Pointer filter)
{
	return this->ModifyFilter(this->GetIndexByName(name), filter);
}

mitk::NavigationDataToNavigationDataFilter::Pointer PipelineManager::FindFilter(int index)
{
	if (index < this->GetSize() && index >= 0)
	{
		return this->imp->vecFilterManage[index];
	}
	return mitk::NavigationDataToNavigationDataFilter::Pointer();
}

mitk::NavigationDataToNavigationDataFilter::Pointer PipelineManager::FindFilter(const std::string& name)
{
	return this->FindFilter(this->GetIndexByName(name));
}

int PipelineManager::GetIndexByName(const std::string& name)
{
	for (int index = 0; index < this->GetSize(); ++index)
	{
		if (name == this->imp->vecFilterManage.at(index).GetPointer()->GetName())
		{
			return index;
		}
	}
	return -1;
}

bool PipelineManager::IsEmpty() const
{
	return this->imp->vecFilterManage.empty();
}

int PipelineManager::GetSize() const
{
	return this->imp->vecFilterManage.size();
}

void PipelineManager::UpdateFilter()
{
	if (false == this->IsEmpty())
	{
		this->FindFilter(this->GetSize() - 1)->Update();
	}	
}

void PipelineManager::UpdateConntedToFilter()
{
	this->UpdateDisConntedToFilter();
	for (size_t index = 1; index < this->GetSize(); ++index)
	{
		this->FindFilter(index)->ConnectTo(this->FindFilter(index - 1));
	}
	if (false == this->IsEmpty())
	{
		this->FindFilter(0)->ConnectTo(this);
	}
}

void PipelineManager::UpdateDisConntedToFilter()
{
	for (size_t index = 0; index < this->GetSize(); ++index)
	{
		DataObjectPointerArray outputs = this->FindFilter(index)->GetInputs();
		for (DataObjectPointerArray::size_type i = 0; i < outputs.size(); ++i)
		{
			if (static_cast<mitk::NavigationData*>(outputs.at(i).GetPointer()))
			{ 
				(static_cast<mitk::NavigationData*>(outputs.at(i).GetPointer()))->DisconnectPipeline();
			}
		}
	}
}

void PipelineManager::GenerateData()
{
	MITK_INFO << "log";

	this->CreateOutputsForAllInputs();

	for (size_t index = 0; index < this->GetNumberOfInputs(); ++index)
	{
		if (this->GetInput(index) && this->GetOutput(index))
		{
			this->GetOutput(index)->Graft(this->GetInput(index));
		}
	}
}


END_SPATIAL_FITTING_NAMESPACE
