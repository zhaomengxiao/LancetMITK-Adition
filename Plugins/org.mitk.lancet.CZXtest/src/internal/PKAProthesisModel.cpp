#include "PKAProthesisModel.h"

void lancetAlgorithm::PKAProthesisModel::UpdateFemur()
{
	UpdateFemurResectionLine();
	UpdateFemurAnteriorChamCut();
	UpdateFemurDistalCut();
	UpdateFemurPosteriorCut();
}

void lancetAlgorithm::PKAProthesisModel::UpdateTibia()
{
	UpdateTibiaAxis();
	UpdateTibiaPlaneSymmetryAxis();
}

void lancetAlgorithm::PKAProthesisModel::UpdateFemurResectionLine()
{
	Eigen::Vector3d femurImplantResectionMedial;
	Eigen::Vector3d femurImplantResectionLateral;

	if (!GetLandMark(PKALandmarks::fi_ResectionMedial, femurImplantResectionMedial) || !GetLandMark(PKALandmarks::fi_ResectionLateral, femurImplantResectionLateral))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(femurImplantResectionMedial, femurImplantResectionLateral);
	SetAxis(PKAAxes::fi_BRL, femurImplantResectionMedial.data(), direction.data());
}

void lancetAlgorithm::PKAProthesisModel::UpdateFemurAnteriorChamCut()
{
	Eigen::Vector3d Start;
	Eigen::Vector3d End;

	if (!(GetLandMark(PKALandmarks::fi_ANTERIORCHAMSTART, Start)) || !(GetLandMark(PKALandmarks::fi_ANTERIORCHAMEND, End)))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(Start, End);
	SetPlane(PKAPlanes::FEMURANTERIORCHAM, Start.data(), direction.data());
}

void lancetAlgorithm::PKAProthesisModel::UpdateFemurDistalCut()
{
	Eigen::Vector3d Start;
	Eigen::Vector3d End;

	if (!(GetLandMark(PKALandmarks::fi_DISTALSTART, Start)) || !(GetLandMark(PKALandmarks::fi_DISTALEND, End)))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(Start, End);
	SetAxis(PKAAxes::fi_A, Start.data(), direction.data());
	SetPlane(PKAPlanes::FEMURDISTAL, Start.data(), direction.data());
}

void lancetAlgorithm::PKAProthesisModel::UpdateFemurPosteriorCut()
{
	Eigen::Vector3d Start;
	Eigen::Vector3d End;

	if (!(GetLandMark(PKALandmarks::fi_POSTERIORSTART, Start)) || !(GetLandMark(PKALandmarks::fi_POSTERIOREND, End)))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(Start, End);
	SetPlane(PKAPlanes::FEMURPOSTERIOR, Start.data(), direction.data());
}

void lancetAlgorithm::PKAProthesisModel::UpdateTibiaAxis()
{
	Eigen::Vector3d Start;
	Eigen::Vector3d End;

	if (!(GetLandMark(PKALandmarks::ti_PROXIMALSTART, Start)) || !(GetLandMark(PKALandmarks::ti_PROXIMALEND, End)))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(Start, End);
	SetAxis(PKAAxes::ti_A, Start.data(), direction.data());
	SetPlane(PKAPlanes::TIBIAPROXIMAL, Start.data(), direction.data());
}

void lancetAlgorithm::PKAProthesisModel::UpdateTibiaPlaneSymmetryAxis()
{
	Eigen::Vector3d Start;
	Eigen::Vector3d End;

	if (!(GetLandMark(PKALandmarks::ti_SYMMETRYSTART, Start)) || !(GetLandMark(PKALandmarks::ti_SYMMETRYEND, End)))
	{
		return;
	}
	Eigen::Vector3d direction = CalculationHelper::CalculateDirection(Start, End);
	SetAxis(PKAAxes::ti_SA, Start.data(), direction.data());
}


