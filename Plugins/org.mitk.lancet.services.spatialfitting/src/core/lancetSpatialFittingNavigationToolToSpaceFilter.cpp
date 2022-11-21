#include "lancetSpatialFittingNavigationToolToSpaceFilter.h"
BEGIN_SPATIAL_FITTING_NAMESPACE

struct NavigationToolToSpaceFilter::NavigationToolToSpaceFilterPrivateImp
{
	vtkMatrix4x4* convertMatrix4x4 = vtkMatrix4x4::New();
};

NavigationToolToSpaceFilter::NavigationToolToSpaceFilter()
	: imp(std::make_shared<NavigationToolToSpaceFilterPrivateImp>())
{
	
}

NavigationToolToSpaceFilter::~NavigationToolToSpaceFilter()
{
}

vtkMatrix4x4* NavigationToolToSpaceFilter::GetConvertMatrix4x4() const
{
	return this->imp->convertMatrix4x4;
}

void NavigationToolToSpaceFilter::SetConvertMatrix4x4(const vtkMatrix4x4* matrix)
{
	this->imp->convertMatrix4x4->DeepCopy(matrix);
}

void NavigationToolToSpaceFilter::GenerateData()
{
}
END_SPATIAL_FITTING_NAMESPACE

