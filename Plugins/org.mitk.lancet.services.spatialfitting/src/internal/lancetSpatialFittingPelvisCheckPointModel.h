#ifndef LancetSpatialFittingPelvisCheckPointModel_H
#define LancetSpatialFittingPelvisCheckPointModel_H

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN PelvisCheckPointModel
	: public itk::Object
{
public:
	mitkClassMacroItkParent(PelvisCheckPointModel, itk::Object)

	itkNewMacro(PelvisCheckPointModel)

	PelvisCheckPointModel();
public:
	itk::SmartPointer<PipelineManager> GetCheckPointPipeline() const;
	void SetCheckPointPipeline(const itk::SmartPointer<PipelineManager>&);

	mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	mitk::NavigationDataSource::Pointer GetRoboticsNavigationDataSource() const;
	void SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	void ConfigureGetCheckPointPipeline();

	void SetCheckPoint(mitk::NavigationData::Pointer);
	mitk::NavigationData::Pointer GetCheckPoint() const;
private:
	struct PelvisCheckPointModelPrivateImp;
	std::shared_ptr<PelvisCheckPointModelPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisCheckPointModel_H
