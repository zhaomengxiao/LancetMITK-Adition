#include "lancetSpatialFittingNavigationToolToNavigationToolFilter.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct NavigationToolToNavigationToolFilter::NavigationToolToNavigationToolFilterPrivateImp
{
	mitk::NavigationData::Pointer ucsTool;
};

NavigationToolToNavigationToolFilter::NavigationToolToNavigationToolFilter()
	: imp(std::make_shared<NavigationToolToNavigationToolFilterPrivateImp>())
{
	
}

NavigationToolToNavigationToolFilter::~NavigationToolToNavigationToolFilter()
{
}

mitk::NavigationData::Pointer 
  NavigationToolToNavigationToolFilter::GetUCSTool() const
{
	return mitk::NavigationData::Pointer();
}

void NavigationToolToNavigationToolFilter::SetUCSTool(
	mitk::NavigationData::Pointer tool)
{
}

void NavigationToolToNavigationToolFilter::GenerateData()
{

}

END_SPATIAL_FITTING_NAMESPACE
