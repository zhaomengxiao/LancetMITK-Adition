#include "lancetSpatialFittingPelvicRoughRegistrationsModel.h"

#include "core/lancetSpatialFittingPipelineManager.h"
#include "lancetSpatialFittingAbstractPipelineBuilder.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsDirector.h"
#include "core/lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.h"

#include <QTimer>

BEGIN_SPATIAL_FITTING_NAMESPACE

struct PelvicRoughRegistrationsModel::PrivateImp
{
	QTimer tm;

	PipelineManager::Pointer registrationPipeline;
	PipelineManager::Pointer registrationVerifyPipeline;

	PelvicRoughRegistrationsPoints registrationsPoints;
};

PelvicRoughRegistrationsModel::PelvicRoughRegistrationsModel(const QString& serialNumber)
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
		if (this->imp->registrationsPoints.IsModify())
		{
			this->imp->registrationsPoints.SetModify(false);
			emit this->UpdatePointSetpArray();
		}
	});
}

bool PelvicRoughRegistrationsModel::Initialize()
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

bool PelvicRoughRegistrationsModel::Uninitialize()
{
	return false;
}

bool PelvicRoughRegistrationsModel::isInitialize() const
{
	return false;
}

bool PelvicRoughRegistrationsModel::StartWorking()
{
	this->imp->tm.start();
	return this->IsWorking();
}

bool PelvicRoughRegistrationsModel::StopWorking()
{
	this->imp->tm.stop();
	return !this->IsWorking();
}

bool PelvicRoughRegistrationsModel::IsWorking() const
{
	return this->imp->tm.isActive();
}

void PelvicRoughRegistrationsModel::SetWorkingStream(const WorkingStream& v)
{
	this->AbstractPelvicRegistrationsModel::SetWorkingStream(v);
	emit this->UpdateWorkingStream(v);
}

bool PelvicRoughRegistrationsModel::InitializeVerifyPipeline()
{
	if (this->GetNdiNavigationDataSource().IsNull() ||
		this->GetRoboticsNavigationDataSource().IsNull())
	{
		return false;
	}

	PelvicRoughRegistrationsVerifyDirector::Pointer pelvicRoughRegistrationsVerifyDirector =
		PelvicRoughRegistrationsVerifyDirector::New();

	pelvicRoughRegistrationsVerifyDirector->SetNdiNavigationDataSource(this->GetNdiNavigationDataSource());

	// Convert the transformation matrix obtained after image registration to the 
	// type of mitk::AffineTransform3D.
	{
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
	}

	if (false == pelvicRoughRegistrationsVerifyDirector->Builder())
	{
		return false;
	}
	auto pipeline = pelvicRoughRegistrationsVerifyDirector->GetBuilder()->GetOutput();
	this->SetRegistrationVerifyPipeline(pipeline);

	return true;
}

bool PelvicRoughRegistrationsModel::ComputeLandMarkResult(mitk::PointSet::Pointer,
	mitk::PointSet::Pointer, mitk::Surface::Pointer)
{
	return true;
}

itk::SmartPointer<PipelineManager> 
	PelvicRoughRegistrationsModel::GetRegistrationPipeline() const
{
	return this->imp->registrationPipeline;
}

void PelvicRoughRegistrationsModel::SetRegistrationPipeline(
	const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->registrationPipeline = pipeline;
}

itk::SmartPointer<PipelineManager> 
	PelvicRoughRegistrationsModel::GetRegistrationVerifyPipeline() const
{
	return this->imp->registrationVerifyPipeline;
}
void PelvicRoughRegistrationsModel::SetRegistrationVerifyPipeline(
	const itk::SmartPointer<PipelineManager>& pipeline)
{
	this->imp->registrationVerifyPipeline = pipeline;
}

PelvicRoughRegistrationsModel::PelvicRoughRegistrationsPoints& 
	PelvicRoughRegistrationsModel::GetRegistrationsPoints()
{
	return this->imp->registrationsPoints;
}

END_SPATIAL_FITTING_NAMESPACE