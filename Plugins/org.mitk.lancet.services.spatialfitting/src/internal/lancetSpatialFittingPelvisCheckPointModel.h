#ifndef LancetSpatialFittingPelvisCheckPointModel_H
#define LancetSpatialFittingPelvisCheckPointModel_H

#include <QObject>

#include "lancetSpatialFittingGlobal.h"

#include <mitkNavigationDataSource.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN PelvisCheckPointModel
	: public QObject, public itk::Object
{
	Q_OBJECT
public:
	mitkClassMacroItkParent(PelvisCheckPointModel, itk::Object)

	itkNewMacro(PelvisCheckPointModel)

	PelvisCheckPointModel();

	void Start();
	void Stop();
public:
	itk::SmartPointer<PipelineManager> GetCheckPointPipeline() const;
	void SetCheckPointPipelinePipeline(const itk::SmartPointer<PipelineManager>&);

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
