#include "lancetSpatialFittingPelvicRoughRegistrationsModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModelPrivateImp
{
	PipelineManager::Pointer pipelineManager;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;

	std::array<mitk::Point3D, 3> imagePointArray;
	std::array<mitk::Point3D, 3> vegaPointArray;
};

PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModel()
	: imp(std::make_shared<PelvicRoughRegistrationsModelPrivateImp>())
{
}

mitk::NavigationDataSource::Pointer 
  PelvicRoughRegistrationsModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvicRoughRegistrationsModel::SetNdiNavigationDataSource(
	mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

itk::SmartPointer<PipelineManager> 
  PelvicRoughRegistrationsModel::GetRegistrationPipeline() const
{
	return this->imp->pipelineManager;
}

void PelvicRoughRegistrationsModel::SetRegistrationPipeline(
	const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->pipelineManager = pipeline;
}

void PelvicRoughRegistrationsModel::ConfigureRegistrationsPipeline()
{
	using namespace lancet::spatial_fitting;

	PelvicRoughRegistrationsDirector::Pointer pelvicRoughRegistrationsDirector =
		PelvicRoughRegistrationsDirector::New();

	pelvicRoughRegistrationsDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());

	this->SetRegistrationPipeline(pelvicRoughRegistrationsDirector->GetBuilder()->GetOutput());
}

void PelvicRoughRegistrationsModel::SetImagePointArray(int index, const mitk::Point3D& pt)
{
	if (index < this->imp->imagePointArray.size())
	{
		this->imp->imagePointArray[index] = pt;
	}
}

void PelvicRoughRegistrationsModel::SetImagePointArray(const std::array<mitk::Point3D, 3>& ptArray)
{
	this->SetImagePointArray(0, ptArray[0]);
	this->SetImagePointArray(1, ptArray[1]);
	this->SetImagePointArray(2, ptArray[2]);
}

mitk::Point3D PelvicRoughRegistrationsModel::GetImagePoint(int index) const
{
	if (index < this->imp->imagePointArray.size())
	{
		return this->imp->imagePointArray[index];
	}
	return mitk::Point3D();
}

void PelvicRoughRegistrationsModel::SetVegaPointArray(int index, const mitk::Point3D& pt)
{
	if (index < this->imp->vegaPointArray.size())
	{
		this->imp->vegaPointArray[index] = pt;
	}
}

void PelvicRoughRegistrationsModel::SetVegaPointArray(const std::array<mitk::Point3D, 3>& ptArray)
{
	this->SetVegaPointArray(0, ptArray[0]);
	this->SetVegaPointArray(1, ptArray[1]);
	this->SetVegaPointArray(2, ptArray[2]);
}

mitk::Point3D PelvicRoughRegistrationsModel::GetVegaPoint(int index) const
{
	if (index < this->imp->vegaPointArray.size())
	{
		return this->imp->vegaPointArray[index];
	}
	return mitk::Point3D();
}

END_SPATIAL_FITTING_NAMESPACE
