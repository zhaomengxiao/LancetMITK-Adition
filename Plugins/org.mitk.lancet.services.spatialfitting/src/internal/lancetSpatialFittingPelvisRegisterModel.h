#ifndef LancetSpatialFittingPelvisRegisterModel_H
#define LancetSpatialFittingPelvisRegisterModel_H

#include "lancetSpatialFittingGlobal.h"
#include <mitkNavigationDataSource.h>

namespace mitk
{
	class SurfaceRegistration;
}

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN PelvisRegisterModel
	: public itk::Object
{
public:
	mitkClassMacroItkParent(PelvisRegisterModel, itk::Object)

	itkNewMacro(PelvisRegisterModel)

	PelvisRegisterModel();
public:
	mitk::SurfaceRegistration& GetSurfaceRegistration();

	itk::SmartPointer<PipelineManager> GetPelvisRegisterPipeline() const;
	void SetPelvisRegisterPipeline(const itk::SmartPointer<PipelineManager>&);

	itk::SmartPointer<PipelineManager> GetPelvisVerifyPipeline() const;
	void SetPelvisVerifyPipeline(const itk::SmartPointer<PipelineManager>&);

	mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	mitk::NavigationDataSource::Pointer GetRoboticsNavigationDataSource() const;
	void SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	void ConfigurePelvisRegisterPipeline();
	void ConfigurePelvisVerifyPipeline();

	void SetRegisterPointSet(mitk::PointSet::Pointer);
	mitk::PointSet::Pointer GetRegisterPointSet() const;

	void SetImageSurface(mitk::Surface::Pointer);
	mitk::Surface::Pointer GetImageSurface() const;

	vtkMatrix4x4* GetRegisterMatrix();

private:
	struct PelvisRegisterModelPrivateImp;
	std::shared_ptr<PelvisRegisterModelPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisRegisterModel_H
