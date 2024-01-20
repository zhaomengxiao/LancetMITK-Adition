#ifndef PHYSIOCONST_H
#define PHYSIOCONST_H

enum class ESide
{
	right,
	left
};

inline const char* to_string(ESide e)
{
	switch (e)
	{
	case ESide::right: return "right";
	case ESide::left: return "left";
	default: return "unknown";
	}
}

enum class ELandMarks
{
	/** \brief [UserInput]��ǰ�ϼ�_�� Right ASIS(Anterior superior iliac spine) */
	p_RASI = 0,
	/** \brief [UserInput] Left ASIS(Anterior superior iliac spine) */
	p_LASI,
	/** \brief [UserInput] Pubic tubercle */
	p_PT,
	/** \brief [ModelOutput] the center of Anterior Pelvic Plane */
	p_APPCenter,
	/** \brief [ModelOutput] the Center of Right Acetabular */
	p_RAC,
	/** \brief [ModelOutput] the Center of Left Acetabular */
	p_LAC,

	/** \brief [UserInput] Greater Trochanter */
	f_GT,
	/** \brief [UserInput] Lesser Trochanter */
	f_LT,
	/** \brief [UserInput] Femur Head Center */
	f_FHC,
	/** \brief [UserInput] Proximal point of Femoral canal axis */
	f_PFCA,
	/** \brief [UserInput] Distal point of Femoral canal axis  */
	f_DFCA,
	/** \brief [UserInput] Medial Femoral Epicondyle */
	f_MFC,
	/** \brief [UserInput] Lateral Femoral Epicondyle  */
	f_LFC,
	/** \brief [UserInput] Proximal Checkpoint,The position before the operation   */
	f_CheckPointP_pre,
	/** \brief [UserInput] Distal Checkpoint,The position before the operation    */
	f_CheckPointD_pre,
	/** \brief [UserInput] Proximal Checkpoint,The position after the operation   */
	f_CheckPointP_post,
	/** \brief [UserInput] Distal Checkpoint,The position after the operation    */
	f_CheckPointD_post,
	/** \brief [UserInput] Femur Head Center During the operation, the surgeon will place four points on the cup to verify its placement.  */
	f_FHC_inOp
};

inline const char* to_string(ELandMarks e)
{
	switch (e)
	{
	case ELandMarks::p_RASI: return "p_RASI";
	case ELandMarks::p_LASI: return "p_LASI";
	case ELandMarks::p_PT: return "p_PT";
	case ELandMarks::p_APPCenter: return "p_APPCenter";
	case ELandMarks::p_RAC: return "p_RAC";
	case ELandMarks::p_LAC: return "p_LAC";
	case ELandMarks::f_GT: return "f_GT";
	case ELandMarks::f_LT: return "f_LT";
	case ELandMarks::f_FHC: return "f_FHC";
	case ELandMarks::f_PFCA: return "f_PFCA";
	case ELandMarks::f_DFCA: return "f_DFCA";
	case ELandMarks::f_MFC: return "f_MFC";
	case ELandMarks::f_LFC: return "f_LFC";
	case ELandMarks::f_CheckPointP_pre: return "f_CheckPointP_pre";
	case ELandMarks::f_CheckPointD_pre: return "f_CheckPointD_pre";
	case ELandMarks::f_CheckPointP_post: return "f_CheckPointP_post";
	case ELandMarks::f_CheckPointD_post: return "f_CheckPointD_post";
	case ELandMarks::f_FHC_inOp: return "f_FHC_inOp";
	default: return "unknown";
	}
}


enum class EAxes
{
	/** \brief [ModelOutput] Pelvic Horizontal Axis, The line connecting LASI and RASI */
	p_PHA =0,
	/** \brief [ModelOutput] Pelvic Longitudinal Axis, The line that passes through the midpoint of LASI & RASI, perpendicular to PHA */
	p_PLA,
	/** \brief [ModelOutput] Pelvic Sagittal Axis, perpendicular to PHA & PLA*/
	p_PSA,
	/** \brief [ModelOutput] Right Acetabular Axis*/
	p_RAA,
	/** \brief [ModelOutput] Left Acetabular Axis*/
	p_LAA,

	/** \brief [ModelOutput] the mechanical axis*/
	f_Mechanics,
	/** \brief [ModelOutput] the canal axis*/
	f_Canal,
};

inline const char* to_string(EAxes e)
{
	switch (e)
	{
	case EAxes::p_PHA: return "p_PHA";
	case EAxes::p_PLA: return "p_PLA";
	case EAxes::p_PSA: return "p_PSA";
	case EAxes::p_RAA: return "p_RAA";
	case EAxes::p_LAA: return "p_LAA";
	case EAxes::f_Mechanics: return "f_Mechanics";
	case EAxes::f_Canal: return "f_Canal";
	default: return "unknown";
	}
}

enum class EPlanes
{
	MIDPLANE=0,
};

inline const char* to_string(EPlanes e)
{
	switch (e)
	{
	case EPlanes::MIDPLANE: return "MIDPLANE";
	default: return "unknown";
	}
}


enum class EResult
{
	/** \brief [ModelOutput] ����ǰ���*/
	f_PT = 0,
	/** \brief [ModelOutput] ��˫��ɹǻ�е����룬�ɹ�ƫ�ľ�������״��ƽ�е�ǰ���£���Сת�ӵ���ǰ�ϼ����ߣ���������ᣩ�ľ���*/
	f_HipLength ,
	/** \brief [ModelOutput] �ڹɹǸɴ�ֱ���ɹ�ƫ�ľ�������״��ƽ�У��ɹǸ��ᵽ����������ľ���*/
	f_Offset,

	f_HipLength_post,
	f_Offset_post,
	/** \brief [ModelOutput] Difference(mm) in hip length between the operative and contralateral sides before operation*/
	f_HipLengthDiff_preOp2Contral,
	/** \brief [ModelOutput] Difference(mm) in hip length between the operative and contralateral sides after operation*/
	f_HipLengthDiff_Op2Contralateral,
	/** \brief [ModelOutput] Difference(mm) in hip length before and after operation */
	f_HipLengthDiff_PrePostOp,
	/** \brief[ModelOutput] Difference(mm) in Combined Offset between the operative and contralateral sides before operation */
	f_OffsetDiff_preOp2Contral,
	/** \brief[ModelOutput] Difference(mm) in Combined Offset between the operative and contralateral sides after operation*/
	f_OffsetDiff_Op2Contralateral,
	/** \brief[ModelOutput] Difference(mm) in Combined Offset before and after operation */
	f_OffsetDiff_PrePostOp,
	/** \brief [ModelOutput] ����ɹ����*/
	f_OpVersion,

	/** \brief [ModelOutput] �Ҳ�ʱ�ǰ���*/
	p_RightAnteversion,
	/** \brief [ModelOutput] �Ҳ�ʱ���չ��*/
	p_RightInclination,
	/** \brief [ModelOutput] ���ʱ�ǰ���*/
	p_LeftAnteversion,
	/** \brief [ModelOutput] ���ʱ���չ��*/
	p_LeftInclination
};

inline const char* to_string(EResult e)
{
	switch (e)
	{
	case EResult::f_PT: return "f_PT";
	case EResult::f_HipLength: return "f_HipLength";
	case EResult::f_Offset: return "f_Offset";
	case EResult::f_HipLength_post: return "f_HipLength_post";
	case EResult::f_Offset_post: return "f_Offset_post";
	case EResult::f_HipLengthDiff_preOp2Contral: return "f_HipLengthDiff_preOp2Contral";
	case EResult::f_HipLengthDiff_Op2Contralateral: return "f_HipLengthDiff_Op2Contralateral";
	case EResult::f_HipLengthDiff_PrePostOp: return "f_HipLengthDiff_PrePostOp";
	case EResult::f_OffsetDiff_preOp2Contral: return "f_OffsetDiff_preOp2Contral";
	case EResult::f_OffsetDiff_Op2Contralateral: return "f_OffsetDiff_Op2Contralateral";
	case EResult::f_OffsetDiff_PrePostOp: return "f_OffsetDiff_PrePostOp";
	case EResult::f_OpVersion: return "f_OpVersion";
	case EResult::p_RightAnteversion: return "p_RightAnteversion";
	case EResult::p_RightInclination: return "p_RightInclination";
	case EResult::p_LeftAnteversion: return "p_LeftAnteversion";
	case EResult::p_LeftInclination: return "p_LeftInclination";
	default: return "unknown";
	}


}

//TKA Parameters
enum class TKASide
{
	right,
	left
};

inline const char* to_string(TKASide t)
{
	switch (t)
	{
	case TKASide::right: return "right";
	case TKASide::left: return "left";
	default: return "unknown";
	}
}

enum class TKALandmarks
{
	/** \brief [UserInput] �ɹ������� femur medial epicondyle*/
	f_ME = 0,
	/** \brief [UserInput] �ɹ������� femur lateral epicondyle*/
	f_LE,
	/** \brief [UserInput] �ɹ��ڲ���� femur posterior medial*/
	f_PM,
	/** \brief [UserInput] �ɹ�������  femur posterior lateral*/
	f_PL,
	/** \brief [UserInput] �ɹ��ڲ�Զ��ƽ���(�ֶ�����) femur medial distal point*/
	f_MDP,
	/** \brief [UserInput] �ɹ����Զ��ƽ���(�ֶ�����) femur lateral distal point*/
	f_LDP,
	/** \brief [UserInput] �ɹ��ڲ���ƽ���(�ֶ�����) femur posterior medial point*/
	f_MPP,
	/** \brief [UserInput] �ɹ������ƽ���(�ֶ�����) femur posterior lateral point*/
	f_LPP,
	/** \brief [UserInput] �Źؽ�����  hip center*/
	HIP_CENTER,
	/** \brief [UserInput] �ɹ�ϥ�ؽ�����  femur knee center*/
	fKNEE_CENTER,
	/** \brief [UserInput] �󽻲��ʹ�����	 posterior cruciate ligament center*/
	PCL_CENTER,
	/** \brief [UserInput] �ֹ�ϥ�ؽ�����  tibia knee center*/
	tKNEE_CENTER,
	/** \brief [UserInput] �ֹ��׹ؽ�����  tibia ankle joint center*/
	tANKLE_CENTER,
	/** \brief [UserInput] �ֹǽ���ڲ�1/3��	 Medial 1/3 of the tubercle*/
	TUBERCLE,
	/** \brief [UserInput] �ֹ�ƽ̨�ڲ� tibia proximal medial*/
	t_PM,
	/** \brief [UserInput] �ֹ�ƽ̨��� tibia proximal lateral*/
	t_PL,
	/** \brief [UserInput] �ɹǼ���ǰ�˷������ femur implant anterior cut start*/
	fi_ANTERIORSTART,
	/** \brief [UserInput] �ɹǼ���ǰ�˷����յ� femur implant anterior cut end*/
	fi_ANTERIOREND,
	/** \brief [UserInput] �ɹǼ���ǰ���Ƿ������ femur implant anterior chamfer cut start*/
	fi_ANTERIORCHAMSTART,
	/** \brief [UserInput] �ɹǼ���ǰ���Ƿ����յ� femur implant anterior chamfer cut end*/
	fi_ANTERIORCHAMEND,
	/** \brief [UserInput] �ɹǼ���Զ�˷������ femur implant distal cut start*/
	fi_DISTALSTART,
	/** \brief [UserInput] �ɹǼ���Զ�˷����յ� femur implant distal cut end*/
	fi_DISTALEND,
	/** \brief [UserInput] �ɹǼ���󵹽Ƿ������ femur implant posterior chamfer cut start*/
	fi_POSTERIORCHAMSTART,
	/** \brief [UserInput] �ɹǼ���󵹽Ƿ����յ� femur implant posterior chamfer cut end*/
	fi_POSTERIORCHAMEND,
	/** \brief [UserInput] �ɹǼ����˷������ femur implant posterior cut start*/
	fi_POSTERIORSTART,
	/** \brief [UserInput] �ɹǼ����˷����յ� femur implant posterior cut end*/
	fi_POSTERIOREND,
	/** \brief [UserInput] �ɹǼ���ع����ڲ�� femur implant resection line medial*/
	fi_ResectionMedial,
	/** \brief [UserInput] �ɹǼ���ع������� femur implant resection line lateral*/
	fi_ResectionLateral,
	/** \brief [UserInput] �ֹǼ�����˷������ tibia implant proximal cut start*/
	ti_PROXIMALSTART,
	/** \brief [UserInput] �ֹǼ�����˷����յ� tibia implant proximal cut end*/
	ti_PROXIMALEND,
	/** \brief [UserInput] �ֹǼ���Գ������ tibia implant symmetry axis start*/
	ti_SYMMETRYSTART,
	/** \brief [UserInput] �ֹǼ���Գ����յ� tibia implant symmetry axis end*/
	ti_SYMMETRYEND,
	/** \brief [ModelOutput] ��ֱλ�ɹǽع�ƽ���ڲ��(���ڼ���ؽڼ�϶)*/
	fi_FLEXIONMEDIAL,
	/** \brief [ModelOutput] ��ֱλ�ɹǽع�ƽ������(���ڼ���ؽڼ�϶)*/
	fi_FLEXIONLATERAL,
	/** \brief [ModelOutput] ����λ�ɹǽع�ƽ���ڲ��(���ڼ���ؽڼ�϶)*/
	fi_EXTENSIONMEDIAL,
	/** \brief [ModelOutput] ����λ�ɹǽع�ƽ������(���ڼ���ؽڼ�϶)*/
	fi_EXTENSIONLATERAL,
	/** \brief [ModelOutput] ��ֱλ�ֹǽع�ƽ���ڲ��(���ڼ���ؽڼ�϶)*/
	ti_FLEXIONMEDIAL,
	/** \brief [ModelOutput] ��ֱλ�ֹǽع�ƽ������(���ڼ���ؽڼ�϶)*/
	ti_FLEXIONLATERAL,
	/** \brief [ModelOutput] ����λ�ֹǽع�ƽ���ڲ��(���ڼ���ؽڼ�϶)*/
	ti_EXTENSIONMEDIAL,
	/** \brief [ModelOutput] ����λ�ֹǽع�ƽ������(���ڼ���ؽڼ�϶)*/
	ti_EXTENSIONLATERAL,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���1*/
	//fi_DISTALMEDIAL_1,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���2*/
	//fi_DISTALMEDIAL_2,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���3*/
	//fi_DISTALMEDIAL_3,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���4*/
	//fi_DISTALMEDIAL_4,
	///** \brief [UserInput] �ɹǼ���Զ�����ƽ���1*/
	//fi_DISTALLATERAL_1,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���2*/
	//fi_DISTALLATERAL_2,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���3*/
	//fi_DISTALLATERAL_3,
	///** \brief [UserInput] �ɹǼ���Զ���ڲ�ƽ���4*/
	//fi_DISTALLATERAL_4,

};

inline const char* to_string(TKALandmarks t)
{
	switch (t)
	{
	case TKALandmarks::f_ME: return "f_ME";
	case TKALandmarks::f_LE: return "f_LE";
	case TKALandmarks::f_PM: return "f_PM";
	case TKALandmarks::f_PL: return "f_PL";
	case TKALandmarks::f_MDP: return "f_MDP";
	case TKALandmarks::f_LDP: return "f_LDP";
	case TKALandmarks::f_MPP: return "f_MPP";
	case TKALandmarks::f_LPP: return "f_LPP";
	case TKALandmarks::HIP_CENTER: return "HIP_CENTER";
	case TKALandmarks::PCL_CENTER: return "PCL_CENTER";
	case TKALandmarks::fKNEE_CENTER: return "fKNEE_CENTER";
	case TKALandmarks::tKNEE_CENTER: return "tKNEE_CENTER";
	case TKALandmarks::tANKLE_CENTER: return "tANKLE_CENTER";
	case TKALandmarks::TUBERCLE: return "TUBERCLE";
	case TKALandmarks::t_PM: return "t_PM";
	case TKALandmarks::t_PL: return "t_PL";
	case TKALandmarks::fi_ANTERIORSTART: return "fi_ANTERIORSTART";
	case TKALandmarks::fi_ANTERIOREND: return "fi_ANTERIOREND";
	case TKALandmarks::fi_ANTERIORCHAMSTART: return "fi_ANTERIORCHAMSTART";
	case TKALandmarks::fi_ANTERIORCHAMEND: return "fi_ANTERIORCHAMEND";
	case TKALandmarks::fi_DISTALSTART: return "fi_DISTALSTART";
	case TKALandmarks::fi_DISTALEND: return "fi_DISTALEND";
	case TKALandmarks::fi_POSTERIORCHAMSTART: return "fi_POSTERIORCHAMSTART";
	case TKALandmarks::fi_POSTERIORCHAMEND: return "fi_POSTERIORCHAMEND";
	case TKALandmarks::fi_POSTERIORSTART: return "fi_POSTERIORSTART";
	case TKALandmarks::fi_POSTERIOREND: return "fi_POSTERIOREND";
	case TKALandmarks::fi_ResectionMedial: return "fi_ResectionMedial";
	case TKALandmarks::fi_ResectionLateral: return "fi_ResectionLateral";
	case TKALandmarks::ti_PROXIMALSTART: return "ti_PROXIMALSTART";
	case TKALandmarks::ti_PROXIMALEND: return "ti_PROXIMALEND";
	case TKALandmarks::ti_SYMMETRYSTART: return "ti_SYMMETRYSTART";
	case TKALandmarks::ti_SYMMETRYEND: return "ti_SYMMETRYEND";
	case TKALandmarks::fi_FLEXIONMEDIAL: return "fi_FLEXIONMEDIAL";
	case TKALandmarks::fi_FLEXIONLATERAL: return "fi_FLEXIONLATERAL";
	case TKALandmarks::fi_EXTENSIONMEDIAL: return "fi_EXTENSIONMEDIAL";
	case TKALandmarks::fi_EXTENSIONLATERAL: return "fi_EXTENSIONLATERAL";
	case TKALandmarks::ti_FLEXIONMEDIAL: return "ti_FLEXIONMEDIAL";
	case TKALandmarks::ti_FLEXIONLATERAL: return "ti_FLEXIONLATERAL";
	case TKALandmarks::ti_EXTENSIONMEDIAL: return "ti_EXTENSIONMEDIAL";
	case TKALandmarks::ti_EXTENSIONLATERAL: return "ti_EXTENSIONLATERAL";
	//case TKALandmarks::fi_DISTALMEDIAL_1: return "fi_DISTALMEDIAL_1";
	//case TKALandmarks::fi_DISTALMEDIAL_2: return "fi_DISTALMEDIAL_2";
	//case TKALandmarks::fi_DISTALMEDIAL_3: return "fi_DISTALMEDIAL_3";
	//case TKALandmarks::fi_DISTALMEDIAL_4: return "fi_DISTALMEDIAL_4";
	//case TKALandmarks::fi_DISTALLATERAL_1: return "fi_DISTALLATERAL_1";
	//case TKALandmarks::fi_DISTALLATERAL_2: return "fi_DISTALLATERAL_2";
	//case TKALandmarks::fi_DISTALLATERAL_3: return "fi_DISTALLATERAL_3";
	//case TKALandmarks::fi_DISTALLATERAL_4: return "fi_DISTALLATERAL_4";
	default: return "unknown";
	}
}

enum class TKAAxes
{
	/** \brief [UserInput] �ɹǽ����� femur anatomical axis*/
	f_AA = 0,
	/** \brief [UserInput] �ɹǻ�е�� mechanical axis of femur*/
	f_MA,
	/** \brief [UserInput] �ɹ��������� femur TE axis*/
	f_TEA,
	/** \brief [UserInput] �ɹǺ����� femur PC axis*/
	f_PCA,
	/** \brief [UserInput] �ֹǻ�е�� mechanical axis of tibia*/
	t_MA,
	/** \brief [UserInput] �ֹ�ǰ���� tibia AP axis*/
	t_APA,
	/** \brief [UserInput] �ɹǼ������� femur implant axis*/
	fi_A,
	/** \brief [UserInput] �ɹǼ���ع��� femur implant bone resection line*/
	fi_BRL,
	/** \brief [UserInput] �ֹǼ������� tibia implant axis*/
	ti_A,
	/** \brief [UserInput] �ֹǼ���ƽ̨�Գ����� tibia implant symmetry axis*/
	ti_SA,
	/** \brief [UserInput] �ɹǹ�״�淨�� ʸ��˺� femur coronal plane normal*/
	f_coronal,
	/** \brief [UserInput] �ɹǺ���淨�� ���ϵ��� femur transverse plane normal*/
	f_transverse,
	/** \brief [UserInput] �ɹ�ʸ״�淨�� ���⵽�� femur sagittal plane normal */
	f_sagittal,
	/** \brief [UserInput] �ֹǹ�״�淨�� �Ӻ���ǰtibia coronal plane normal*/
	t_coronal,
	/** \brief [UserInput] �ֹǺ���淨�� ���ϵ��� tibia transverse plane normal*/
	t_transverse,
	/** \brief [UserInput] �ֹ�ʸ״�淨�� �ڲ�˺� tibia sagittal plane normal*/
	t_sagittal,
};

inline const char* to_string(TKAAxes t)
{
	switch (t)
	{
	case TKAAxes::f_AA: return "f_AA";
	case TKAAxes::f_MA: return "f_MA";
	case TKAAxes::f_TEA: return "f_TEA";
	case TKAAxes::f_PCA: return "f_PCA";
	case TKAAxes::t_MA: return "t_MA";
	case TKAAxes::t_APA: return "t_APA";
	case TKAAxes::fi_A: return "fi_A";
	case TKAAxes::fi_BRL: return "fi_BRL";
	case TKAAxes::ti_A: return "ti_A";
	case TKAAxes::ti_SA: return "ti_PSA";
	case TKAAxes::f_coronal: return "f_coronal";
	case TKAAxes::f_transverse: return "f_transverse";
	case TKAAxes::f_sagittal: return "f_sagittal";
	case TKAAxes::t_coronal: return "t_coronal";
	case TKAAxes::t_transverse: return "t_transverse";
	case TKAAxes::t_sagittal: return "t_sagittal";
	default: return "unknown";
	}
}

enum class TKAPlanes
{
	/** \brief [UserInput] �ɹ�ǰ�˽ع��� femur anterior planar cut*/
	FEMURANTERIOR,
	/** \brief [UserInput] �ɹ�ǰ���ǽع��� femur anterior chamfer planar cut*/
	FEMURANTERIORCHAM,
	/** \brief [UserInput] �ɹ�Զ�˽ع��� femur distal planar cut*/
	FEMURDISTAL,
	/** \brief [UserInput] �ɹǺ󵹽ǽع��� femur posterior planar cut*/
	FEMURPOSTERIORCHAM,
	/** \brief [UserInput] �ɹǺ�˽ع��� femur posterior planar cut*/
	FEMURPOSTERIOR,
	/** \brief [UserInput] �ֹǽ��˽ع��� tibia proximal planar cut*/
	TIBIAPROXIMAL,
};

inline const char* to_string(TKAPlanes t)
{
	switch (t)
	{
	case TKAPlanes::FEMURANTERIOR: return "FEMURANTERIOR";
	case TKAPlanes::FEMURANTERIORCHAM: return "FEMURANTERIORCHAM";
	case TKAPlanes::FEMURDISTAL: return "FEMURDISTAL";
	case TKAPlanes::FEMURPOSTERIORCHAM: return "FEMURPOSTERIORCHAM";
	case TKAPlanes::FEMURPOSTERIOR: return "FEMURPOSTERIOR";
	case TKAPlanes::TIBIAPROXIMAL: return "TIBIAPROXIMAL";
	default: return "unknown";
	}
}

enum class TKAResult
{
	/** \brief [ModelOutput] �ɹ��ڷ��� �ɹǼ���������ɹǻ�е��ļн�*/
	f_Varus,
	/** \brief [ModelOutput] �ɹ������� �ɹǼ���ع�����ɹǺ�����ļн�*/
	f_TEAExternal,
	/** \brief [ModelOutput] �ɹ������� �ɹǼ���ع�����ɹ���������ļн�*/
	f_PCAExternal,
	/** \brief [ModelOutput] �ɹ������� �ɹǼ���������ɹǻ�е��ļн�*/
	f_flexion,
	/** \brief [ModelOutput] �ɹ�Զ���ڲ�ع���� medial resection Depth of the distal femur*/
	f_Distal_MRD,
	/** \brief [ModelOutput] �ɹ�Զ�����ع���� lateral resection Depth of the distal femur*/
	f_Distal_LRD,
	/** \brief [ModelOutput] �ɹǺ���ڲ�ع���� medial resection Depth of the posterior femur*/
	f_Posterior_MRD,
	/** \brief [ModelOutput] �ɹǺ�����ع���� lateral resection Depth of the posterior femur*/
	f_Posterior_LRD,
	/** \brief [ModelOutput] �ֹ��ڷ��� �ֹǽع��ߵĳ���������֫��е��ļн�*/
	t_Varus,
	/** \brief [ModelOutput] �ֹ������ �ֹǼ����������ֹǻ�е��ļн�*/
	t_External,
	/** \brief [ModelOutput] �ֹǺ���� �ֹǼ����������ֹǻ�е��ļн�*/
	t_PSlope,
	/** \brief [ModelOutput] �ֹǽ����ڲ�ع���� medial resection depth of the proximal tibia*/
	t_Proximal_MRD,
	/** \brief [ModelOutput] �ֹǽ������ع���� lateral resection depth of the proximal tibia*/
	t_Proximal_LRD,
	/** \brief [ModelOutput] ��ֱλ�ؽ��ڲ��϶*/
	ExtensionMedialGap,
	/** \brief [ModelOutput] ��ֱλ�ؽ�����϶*/
	ExtensionLateralGap,
	/** \brief [ModelOutput] ����λ�ؽ��ڲ��϶*/
	flexionMedialGap,
	/** \brief [ModelOutput] ����λ�ؽ�����϶*/
	flexionLateralGap,
	/** \brief [ModelOutput] ��֫������*/
	Limb_Flexion,
	/** \brief [ModelOutput] ��֫���ⷭ*/
	Limb_Varus,
	/** \brief [ModelOutput] �滮���ⷭ*/
	Planned_Varus,
	/** \brief [ModelOutput] ��ֱ��϶*/
	Extension,
	/** \brief [ModelOutput] ������϶*/
	Flexion,
};

inline const char* to_string(TKAResult t)
{
	switch (t)
	{
	case TKAResult::f_Varus: return "f_Varus";
	case TKAResult::f_TEAExternal: return "f_External";
	case TKAResult::f_PCAExternal: return "f_Internal";
	case TKAResult::f_flexion: return "f_flexion";
	case TKAResult::f_Distal_MRD: return "f_Distal_MRD";
	case TKAResult::f_Distal_LRD: return "f_Distal_LRD";
	case TKAResult::f_Posterior_MRD: return "f_Posterior_MRD";
	case TKAResult::f_Posterior_LRD: return "f_Posterior_LRD";
	case TKAResult::t_Varus: return "t_Varus";
	case TKAResult::t_External: return "t_External";
	case TKAResult::t_PSlope: return "t_PSlope";
	case TKAResult::t_Proximal_MRD: return "t_Proximal_MRD";
	case TKAResult::t_Proximal_LRD: return "t_Proximal_LRD";
	case TKAResult::ExtensionMedialGap: return "ExtensionMedialGap";
	case TKAResult::ExtensionLateralGap: return "ExtensionLateralGap";
	case TKAResult::flexionMedialGap: return "flexionMedialGap";
	case TKAResult::flexionLateralGap: return "flexionLateralGap";
	case TKAResult::Limb_Flexion: return "Limb_Flexion";
	case TKAResult::Limb_Varus: return "Limb_Varus";
	case TKAResult::Planned_Varus: return "Planned_Varus";
	case TKAResult::Extension: return "Extension";
	case TKAResult::Flexion: return "Flexion";
	default: return "unknown";
	}
}

#endif
