#ifndef lancetSpatialFittingProbeCheckPointModel_H
#define lancetSpatialFittingProbeCheckPointModel_H

#include <lancetSpatialFittingGlobal.h>
#include <lancetSpatialFittingAbstractModel.h>
#include <internal/lancetSpatiaFittingModulsFactor.h>

namespace mitk { class NavigationData; }

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  ProbeCheckPointModel : public QObject, public AbstractModel
{
  Q_OBJECT
public:
	berryObjectMacro(lancet::spatial_fitting::ProbeCheckPointModel)

	ProbeCheckPointModel(const QString & = ModulsFactor::Items::PROBE_REGISTER_MODEL);
public:
	virtual bool Initialize() override;

	virtual bool Uninitialize() override;

	virtual bool isInitialize() const override;

	virtual bool StartWorking() override;

	virtual bool StopWorking() override;
	virtual bool IsWorking() const override;

public:
	itk::SmartPointer<PipelineManager> GetVerifyPipeline() const;
	void SetVerifyPipeline(const itk::SmartPointer<PipelineManager>&);

	mitk::NavigationData* GetVerifyEndMarkerData() const;
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};


END_SPATIAL_FITTING_NAMESPACE
#endif // !lancetSpatialFittingProbeCheckPointModel_H