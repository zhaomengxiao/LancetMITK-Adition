#ifndef lancetSpatialFittingPelvicPrecisionRegistrationsModel_H
#define lancetSpatialFittingPelvicPrecisionRegistrationsModel_H

#include <lancetSpatialFittingGlobal.h>
#include <lancetSpatialFittingAbstractModel.h>
#include <internal/lancetSpatiaFittingModulsFactor.h>
#include <internal/moduls/lancetSpatialFittingAbstractPelvicRegistrationsModel.h>

#include "lancetSpatialFittingRegisterPointSet.inc"

BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;


class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PelvicPrecisionRegistrationsModel 
	: public QObject
	, public AbstractPelvicRegistrationsModel
{
  Q_OBJECT
Q_SIGNALS:
  void UpdatePointSetpArray();
  void UpdateWorkingStream(int);
public:
  typedef RegisterPointSet<mitk::Point3D, 40U> PelvicPrecisionRegistrationsPoints;
  berryObjectMacro(lancet::spatial_fitting::PelvicPrecisionRegistrationsModel)

  PelvicPrecisionRegistrationsModel(const QString & = ModulsFactor::Items::PELVIS_PELVIC_REGISTER_MODEL);

public:
  virtual bool Initialize() override;
  
  virtual bool Uninitialize() override;
  
  virtual bool isInitialize() const override;
  
  virtual bool StartWorking() override;
  
  virtual bool StopWorking() override;
  virtual bool IsWorking() const override;
  
  virtual void SetWorkingStream(const WorkingStream&) override;
  
  virtual bool InitializeVerifyPipeline();
  
  virtual bool ComputeLandMarkResult(mitk::PointSet::Pointer,
  	mitk::PointSet::Pointer, mitk::Surface::Pointer) override;
public:
  
  virtual PelvicPrecisionRegistrationsPoints& GetRegistrationsPoints();
private:
  struct PrivateImp;
  std::shared_ptr<PrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE
#endif // !lancetSpatialFittingPelvicPrecisionRegistrationsModel_H
