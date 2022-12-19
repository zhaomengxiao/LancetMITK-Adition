#include "lancetSpatialFittingPelvisRegisterModel.h"
#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicRegisterDirector.h"
#include "core/lancetSpatialFittingPelvicVerifyDirector.h"
#include <mitkMatrixConvert.h>
#include <surfaceregistraion.h>
BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvisRegisterModel::PelvisRegisterModelPrivateImp
{
	mitk::SurfaceRegistration::Pointer surfaceRegistration;

	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
	mitk::NavigationDataSource::Pointer roboticsNavigationDataSource;

	PipelineManager::Pointer pelvisRegisterPipeline;
	PipelineManager::Pointer pelvisVerifyPipeline;

	mitk::PointSet::Pointer registerPointSet;
	mitk::Surface::Pointer imageSurface;
};

PelvisRegisterModel::PelvisRegisterModel()
	: imp(std::make_shared<PelvisRegisterModelPrivateImp>())
{
	this->imp->surfaceRegistration = mitk::SurfaceRegistration::New();
}

itk::SmartPointer<PipelineManager> PelvisRegisterModel::GetPelvisRegisterPipeline() const
{
	return this->imp->pelvisRegisterPipeline;
}

void PelvisRegisterModel::SetPelvisRegisterPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->pelvisRegisterPipeline = pipeline;
}

itk::SmartPointer<PipelineManager> PelvisRegisterModel::GetPelvisVerifyPipeline() const
{
	return this->imp->pelvisVerifyPipeline;
}

void PelvisRegisterModel::SetPelvisVerifyPipeline(const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->pelvisVerifyPipeline = pipeline;
}

mitk::NavigationDataSource::Pointer PelvisRegisterModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvisRegisterModel::SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->ndiNavigationDataSource = source;
}

mitk::NavigationDataSource::Pointer PelvisRegisterModel::GetRoboticsNavigationDataSource() const
{
	return this->imp->roboticsNavigationDataSource;
}

void PelvisRegisterModel::SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source)
{
	this->imp->roboticsNavigationDataSource = source;
}

void PelvisRegisterModel::ConfigurePelvisRegisterPipeline()
{
	using namespace lancet::spatial_fitting;

	PelvisRegisterDirector::Pointer pelvisRegisterDirector = PelvisRegisterDirector::New();
	pelvisRegisterDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	pelvisRegisterDirector->Builder();

	this->SetPelvisRegisterPipeline(pelvisRegisterDirector->GetBuilder()->GetOutput());
}
mitk::SurfaceRegistration& PelvisRegisterModel::GetSurfaceRegistration()
{
	return (*(this->imp->surfaceRegistration.GetPointer()));
}
void PelvisRegisterModel::ConfigurePelvisVerifyPipeline()
{
	using namespace lancet::spatial_fitting;
	PelvisVerifyDirector::Pointer pelvisVerifyDirector = PelvisVerifyDirector::New();

	mitk::AffineTransform3D::Pointer mitkTransform3D = mitk::AffineTransform3D::New();

	pelvisVerifyDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());

	mitk::TransferVtkMatrixToItkTransform(this->GetRegisterMatrix(), mitkTransform3D.GetPointer());
	pelvisVerifyDirector->SetConvertMatrix(mitkTransform3D);

	pelvisVerifyDirector->Builder();
	this->SetPelvisVerifyPipeline(pelvisVerifyDirector->GetBuilder()->GetOutput());
}
void PelvisRegisterModel::SetRegisterPointSet(mitk::PointSet::Pointer pt)
{
	this->imp->registerPointSet = pt;
}

mitk::PointSet::Pointer PelvisRegisterModel::GetRegisterPointSet() const
{
	return this->imp->registerPointSet;
}

void PelvisRegisterModel::SetImageSurface(mitk::Surface::Pointer pt)
{
	this->imp->imageSurface = pt;
}

mitk::Surface::Pointer PelvisRegisterModel::GetImageSurface() const
{
	return this->imp->imageSurface;
}

vtkMatrix4x4* PelvisRegisterModel::GetRegisterMatrix()
{
	vtkNew<vtkMatrix4x4> registerMatrix;
	registerMatrix->DeepCopy(this->GetSurfaceRegistration().GetResult());
	return registerMatrix;
}	
END_SPATIAL_FITTING_NAMESPACE
