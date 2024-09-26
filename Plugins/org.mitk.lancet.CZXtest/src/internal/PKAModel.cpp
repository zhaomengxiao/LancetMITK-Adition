//#include "PKAModel.h"
//#include "physioConst.h"
//#include <cstdarg>
//
//PKAModel::~PKAModel()
//{
//	std::cout << "PKAModel deconstructed" << std::endl;
//}
//
//void PKAModel::Delete()
//{
//	std::cout << "PKAModel deconstructed" << std::endl;
//	if (m_Femur != nullptr)
//	{
//		delete m_Femur;
//		m_Femur = nullptr;
//	}
//	if (m_Tibia != nullptr)
//	{
//		delete m_Tibia;
//		m_Tibia = nullptr;
//	}
//	if (m_FemurProsthesis != nullptr)
//	{
//		delete m_FemurProsthesis;
//		m_FemurProsthesis = nullptr;
//	}
//	if (m_TibiaProsthesis != nullptr)
//	{
//		delete m_TibiaProsthesis;
//		m_TibiaProsthesis = nullptr;
//	}
//	std::cout << "deconstruct finished" << std::endl;
//}
//
//void PKAModel::BuildFemur(unsigned int n, ...)
//{
//	if (nullptr == m_Femur)
//	{
//		m_Femur = new  PKAPrePlanBoneModel();
//	}
//	try
//	{
//		std::va_list args;
//		va_start(args, n);
//		m_Femur->SetLandMark(PKALandmarks::FemurMedialEpicondyle, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::FemurLateralEpicondyle, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::FemurMedialPosterior, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::FemurLateralPosterior, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::f_MDP, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::f_LDP, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::f_MPP, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::f_LPP, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::HipCenter, va_arg(args, double*));
//		m_Femur->SetLandMark(PKALandmarks::FemurKneeCenter, va_arg(args, double*));
//		va_end(args);
//	}
//	catch (std::exception& e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	m_Femur->UpdateFemurModel();
//	std::cout << "end build femur" << std::endl;
//}
//
//void PKAModel::BuildTibia(unsigned int n, ...)
//{
//	if (nullptr == m_Tibia)
//	{
//		//mitk::DataStorage* dataStorage = mitk::RenderingManager::GetDataStorage();
//		m_Tibia = new PKAPrePlanBoneModel();
//	}
//	try
//	{
//		std::va_list args;
//		va_start(args, n);
//		m_Tibia->SetLandMark(PKALandmarks::PCL_CENTER, va_arg(args, double*));
//		m_Tibia->SetLandMark(PKALandmarks::TibiaKneeCenter, va_arg(args, double*));
//		m_Tibia->SetLandMark(PKALandmarks::TibiaAngleJointCenter, va_arg(args, double*));
//		m_Tibia->SetLandMark(PKALandmarks::TUBERCLE, va_arg(args, double*));
//		m_Tibia->SetLandMark(PKALandmarks::t_PM, va_arg(args, double*));
//		m_Tibia->SetLandMark(PKALandmarks::t_PL, va_arg(args, double*));
//		va_end(args);
//	}
//	catch (std::exception& e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	m_Tibia->UpdateTibiaModel();
//	std::cout << "end build tibia" << std::endl;
//}
//
//void PKAModel::BuildFemurImplant(unsigned int n, ...)
//{
//	if (nullptr == m_FemurProsthesis)
//	{
//		m_FemurProsthesis = new PKAProthesisModel;
//	}
//	try
//	{
//		std::va_list args;
//		va_start(args, n);
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_ANTERIORCHAMSTART, va_arg(args, double*));
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_ANTERIORCHAMEND, va_arg(args, double*));
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_DISTALSTART, va_arg(args, double*));
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_DISTALEND, va_arg(args, double*));
//
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_POSTERIORSTART, va_arg(args, double*));
//		m_FemurProsthesis->SetLandMark(PKALandmarks::fi_POSTERIOREND, va_arg(args, double*));
//		//m_FemurProsthesis->SetLandMark(PKALandmarks::fi_ResectionMedial, va_arg(args, double*));
//		//m_FemurProsthesis->SetLandMark(PKALandmarks::fi_ResectionLateral, va_arg(args, double*));
//		va_end(args);
//	}
//	catch (std::exception& e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	m_FemurProsthesis->UpdateFemur();
//	std::cout << "end build femur implant";
//}
//
//void PKAModel::BuildTibiaImplant(unsigned int n, ...)
//{
//	if (nullptr == m_TibiaProsthesis)
//	{
//		m_TibiaProsthesis = new PKAProthesisModel;
//	}
//	try
//	{
//		std::va_list args;
//		va_start(args, n);
//		m_TibiaProsthesis->SetLandMark(PKALandmarks::ti_PROXIMALSTART, va_arg(args, double*));
//		m_TibiaProsthesis->SetLandMark(PKALandmarks::ti_PROXIMALEND, va_arg(args, double*));
//		m_TibiaProsthesis->SetLandMark(PKALandmarks::ti_SYMMETRYSTART, va_arg(args, double*));
//		m_TibiaProsthesis->SetLandMark(PKALandmarks::ti_SYMMETRYEND, va_arg(args, double*));
//		va_end(args);
//	}
//	catch (std::exception& e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	m_TibiaProsthesis->UpdateTibia();
//	std::cout << "end build tibia implant";
//}
//
//void PKAModel::SetOperationSide(PKASurgicalSide side)
//{
//	m_opSide = side;
//	UpdateResultSymbol();
//}
//
//void PKAModel::UpdateResultSymbol()
//{
//	if (m_opSide == PKASurgicalSide::Left)
//		m_resultSymbol = 1.0;
//	else 
//		m_resultSymbol = -1.0;
//}
//
//void PKAModel::CalFemurVarus()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	AxisType SAG{};
//	AxisType FIA{};
//
//	if (!(m_Femur->GetAxis(PKAAxes::femurSagittalNormal, SAG)) || !(m_FemurProsthesis->GetAxis(PKAAxes::fi_A, FIA)))
//	{
//		return;
//	}
//
//	Eigen::Vector3d SAG_dir = Eigen::Map<Eigen::Vector3d>(SAG.direction.data());
//	Eigen::Vector3d FIA_dir = Eigen::Map<Eigen::Vector3d>(FIA.direction.data());
//
//	double result = -CalculationHelper::CalculateDirectedAngleWithPlaneNormal(FIA_dir.data(), SAG_dir.data());
//	SetResult(PKAResult::f_Varus, result);
//}
//
//void PKAModel::CalFemurRotation()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	AxisType PCA;
//	AxisType TEA;
//	AxisType FI_BoneResectionLine;
//
//	if (!(m_Femur->GetAxis(PKAAxes::FemurPosteriorCondyleAxis, PCA)) || !(m_Femur->GetAxis(PKAAxes::f_TEA, TEA))
//		|| !(m_FemurProsthesis->GetAxis(PKAAxes::fi_BRL, FI_BoneResectionLine)))
//	{
//		return;
//	}
//
//	Eigen::Vector3d PCA_dir = Eigen::Map<Eigen::Vector3d>(PCA.direction.data());
//	Eigen::Vector3d TEA_dir = Eigen::Map<Eigen::Vector3d>(TEA.direction.data());
//	Eigen::Vector3d FI_BoneResectionLine_dir = Eigen::Map<Eigen::Vector3d>(FI_BoneResectionLine.direction.data());
//
//	Eigen::Vector3d origin;
//	Eigen::Vector3d PCA_Project;
//	Eigen::Vector3d TEA_Project;
//	Eigen::Vector3d FI_BoneResectionLine_Project;
//
//	PCA_Project = CalculationHelper::ProjectPointOntoPlane(PCA_dir, origin, TransverseNormal);
//	TEA_Project = CalculationHelper::ProjectPointOntoPlane(TEA_dir, origin, TransverseNormal);
//	FI_BoneResectionLine_Project = CalculationHelper::ProjectPointOntoPlane(FI_BoneResectionLine_dir, origin, TransverseNormal);
//
//	double result = CalculationHelper::ComputeSignedAngle(FI_BoneResectionLine_Project, PCA_Project, TransverseNormal) * (180.0 / EIGEN_PI) * m_resultSymbol;
//	SetResult(PKAResult::f_PCAExternal, result);
//	result = CalculationHelper::ComputeSignedAngle(FI_BoneResectionLine_Project, TEA_Project, TransverseNormal) * (180.0 / EIGEN_PI) * m_resultSymbol;
//	SetResult(PKAResult::f_TEAExternal, result);
//}
//
//void PKAModel::CalFemurFlexion()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	AxisType MA;
//	AxisType FIA;
//
//	if (!(m_Femur->GetAxis(PKAAxes::FemurMechanicalAxis, MA)) || !(m_FemurProsthesis->GetAxis(PKAAxes::fi_A, FIA)))
//	{
//		return;
//	}
//
//	Eigen::Vector3d MA_dir = Eigen::Map<Eigen::Vector3d>(MA.direction.data());
//	Eigen::Vector3d FIA_dir = Eigen::Map<Eigen::Vector3d>(FIA.direction.data());
//
//	Eigen::Vector3d origin(0,0,0);
//	Eigen::Vector3d MA_Project;
//	Eigen::Vector3d FIA_Project;
//
//	MA_Project = CalculationHelper::ProjectPointOntoPlane(MA_dir, origin, SagittalNormal);
//	FIA_Project = CalculationHelper::ProjectPointOntoPlane(FIA_dir, origin, SagittalNormal);
//
//	double result = CalculationHelper::ComputeSignedAngle(FIA_Project, MA_Project, SagittalNormal) * (180.0 / EIGEN_PI);
//	SetResult(PKAResult::f_flexion, result);
//}
//
//void PKAModel::CalTibiaVarus()
//{
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//	//AxisType MA;
//	AxisType TIA;
//	AxisType SAG;
//
//	if (!(m_Tibia->GetAxis(PKAAxes::TibiaSagittalNormal, SAG)) || !(m_TibiaProsthesis->GetAxis(PKAAxes::ti_A, TIA)))
//	{
//		return;
//	}
//
//	Eigen::Vector3d SAG_dir = Eigen::Map<Eigen::Vector3d>(SAG.direction.data());
//	Eigen::Vector3d TIA_dir = Eigen::Map<Eigen::Vector3d>(TIA.direction.data());
//
//	double result = -CalculationHelper::CalculateDirectedAngleWithPlaneNormal(TIA_dir.data(), SAG_dir.data()) * m_resultSymbol;
//
//	SetResult(PKAResult::t_Varus, result);
//}
//
//void PKAModel::CalTibiaExternal()
//{
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//	//AxisType APA;
//	AxisType SAG;
//	AxisType SA;
//
//	if (!(m_Tibia->GetAxis(PKAAxes::TibiaSagittalNormal, SAG)) || !(m_TibiaProsthesis->GetAxis(PKAAxes::ti_SA, SA)))
//	{
//		return;
//	}
//
//	double result =  CalculationHelper::CalculateDirectedAngleWithPlaneNormal(SA.direction.data(), SAG.direction.data()) * m_resultSymbol;
//
//	SetResult(PKAResult::t_External, result);
//}
//
//void PKAModel::CalTibiaPostSlope()
//{
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//
//	AxisType COR;
//	AxisType TIA;
//	if (!(m_Tibia->GetAxis(PKAAxes::TibiaCoronalNormal, COR)) || !(m_TibiaProsthesis->GetAxis(PKAAxes::ti_A, TIA)))
//	{
//		return;
//	}
//
//	double result = CalculationHelper::CalculateDirectedAngleWithPlaneNormal(TIA.direction.data(), COR.direction.data());
//
//	SetResult(PKAResult::t_PSlope, result);
//}
//
//void PKAModel::CalFemurDistalResectionDepth()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	Eigen::Vector3d f_MDP;
//	Eigen::Vector3d f_LDP;
//	PlaneType f_distal;
//
//	if (!(m_Femur->GetLandMark(PKALandmarks::f_MDP, f_MDP)) || !(m_Femur->GetLandMark(PKALandmarks::f_LDP, f_LDP))
//		|| !(m_FemurProsthesis->GetPlane(PKAPlanes::FEMURDISTAL, f_distal)))
//	{
//		return;
//	}
//
//	AxisType normal = f_distal.normal;
//	Eigen::Vector3d planPoint = Eigen::Map<Eigen::Vector3d>(normal.startPoint.data());
//	Eigen::Vector3d normalDirection = Eigen::Map<Eigen::Vector3d>(normal.direction.data());
//	double m_flag = f_MDP.data()[2] < normal.startPoint.data()[2] ? 1.0 : -1.0;
//	double l_flag = f_LDP.data()[2] < normal.startPoint.data()[2] ? 1.0 : -1.0;
//
//	double result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(f_MDP, normalDirection, planPoint) * m_flag;
//	SetResult(PKAResult::f_Distal_MRD, result);
//	result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(f_LDP, normalDirection, planPoint) * l_flag;
//	SetResult(PKAResult::f_Distal_LRD, result);
//}
//
//void PKAModel::CalFemurPosteriorResectionDepth()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	Eigen::Vector3d f_MPP;
//	Eigen::Vector3d f_LPP;
//	PlaneType f_posterior;
//
//	if (!(m_Femur->GetLandMark(PKALandmarks::f_MPP, f_MPP)) || !(m_Femur->GetLandMark(PKALandmarks::f_LPP, f_LPP))
//		|| !(m_FemurProsthesis->GetPlane(PKAPlanes::FEMURPOSTERIOR, f_posterior)))
//	{
//		return;
//	}
//
//	AxisType normal = f_posterior.normal;
//	Eigen::Vector3d normalStart = Eigen::Map<Eigen::Vector3d>(normal.startPoint.data());
//	Eigen::Vector3d normalDirection = Eigen::Map<Eigen::Vector3d>(normal.direction.data());
//	double m_flag = f_MPP.data()[1] >= normal.startPoint.data()[1] ? 1.0 : -1.0;
//	double l_flag = f_LPP.data()[1] >= normal.startPoint.data()[1] ? 1.0 : -1.0;
//
//	double result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(f_MPP, normalDirection, normalStart) * m_flag;
//	SetResult(PKAResult::f_Posterior_MRD, result);
//	result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(f_LPP, normalDirection, normalStart) * l_flag;
//	PKAModel::Instance().SetResult(PKAResult::f_Posterior_LRD, result);
//}
//
//void PKAModel::CalTibiaResectionDepth()
//{
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//	Eigen::Vector3d t_PM;
//	Eigen::Vector3d t_PL;
//	PlaneType t_proximal;
//
//	if (!(m_Tibia->GetLandMark(PKALandmarks::t_PM, t_PM)) || !(m_Tibia->GetLandMark(PKALandmarks::t_PL, t_PL))
//		|| !(m_TibiaProsthesis->GetPlane(PKAPlanes::TIBIAPROXIMAL, t_proximal)))
//	{
//		return;
//	}
//
//	AxisType normal = t_proximal.normal;
//
//	Eigen::Vector3d normalStart = Eigen::Map<Eigen::Vector3d>(normal.startPoint.data());
//	Eigen::Vector3d normalDirection = Eigen::Map<Eigen::Vector3d>(normal.direction.data());
//
//	auto pmDirection = CalculationHelper::CalculateDirection(normalStart, t_PM);
//	auto plDirection = CalculationHelper::CalculateDirection(normalStart, t_PL);
//
//	double m_flag = CalculationHelper::IsSameDirection(pmDirection, normalDirection) ? -1.0 : 1.0;
//	double l_flag = CalculationHelper::IsSameDirection(plDirection, normalDirection) ? -1.0 : 1.0;
//
//	double result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(t_PM, normalDirection,normalStart) * m_flag;
//	SetResult(PKAResult::t_Proximal_MRD, result);
//	result = CalculationHelper::CalculationHelper::DistanceFromPointToPlane(t_PL, normalDirection, normalStart) * l_flag;
//	SetResult(PKAResult::t_Proximal_LRD, result);
//}
//
//void PKAModel::CalExtensionGap()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//	Eigen::Vector3d f_MDP;
//	Eigen::Vector3d f_LDP;
//	PlaneType fi_DISTAL;
//	PlaneType ti_PROXIMAL;
//
//	if (!m_Femur->GetLandMark(PKALandmarks::f_MDP, f_MDP))
//	{
//		std::cout << "cant find femur medial distal point";
//		return;
//	}
//	if (!m_Femur->GetLandMark(PKALandmarks::f_LDP, f_LDP))
//	{
//		std::cout << "cant find femur lateral distal point";
//		return;
//	}
//	if (!m_FemurProsthesis->GetPlane(PKAPlanes::FEMURDISTAL, fi_DISTAL))
//	{
//		std::cout << "cant find femur implant distal plane";
//		return;
//	}
//	if (!m_TibiaProsthesis->GetPlane(PKAPlanes::TIBIAPROXIMAL, ti_PROXIMAL))
//	{
//		std::cout << "cant find tibia implant proximal plane";
//		return;
//	}
//	Eigen::Vector3d fiDistalNormalDirectionStart = Eigen::Map<Eigen::Vector3d>(fi_DISTAL.normal.startPoint.data());
//	Eigen::Vector3d fiDistalNormalDirection = Eigen::Map<Eigen::Vector3d>(fi_DISTAL.normal.direction.data());
//	Eigen::Vector3d tiProximalNormalStart = Eigen::Map<Eigen::Vector3d>(ti_PROXIMAL.normal.startPoint.data());
//	Eigen::Vector3d tiProximalNormalDirection = Eigen::Map<Eigen::Vector3d>(ti_PROXIMAL.normal.direction.data());
//	Eigen::Vector3d f_medial_project;
//	Eigen::Vector3d f_lateral_project;
//	Eigen::Vector3d t_medial_project;
//	Eigen::Vector3d t_lateral_project;
//
//	f_medial_project = CalculationHelper::ProjectPointOntoPlane(f_MDP, fiDistalNormalDirectionStart, fiDistalNormalDirection);
//	f_lateral_project = CalculationHelper::ProjectPointOntoPlane(f_LDP, fiDistalNormalDirectionStart, fiDistalNormalDirection);
//	SetLandMark(PKALandmarks::fi_EXTENSIONMEDIAL, f_medial_project.data());
//	SetLandMark(PKALandmarks::fi_EXTENSIONLATERAL, f_lateral_project.data());
//
//	t_medial_project = CalculationHelper::ProjectPointOntoPlane(f_medial_project, tiProximalNormalStart, tiProximalNormalDirection);
//	t_lateral_project = CalculationHelper::ProjectPointOntoPlane(f_lateral_project, tiProximalNormalStart, tiProximalNormalDirection);
//	SetLandMark(PKALandmarks::ti_EXTENSIONMEDIAL, t_medial_project.data());
//	SetLandMark(PKALandmarks::ti_EXTENSIONLATERAL, t_lateral_project.data());
//
//	double result =  CalculationHelper::DistanceFromPointToPlane(f_medial_project, tiProximalNormalDirection, tiProximalNormalStart);
//	SetResult(PKAResult::ExtensionMedialGap, result);
//	result = CalculationHelper::DistanceFromPointToPlane(f_lateral_project, tiProximalNormalDirection, tiProximalNormalStart);
//	SetResult(PKAResult::ExtensionLateralGap, result);
//}
//
//void PKAModel::CalFlexionGap()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_FemurProsthesis == nullptr)
//	{
//		std::cout << "femur implant null ptr";
//		return;
//	}
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	if (m_TibiaProsthesis == nullptr)
//	{
//		std::cout << "tibia implant null ptr";
//		return;
//	}
//	Eigen::Vector3d f_MPP;
//	Eigen::Vector3d f_LPP;
//	PlaneType fi_POSTERIOR;
//	PlaneType ti_PROXIMAL;
//
//	if (!(m_Femur->GetLandMark(PKALandmarks::f_MPP, f_MPP)) || !(m_Femur->GetLandMark(PKALandmarks::f_LPP, f_LPP))
//		|| !(m_FemurProsthesis->GetPlane(PKAPlanes::FEMURPOSTERIOR, fi_POSTERIOR)) || !(m_TibiaProsthesis->GetPlane(PKAPlanes::TIBIAPROXIMAL, ti_PROXIMAL)))
//	{
//		return;
//	}
//
//	Eigen::Vector3d fiPosteriorNormalStart = Eigen::Map<Eigen::Vector3d>(fi_POSTERIOR.normal.startPoint.data());
//	Eigen::Vector3d fiPosteriorNormalDirection = Eigen::Map<Eigen::Vector3d>(fi_POSTERIOR.normal.direction.data());
//	Eigen::Vector3d tiProximalNormalStart = Eigen::Map<Eigen::Vector3d>(ti_PROXIMAL.normal.startPoint.data());
//	Eigen::Vector3d tiProximalNormalDirection = Eigen::Map<Eigen::Vector3d>(ti_PROXIMAL.normal.direction.data());
//	Eigen::Vector3d f_medial_project;
//	Eigen::Vector3d f_lateral_project;
//	Eigen::Vector3d t_medial_project;
//	Eigen::Vector3d t_lateral_project;
//
//
//	f_medial_project = CalculationHelper::ProjectPointOntoPlane(f_MPP, fiPosteriorNormalStart, fiPosteriorNormalDirection);
//	f_lateral_project = CalculationHelper::ProjectPointOntoPlane(f_LPP, fiPosteriorNormalStart, fiPosteriorNormalDirection);
//	SetLandMark(PKALandmarks::fi_FLEXIONMEDIAL, f_medial_project.data());
//	SetLandMark(PKALandmarks::fi_FLEXIONLATERAL, f_lateral_project.data());
//
//	t_medial_project = CalculationHelper::ProjectPointOntoPlane(f_medial_project, tiProximalNormalStart, tiProximalNormalDirection);
//	t_lateral_project = CalculationHelper::ProjectPointOntoPlane(f_lateral_project, tiProximalNormalStart, tiProximalNormalDirection);
//	SetLandMark(PKALandmarks::ti_FLEXIONMEDIAL, t_medial_project.data());
//	SetLandMark(PKALandmarks::ti_FLEXIONLATERAL, t_lateral_project.data());
//
//	double result = CalculationHelper::DistanceFromPointToPlane(f_medial_project, tiProximalNormalDirection, tiProximalNormalStart);
//	SetResult(PKAResult::flexionMedialGap, result);
//	result = CalculationHelper::DistanceFromPointToPlane(f_lateral_project, tiProximalNormalDirection, tiProximalNormalStart);
//	SetResult(PKAResult::flexionLateralGap, result);
//}
//
//void PKAModel::CalLimbFlexionAndVarus()
//{
//	if (m_Femur == nullptr)
//	{
//		std::cout << "femur null ptr";
//		return;
//	}
//	if (m_Tibia == nullptr)
//	{
//		std::cout << "tibia null ptr";
//		return;
//	}
//	AxisType f_SAG;
//	//AxisType f_COR;
//	AxisType f_MA;
//	AxisType t_MA;
//
//
//	if (!(m_Femur->GetAxis(PKAAxes::femurSagittalNormal, f_SAG)) || !(m_Femur->GetAxis(PKAAxes::FemurMechanicalAxis, f_MA)) || !(m_Tibia->GetAxis(PKAAxes::tibiaMechanicalAxis, t_MA)))
//	{
//		return;
//	}
//	Eigen::Vector3d fMADirection = Eigen::Map<Eigen::Vector3d>(f_MA.direction.data());
//	Eigen::Vector3d origin(0,0,0);
//	Eigen::Vector3d f_MA_Project;
//	Eigen::Vector3d t_MA_Project;
//
//	f_MA_Project = CalculationHelper::ProjectPointOntoPlane(fMADirection, origin, SagittalNormal);
//	t_MA_Project = CalculationHelper::ProjectPointOntoPlane(fMADirection, origin, SagittalNormal);
//	double result =  CalculationHelper::ComputeSignedAngle(t_MA_Project, f_MA_Project, SagittalNormal) * (180.0 / EIGEN_PI);
//	SetResult(PKAResult::Limb_Flexion, result);
//
//	//based on plane
//	result =  CalculationHelper::CalculateDirectedAngleWithPlaneNormal(t_MA.direction.data(), f_SAG.direction.data());
//	SetResult(PKAResult::Limb_Varus, result);
//}
//
//void PKAModel::CalPlanned_Varus()
//{
//	double femur_Varus;
//	double tibia_Varus;
//	if (!(this->GetResult(PKAResult::f_Varus, femur_Varus)) || !(this->GetResult(PKAResult::t_Varus, tibia_Varus)))
//	{
//		return;
//	}
//	double result = femur_Varus + tibia_Varus;
//	SetResult(PKAResult::Planned_Varus, result);
//}
//
//void PKAModel::CalPrePlanning()
//{
//	CalFemurVarus();
//	CalTibiaVarus();
//	CalFemurRotation();
//	CalTibiaExternal();
//	CalFemurFlexion();
//	CalTibiaPostSlope();
//	CalFemurDistalResectionDepth();
//	CalFemurPosteriorResectionDepth();
//	CalTibiaResectionDepth();
//}
//
//void PKAModel::CalIntraPlanning()
//{
//	CalLimbFlexionAndVarus();
//	CalExtensionGap();
//	CalFlexionGap();
//}
//
//PKAPrePlanBoneModel* PKAModel::Femur()
//{
//	return m_Femur;
//}
//
//PKAPrePlanBoneModel* PKAModel::Tibia()
//{
//	return m_Tibia;
//}
//
//PKAProthesisModel* PKAModel::FemurProsthesis()
//{
//	return m_FemurProsthesis;
//}
//
//PKAProthesisModel* PKAModel::TibiaProsthesis()
//{
//	return m_TibiaProsthesis;
//}
//
//PKADataBase* PKAModel::PKA()
//{
//	return m_PKA;
//}
//
//PKAModel& PKAModel::Instance()
//{
//	static PKAModel instance;
//	return instance;
//}
