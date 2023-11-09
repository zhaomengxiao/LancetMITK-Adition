
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

void Reduction::FemurPreopReduction_Canal(Pelvis::Pointer pelvis, Femur::Pointer femur)
{
	femur->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	Eigen::Vector3d hipCOR, FHC;
	if (femur->m_Side == ESide::left)
	{
		if (!pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_L, hipCOR))
		{
			MITK_WARN << "PreOperativeReduction_Canal Failed: missing p_FemurAssemblyPoint_L";
			return;
		}
	}
	else
	{
		if (!pelvis->GetGlobalLandMark(ELandMarks::p_FemurAssemblyPoint_R, hipCOR))
		{
			MITK_WARN << "PreOperativeReduction_Canal Failed: missing p_FemurAssemblyPoint_R";
			return;
		}
	}

	if (!femur->GetGlobalLandMark(ELandMarks::f_FHC, FHC))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing f_FHC";
		return;
	}

	Eigen::Vector3d t = hipCOR - FHC;

	Eigen::Matrix4d trans;
	trans.setIdentity();
	trans.block<3, 1>(0, 3) = t;
	//MITK_INFO << trans;
	femur->SetIndexToWorldTransform(trans);
}

void Reduction::FemurPreopReduction_Mechanical(Pelvis::Pointer pelvis, Femur::Pointer femur)
{
	femur->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	Eigen::Vector3d FHC, FNC, ME, LE;
	if (!femur->GetGlobalLandMark(ELandMarks::f_FHC, FHC)
		|| !femur->GetGlobalLandMark(ELandMarks::f_FNC, FNC)
		|| !femur->GetGlobalLandMark(ELandMarks::f_ME, ME)
		|| !femur->GetGlobalLandMark(ELandMarks::f_LE, LE)
		)
	{
		MITK_WARN << "PreOperativeReduction_Mechanical Failed: missing landmarks";
		return;
	}
	auto correctionMatrix = CalFemurMechanicalCorrectionMatrix(FHC, FNC, ME, LE, femur->m_Side);

	//move femur center to hip center
	Eigen::Vector3d hipCOR;
	ELandMarks FemurAssemblyPoint;

	if (femur->m_Side == ESide::left){FemurAssemblyPoint = ELandMarks::p_FemurAssemblyPoint_L;}
	else{ FemurAssemblyPoint = ELandMarks::p_FemurAssemblyPoint_R; }

	if (!m_Pelvis->GetGlobalLandMark(FemurAssemblyPoint, hipCOR))
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing "<< to_String(FemurAssemblyPoint);
		return;
	}
	Eigen::Vector3d t = hipCOR - FHC;
	correctionMatrix.block<3, 1>(0, 3) += t;

	femur->SetIndexToWorldTransform(correctionMatrix);
}

Eigen::Matrix4d Reduction::GetStemReductionTransform(Cup::Pointer cup, Stem::Pointer stem, ELandMarks HeadAssemblyPoint)
{
	Eigen::Matrix4d stemTrans;
	Eigen::Vector3d cupCOR, headAssemblyPoint;
	if (!cup->GetGlobalLandMark(ELandMarks::cup_COR, cupCOR)
		|| !stem->GetGlobalLandMark(HeadAssemblyPoint, headAssemblyPoint))
	{
		MITK_WARN << "StemReduction Failed: missing landmarks";
	}

	stemTrans.setIdentity();
	Eigen::Vector3d t = cupCOR - headAssemblyPoint;
	stemTrans.block<3, 1>(0, 3) = t;

	return stemTrans;
}

void Reduction::StemReduction(Cup::Pointer cup, Stem::Pointer stem,ELandMarks HeadAssemblyPoint)
{
	stem->SetIndexToWorldTransform(GetStemReductionTransform(cup, stem, HeadAssemblyPoint));
}

void Reduction::HeadReduction(Body::Pointer head, Stem::Pointer stem, ELandMarks HeadAssemblyPoint)
{
	head->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

  AxisType stemNeck;
	if (!stem->GetGlobalAxis(EAxes::stem_Neck, stemNeck))
	{
		MITK_WARN << "HeadReduction Failed:missing Axis stem_Neck";
		return;
	}
	Eigen::Vector3d headAssemblyPoint;
	if (!stem->GetGlobalLandMark(HeadAssemblyPoint, headAssemblyPoint))
	{
		MITK_WARN << "HeadReduction Failed:missing headAssemblyPoint";
		return;
	}
	//todo odd bug head set this IndexToWorldTransform (45 degree rot along y axis):
	//todo Eigen::Matrix3d rot = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0, 0, -1).normalized(), Eigen::Vector3d(1, 0, -1).normalized()).matrix();
	Eigen::Matrix3d rot = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0, 0.00001, -1).normalized(), stemNeck.direction).matrix();
	// MITK_WARN << "direction:";
	// MITK_WARN << stemNeck.direction;
	Eigen::Vector3d t = headAssemblyPoint;
	// MITK_WARN << "rot:";
	// MITK_WARN << rot;

	Eigen::Matrix4d headTrans;
	headTrans.setIdentity();
	headTrans.block<3, 3>(0, 0) = rot;
	headTrans.block<3, 1>(0, 3) = t;
	
	// MITK_WARN << "headTrans:";
	// MITK_WARN << headTrans;
	head->SetIndexToWorldTransform(headTrans);
}

void Reduction::PreOperativeReduction_Canal()
{
	if (m_Pelvis == nullptr|| m_Femur_op == nullptr || m_Femur_contra == nullptr)
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing bodies";
		return;
	}
	//correction and move to origin
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	
	FemurPreopReduction_Canal(m_Pelvis, m_Femur_contra);

	FemurPreopReduction_Canal(m_Pelvis, m_Femur_op);
}

void Reduction::PreOperativeReduction_Mechanical()
{
	if (m_Pelvis == nullptr || m_Femur_op == nullptr || m_Femur_contra == nullptr)
	{
		MITK_WARN << "PreOperativeReduction_Canal Failed: missing bodies";
		return;
	}
	// move to origin
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	// correction
	FemurPreopReduction_Mechanical(m_Pelvis, m_Femur_op);

	FemurPreopReduction_Mechanical(m_Pelvis, m_Femur_contra);
}

bool Reduction::GetCombinedOffset(Femur::Pointer femur,Pelvis::Pointer pelvis,double& offset)
{
	Eigen::Vector3d PFCA, DFCA, MidLine;
	if (!femur->GetGlobalLandMark(ELandMarks::f_PFCA, PFCA) ||
		!femur->GetGlobalLandMark(ELandMarks::f_DFCA, DFCA) ||
		!pelvis->GetGlobalLandMark(ELandMarks::p_MidLine, MidLine))
	{
		MITK_WARN << "Can't Cal Combined Offset: missing landmarks";
		return false;
	}

	offset = CombinedOffset(PFCA.data(), DFCA.data(), MidLine.data());
	return true;
}

bool Reduction::GetHipLength(Femur::Pointer femur, Pelvis::Pointer pelvis, double& hipLength)
{
	Eigen::Vector3d LT, ASIS_L, ASIS_R;
	if (!femur->GetGlobalLandMark(ELandMarks::f_LT, LT) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_ASIS_L, ASIS_L) ||
		!m_Pelvis->GetGlobalLandMark(ELandMarks::p_ASIS_R, ASIS_R))
	{
		MITK_WARN << "Can't Cal HipLength: missing landmarks";
		return false;
	}

	hipLength = HipLength(LT.data(), ASIS_L.data(), ASIS_R.data());
	return true;
}

void Reduction::CalPreopOffset()
{
	//cal combined offset
	//op
	double offset_op = 0;
	GetCombinedOffset(m_Femur_op, m_Pelvis, offset_op);
	//contra
	double offset_contra = 0;
	GetCombinedOffset(m_Femur_contra, m_Pelvis, offset_contra);

	SetResult(EResult::r_Offset_preop, offset_op);
	SetResult(EResult::r_Offset_contra, offset_contra);
	SetResult(EResult::r_OffsetDiff_preop_vs_contra, offset_op - offset_contra);
}

void Reduction::CalPreopHipLength()
{
	//cal hip length
	//op
	double hipLength_op = 0;
	GetHipLength(m_Femur_op, m_Pelvis, hipLength_op);

	//contra
	double hipLength_contra = 0;
	GetHipLength(m_Femur_contra, m_Pelvis, hipLength_contra);

	SetResult(EResult::r_HipLength_preop, hipLength_op);
	SetResult(EResult::r_HipLength_contra, hipLength_contra);
	SetResult(EResult::r_HipLengthDiff_preop_vs_contra, hipLength_op - hipLength_contra);
}

void Reduction::PlanReduction_Canal(Eigen::Matrix4d cupPlanMatrix,Eigen::Matrix4d stemPlanMatrix)
{
	if (m_Pelvis == nullptr || m_Femur_op == nullptr || m_Femur_contra == nullptr || m_Cup == nullptr || m_Stem == nullptr || m_Head == nullptr || m_Liner == nullptr)
	{
		MITK_WARN << "PlanReduction_Canal Failed: missing bodies";
		return;
	}

	//init pose
	m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Cup->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Stem->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());

	Eigen::Matrix4d pelvisTrans = Eigen::Matrix4d::Identity();
	m_Pelvis->SetIndexToWorldTransform(pelvisTrans);

	Eigen::Matrix4d cupTrans = pelvisTrans* cupPlanMatrix;
	m_Cup->SetIndexToWorldTransform(cupTrans);

	//m_Femur_op move to pelvis COR and canal correction
	FemurPreopReduction_Canal(m_Pelvis, m_Femur_op);
	//stem move to m_Femur_op
	Eigen::Matrix4d stemTrans = m_Femur_op->m_T_world_local * stemPlanMatrix;
	m_Stem->SetIndexToWorldTransform(stemTrans);

	//reduct stem and cup,append transform to stem and femur and head
	Eigen::Matrix4d stemReductionTransform = GetStemReductionTransform(m_Cup, m_Stem, ELandMarks::stem_HeadAssemblyPoint_M);
	// MITK_WARN << "stemReductionTransform";
	// MITK_WARN << stemReductionTransform;
	m_Stem->AppendTransform(stemReductionTransform);
	m_Femur_op->AppendTransform(stemReductionTransform);
	// //move stem center to cup center
	// StemReduction(m_Cup, m_Stem, ELandMarks::stem_HeadAssemblyPoint_M);

	// //femur_operation move to stem
	// Eigen::Matrix4d femurTrans_op = m_Stem->m_T_world_local * (stemPlanMatrix.inverse());
	// m_Femur_op->SetIndexToWorldTransform(femurTrans_op);
	//
	//liner and head
	m_Liner->SetIndexToWorldTransform(m_Cup->m_T_world_local);
	
	HeadReduction(m_Head,m_Stem, ELandMarks::stem_HeadAssemblyPoint_M);

	FemurPreopReduction_Canal(m_Pelvis, m_Femur_contra);
}

void Reduction::PlanReduction_Mechanical(Eigen::Matrix4d cupPlanMatrix, Eigen::Matrix4d stemPlanMatrix)
{
	if (m_Pelvis == nullptr || m_Femur_op == nullptr || m_Femur_contra == nullptr || m_Cup == nullptr || m_Stem == nullptr)
	{
		MITK_WARN << "PlanReduction_Canal Failed: missing bodies";
		return;
	}
	
	//init pose
	//m_Pelvis->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Cup->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	m_Stem->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	//m_Femur_L->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	//m_Femur_R->SetIndexToWorldTransform(Eigen::Matrix4d::Identity());
	
	Eigen::Matrix4d pelvisTrans = Eigen::Matrix4d::Identity();
	m_Pelvis->SetIndexToWorldTransform(pelvisTrans);
	
	Eigen::Matrix4d cupTrans = pelvisTrans * cupPlanMatrix;
	m_Cup->SetIndexToWorldTransform(cupTrans);

	//op
	//m_Femur_op move to pelvis COR and mechanical correction
	FemurPreopReduction_Mechanical(m_Pelvis, m_Femur_op);

	//stem move m_Femur_op
	Eigen::Matrix4d stemTrans = m_Femur_op->m_T_world_local * stemPlanMatrix;
	m_Stem->SetIndexToWorldTransform(stemTrans);

	//reduct stem and cup,append transform to stem and femur and head
	Eigen::Matrix4d stemReductionTransform = GetStemReductionTransform(m_Cup, m_Stem, ELandMarks::stem_HeadAssemblyPoint_M);
	// MITK_WARN << "stemReductionTransform";
	// MITK_WARN << stemReductionTransform;
	m_Stem->AppendTransform(stemReductionTransform);
	m_Femur_op->AppendTransform(stemReductionTransform);

	//liner and head
	m_Liner->SetIndexToWorldTransform(m_Cup->m_T_world_local);

	HeadReduction(m_Head, m_Stem, ELandMarks::stem_HeadAssemblyPoint_M);

	//contra
	//m_Femur_contra move to pelvis COR
	FemurPreopReduction_Mechanical(m_Pelvis, m_Femur_contra);
}

void Reduction::CalPlanedOffset()
{
	//cal combined offset
	//op
	double offset_plan = 0;
	GetCombinedOffset(m_Femur_op, m_Pelvis, offset_plan);
	//contra
	double offset_contra = 0;
	GetCombinedOffset(m_Femur_contra, m_Pelvis, offset_contra);

	SetResult(EResult::r_Offset_planed, offset_plan);
	SetResult(EResult::r_Offset_contra, offset_contra);
	SetResult(EResult::r_OffsetDiff_planed_vs_contra, offset_plan - offset_contra);

	double offset_preop;
	if (GetResult(EResult::r_Offset_preop, offset_preop))
	{
		SetResult(EResult::r_OffsetDiff_planed_vs_preop, offset_plan - offset_preop);
	}
	else
	{
		MITK_WARN << "OffsetDiff_planed_vs_preop can not be calculated: missing result r_Offset_preop";
	}
}

void Reduction::CalPlanedHipLength()
{
	//cal hip length
	//op planed
	double hipLength_plan = 0;
	GetHipLength(m_Femur_op, m_Pelvis, hipLength_plan);

	//contra
	double hipLength_contra = 0;
	GetHipLength(m_Femur_contra, m_Pelvis, hipLength_contra);

	SetResult(EResult::r_HipLength_planed, hipLength_plan);
	SetResult(EResult::r_HipLength_contra, hipLength_contra);
	SetResult(EResult::r_HipLengthDiff_planed_vs_contra, hipLength_plan - hipLength_contra);

	double hipLength_preop;
	if (GetResult(EResult::r_HipLength_preop, hipLength_preop))
	{
		SetResult(EResult::r_HipLengthDiff_planed_vs_preop, hipLength_plan - hipLength_preop);
	}
	else
	{
		MITK_WARN << "HipLengthDiff_planed_vs_preop can not be calculated: missing result r_HipLength_preop";
	}
}
