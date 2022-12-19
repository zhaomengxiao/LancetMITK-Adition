#include "lancetSpatialFittingPelvisMarkPointModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicMarkPointDirector.h"
#include <mitkMatrixConvert.h>
#include <surfaceregistraion.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvisMarkPointModel::PelvisMarkPointModelPrivateImp
{
	mitk::SurfaceRegistration::Pointer surfaceRegistration;

	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	PipelineManager::Pointer markPointPipeline;
	mitk::PointSet::Pointer markPointSet;
	mitk::Surface::Pointer imageSurface;
	mitk::AffineTransform3D::Pointer convertMatrix;
};

PelvisMarkPointModel::PelvisMarkPointModel()
	: imp(std::make_shared<PelvisMarkPointModelPrivateImp>())
{
}
mitk::SurfaceRegistration& PelvisMarkPointModel::GetSurfaceRegistration()
{
	return (*(this->imp->surfaceRegistration.GetPointer()));
}
itk::SmartPointer<PipelineManager> PelvisMarkPointModel::GetMarkPointPipeline() const
{
	return this->imp->markPointPipeline;
}

void PelvisMarkPointModel::SetMarkPointPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->markPointPipeline = pipeline;
}

mitk::NavigationDataSource::Pointer PelvisMarkPointModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvisMarkPointModel::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

mitk::NavigationDataSource::Pointer PelvisMarkPointModel::GetRoboticsNavigationDataSource() const
{
	return this->imp->roboticsNavigationDataSource;
}

void PelvisMarkPointModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->roboticsNavigationDataSource = source;
}

void PelvisMarkPointModel::ConfigurePelvisMarkPointPipeline()
{
	using namespace lancet::spatial_fitting;

	PelvicMarkPointDirector::Pointer pelvicMarkPointDirector = PelvicMarkPointDirector::New();
	pelvicMarkPointDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	pelvicMarkPointDirector->Builder();

	this->SetMarkPointPipeline(pelvicMarkPointDirector->GetBuilder()->GetOutput());
}

void PelvisMarkPointModel::SetMarkPointSet(mitk::PointSet::Pointer pt)
{
	this->imp->markPointSet = pt;
}

mitk::PointSet::Pointer PelvisMarkPointModel::GetMarkPointSet() const
{
	return this->imp->markPointSet;
}

void PelvisMarkPointModel::SetImageSurface(mitk::Surface::Pointer pt)
{
	this->imp->imageSurface = pt;
}

mitk::Surface::Pointer PelvisMarkPointModel::GetImageSurface() const
{
	return this->imp->imageSurface;
}

vtkMatrix4x4* PelvisMarkPointModel::GetRegisterMatrix()
{
	vtkNew<vtkMatrix4x4> registerMatrix;
	registerMatrix->DeepCopy(this->GetSurfaceRegistration().GetResult());
	return registerMatrix;
}
END_SPATIAL_FITTING_NAMESPACE
