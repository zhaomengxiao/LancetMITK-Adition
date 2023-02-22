#include "lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.h"
#include "lancetSpatialFittingPipelineBuilder.h"
#include "lancetSpatialFittingPipelineManager.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsVerifyDirector::PelvicRoughRegistrationsVerifyDirectorPrivateImp
{
	mitk::AffineTransform3D::Pointer imageConvertMatrix;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
};

PelvicRoughRegistrationsVerifyDirector::PelvicRoughRegistrationsVerifyDirector()
	: imp(std::make_shared<PelvicRoughRegistrationsVerifyDirectorPrivateImp>())
{
	PipelineBuilder::Pointer pipelineBuilder = PipelineBuilder::New();
	this->SetBuilder(pipelineBuilder);
}

PelvicRoughRegistrationsVerifyDirector::~PelvicRoughRegistrationsVerifyDirector()
{
}

bool PelvicRoughRegistrationsVerifyDirector::Builder()
{
	PipelineBuilder::Pointer pipelineBuilder = 
		dynamic_cast<PipelineBuilder*>(this->GetBuilder().GetPointer());

	if (pipelineBuilder.IsNull() || this->GetNdiNavigationDataSource().IsNull())
	{
		return false;
	}

	try
	{
		pipelineBuilder->BuilderNavigationToolToSpaceFilter(0, 
			this->GetImageConvertMatrix().GetPointer());

		pipelineBuilder->GetOutput()->FindFilter(0)->SetName("PelvisRF2Image");

		pipelineBuilder->GetOutput()->ConnectTo(this->GetNdiNavigationDataSource());
	}
	catch (...)
	{
		return false;
	}

	return true;
}

mitk::NavigationDataSource::Pointer 
  PelvicRoughRegistrationsVerifyDirector::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvicRoughRegistrationsVerifyDirector::SetNdiNavigationDataSource(
	mitk::NavigationDataSource::Pointer navigationDataSource)
{
	this->imp->ndiNavigationDataSource = navigationDataSource;
}

mitk::AffineTransform3D::Pointer 
  PelvicRoughRegistrationsVerifyDirector::GetImageConvertMatrix() const
{
	return this->imp->imageConvertMatrix;
}

void PelvicRoughRegistrationsVerifyDirector::SetImageConvertMatrix(
	const mitk::AffineTransform3D::Pointer& matrix)
{
	this->imp->imageConvertMatrix = matrix;
}






END_SPATIAL_FITTING_NAMESPACE
