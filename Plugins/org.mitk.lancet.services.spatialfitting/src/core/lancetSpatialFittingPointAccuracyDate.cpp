#include "lancetSpatialFittingPointAccuracyDate.h"


BEGIN_SPATIAL_FITTING_NAMESPACE

struct PointAccuracyDate::PointAccuracyDatePrivateImp
{
	mitk::Point3D srcPoint;
	mitk::Point3D targetPoint;
};

PointAccuracyDate::PointAccuracyDate() 
	: imp(std::make_shared<PointAccuracyDatePrivateImp>())
{
}

PointAccuracyDate::PointAccuracyDate(const mitk::Point3D& src,
	const mitk::Point3D& target)
	: imp(std::make_shared<PointAccuracyDatePrivateImp>())
{
	this->SetSourcePoint(src);
	this->SetTargetPoint(target);
}

mitk::Point3D PointAccuracyDate::GetSourcePoint() const
{
	return mitk::Point3D();
}

void PointAccuracyDate::SetSourcePoint(const mitk::Point3D&)
{
}

mitk::Point3D PointAccuracyDate::GetTargetPoint() const
{
	return mitk::Point3D();
}

void PointAccuracyDate::SetTargetPoint(const mitk::Point3D&)
{
}

double PointAccuracyDate::Compute() const
{
	return 0.0;
}



END_SPATIAL_FITTING_NAMESPACE
