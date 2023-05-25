#include "lancetSpatialFittingPelvicPrecisionRegistrationsModel.h"

#include <QTimer>

#include <internal/lancetTrackingDeviceManage.h>
#include <lancetSpatialFittingAbstractPipelineBuilder.h>
#include <core/lancetSpatialFittingPipelineManager.h>
#include <lancetApplySurfaceRegistratioinStaticImageFilter.h>

#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.h"

#include <lancetIDevicesAdministrationService.h>
#include <lancetNavigationObjectVisualizationFilter.h>
#include <lancetApplySurfaceRegistratioinStaticImageFilter.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicPrecisionRegistrationsModel::PrivateImp
{
  QTimer tm;

  PipelineManager::Pointer registrationPipeline;
  PipelineManager::Pointer registrationVerifyPipeline;

  PelvicPrecisionRegistrationsPoints pelvicPrecisionRegistrationsPoints;
  lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer surfaceRegistrationStaticImageFilter;
};

PelvicPrecisionRegistrationsModel::PelvicPrecisionRegistrationsModel(const QString& serialNumber)
  : AbstractPelvicRegistrationsModel(serialNumber)
  , imp(std::make_shared<PrivateImp>())
{
	connect(&this->imp->tm, &QTimer::timeout, this, [=]()
		{
			if (this->GetRegistrationPipeline().IsNotNull())
			{
				this->GetRegistrationPipeline()->UpdateFilter();
			}
			if (this->GetRegistrationVerifyPipeline().IsNotNull())
			{
				this->GetRegistrationVerifyPipeline()->UpdateFilter();
			}
			if (this->GetRegistrationsPoints().IsModify())
			{
				this->GetRegistrationsPoints().SetModify(false);
				emit this->UpdatePointSetpArray();
			}
		});
}

bool PelvicPrecisionRegistrationsModel::Initialize()
{
	this->imp->tm.setInterval(20);

	if (this->GetNdiNavigationDataSource().IsNull() ||
		this->GetRoboticsNavigationDataSource().IsNull())
	{
		return false;
	}

	// Initialize registration pipeline.
	PelvicRoughRegistrationsDirector::Pointer pelvicRoughRegistrationsDirector =
		PelvicRoughRegistrationsDirector::New();

	pelvicRoughRegistrationsDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());
	if (pelvicRoughRegistrationsDirector->Builder())
	{
		this->SetRegistrationPipeline(pelvicRoughRegistrationsDirector->GetBuilder()->GetOutput());
	}

	// Initialize registration verification pipeline.
	// The validation pipeline can only be initialized after registration, 
	// integrating the initialization function code of this part into the 
	// PelvicRoughRegistrationsModel#InitializeVerifyPipeline() interface.

	return true;
}

bool PelvicPrecisionRegistrationsModel::Uninitialize()
{
	return false;
}

bool PelvicPrecisionRegistrationsModel::isInitialize() const
{
	return false;
}

bool PelvicPrecisionRegistrationsModel::StartWorking()
{
	this->imp->tm.start();
	return this->IsWorking();
}

bool PelvicPrecisionRegistrationsModel::StopWorking()
{
	this->imp->tm.stop();
	return false == this->IsWorking();
}

bool PelvicPrecisionRegistrationsModel::IsWorking() const
{
	return this->imp->tm.isActive();
}

void PelvicPrecisionRegistrationsModel::SetWorkingStream(const WorkingStream& workingStream)
{
	this->AbstractPelvicRegistrationsModel::SetWorkingStream(workingStream);
	emit this->UpdateWorkingStream(workingStream);
}

bool PelvicPrecisionRegistrationsModel::InitializeVerifyPipeline()
{
	return false;
}

bool PelvicPrecisionRegistrationsModel::ComputeLandMarkResult(
	mitk::PointSet::Pointer,
	mitk::PointSet::Pointer, 
	mitk::Surface::Pointer)
{
	return false;
}

PelvicPrecisionRegistrationsModel::PelvicPrecisionRegistrationsPoints& 
  PelvicPrecisionRegistrationsModel::GetRegistrationsPoints()
{
  return this->imp->pelvicPrecisionRegistrationsPoints;
}



END_SPATIAL_FITTING_NAMESPACE