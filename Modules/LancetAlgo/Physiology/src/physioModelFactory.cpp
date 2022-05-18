#include "physioModelFactory.h"

#include <cstdarg>
#include <iostream>

#include "basic.h"
#include "Eigen/Eigen"


namespace lancetAlgorithm
{

	THA_Model::~THA_Model()
	{
		std::cout << "THA_Model deconstructed" << std::endl;
	}

	void THA_Model::Delete()
	{
		std::cout << "THA_Model deconstructed" << std::endl;
		if (m_pelvis != nullptr)
		{
			delete m_pelvis;
			m_pelvis = nullptr;
		}
		if (m_femur != nullptr)
		{
			delete m_femur;
			m_femur = nullptr;
		}
		if (m_femur_op != nullptr)
		{
			delete m_femur_op;
			m_femur_op = nullptr;
		}
		std::cout << "deconstruct finish" << std::endl;
	}

	void THA_Model::BuildPelvis(unsigned int n, double* data, ...)
	{
		if (nullptr == m_pelvis)
		{
			m_pelvis =new PelvisModel;
		}

		try
		{
			std::va_list args;
			va_start(args, n);
			m_pelvis->SetLandMark(ELandMarks::p_RASI, va_arg(args, double *));
			m_pelvis->SetLandMark(ELandMarks::p_LASI, va_arg(args, double *));
			m_pelvis->SetLandMark(ELandMarks::p_PT, va_arg(args, double *));
			va_end(args);
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		m_pelvis->Update();
		std::cout << "end BuildPelvis" << std::endl;
	}

	void THA_Model::BuildFemur(ESide side , ...)
	{
		std::cout << "Build " << to_string(side) << " side Femur" << std::endl;
		if (side == e_opSide)
		{
			std::cout << "operation side" << std::endl;
			if (nullptr == m_femur_op)
			{
				m_femur_op = new femurModel_OpSide(side);
			}
			try
			{
				std::va_list args;
				va_start(args, side);
				m_femur_op->SetLandMark(ELandMarks::f_DFCA, va_arg(args, double *));
				m_femur_op->SetLandMark(ELandMarks::f_PFCA, va_arg(args, double *));
				m_femur_op->SetLandMark(ELandMarks::f_FHC, va_arg(args, double *));
				m_femur_op->SetLandMark(ELandMarks::f_LT, va_arg(args, double *));
				va_end(args);
			}
			catch (std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			m_femur_op->Update();		
		}
		else
		{
			if (nullptr == m_femur)
			{
				m_femur = new femurModel(side);
			}
			try
			{
				std::va_list args;
				va_start(args, side);
				m_femur->SetLandMark(ELandMarks::f_DFCA, va_arg(args, double *));
				m_femur->SetLandMark(ELandMarks::f_PFCA, va_arg(args, double *));
				m_femur->SetLandMark(ELandMarks::f_FHC, va_arg(args, double *));
				m_femur->SetLandMark(ELandMarks::f_LT, va_arg(args, double *));
				va_end(args);
			}
			catch (std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			m_femur->Update();
		}
		std::cout << "end Build Femur" << std::endl;
	}

	void THA_Model::SetFHCinOp(double* data)
	{
		if (m_femur_op==nullptr)
		{
			return;
		}
		m_femur_op->SetLandMark(ELandMarks::f_FHC_inOp, data);
	}

	void THA_Model::SetPreCheckPoints(double* distal, double* proximal)
	{
		if (m_femur_op == nullptr)
		{
			return;
		}
		m_femur_op->SetLandMark(ELandMarks::f_CheckPointD_pre, distal);
		m_femur_op->SetLandMark(ELandMarks::f_CheckPointP_pre, proximal);
	}

	void THA_Model::SetPostCheckPoints(double* distal, double* proximal)
	{
		if (m_femur_op == nullptr)
		{
			return;
		}
		m_femur_op->SetLandMark(ELandMarks::f_CheckPointD_post, distal);
		m_femur_op->SetLandMark(ELandMarks::f_CheckPointP_post, proximal);

		m_femur_op->Update_inOp();
		CalHipLengthAndOffsetPostOp();
		CalHipLengthDiff_Op2Contralateral();
		CalOffsetDiff_Op2Contralateral();
	}

	PelvisModel* THA_Model::Pelvis()
	{
		return m_pelvis;
	}

	femurModel* THA_Model::Femur() const
	{
		return m_femur;
	}

	femurModel_OpSide* THA_Model::Femur_opSide() const
	{
		return m_femur_op;
	}

	ESide THA_Model::GetOprationSide() const
	{
		return e_opSide;
	}

	void THA_Model::SetOprationSide(ESide opside)
	{
		e_opSide = opside;
	}

	double THA_Model::CalHipLength(ESide side)
	{
		double res = 0.0;
		femurModel* femur = nullptr;
		if (side == e_opSide)
		{
			femur = m_femur_op;
		}
		else
		{
			femur = m_femur;
		}
		if (nullptr == femur)
		{
			std::cout << "CalHipLength Err: femur null" << std::endl;
			return res;
		}
		auto LT = femur->GetLT(ECorrection::MECHANICS);
		LandMarkType LASI{};
		m_pelvis->GetLandMark(ELandMarks::p_LASI, LASI);

		double norm[3]{ 0,0,1 };
		res = DistanceFromPointToPlane(LT.data(), norm, LASI.data());
		femur->SetResult(EResult::f_HipLength, res);
		return res;
	}

	double THA_Model::CalCombineOffset(ESide side)
	{
		double res = 0.0;
		femurModel* femur = nullptr;
		if (side == e_opSide)
		{
			if (nullptr == m_femur_op)
			{
				std::cout << "CalHipLength Err: operation side of femur null" << std::endl;
				return res;
			}
			femur = m_femur_op;
		}
		else
		{
			if (nullptr == m_femur)
			{
				std::cout << "CalHipLength Err: contralateral side of femur null" << std::endl;
				return 0;
			}
			femur = m_femur;
		}

		//PlaneType midPlane{};
		//m_pelvis->GetPlane(EPlanes::MIDPLANE,midPlane);
		//res = DistanceFromPointToPlane(femur->GetPFCA(ECorrection::CANAL).data(), midPlane.normal.direction.data(), midPlane.normal.startPoint.data());
		auto PFCA = femur->GetPFCA(ECorrection::CANAL);
		LandMarkType PT{};
		m_pelvis->GetLandMark(ELandMarks::p_PT, PT);
		res = DistanceOfTwoPoints(PFCA.data(), PT.data());
		femur->SetResult(EResult::f_Offset, res);
		return res;
	}

	std::array<double, 2> THA_Model::CalHipLenAndOffset(ESide side)
	{
		return std::array<double, 2>{CalHipLength(side), CalCombineOffset(side)};
	}

	std::array<double, 4> THA_Model::CalHipLenAndOffsetBothSides()
	{
		std::array<double, 4>res{CalHipLength(ESide::right), CalCombineOffset(ESide::right)
			, CalHipLength(ESide::left), CalCombineOffset(ESide::left)};

		CalOffsetDiff_preOp2Contral();
		CalHipLengthDiff_preOp2Contral();
		return res;
	}

	bool THA_Model::CalHipLengthAndOffsetPostOp()
	{
		if (!CalHipLengthPostOp())
		{
			return false;
		}
		if (!CalOffsetPostOp())
		{
			return false;
		}
		return true;
	}

	bool THA_Model::CalOffsetPostOp()
	{
		double offsetDiff_prePostOp = 0;
		double offset = 0;
		if (m_femur_op == nullptr)
		{
			std::cout << "Err CalOffsetPostOp:Call BuildFemur Opside first" << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_Offset, offset))
		{
			std::cout << "Err CalOffsetPostOp:Call CalCombineOffset first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_OffsetDiff_PrePostOp, offsetDiff_prePostOp))
		{
			std::cout << "Err CalOffsetPostOp:Call SetPostCheckPoints first " << std::endl;
			return false;
		}
		
		m_femur_op->SetResult(EResult::f_Offset_post, offset + offsetDiff_prePostOp);
		return true;
	}

	

	bool THA_Model::CalHipLengthPostOp()
	{
		double hiplengthDiff_prePostOp = 0;
		double hiplength = 0;
		if (m_femur_op == nullptr)
		{
			std::cout << "Err CalHipLengthPostOp:Call BuildFemur Opside first" << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_HipLength, hiplength))
		{
			std::cout << "Err CalHipLengthPostOp:Call CalCombineOffset first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_HipLengthDiff_PrePostOp, hiplengthDiff_prePostOp))
		{
			std::cout << "Err CalHipLengthPostOp:Call SetPostCheckPoints first " << std::endl;
			return false;
		}
		
		m_femur_op->SetResult(EResult::f_HipLength_post, hiplength + hiplengthDiff_prePostOp);
		return true;
	}

	bool THA_Model::CalOffsetDiff_preOp2Contral()
	{
		double offset = 0;
		double offset_opside = 0;
		if (m_femur_op == nullptr || m_femur == nullptr)
		{
			std::cout << "Err CalOffsetDiff_preOp2Contral:Call BuildFemur first" << std::endl;
			return false;
		}
		if (!m_femur->GetResult(EResult::f_Offset, offset))
		{
			std::cout << "Err CalOffsetDiff_preOp2Contral:Call CalOffset first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_Offset, offset_opside))
		{
			std::cout << "Err CalOffsetDiff_preOp2Contral:Call CalOffset first " << std::endl;
			return false;
		}

		m_femur_op->SetResult(EResult::f_OffsetDiff_preOp2Contral, offset_opside - offset);
		return true;
	}


	bool THA_Model::CalHipLengthDiff_preOp2Contral()
	{
		double hiplength = 0;
		double hiplength_opside = 0;
		if (m_femur_op == nullptr || m_femur == nullptr)
		{
			std::cout << "Err CalHipLengthDiff_preOp2Contral:Call BuildFemur first" << std::endl;
			return false;
		}
		if (!m_femur->GetResult(EResult::f_HipLength, hiplength))
		{
			std::cout << "Err CalHipLengthDiff_preOp2Contral:Call CalHipLength first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_HipLength, hiplength_opside))
		{
			std::cout << "Err CalHipLengthDiff_preOp2Contral:Call CalHipLength first " << std::endl;
			return false;
		}

		m_femur_op->SetResult(EResult::f_HipLengthDiff_preOp2Contral, hiplength_opside - hiplength);
		return true;
	}

	bool THA_Model::CalHipLengthDiff_Op2Contralateral()
	{
		double hiplength_post = 0;
		double hiplength = 0;
		if (m_femur_op == nullptr || m_femur == nullptr)
		{
			std::cout << "Err CalHipLengthDiff_Op2Contralateral:Call BuildFemur first" << std::endl;
			return false;
		}
		if (!m_femur->GetResult(EResult::f_HipLength, hiplength))
		{
			std::cout << "Err CalHipLengthDiff_Op2Contralateral:Call CalHipLength first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_HipLength_post, hiplength_post))
		{
			std::cout << "Err CalHipLengthDiff_Op2Contralateral:Call CalHipLengthPostOp first " << std::endl;
			return false;
		}
		
		//auto res = m_femur_op->GetResult(EResult::f_HipLength_post,) - m_femur->GetResult(EResult::f_HipLength,);
		m_femur_op->SetResult(EResult::f_HipLengthDiff_Op2Contralateral, hiplength_post- hiplength);
		return true;
	}

	bool THA_Model::CalOffsetDiff_Op2Contralateral()
	{
		double offset_post = 0;
		double offset = 0;
		if (m_femur_op == nullptr || m_femur == nullptr)
		{
			std::cout << "Err CalOffsetDiff_Op2Contralateral:Call BuildFemur first" << std::endl;
			return false;
		}
		if (!m_femur->GetResult(EResult::f_Offset, offset))
		{
			std::cout << "Err CalOffsetDiff_Op2Contralateral:Call CalOffset first " << std::endl;
			return false;
		}
		if (!m_femur_op->GetResult(EResult::f_Offset_post, offset_post))
		{
			std::cout << "Err CalOffsetDiff_Op2Contralateral:Call CalOffsetPostOp first " << std::endl;
			return false;
		}
		//auto res = m_femur_op->GetResult(EResult::f_Offset_post,) - m_femur->GetResult(EResult::f_Offset,);
		m_femur_op->SetResult(EResult::f_OffsetDiff_Op2Contralateral, offset_post-offset);
		return true;
	};

	THA_Model& THA_Model::Instance()
	{
		static THA_Model instance;
		return instance;
	}

	THA_Model::THA_Model()
	{
		std::cout << "THA Model constructed" << std::endl;
	}

	TKA_Model::~TKA_Model()
	{
		std::cout << "TKA_Model deconstructed" << std::endl;
	}

	void TKA_Model::Delete()
	{
		std::cout << "TKA_Model deconstructed" << std::endl;
		if (m_femur != nullptr)
		{
			delete m_femur;
			m_femur = nullptr;
		}
		if (m_tibia != nullptr)
		{
			delete m_tibia;
			m_tibia = nullptr;
		}
		if (m_femurimplant != nullptr)
		{
			delete m_femurimplant;
			m_femurimplant = nullptr;
		}
		if (m_tibiaimplant != nullptr)
		{
			delete m_tibiaimplant;
			m_tibiaimplant = nullptr;
		}
		std::cout << "deconstruct finished" << std::endl;
	}

	void TKA_Model::buildFemur(unsigned int n, ...)
	{
		if (nullptr == m_femur)
		{
			m_femur = new TKAFemurModel;
		}
		try
		{
			std::va_list args;
			va_start(args, n);
			m_femur->SetLandMark(TKALandmarks::f_ME, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_LE, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_PM, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_PL, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_MDP, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_LDP, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_MPP, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::f_LPP, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::HIP_CENTER, va_arg(args, double*));
			m_femur->SetLandMark(TKALandmarks::fKNEE_CENTER, va_arg(args, double*));
			va_end(args);
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		m_femur->update();
		std::cout << "end build femur" << std::endl;
	}

	void TKA_Model::buildTibia(unsigned int n, ...)
	{
		if (nullptr == m_tibia)
		{
			m_tibia = new TKATibiaModel;
		}
		try
		{
			std::va_list args;
			va_start(args, n);
			m_tibia->SetLandMark(TKALandmarks::PCL_CENTER, va_arg(args, double*));
			m_tibia->SetLandMark(TKALandmarks::tKNEE_CENTER, va_arg(args, double*));
			m_tibia->SetLandMark(TKALandmarks::tANKLE_CENTER, va_arg(args, double*));
			m_tibia->SetLandMark(TKALandmarks::TUBERCLE, va_arg(args, double*));
			m_tibia->SetLandMark(TKALandmarks::t_PM, va_arg(args, double*));
			m_tibia->SetLandMark(TKALandmarks::t_PL, va_arg(args, double*));
			va_end(args);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
		m_tibia->update();
		std::cout << "end build tibia" << std::endl;
	}

	void TKA_Model::buildFemurImplant(unsigned int n, ...)
	{
		if (nullptr == m_femurimplant)
		{
			m_femurimplant = new TKAFemurImplantModel;
		}
		try
		{
			std::va_list args;
			va_start(args,n);
			m_femurimplant->SetLandMark(TKALandmarks::fi_ANTERIORSTART, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_ANTERIOREND, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_ANTERIORCHAMSTART, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_ANTERIORCHAMEND, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_DISTALSTART, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_DISTALEND, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_POSTERIORCHAMSTART, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_POSTERIORCHAMEND, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_POSTERIORSTART, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_POSTERIOREND, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_ResectionMedial, va_arg(args, double*));
			m_femurimplant->SetLandMark(TKALandmarks::fi_ResectionLateral, va_arg(args, double*));
			va_end(args);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
		m_femurimplant->update();
		std::cout << "end build femur implant";
	}

	void TKA_Model::buildTibiaImplant(unsigned int n, ...)
	{
		if (nullptr == m_tibiaimplant)
		{
			m_tibiaimplant = new TKATibiaImplantModel;
		}
		try
		{
			std::va_list args;
			va_start(args, n);
			m_tibiaimplant->SetLandMark(TKALandmarks::ti_PROXIMALSTART, va_arg(args, double*));
			m_tibiaimplant->SetLandMark(TKALandmarks::ti_PROXIMALEND, va_arg(args, double*));
			m_tibiaimplant->SetLandMark(TKALandmarks::ti_SYMMETRYSTART, va_arg(args, double*));
			m_tibiaimplant->SetLandMark(TKALandmarks::ti_SYMMETRYEND, va_arg(args, double*));
			va_end(args);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
		m_tibiaimplant->update();
		std::cout << "end build tibia implant";
	}

	void TKA_Model::SetOperationSide(TKASide side)
	{
		m_opSide = side;
		UpdateResultSymbol();
	}

	void TKA_Model::UpdateResultSymbol()
	{
		if (m_opSide == TKASide::left)
			m_resultSymbol = 1.0;
		else if (m_opSide == TKASide::right)
			m_resultSymbol = -1.0;
	}

	void TKA_Model::CalTransformToStandardPlane(double * p1, double * p2, double * p3, double * p4)
	{
		Eigen::Vector3d axisX = { p2[0] - p1[0],p2[1] - p1[1],p2[2] - p1[2] };
		Eigen::Vector3d axisZ = { p4[0] - p3[0],p4[1] - p3[1],p4[2] - p3[2] };
		Eigen::Vector3d axisY = axisZ.cross(axisX);

		axisX.normalize();
		axisY.normalize();
		axisZ.normalize();

		Eigen::Matrix3d transform;
		transform << axisX[0], axisY[0], axisZ[0],
			axisX[1], axisY[1], axisZ[1],
			axisX[2], axisY[2], axisZ[2];
		Eigen::Matrix3d inv = transform.inverse();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m_transform[3 * i + j] = inv(i, j);
			}
		}
	}


	void TKA_Model::CalFemurVarus()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		AxisType SAG{};
		AxisType FIA{};
		
		if (!(m_femur->GetAxis(TKAAxes::f_sagittal, SAG))||!(m_femurimplant->GetAxis(TKAAxes::fi_A, FIA)))
		{
			return;
		}

		VectorType SAG_dir = SAG.direction;
		VectorType FIA_dir = FIA.direction;

		//double origin[] = { 0,0,0 };
		//double MA_Project[3];
		//double FIA_Project[3];
		//projectToPlane(MA_dir.data(), origin, CoronalNormal, MA_Project);
		//projectToPlane(FIA_dir.data(), origin, CoronalNormal, FIA_Project);
		//double result = AngleBetween2Vector(FIA_Project, MA_Project, CoronalNormal) * (180.0 / M_PI)*m_resultSymbol;
		double result = -DirectedAngleBetweenVectorAndPlane(FIA_dir.data(), SAG_dir.data());
		SetResult(TKAResult::f_Varus, result);
	}

	void TKA_Model::CalFemurRotation()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		AxisType PCA;
		AxisType TEA;
		AxisType FI_BoneResectionLine;

		if (!(m_femur->GetAxis(TKAAxes::f_PCA, PCA)) || !(m_femur->GetAxis(TKAAxes::f_TEA, TEA)) 
			|| !(m_femurimplant->GetAxis(TKAAxes::fi_BRL, FI_BoneResectionLine)))
		{
			return;
		}

		VectorType PCA_dir = PCA.direction;
		VectorType TEA_dir = TEA.direction;
		VectorType FI_BoneResectionLine_dir = FI_BoneResectionLine.direction;

		double origin[] = { 0,0,0 };
		double PCA_Project[3];
		double TEA_Project[3];
		double FI_BoneResectionLine_Project[3];

		projectToPlane(PCA_dir.data(), origin, TransverseNormal, PCA_Project);
		projectToPlane(TEA_dir.data(), origin, TransverseNormal, TEA_Project);
		projectToPlane(FI_BoneResectionLine_dir.data(), origin, TransverseNormal, FI_BoneResectionLine_Project);

		double result = AngleBetween2Vector(FI_BoneResectionLine_Project, PCA_Project, TransverseNormal) * (180.0 / EIGEN_PI) * m_resultSymbol;
		SetResult(TKAResult::f_PCAExternal, result);
		result = AngleBetween2Vector(FI_BoneResectionLine_Project, TEA_Project, TransverseNormal) * (180.0 / EIGEN_PI) * m_resultSymbol;
		SetResult(TKAResult::f_TEAExternal, result);
	}

	void TKA_Model::CalFemurFlexion()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		AxisType MA;
		AxisType FIA;

		if (!(m_femur->GetAxis(TKAAxes::f_MA, MA)) || !(m_femurimplant->GetAxis(TKAAxes::fi_A, FIA)))
		{
			return;
		}

		VectorType MA_dir = MA.direction;
		VectorType FIA_dir = FIA.direction;

		double origin[] = { 0,0,0 };
		double MA_Project[3];
		double FIA_Project[3];

		projectToPlane(MA_dir.data(), origin, SagittalNormal, MA_Project);
		projectToPlane(FIA_dir.data(), origin, SagittalNormal, FIA_Project);

		double result = AngleBetween2Vector(FIA_Project,MA_Project, SagittalNormal) * (180.0 / EIGEN_PI);
		SetResult(TKAResult::f_flexion, result);
	}

	void TKA_Model::CalTibiaVarus()
	{
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		//AxisType MA;
		AxisType TIA;
		AxisType SAG;

		if (!(m_tibia->GetAxis(TKAAxes::t_sagittal, SAG)) || !(m_tibiaimplant->GetAxis(TKAAxes::ti_A, TIA)))
		{
			return;
		}

		VectorType SAG_dir = SAG.direction;
		VectorType TIA_dir = TIA.direction;

		//double origin[] = { 0,0,0 };
		//double MA_Project[3];
		//double TIA_Project[3];

		//projectToPlane(MA_dir.data(), origin, CoronalNormal, MA_Project);
		//projectToPlane(TIA_dir.data(), origin, CoronalNormal, TIA_Project);

		//double result = AngleBetween2Vector(MA_Project, TIA_Project, CoronalNormal) * (180.0 / M_PI) *m_resultSymbol;

		double result = -DirectedAngleBetweenVectorAndPlane(TIA_dir.data(), SAG_dir.data())*m_resultSymbol;

		SetResult(TKAResult::t_Varus, result);
	}

	void TKA_Model::CalTibiaExternal()
	{
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		//AxisType APA;
		AxisType SAG;
		AxisType SA;

		if (!(m_tibia->GetAxis(TKAAxes::t_sagittal, SAG)) || !(m_tibiaimplant->GetAxis(TKAAxes::ti_SA, SA)))
		{
			return;
		}

		//VectorType APA_dir = APA.direction;
		//VectorType SA_dir = SA.direction;

		//double origin[] = { 0,0,0 };
		//double APA_Project[3];
		//double SA_Project[3];

		//projectToPlane(APA_dir.data(), origin, TransverseNormal, APA_Project);
		//projectToPlane(SA_dir.data(), origin, TransverseNormal, SA_Project);

		//double result = AngleBetween2Vector(SA_Project, APA_Project, TransverseNormal) * (180.0 / M_PI) * m_resultSymbol;

		double result = DirectedAngleBetweenVectorAndPlane(SA.direction.data(), SAG.direction.data()) * m_resultSymbol;

		SetResult(TKAResult::t_External, result);
	}

	void TKA_Model::CalTibiaPostSlope()
	{
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		//AxisType TMA;
		//AxisType TIA;

		//if (!(m_tibia->GetAxis(TKAAxes::t_MA, TMA)) || !(m_tibiaimplant->GetAxis(TKAAxes::ti_A, TIA)))
		//{
		//	return;
		//}

		//VectorType TMA_dir = TMA.direction;
		//VectorType TIA_dir = TIA.direction;

		//double origin[] = { 0,0,0 };
		//double TMA_Project[3];
		//double TIA_Project[3];

		//projectToPlane(TMA_dir.data(), origin, SagittalNormal, TMA_Project);
		//projectToPlane(TIA_dir.data(), origin, SagittalNormal, TIA_Project);

		//double result = AngleBetween2Vector(TMA_Project, TIA_Project, SagittalNormal) * (180.0 / M_PI);
		//SetResult(TKAResult::t_PSlope, result);

		AxisType COR;
		AxisType TIA;
		if (!(m_tibia->GetAxis(TKAAxes::t_coronal, COR)) || !(m_tibiaimplant->GetAxis(TKAAxes::ti_A, TIA)))
		{
			return;
		}

		double result = DirectedAngleBetweenVectorAndPlane(TIA.direction.data(), COR.direction.data());

		SetResult(TKAResult::t_PSlope, result);
	}

	void TKA_Model::CalFemurDistalResectionDepth()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		LandMarkType f_MDP;
		LandMarkType f_LDP;
		PlaneType f_distal;

		if (!(m_femur->GetLandMark(TKALandmarks::f_MDP, f_MDP)) || !(m_femur->GetLandMark(TKALandmarks::f_LDP, f_LDP))
			|| !(m_femurimplant->GetPlane(TKAPlanes::FEMURDISTAL, f_distal)))
		{
			return;
		}

		AxisType normal = f_distal.normal;

		double m_flag = f_MDP.data()[2] < normal.startPoint.data()[2] ? 1.0 : -1.0;
		double l_flag = f_LDP.data()[2] < normal.startPoint.data()[2] ? 1.0 : -1.0;

		double result = DistanceFromPointToPlane(f_MDP.data(), normal.direction.data(), normal.startPoint.data()) * m_flag;
		SetResult(TKAResult::f_Distal_MRD, result);
		result = DistanceFromPointToPlane(f_LDP.data(), normal.direction.data(), normal.startPoint.data()) * l_flag;
		SetResult(TKAResult::f_Distal_LRD, result);
	}

	void TKA_Model::CalFemurPosteriorResectionDepth()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		LandMarkType f_MPP;
		LandMarkType f_LPP;
		PlaneType f_posterior;

		if (!(m_femur->GetLandMark(TKALandmarks::f_MPP, f_MPP)) || !(m_femur->GetLandMark(TKALandmarks::f_LPP, f_LPP))
			|| !(m_femurimplant->GetPlane(TKAPlanes::FEMURPOSTERIOR, f_posterior)))
		{
			return;
		}

		AxisType normal = f_posterior.normal;

		double m_flag = f_MPP.data()[1] >= normal.startPoint.data()[1] ? 1.0 : -1.0;
		double l_flag = f_LPP.data()[1] >= normal.startPoint.data()[1] ? 1.0 : -1.0;

		double result = DistanceFromPointToPlane(f_MPP.data(), normal.direction.data(), normal.startPoint.data()) * m_flag;
		SetResult(TKAResult::f_Posterior_MRD, result);
		result = DistanceFromPointToPlane(f_LPP.data(), normal.direction.data(), normal.startPoint.data()) * l_flag;
		TKA_Model::Instance().SetResult(TKAResult::f_Posterior_LRD, result);
	}

	void TKA_Model::CalTibiaResectionDepth()
	{
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		LandMarkType t_PM;
		LandMarkType t_PL;
		PlaneType t_proximal;

		if (!(m_tibia->GetLandMark(TKALandmarks::t_PM, t_PM)) || !(m_tibia->GetLandMark(TKALandmarks::t_PL, t_PL)) 
			|| !(m_tibiaimplant->GetPlane(TKAPlanes::TIBIAPROXIMAL, t_proximal)))
		{
			return;
		}

		AxisType normal = t_proximal.normal;

		double normal_start[3] = { normal.startPoint.data()[0],normal.startPoint.data()[1],normal.startPoint.data()[2]};
		double normal_dir[3] = { normal.direction.data()[0],normal.direction.data()[1],normal.direction.data()[2] };

		double PM_dir[3] = { t_PM.data()[0] - normal_start[0],t_PM.data()[1] - normal_start[1],t_PM.data()[2] - normal_start[2] };
		double PL_dir[3] = { t_PL.data()[0] - normal_start[0],t_PL.data()[1] - normal_start[1],t_PL.data()[2] - normal_start[2] };

		double m_flag = IsSameDirection(PM_dir,normal_dir) ? -1.0 : 1.0;
		double l_flag = IsSameDirection(PL_dir,normal_dir) ? -1.0 : 1.0;

		double result = DistanceFromPointToPlane(t_PM.data(), normal.direction.data(), normal.startPoint.data()) * m_flag;
		SetResult(TKAResult::t_Proximal_MRD, result);
		result = DistanceFromPointToPlane(t_PL.data(), normal.direction.data(), normal.startPoint.data()) * l_flag;
		SetResult(TKAResult::t_Proximal_LRD, result);
	}

	void TKA_Model::CalExtensionGap()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		LandMarkType f_MDP;
		LandMarkType f_LDP;
		PlaneType fi_DISTAL;
		PlaneType ti_PROXIMAL;

		if (!m_femur->GetLandMark(TKALandmarks::f_MDP, f_MDP))
		{
			std::cout << "cant find femur medial distal point";
			return;
		}
		if (!m_femur->GetLandMark(TKALandmarks::f_LDP, f_LDP))
		{
			std::cout << "cant find femur lateral distal point";
			return;
		}
		if (!m_femurimplant->GetPlane(TKAPlanes::FEMURDISTAL,fi_DISTAL))
		{
			std::cout << "cant find femur implant distal plane";
			return;
		}
		if (!m_tibiaimplant->GetPlane(TKAPlanes::TIBIAPROXIMAL, ti_PROXIMAL))
		{
			std::cout << "cant find tibia implant proximal plane";
			return;
		}

		double f_medial_project[3];
		double f_lateral_project[3];
		double t_medial_project[3];
		double t_lateral_project[3];

		projectToPlane(f_MDP.data(), fi_DISTAL.normal.startPoint.data(), fi_DISTAL.normal.direction.data(),f_medial_project);
		projectToPlane(f_LDP.data(), fi_DISTAL.normal.startPoint.data(), fi_DISTAL.normal.direction.data(),f_lateral_project);
		SetLandMark(TKALandmarks::fi_EXTENSIONMEDIAL, f_medial_project);
		SetLandMark(TKALandmarks::fi_EXTENSIONLATERAL, f_lateral_project);

		projectToPlane(f_medial_project, ti_PROXIMAL.normal.startPoint.data(), ti_PROXIMAL.normal.direction.data(), t_medial_project);
		projectToPlane(f_lateral_project, ti_PROXIMAL.normal.startPoint.data(), ti_PROXIMAL.normal.direction.data(), t_lateral_project);
		SetLandMark(TKALandmarks::ti_EXTENSIONMEDIAL, t_medial_project);
		SetLandMark(TKALandmarks::ti_EXTENSIONLATERAL, t_lateral_project);

		double result = DistanceFromPointToPlane(f_medial_project, ti_PROXIMAL.normal.direction.data(), ti_PROXIMAL.normal.startPoint.data());
		SetResult(TKAResult::ExtensionMedialGap, result);
		result = DistanceFromPointToPlane(f_lateral_project, ti_PROXIMAL.normal.direction.data(), ti_PROXIMAL.normal.startPoint.data());
		SetResult(TKAResult::ExtensionLateralGap, result);
	}

	void TKA_Model::CalFlexionGap()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_femurimplant == nullptr)
		{
			std::cout << "femur implant null ptr";
			return;
		}
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		if (m_tibiaimplant == nullptr)
		{
			std::cout << "tibia implant null ptr";
			return;
		}
		LandMarkType f_MPP;
		LandMarkType f_LPP;
		PlaneType fi_POSTERIOR;
		PlaneType ti_PROXIMAL;

		if (!(m_femur->GetLandMark(TKALandmarks::f_MPP, f_MPP)) || !(m_femur->GetLandMark(TKALandmarks::f_LPP, f_LPP))
			|| !(m_femurimplant->GetPlane(TKAPlanes::FEMURPOSTERIOR, fi_POSTERIOR)) || !(m_tibiaimplant->GetPlane(TKAPlanes::TIBIAPROXIMAL, ti_PROXIMAL)))
		{
			return;
		}

		double f_medial_project[3];
		double f_lateral_project[3];
		double t_medial_project[3];
		double t_lateral_project[3];


		projectToPlane(f_MPP.data(), fi_POSTERIOR.normal.startPoint.data(), fi_POSTERIOR.normal.direction.data(), f_medial_project);
		projectToPlane(f_LPP.data(), fi_POSTERIOR.normal.startPoint.data(), fi_POSTERIOR.normal.direction.data(), f_lateral_project);
		SetLandMark(TKALandmarks::fi_FLEXIONMEDIAL, f_medial_project);
		SetLandMark(TKALandmarks::fi_FLEXIONLATERAL, f_lateral_project);

		projectToPlane(f_medial_project, ti_PROXIMAL.normal.startPoint.data(), ti_PROXIMAL.normal.direction.data(), t_medial_project);
		projectToPlane(f_lateral_project, ti_PROXIMAL.normal.startPoint.data(), ti_PROXIMAL.normal.direction.data(), t_lateral_project);
		SetLandMark(TKALandmarks::ti_FLEXIONMEDIAL, t_medial_project);
		SetLandMark(TKALandmarks::ti_FLEXIONLATERAL, t_lateral_project);

		double result = DistanceFromPointToPlane(f_medial_project, ti_PROXIMAL.normal.direction.data(), ti_PROXIMAL.normal.startPoint.data());
		SetResult(TKAResult::flexionMedialGap, result);
		result = DistanceFromPointToPlane(f_lateral_project, ti_PROXIMAL.normal.direction.data(), ti_PROXIMAL.normal.startPoint.data());
		SetResult(TKAResult::flexionLateralGap, result);
	}

	void TKA_Model::CalLimbFlexionAndVarus()
	{
		if (m_femur == nullptr)
		{
			std::cout << "femur null ptr";
			return;
		}
		if (m_tibia == nullptr)
		{
			std::cout << "tibia null ptr";
			return;
		}
		AxisType f_SAG;
		//AxisType f_COR;
		AxisType f_MA;
		AxisType t_MA;


		if (!(m_femur->GetAxis(TKAAxes::f_sagittal, f_SAG)) || !(m_femur->GetAxis(TKAAxes::f_MA, f_MA)) || !(m_tibia->GetAxis(TKAAxes::t_MA, t_MA)))
		{
			return;
		}

		//double origin[] = { 0,0,0 };
		//double t_SAG_Project[3];
		//double t_COR_Project[3];

		//projectToPlane(t_MA.direction.data(), origin, f_SAG.direction.data(), t_SAG_Project);
		//projectToPlane(t_MA.direction.data(), origin, f_COR.direction.data(), t_COR_Project);
		//double result = AngleBetween2Vector(f_MA.direction.data(),t_SAG_Project);
		//SetResult(TKAResult::Limb_Flexion,result);

		//result = AngleBetween2Vector(f_MA.direction.data(), t_COR_Project);
		//SetResult(TKAResult::Limb_Varus, result);


		double origin[] = { 0,0,0 };
		double f_MA_Project[3];
		double t_MA_Project[3];

		/*projectToPlane(f_MA.direction.data(), origin, CoronalNormal, f_MA_Project);
		projectToPlane(t_MA.direction.data(), origin, CoronalNormal, t_MA_Project);
		double result = AngleBetween2Vector(t_MA_Project, f_MA_Project, CoronalNormal) * (180.0 / M_PI) * m_resultSymbol;

		std::cout << "limb varus:" << result;
		SetResult(TKAResult::Limb_Varus, result);*/

		projectToPlane(f_MA.direction.data(), origin, SagittalNormal, f_MA_Project);
		projectToPlane(t_MA.direction.data(), origin, SagittalNormal, t_MA_Project);
		double result = AngleBetween2Vector(t_MA_Project, f_MA_Project, SagittalNormal) * (180.0 / EIGEN_PI);
		SetResult(TKAResult::Limb_Flexion, result);

		//based on plane
		result = DirectedAngleBetweenVectorAndPlane(t_MA.direction.data(), f_SAG.direction.data());
		SetResult(TKAResult::Limb_Varus, result);

		//AxisType f_COR;
		//AxisType f_MA;
		//if (!(m_femur->GetAxis(TKAAxes::f_coronal, f_COR)) || !(m_femur->GetAxis(TKAAxes::f_MA, f_MA)))
		//{
		//	return;
		//}
		//result = -DirectedAngleBetweenVectorAndPlane(t_MA.direction.data(), f_COR.direction.data());
		//std::cout << "line plane angle:" << result;
		//if (AngleBetween2Vector(t_MA.direction.data(), f_MA.direction.data(), true) > 90)
		//{
		//	if (result > 0)
		//		result = 180 - result;
		//	else
		//		result = -180 + result;
		//}
		//result = result * m_resultSymbol;
		//SetResult(TKAResult::Limb_Flexion, result);
	}

	void TKA_Model::CalPlanned_Varus()
	{
		double femur_Varus;
		double tibia_Varus;
		if (!(this->GetResult(TKAResult::f_Varus, femur_Varus)) || !(this->GetResult(TKAResult::t_Varus, tibia_Varus)))
		{
			return;
		}
		double result = femur_Varus + tibia_Varus;
		SetResult(TKAResult::Planned_Varus, result);
	}


	void TKA_Model::CalPrePlanning()
	{
		CalFemurVarus();
		CalTibiaVarus();
		CalFemurRotation();
		CalTibiaExternal();
		CalFemurFlexion();
		CalTibiaPostSlope();
		CalFemurDistalResectionDepth();
		CalFemurPosteriorResectionDepth();
		CalTibiaResectionDepth();
	}

	void TKA_Model::CalIntraPlanning()
	{
		CalLimbFlexionAndVarus();
		CalExtensionGap();
		CalFlexionGap();
	}

	//void TKA_Model::CalGap(double normal_femurImplant[3],double p_femurImplant[3] ,
	//	double normal_tibiaImplant[3], double p_tibiaImplant[3],double medial[3], double lateral[3], double &res_medialGap, double &res_lateralGap)
	//{
	//	double f_medial_project[3];
	//	double f_lateral_project[3];

	//	projectToPlane(medial, p_femurImplant, normal_femurImplant, f_medial_project);
	//	projectToPlane(lateral, p_femurImplant, normal_femurImplant, f_lateral_project);

	//	res_medialGap = DistanceFromPointToPlane(f_medial_project, normal_tibiaImplant, p_tibiaImplant);
	//	res_lateralGap = DistanceFromPointToPlane(f_lateral_project, normal_tibiaImplant, p_tibiaImplant);
	//}

	//void TKA_Model::CalStereotacticBoundray()
	//{

	//}


	TKAFemurModel * TKA_Model::femur()
	{
		return m_femur;
	}

	TKATibiaModel * TKA_Model::tibia()
	{
		return m_tibia;
	}

	TKAFemurImplantModel * TKA_Model::femurimplant()
	{
		return m_femurimplant;
	}

	TKATibiaImplantModel * TKA_Model::tibiaimplant()
	{
		return m_tibiaimplant;
	}

	TKADataBase * TKA_Model::TKA()
	{
		return m_TKA;
	}

	TKA_Model & TKA_Model::Instance()
	{
		static TKA_Model instance;
		return instance;
	}
}
