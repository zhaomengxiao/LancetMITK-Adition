#ifndef LancetSpatialFittingPelvisCheckPointModel_H
#define LancetSpatialFittingPelvisCheckPointModel_H


#include <lancetSpatialFittingGlobal.h>

#include <lancetSpatialFittingAbstractModel.h>
#include <internal/lancetSpatiaFittingModulsFactor.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  PelvisCheckPointModel : public QObject, public AbstractModel
{
  Q_OBJECT
public:
	berryObjectMacro(lancet::spatial_fitting::PelvisCheckPointModel)

  PelvisCheckPointModel(const QString& = ModulsFactor::Items::PELVIS_CHECKPOINT_MODEL);
public:
	virtual bool Initialize() override;

	virtual bool Uninitialize() override;

	virtual bool isInitialize() const override;

	virtual bool StartWorking() override;

	virtual bool StopWorking() override;
	virtual bool IsWorking() const override;
public:
	itk::SmartPointer<PipelineManager> GetCheckPointPipeline() const;
	void SetCheckPointPipelinePipeline(const itk::SmartPointer<PipelineManager>&);
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPelvisCheckPointModel_H