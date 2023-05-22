#ifndef lancetSpatialFittingRoboticsRegisterModel_H
#define lancetSpatialFittingRoboticsRegisterModel_H

#include <lancetSpatialFittingGlobal.h>
#include <lancetSpatialFittingAbstractModel.h>
#include <internal/lancetSpatiaFittingModulsFactor.h>

class RobotRegistration;
BEGIN_SPATIAL_FITTING_NAMESPACE
class PipelineManager;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	RoboticsRegisterModel : public QObject, public AbstractModel
{
	Q_OBJECT
public:
	//using RoboticsRegisterPointSet = RegisterPointSet<mitk::Point3D, 20>;
	berryObjectMacro(lancet::spatial_fitting::RoboticsRegisterModel)

	RoboticsRegisterModel(const QString & = ModulsFactor::Items::ROBOTICS_REGISTER_MODEL);
public:
	virtual bool Initialize() override;

	virtual bool Uninitialize() override;

	virtual bool isInitialize() const override;

	virtual bool StartWorking() override;

	virtual bool StopWorking() override;
	virtual bool IsWorking() const override;

public:
	RobotRegistration& GetRegisterModel();
	//RoboticsRegisterPointSet& GetRegisterPointSet();

	itk::SmartPointer<PipelineManager> GetRegisterNavigationPipeline() const;
	void SetRegisterNavigationPipeline(itk::SmartPointer<PipelineManager> manager);

	void SetAccutacyVerifyPipeline(itk::SmartPointer<PipelineManager> manager);
	itk::SmartPointer<PipelineManager> GetAccutacyVerifyPipeline() const;
private:
	struct PrivateImp;
	std::shared_ptr<PrivateImp> imp;
};


END_SPATIAL_FITTING_NAMESPACE

#endif // !lancetSpatialFittingRoboticsRegisterModel_H