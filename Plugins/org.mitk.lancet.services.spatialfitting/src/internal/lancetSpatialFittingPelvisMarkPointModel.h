#ifndef LancetSpatialFittingPelvisMarkPointModel_H
#define LancetSpatialFittingPelvisMarkPointModel_H

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataSource.h>
namespace mitk
{
	class SurfaceRegistration;
}
BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN PelvisMarkPointModel
	: public itk::Object
{
public:
	mitkClassMacroItkParent(PelvisMarkPointModel, itk::Object)

	itkNewMacro(PelvisMarkPointModel)

	PelvisMarkPointModel();
public:
	mitk::SurfaceRegistration& GetSurfaceRegistration();

	itk::SmartPointer<PipelineManager> GetMarkPointPipeline() const;
	void SetMarkPointPipeline(const itk::SmartPointer<PipelineManager>&);

	mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	mitk::NavigationDataSource::Pointer GetRoboticsNavigationDataSource() const;
	void SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	void ConfigurePelvisMarkPointPipeline();

	void SetMarkPointSet(mitk::PointSet::Pointer);
	mitk::PointSet::Pointer GetMarkPointSet() const;

	void SetImageSurface(mitk::Surface::Pointer);
	mitk::Surface::Pointer GetImageSurface() const;

	vtkMatrix4x4* GetRegisterMatrix();
private:
	struct PelvisMarkPointModelPrivateImp;
	std::shared_ptr<PelvisMarkPointModelPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisMarkPointModel_H
