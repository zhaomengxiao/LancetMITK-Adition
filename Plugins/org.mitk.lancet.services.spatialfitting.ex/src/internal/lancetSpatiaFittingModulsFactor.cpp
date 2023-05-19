#include "lancetSpatiaFittingModulsFactor.h"
#include "lancetSpatialFittingAbstractModel.h"


// moduls 
#include <internal/moduls/lancetSpatialFittingProbeCheckPointModel.h>
#include <internal/moduls/lancetSpatialFittingPelvisCheckPointModel.h>


BEGIN_SPATIAL_FITTING_NAMESPACE

const char* ModulsFactor::Items::PROBE_REGISTER_MODEL = "PROBE_REGISTER_MODEL";
const char* ModulsFactor::Items::PELVIS_CHECKPOINT_MODEL = "PELVIS_CHECKPOINT_MODEL";

ModulsFactor::ModulsFactor()
{
}

berry::SmartPointer<AbstractModel> ModulsFactor::CreateModel(const QString& sn) const
{
	AbstractModel* ret_obj = nullptr;
	if (sn == Items::PROBE_REGISTER_MODEL)
	{
		ret_obj = new ProbeCheckPointModel(Items::PROBE_REGISTER_MODEL);
	}
	else if (sn == Items::PELVIS_CHECKPOINT_MODEL)
	{
		ret_obj = new PelvisCheckPointModel(Items::PELVIS_CHECKPOINT_MODEL);
	}
	return berry::SmartPointer<AbstractModel>(ret_obj);
}


END_SPATIAL_FITTING_NAMESPACE
