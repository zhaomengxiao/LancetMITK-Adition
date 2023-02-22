#include "lancetSpatialFittingPelvicRoughRegistrationsModel.h"

#include <QTimer>

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"


BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModelPrivateImp
{
	QTimer tm;

	PipelineManager::Pointer pipelineManager;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;

	std::array<mitk::Point3D, 3> imagePointArray;
	std::array<mitk::Point3D, 3> vegaPointArray;

	mitk::SurfaceRegistration::Pointer surfaceRegistration = mitk::SurfaceRegistration::New();
};

PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModel()
	: imp(std::make_shared<PelvicRoughRegistrationsModelPrivateImp>())
{
	this->imp->tm.setInterval(20);
	connect(&this->imp->tm, &QTimer::timeout, this, [=]() {
		if (this->GetRegistrationPipeline().IsNotNull())
		{
			this->GetRegistrationPipeline()->UpdateFilter();
		}
		});
}

void PelvicRoughRegistrationsModel::Start()
{
	this->imp->tm.start();
}

void PelvicRoughRegistrationsModel::Stop()
{
	this->imp->tm.stop();
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
	if (pelvicRoughRegistrationsDirector->Builder())
	{
		this->SetRegistrationPipeline(pelvicRoughRegistrationsDirector->GetBuilder()->GetOutput());
	}	
}

void PelvicRoughRegistrationsModel::SetSurfaceSrc(const mitk::Surface::Pointer& src)
{
	this->imp->surfaceRegistration->SetSurfaceSrc(src);
}

mitk::Surface::Pointer PelvicRoughRegistrationsModel::GetSurfaceSrc() const
{
	return this->imp->surfaceRegistration->GetSurfaceSrc();
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
		emit this->VegaPointChange();
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

int PelvicRoughRegistrationsModel::GetVegaPointVaildIndex() const
{
	int index = 0;

	for (int index_t = 0; index_t < this->imp->vegaPointArray.size(); ++index_t)
	{
		if (this->GetVegaPoint(index_t) != mitk::Point3D())
		{
			++index;
		}
	}
	return index;
}

mitk::SurfaceRegistration::Pointer PelvicRoughRegistrationsModel::GetSurfaceRegistration() const
{
	return this->imp->surfaceRegistration;
}

bool PelvicRoughRegistrationsModel::ComputeLandMarkResult(mitk::PointSet::Pointer src,
	mitk::PointSet::Pointer target, mitk::Surface::Pointer surface)
{
	if (src.IsNull() || target.IsNull() || surface.IsNull()) { return false; }

	this->GetSurfaceRegistration()->ClearLandMarks();
	this->GetSurfaceRegistration()->ClearIcpPoints();
	this->GetSurfaceRegistration()->Clear();

	this->GetSurfaceRegistration()->SetLandmarksSrc(src);
	this->GetSurfaceRegistration()->SetLandmarksTarget(target);

	this->GetSurfaceRegistration()->SetSurfaceSrc(surface);
	return this->GetSurfaceRegistration()->ComputeLandMarkResult();
}

END_SPATIAL_FITTING_NAMESPACE
