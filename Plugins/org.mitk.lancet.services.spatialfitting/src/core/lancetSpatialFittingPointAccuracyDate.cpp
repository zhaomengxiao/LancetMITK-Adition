#include "lancetSpatialFittingPointAccuracyDate.h"


BEGIN_SPATIAL_FITTING_NAMESPACE

struct PointAccuracyDate::PointAccuracyDatePrivateImp
{
	mitk::Point3D srcPoint;
	mitk::Point3D targetPoint;
	double mDistance = 0.0;
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
	return this->imp->srcPoint;
}

void PointAccuracyDate::SetSourcePoint(const mitk::Point3D& pt)
{
	this->imp->srcPoint = pt;
}

mitk::Point3D PointAccuracyDate::GetTargetPoint() const
{
	return this->imp->targetPoint;
}

void PointAccuracyDate::SetTargetPoint(const mitk::Point3D& pt)
{
	this->imp->targetPoint = pt;
}

double PointAccuracyDate::Compute() const
{
	this->imp->mDistance = sqrt
	(
		(this->GetSourcePoint()[0] - this->GetTargetPoint()[0]) * (this->GetSourcePoint()[0] - this->GetTargetPoint()[0]) +
		(this->GetSourcePoint()[1] - this->GetTargetPoint()[1]) * (this->GetSourcePoint()[1] - this->GetTargetPoint()[1]) +
		(this->GetSourcePoint()[2] - this->GetTargetPoint()[2]) * (this->GetSourcePoint()[2] - this->GetTargetPoint()[2])
	);
	return this->imp->mDistance;
}



END_SPATIAL_FITTING_NAMESPACE
