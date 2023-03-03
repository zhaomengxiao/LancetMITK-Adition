#include "lancetSpatialFittingPelvicRoughRegistrationsModel.h"
#include "lancetPluginActivator.h"

#include <QTimer>

#include <lancetIDevicesAdministrationService.h>
#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.h"

#include <lancetNavigationObjectVisualizationFilter.h>
#include <lancetApplySurfaceRegistratioinStaticImageFilter.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModelPrivateImp
{
	QTimer tm;
	Model model = Model::Collect;

	PipelineManager::Pointer pipelineManager;
	PipelineManager::Pointer pipelineManagerOnVerify;
	mitk::NavigationDataSource::Pointer ndiNavigationDataSource;

	std::array<mitk::Point3D, 3> imagePointArray;
	std::array<mitk::Point3D, 3> vegaPointArray;

	std::array<mitk::Point3D, 40> imageICPPointArray;
	std::array<mitk::Point3D, 40> vegaICPPointArray;

	mitk::SurfaceRegistration::Pointer surfaceRegistration = mitk::SurfaceRegistration::New();

	lancet::ApplySurfaceRegistratioinStaticImageFilter::Pointer surfaceRegistrationStaticImageFilter;
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

void PelvicRoughRegistrationsModel::SetModel(const Model& model)
{
	this->imp->model = model;
	emit this->RenderModelChange();
}

PelvicRoughRegistrationsModel::Model PelvicRoughRegistrationsModel::GetModel() const
{
	return this->imp->model;
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

itk::SmartPointer<PipelineManager> 
  PelvicRoughRegistrationsModel::GetRegistrationVerifyPipeline() const
{
	return this->imp->pipelineManagerOnVerify;
}

void PelvicRoughRegistrationsModel::SetRegistrationVerifyPipeline(
	const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->pipelineManagerOnVerify = pipeline;
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

lancet::IDevicesAdministrationService* GetDevicesService()
{
	auto context = PluginActivator::GetPluginContext();
	auto serviceRef = context->getServiceReference<lancet::IDevicesAdministrationService>();
	return context->getService<lancet::IDevicesAdministrationService>(serviceRef);
}

void PelvicRoughRegistrationsModel::ConfigureRegistrationsVerifyPipeline()
{
	using namespace lancet::spatial_fitting;

	PelvicRoughRegistrationsVerifyDirector::Pointer pelvicRoughRegistrationsVerifyDirector =
		PelvicRoughRegistrationsVerifyDirector::New();

	pelvicRoughRegistrationsVerifyDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());

	mitk::AffineTransform3D::Pointer convertMatrix = mitk::AffineTransform3D::New();
	vtkNew<vtkMatrix4x4>(tmpMatrix);
	tmpMatrix->DeepCopy(this->GetSurfaceRegistration()->GetMatrixLandMark());
	tmpMatrix->Invert();

	auto tmpPset = mitk::PointSet::New();
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);
	auto affineTrans3D = tmpPset->GetGeometry()->GetIndexToWorldTransform();
	convertMatrix->SetMatrix(affineTrans3D->GetMatrix());
	convertMatrix->SetOffset(affineTrans3D->GetOffset());

	pelvicRoughRegistrationsVerifyDirector->SetImageConvertMatrix(convertMatrix);
	if (pelvicRoughRegistrationsVerifyDirector->Builder())
	{
		auto pipeline = pelvicRoughRegistrationsVerifyDirector->GetBuilder()->GetOutput();
		this->SetRegistrationVerifyPipeline(pipeline);
	}

	{
		// TODO: Debug
		if (nullptr == GetDevicesService() || GetDevicesService()->GetConnector().IsNull()) { return; }
		auto trackingManager = GetDevicesService()->GetConnector();
		auto vegaToolDataStorage = trackingManager->GetNavigationToolStorage("Vega");
		auto pelvisRF = this->GetNdiNavigationDataSource()->GetOutput("PelvisRF");

		this->imp->surfaceRegistrationStaticImageFilter =
			lancet::ApplySurfaceRegistratioinStaticImageFilter::New();

		this->imp->surfaceRegistrationStaticImageFilter->ConnectTo(this->GetNdiNavigationDataSource());
		vegaToolDataStorage->GetToolByName("PelvisRF")->SetToolRegistrationMatrix(convertMatrix);
		this->imp->surfaceRegistrationStaticImageFilter->SetRegistrationMatrix(convertMatrix.GetPointer());
		this->imp->surfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(pelvisRF);

		trackingManager->GetNavigationDataToNavigationDataFilter("Vega")
			->ConnectTo(this->imp->surfaceRegistrationStaticImageFilter);
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
			index = index_t;
			break;
		}
	}
	return index;
}

void PelvicRoughRegistrationsModel::SetImageICPPointArray(int index, const mitk::Point3D& pt)
{
	if (this->imp->imageICPPointArray.size() > index)
	{
		this->imp->imageICPPointArray[index] = pt;
	}
}

void PelvicRoughRegistrationsModel::SetImageICPPointArray(const std::array<mitk::Point3D, 40>& icpArray)
{
	this->imp->imageICPPointArray = icpArray;
}

mitk::Point3D PelvicRoughRegistrationsModel::GetImageICPPoint(int index) const
{
	if (this->imp->imageICPPointArray.size() > index)
	{
		return this->imp->imageICPPointArray[index];
	}
	return mitk::Point3D();
}

void PelvicRoughRegistrationsModel::SetVegaICPPointArray(int index, const mitk::Point3D& pt)
{
	if (this->imp->vegaICPPointArray.size() > index)
	{
		this->imp->vegaICPPointArray[index] = pt;
		emit this->VegaPointChange();
	}
}

void PelvicRoughRegistrationsModel::SetVegaICPPointArray(const std::array<mitk::Point3D, 40>& icpArray)
{
	this->imp->vegaICPPointArray = icpArray;
}

mitk::Point3D PelvicRoughRegistrationsModel::GetVegaICPPoint(int index) const
{
	if (this->imp->vegaICPPointArray.size() > index)
	{
		return this->imp->vegaICPPointArray[index];
	}
	return mitk::Point3D();
}

int PelvicRoughRegistrationsModel::GetVegaICPPointVaildIndex() const
{
	int index = -1;

	for (int index_t = 0; index_t < this->imp->vegaICPPointArray.size(); ++index_t)
	{
		if (this->GetVegaICPPoint(index_t) != mitk::Point3D())
		{
			index = index_t;
			break;
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
