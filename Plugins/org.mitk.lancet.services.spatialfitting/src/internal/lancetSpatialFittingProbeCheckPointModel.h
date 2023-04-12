#ifndef LancetSpatialFittingProbeCheckPointModel_H
#define LancetSpatialFittingProbeCheckPointModel_H

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN ProbeCheckPointModel 
	: public itk::Object
{
public:
	mitkClassMacroItkParent(ProbeCheckPointModel, itk::Object)

	itkNewMacro(ProbeCheckPointModel)

	ProbeCheckPointModel();
public:
	itk::SmartPointer<PipelineManager> GetVerifyPipeline() const;
	void SetVerifyPipeline(const itk::SmartPointer<PipelineManager>&);

	mitk::NavigationDataSource::Pointer GetNdiNavigationDataSource() const;
	void SetNdiNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	mitk::NavigationDataSource::Pointer GetRoboticsNavigationDataSource() const;
	void SetRoboticsNavigationDataSource(mitk::NavigationDataSource::Pointer source);

	void ConfigureVerifyPipeline();

	mitk::NavigationData* GetVerifyEndMarkerData() const;

	void SetCheckPoint(mitk::NavigationData::Pointer);
	mitk::NavigationData::Pointer GetCheckPoint() const;
private:
	struct ProbeCheckPointModelPrivateImp;
	std::shared_ptr<ProbeCheckPointModelPrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingProbeCheckPointModel_H
