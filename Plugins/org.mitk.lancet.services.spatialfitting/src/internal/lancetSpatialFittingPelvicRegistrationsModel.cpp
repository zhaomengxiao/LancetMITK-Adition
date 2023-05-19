#include "lancetSpatialFittingPelvicRegistrationsModel.h"
#include "lancetPluginActivator.h"

// Include header files for Qt.
#include <QTimer>

// Include header files for lancet device.
#include <lancetIDevicesAdministrationService.h>

// Include header file for lancet tracking data.
#include "core/lancetSpatialFittingPipelineManager.h"
#include <lancetNavigationObjectVisualizationFilter.h>
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include <lancetApplySurfaceRegistratioinStaticImageFilter.h>
#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRegistrationsModel::PrivateImp
{
  QTimer tm;
  WidgetWorkingModel roughWidgetWorkingModel;
  WidgetWorkingModel precisionWidgetWorkingModel;

  PelvicRegisterDataModel<RoughRegisterPointSet> roughRegisterPointSet;
  PelvicRegisterDataModel<PrecisionRegisterPointSet> precisionRegisterPointSet;

  PipelineManager::Pointer registerPipelineManager;
  PipelineManager::Pointer pelvisRF2ImagePipelineManager;

  mitk::NavigationDataSource::Pointer ndiNavigationDataSource;
  mitk::SurfaceRegistration::Pointer surfaceRegistration = mitk::SurfaceRegistration::New();

  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer surfaceRegistrationStaticImageFilter;
};

PelvicRegistrationsModel::PelvicRegistrationsModel()
	: imp(std::make_shared<PrivateImp>())
{
}

PelvicRegistrationsModel::~PelvicRegistrationsModel()
{
}

void PelvicRegistrationsModel::StopUpdatePipeline()
{
	this->imp->tm.stop();
}

void PelvicRegistrationsModel::StartUpdatePipeline()
{
	this->imp->tm.start();
}

bool PelvicRegistrationsModel::IsRunningUpdatePipeline() const
{
	return this->imp->tm.isActive();
}

PelvicRegistrationsModel::WidgetWorkingModel 
  PelvicRegistrationsModel::GetPelvicRoughRegisterWorkingModel() const
{
	return this->imp->roughWidgetWorkingModel;
}

void PelvicRegistrationsModel::SetPelvicRoughRegisterWorkingModel(const WidgetWorkingModel& model) const
{
	this->imp->roughWidgetWorkingModel = model;
}

PelvicRegistrationsModel::WidgetWorkingModel 
  PelvicRegistrationsModel::GetPelvicPrecisionRegisterWorkingModel() const
{
	return this->imp->precisionWidgetWorkingModel;
}

void PelvicRegistrationsModel::SetPelvicPrecisionRegisterWorkingModel(const WidgetWorkingModel& model) const
{
	this->imp->precisionWidgetWorkingModel = model;
}

itkPointer<mitk::NavigationDataSource> PelvicRegistrationsModel::GetNdiNavigationDataSource() const
{
	return this->imp->ndiNavigationDataSource;
}

void PelvicRegistrationsModel::SetNdiNavigationDataSource(const itkPointer<mitk::NavigationDataSource>& trackingDataSource)
{
	this->imp->ndiNavigationDataSource = trackingDataSource;
}

itkPointer<PipelineManager> PelvicRegistrationsModel::GetRegistrationPipeline() const
{
	return this->imp->registerPipelineManager;
}

void PelvicRegistrationsModel::SetRegistrationPipeline(const itkPointer<PipelineManager>& pipeline)
{
  this->imp->registerPipelineManager = pipeline;
}

itkPointer<PipelineManager> PelvicRegistrationsModel::GetPelvisRF2ImagePipeline() const
{
	return this->imp->pelvisRF2ImagePipelineManager;
}

void PelvicRegistrationsModel::SetPelvisRF2ImagePipeline(const itkPointer<PipelineManager>& pipeline)
{
  this->imp->registerPipelineManager = pipeline;
}

mitk::Surface::Pointer PelvicRegistrationsModel::GetSurfaceSrc() const
{
	return mitk::Surface::Pointer();
}

void PelvicRegistrationsModel::SetSurfaceSrc(const mitk::Surface::Pointer&)
{
}

PelvicRegistrationsModel::PelvicRegisterDataModel<PelvicRegistrationsModel::RoughRegisterPointSet>& 
  PelvicRegistrationsModel::GetRoughRegisterDataModel() const
{
	return this->imp->roughRegisterPointSet;
}

void PelvicRegistrationsModel::SetRoughRegisterDataModel(const PelvicRegisterDataModel<RoughRegisterPointSet>& pointset)
{
	this->imp->roughRegisterPointSet = pointset;
}

PelvicRegistrationsModel::PelvicRegisterDataModel<PelvicRegistrationsModel::PrecisionRegisterPointSet>& 
  PelvicRegistrationsModel::GetPrecisionRegisterDataModel() const
{
	return this->imp->precisionRegisterPointSet;
}

void PelvicRegistrationsModel::SetPrecisionRegisterDataModel(const PelvicRegisterDataModel<PrecisionRegisterPointSet>& pointset)
{
	this->imp->precisionRegisterPointSet = pointset;
}

mitk::SurfaceRegistration::Pointer PelvicRegistrationsModel::GetSurfaceRegistration() const
{
	return mitk::SurfaceRegistration::Pointer();
}

bool PelvicRegistrationsModel::ComputeRoughLandMarkResult(mitk::PointSet::Pointer, mitk::PointSet::Pointer, mitk::Surface::Pointer)
{
	return false;
}

bool PelvicRegistrationsModel::ComputePrecisionLandMarkResult(mitk::PointSet::Pointer, mitk::PointSet::Pointer, mitk::Surface::Pointer)
{
	return false;
}



END_SPATIAL_FITTING_NAMESPACE
