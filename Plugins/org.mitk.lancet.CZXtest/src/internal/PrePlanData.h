#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <vtkMatrix4x4.h>
class PrePlanData
{
public :
	std::string FemurImplantPath;
	std::string TibiaTrayPath;
	std::string FemurModelPath;
	std::string TibiaModelPath;
	std::vector<double> TTibiaImage2TibiaTrayMatrix;
	std::vector<double> TFemurImage2FemurImplantMatrix;

	PrePlanData(std::string femurImplantPath, std::string tibiaTrayPath, std::string femurModelPath, std::string tibiaModelPath,
		vtkMatrix4x4* tFemurImage2FemurImplantMatrix, vtkMatrix4x4* tTibiaImage2TibiaImplantMatrix) 
	{
		FemurImplantPath = femurImplantPath;
		TibiaTrayPath = tibiaTrayPath;
		FemurModelPath = femurModelPath;
		TibiaModelPath = tibiaModelPath;
		TTibiaImage2TibiaTrayMatrix.resize(16);
		TFemurImage2FemurImplantMatrix.resize(16);

		for (int i = 0; i < 16; ++i) {
			TFemurImage2FemurImplantMatrix[i] = tFemurImage2FemurImplantMatrix->GetData()[i];
			TTibiaImage2TibiaTrayMatrix[i] = tTibiaImage2TibiaImplantMatrix->GetData()[i];
		}
	}
};

inline void to_json(nlohmann::json& json, const PrePlanData& prePlanData)
{
	json = nlohmann::json{ {"FemurImplantPath", prePlanData.FemurImplantPath} ,
		{"TibiaTrayPath", prePlanData.TibiaTrayPath},
		{"FemurModelPath", prePlanData.FemurModelPath},
		{"TibiaModelPath", prePlanData.TibiaModelPath},
		{"TFemurImage2FemurImplantMatrix",prePlanData.TFemurImage2FemurImplantMatrix},
		{"TTibiaImage2TibiaTrayMatrix", prePlanData.TTibiaImage2TibiaTrayMatrix}};
}
