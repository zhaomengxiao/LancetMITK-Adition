
#include "reduction.h"

using namespace othopedics;

void Reduction::SetResult(EResult name, double res)
{
	m_results.insert_or_assign(name, res);
}

bool Reduction::GetResult(EResult name, double& outp_res)
{
	auto iter = m_results.find(name);
	if (iter != m_results.end())
	{
		outp_res = iter->second;
		return true;
	}
	std::cout << "Error: Cant find Result:" << to_string(name) << std::endl;
	return false;
}

void Reduction::PreOperativeReduction_Canal()
{
	if (m_Pelvis == nullptr|| m_Femur_R == nullptr|| m_Femur_L == nullptr)
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing bodies";
		return;
	}
	//correction and move to origin
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_R->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_L->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	//move femur center to hip center
	//L
	Eigen::Vector3d hipCenterL, femurLHipCenter;
	if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_L, hipCenterL)
		|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_FHC, femurLHipCenter))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
		return;
	}
	
	Eigen::Vector3d t = hipCenterL - femurLHipCenter;
	
	Eigen::Matrix4d trans;
	trans.setIdentity();
	trans.block<3, 1>(0, 3) = t;
	//MITK_INFO << trans;
	m_Femur_L->SetIndexToWorldTransform(trans);
	
	//move femur center to hip center
	//R
	Eigen::Vector3d hipCenterR, FHC_R;
	if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_R, hipCenterR)
		|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_FHC, FHC_R))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
		return;
	}
	
	Eigen::Vector3d t2 = hipCenterR - FHC_R;
	
	Eigen::Matrix4d trans2;
	trans2.setIdentity();
	trans2.block<3, 1>(0, 3) = t2;
	//MITK_INFO << trans2;
	m_Femur_R->SetIndexToWorldTransform(trans2);
}

void Reduction::PreOperativeReduction_Mechanical()
{
	if (m_Pelvis == nullptr || m_Femur_R == nullptr || m_Femur_L == nullptr)
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing bodies";
		return;
	}
	// move to origin
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_R->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_L->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	// correction
	//L
	Eigen::Vector3d FHC_L, FNC_L,ME_L,LE_L;
	if (!m_Femur_L->GetGlobalLandMark(ELandMarks::f_FHC, FHC_L)
		|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_FNC, FNC_L)
		|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_ME, ME_L)
		|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_LE, LE_L)
		)
	{
		MITK_WARN << "PreOperativeReduction_Mechanical Failed: missing landmarks";
		return;
	}
	auto correctionMatrix = CalFemurMechanicalCorrectionMatrix(FHC_L, FNC_L, ME_L, LE_L, ESide::left);
	//move femur center to hip center
	Eigen::Vector3d hipCenterL, femurLHipCenter;
	if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_L, hipCenterL)
		|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_FHC, femurLHipCenter))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
		return;
	}
	Eigen::Vector3d t = hipCenterL - femurLHipCenter;
	correctionMatrix.block<3, 1>(0, 3) += t;

	m_Femur_L->SetIndexToWorldTransform(correctionMatrix);

	//R
	Eigen::Vector3d FHC, FNC, ME, LE;
	if (!m_Femur_R->GetGlobalLandMark(ELandMarks::f_FHC, FHC)
		|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_FNC, FNC)
		|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_ME, ME)
		|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_LE, LE)
		)
	{
		MITK_WARN << "PreOperativeReduction_Mechanical Failed: missing landmarks";
		return;
	}
	
	auto correctionMatrix2 = CalFemurMechanicalCorrectionMatrix(FHC, FNC, ME, LE, ESide::right);
	//move femur center to hip center
	Eigen::Vector3d hipCenterR, femurRHipCenter;
	if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_R, hipCenterR)
		|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_FHC, femurRHipCenter))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
		return;
	}
	Eigen::Vector3d t2 = hipCenterR - femurRHipCenter;
	correctionMatrix2.block<3, 1>(0, 3) += t2;
	
	m_Femur_R->SetIndexToWorldTransform(correctionMatrix2);
}

void Reduction::CalPreopOffset()
{
	//cal combined offset
	//left
	Eigen::Vector3d PFCA_L, DFCA_L, MidLine_L;
	if (!m_Femur_L->GetGlobalLandMark(ELandMarks::f_PFCA, PFCA_L) ||
		!m_Femur_L->GetGlobalLandMark(ELandMarks::f_DFCA, DFCA_L) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_MidLine, MidLine_L))
	{
		MITK_WARN << "Can't Cal Combined Offset: missing landmarks";
		return;
	}

	auto offset_L = CombinedOffset(PFCA_L.data(), DFCA_L.data(), MidLine_L.data());

	//right
	Eigen::Vector3d PFCA_R, DFCA_R, MidLine_R;
	if (!m_Femur_R->GetGlobalLandMark(ELandMarks::f_PFCA, PFCA_R) ||
		!m_Femur_R->GetGlobalLandMark(ELandMarks::f_DFCA, DFCA_R) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_MidLine, MidLine_R))
	{
		MITK_WARN << "Can't Cal Combined Offset: missing landmarks";
		return;
	}
	auto offset_R = CombinedOffset(PFCA_R.data(), DFCA_R.data(), MidLine_R.data());

	if (m_OperationSide == ESide::left)
	{
		SetResult(EResult::r_Offset_preop, offset_L);
		SetResult(EResult::r_Offset_contra, offset_R);
		SetResult(EResult::r_OffsetDiff_preop_vs_contra, offset_L - offset_R);
	}
	else
	{
		SetResult(EResult::r_Offset_preop, offset_R);
		SetResult(EResult::r_Offset_contra, offset_L);
		SetResult(EResult::r_OffsetDiff_preop_vs_contra, offset_R - offset_L);
	}
}

void Reduction::CalPreopHipLength()
{
	//cal hip length
	//left
	Eigen::Vector3d LT_L, ASIS_L, ASIS_R;
	if (!m_Femur_L->GetGlobalLandMark(ELandMarks::f_LT, LT_L) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_ASIS_L, ASIS_L) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_ASIS_R, ASIS_R))
	{
		MITK_WARN << "Can't Cal HipLength: missing landmarks";
		return;
	}

	auto hiplength_L = HipLength(LT_L.data(),ASIS_L.data(),ASIS_R.data());

	//right
	Eigen::Vector3d LT_R;
	if (!m_Femur_R->GetGlobalLandMark(ELandMarks::f_LT, LT_R))
	{
		MITK_WARN << "Can't Cal HipLength: missing landmarks";
		return;
	}

	auto hiplength_R = HipLength(LT_R.data(), ASIS_L.data(), ASIS_R.data());

	if (m_OperationSide == ESide::left)
	{
		SetResult(EResult::r_HipLength_preop, hiplength_L);
		SetResult(EResult::r_HipLength_contra, hiplength_R);
		SetResult(EResult::r_HipLengthDiff_preop_vs_contra, hiplength_L - hiplength_R);
	}
	else
	{
		SetResult(EResult::r_HipLength_preop, hiplength_R);
		SetResult(EResult::r_HipLength_contra, hiplength_L);
		SetResult(EResult::r_HipLengthDiff_preop_vs_contra, hiplength_R - hiplength_L);
	}
}

void Reduction::PlanReduction(Eigen::Matrix4d cupPlanMatrix,Eigen::Matrix4d stemPlanMatrix)
{
	if (m_Pelvis == nullptr || m_Femur_R == nullptr || m_Femur_L == nullptr || m_Cup == nullptr || m_Stem == nullptr)
	{
		MITK_WARN << "PlanReduction_Canal Failed: missing bodies";
		return;
	}

	//init pose
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Cup->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Stem->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_L->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Femur_R->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	Eigen::Matrix4d pelvisTrans = Eigen::Matrix4d::Identity();
	m_Pelvis->SetIndexToWorldTransform(pelvisTrans);

	Eigen::Matrix4d cupTrans = pelvisTrans* cupPlanMatrix;
	m_Cup->SetIndexToWorldTransform(cupTrans);

	//move stem center to cup center
	//R
	Eigen::Vector3d cupCOR, headAssemblyPoint;
	if (!m_Cup->GetGlobalLandMark(ELandMarks::cup_COR, cupCOR)
		|| !m_Stem->GetGlobalLandMark(ELandMarks::stem_HeadAssemblyPoint_M, headAssemblyPoint))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
		return;
	}
	Eigen::Matrix4d stemTrans;
	stemTrans.setIdentity();
	Eigen::Vector3d t = cupCOR - headAssemblyPoint;
	stemTrans.block<3, 1>(0, 3) = t;
	
	m_Stem->SetIndexToWorldTransform(stemTrans);

	//move femur to stem
	if (m_OperationSide == ESide::left)
	{
		//left femur
		Eigen::Matrix4d femurTrans_L = stemTrans * (stemPlanMatrix.inverse());
		m_Femur_L->SetIndexToWorldTransform(femurTrans_L);

		//move right femur center to hip center
		Eigen::Vector3d hipCenterR, FHC_R;
		if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_R, hipCenterR)
			|| !m_Femur_R->GetGlobalLandMark(ELandMarks::f_FHC, FHC_R))
		{
			MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
			return;
		}

		Eigen::Vector3d t_femur_r = hipCenterR - FHC_R;

		Eigen::Matrix4d femurTrans_R;
		femurTrans_R.setIdentity();
		femurTrans_R.block<3, 1>(0, 3) = t_femur_r;
		//MITK_INFO << trans2;
		m_Femur_R->SetIndexToWorldTransform(femurTrans_R);

	}
	else
	{
		//right femur
		Eigen::Matrix4d femurTrans_R = stemTrans * (stemPlanMatrix.inverse());
		m_Femur_R->SetIndexToWorldTransform(femurTrans_R);

		//move right femur center to hip center
		Eigen::Vector3d hipCenterL, FHC_L;
		if (!m_Pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_L, hipCenterL)
			|| !m_Femur_L->GetGlobalLandMark(ELandMarks::f_FHC, FHC_L))
		{
			MITK_WARN << "PreOperativeReduction_Canal Failed: missing landmarks";
			return;
		}

		Eigen::Vector3d t_femur_l = hipCenterL - FHC_L;

		Eigen::Matrix4d femurTrans_L;
		femurTrans_L.setIdentity();
		femurTrans_L.block<3, 1>(0, 3) = t_femur_l;
		//MITK_INFO << trans2;
		m_Femur_R->SetIndexToWorldTransform(femurTrans_L);
	}

}
