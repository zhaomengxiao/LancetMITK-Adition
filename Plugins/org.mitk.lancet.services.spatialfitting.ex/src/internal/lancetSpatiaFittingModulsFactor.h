#ifndef lancetSpatiaFittingModulsFactor_H
#define lancetSpatiaFittingModulsFactor_H

#include <lancetSpatialFittingGlobal.h>

BEGIN_SPATIAL_FITTING_NAMESPACE
class AbstractModel;

class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
  ModulsFactor : public berry::Object
{
public:
  berryObjectMacro(lancet::spatial_fitting::ModulsFactor);

  ModulsFactor();

	struct ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN Items
	{
		static const char* ROBOTICS_REGISTER_MODEL;
		static const char* PROBE_REGISTER_MODEL;
		static const char* PELVIS_CHECKPOINT_MODEL;
		static const char* PELVIS_ROUGH_REGISTER_MODEL;
		static const char* PELVIS_PELVIC_REGISTER_MODEL;
	};

  virtual berry::SmartPointer<AbstractModel> CreateModel(const QString&) const;
};

END_SPATIAL_FITTING_NAMESPACE
#endif // !lancetSpatiaFittingModulsFactor_H