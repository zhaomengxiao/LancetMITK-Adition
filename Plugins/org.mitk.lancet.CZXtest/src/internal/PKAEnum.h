#pragma once
#include <string>
#include "PKAData.h"
#ifndef PKAENUM
#define PKAENUM

namespace lancetAlgorithm
{
	enum class KneeModel
	{
		Femur,        //�ɹ�
		Tibia         //�ֹ�
	};

	enum class SurgicalType
	{
		LM,
		RM,
		LL,
		RL
	};
	enum class MedioLateral
	{
		Medial,
		Lateral
	};
	enum TCPRegistraionMethod
	{
		FourPoints,
		ThreePoints
	};
	enum class MalleolusPoint
	{
		MalleolusMedialis,  //����
		LateralMalleolus    //����
	};

	enum class ViewType
	{
		Axial,
		Sagittal,
		Coronal
	};

	inline const char* to_string(ViewType type)
	{
		switch (type)
		{
		case ViewType::Axial: return "Axial";
		case ViewType::Sagittal: return "Sagittal";
		case ViewType::Coronal: return "Coronal";
		default: return "unknown";
		}
	}

	enum class Direction
	{
		Up,
		Down,
		Left,
		Right
	};

	enum class Tilt
	{
		FrontTilt,
		BackTilt
	};

	inline std::string to_string(Tilt t)
	{
		switch (t)
		{
		case Tilt::FrontTilt:
			return "FrontTilt";
		case Tilt::BackTilt:
			return "BackTilt";
		default:
			return "unknow";
		}
	}

	enum class Va
	{
		Valgus, //�ⷭ
		Varus   //�ڷ�
	};

	inline std::string to_string(Va va)
	{
		switch (va)
		{
		case Va::Valgus:
			return "Valgus";
		case Va::Varus:
			return "Varus";
		default:
			return "unknow";
		}
	}

	enum class ProsRotation
	{
		IntenalRotation,
		ExternalRotation
	};

	inline std::string to_string(ProsRotation rotate)
	{
		switch (rotate)
		{
		case ProsRotation::IntenalRotation:
			return "Internal";
		case ProsRotation::ExternalRotation:
			return "External";
		default:
			return "unknow";
		}
	}

	enum class PKASurgicalSide
	{
		Left,
		Right
	};

	inline const char* to_string(PKASurgicalSide e)
	{
		switch (e)
		{
		case PKASurgicalSide::Right: return "right";
		case PKASurgicalSide::Left: return "left";
		default: return "unknown";
		}
	}

	enum class PKALandmarks
	{
		/** \brief [UserInput] �ɹ������� femur medial epicondyle*/
		FemurMedialEpicondyle = 0,
		/** \brief [UserInput] �ɹ������� femur lateral epicondyle*/
		FemurLateralEpicondyle,
		/** \brief [UserInput] �ɹ��ڲ���� femur posterior medial*/
		FemurMedialPosterior,
		/** \brief [UserInput] �ɹ�������  femur posterior lateral*/
		FemurLateralPosterior,
		/** \brief [UserInput] �ɹ��ڲ�Զ��ƽ���(�ֶ�����) femur medial distal point*/
		f_MDP,
		/** \brief [UserInput] �ɹ����Զ��ƽ���(�ֶ�����) femur lateral distal point*/
		f_LDP,
		/** \brief [UserInput] �ɹ��ڲ���ƽ���(�ֶ�����) femur posterior medial point*/
		f_MPP,
		/** \brief [UserInput] �ɹ������ƽ���(�ֶ�����) femur posterior lateral point*/
		f_LPP,
		/** \brief [UserInput] �Źؽ�����  hip center*/
		HipCenter,
		/** \brief [UserInput] �ɹ�ϥ�ؽ�����  femur knee center*/
		FemurKneeCenter,
		/** \brief [UserInput] �󽻲��ʹ�����	 posterior cruciate ligament center*/
		PCL_CENTER,
		/** \brief [UserInput] �ֹ�ϥ�ؽ�����  tibia knee center*/
		TibiaKneeCenter,
		/** \brief [UserInput] �ֹ��׹ؽ�����  tibia ankle joint center*/
		TibiaAngleJointCenter,
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

	inline const char* to_string(PKALandmarks t)
	{
		switch (t)
		{
		case PKALandmarks::FemurMedialEpicondyle: return "f_ME";
		case PKALandmarks::FemurLateralEpicondyle: return "f_LE";
		case PKALandmarks::FemurMedialPosterior: return "f_PM";
		case PKALandmarks::FemurLateralPosterior: return "f_PL";
		case PKALandmarks::f_MDP: return "f_MDP";
		case PKALandmarks::f_LDP: return "f_LDP";
		case PKALandmarks::f_MPP: return "f_MPP";
		case PKALandmarks::f_LPP: return "f_LPP";
		case PKALandmarks::HipCenter: return "HIP_CENTER";
		case PKALandmarks::PCL_CENTER: return "PCL_CENTER";
		case PKALandmarks::FemurKneeCenter: return "fKNEE_CENTER";
		case PKALandmarks::TibiaKneeCenter: return "tKNEE_CENTER";
		case PKALandmarks::TibiaAngleJointCenter: return "tANKLE_CENTER";
		case PKALandmarks::TUBERCLE: return "TUBERCLE";
		case PKALandmarks::t_PM: return "t_PM";
		case PKALandmarks::t_PL: return "t_PL";
		case PKALandmarks::fi_ANTERIORSTART: return "fi_ANTERIORSTART";
		case PKALandmarks::fi_ANTERIOREND: return "fi_ANTERIOREND";
		case PKALandmarks::fi_ANTERIORCHAMSTART: return "fi_ANTERIORCHAMSTART";
		case PKALandmarks::fi_ANTERIORCHAMEND: return "fi_ANTERIORCHAMEND";
		case PKALandmarks::fi_DISTALSTART: return "fi_DISTALSTART";
		case PKALandmarks::fi_DISTALEND: return "fi_DISTALEND";
		case PKALandmarks::fi_POSTERIORCHAMSTART: return "fi_POSTERIORCHAMSTART";
		case PKALandmarks::fi_POSTERIORCHAMEND: return "fi_POSTERIORCHAMEND";
		case PKALandmarks::fi_POSTERIORSTART: return "fi_POSTERIORSTART";
		case PKALandmarks::fi_POSTERIOREND: return "fi_POSTERIOREND";
		case PKALandmarks::fi_ResectionMedial: return "fi_ResectionMedial";
		case PKALandmarks::fi_ResectionLateral: return "fi_ResectionLateral";
		case PKALandmarks::ti_PROXIMALSTART: return "ti_PROXIMALSTART";
		case PKALandmarks::ti_PROXIMALEND: return "ti_PROXIMALEND";
		case PKALandmarks::ti_SYMMETRYSTART: return "ti_SYMMETRYSTART";
		case PKALandmarks::ti_SYMMETRYEND: return "ti_SYMMETRYEND";
		case PKALandmarks::fi_FLEXIONMEDIAL: return "fi_FLEXIONMEDIAL";
		case PKALandmarks::fi_FLEXIONLATERAL: return "fi_FLEXIONLATERAL";
		case PKALandmarks::fi_EXTENSIONMEDIAL: return "fi_EXTENSIONMEDIAL";
		case PKALandmarks::fi_EXTENSIONLATERAL: return "fi_EXTENSIONLATERAL";
		case PKALandmarks::ti_FLEXIONMEDIAL: return "ti_FLEXIONMEDIAL";
		case PKALandmarks::ti_FLEXIONLATERAL: return "ti_FLEXIONLATERAL";
		case PKALandmarks::ti_EXTENSIONMEDIAL: return "ti_EXTENSIONMEDIAL";
		case PKALandmarks::ti_EXTENSIONLATERAL: return "ti_EXTENSIONLATERAL";
		default: return "unknown";
		}
	}

	enum class PKAAxes
	{
		/** \brief [UserInput] �ɹǽ����� femur anatomical axis*/
		femurAnatomicalAxis = 0,
		/** \brief [UserInput] �ɹǻ�е�� mechanical axis of femur*/
		FemurMechanicalAxis,
		/** \brief [UserInput] �ɹ��������� femur TE axis*/
		f_TEA,
		/** \brief [UserInput] �ɹǺ����� femur PC axis*/
		FemurPosteriorCondyleAxis,
		/** \brief [UserInput] �ֹǻ�е�� mechanical axis of tibia*/
		tibiaMechanicalAxis,
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
		FemurCoronalNormal,
		/** \brief [UserInput] �ɹǺ���淨�� ���ϵ��� femur transverse plane normal*/
		FemurTransverseNormal,
		/** \brief [UserInput] �ɹ�ʸ״�淨�� ���⵽�� femur sagittal plane normal */
		femurSagittalNormal,
		/** \brief [UserInput] �ֹǹ�״�淨�� �Ӻ���ǰtibia coronal plane normal*/
		TibiaCoronalNormal,
		/** \brief [UserInput] �ֹǺ���淨�� ���ϵ��� tibia transverse plane normal*/
		TibiaTransverseNormal,
		/** \brief [UserInput] �ֹ�ʸ״�淨�� �ڲ�˺� tibia sagittal plane normal*/
		TibiaSagittalNormal,
	};

	inline const char* to_string(PKAAxes t)
	{
		switch (t)
		{
		case PKAAxes::femurAnatomicalAxis: return "f_AA";
		case PKAAxes::FemurMechanicalAxis: return "f_MA";
		case PKAAxes::f_TEA: return "f_TEA";
		case PKAAxes::FemurPosteriorCondyleAxis: return "f_PCA";
		case PKAAxes::tibiaMechanicalAxis: return "t_MA";
		case PKAAxes::t_APA: return "t_APA";
		case PKAAxes::fi_A: return "fi_A";
		case PKAAxes::fi_BRL: return "fi_BRL";
		case PKAAxes::ti_A: return "ti_A";
		case PKAAxes::ti_SA: return "ti_PSA";
		case PKAAxes::FemurCoronalNormal: return "f_coronal";
		case PKAAxes::FemurTransverseNormal: return "f_transverse";
		case PKAAxes::femurSagittalNormal: return "f_sagittal";
		case PKAAxes::TibiaCoronalNormal: return "t_coronal";
		case PKAAxes::TibiaTransverseNormal: return "t_transverse";
		case PKAAxes::TibiaSagittalNormal: return "t_sagittal";
		default: return "unknown";
		}
	}

	enum class PKAPlanes
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

	inline const char* to_string(PKAPlanes t)
	{
		switch (t)
		{
		case PKAPlanes::FEMURANTERIOR: return "FEMURANTERIOR";
		case PKAPlanes::FEMURANTERIORCHAM: return "FEMURANTERIORCHAM";
		case PKAPlanes::FEMURDISTAL: return "FEMURDISTAL";
		case PKAPlanes::FEMURPOSTERIORCHAM: return "FEMURPOSTERIORCHAM";
		case PKAPlanes::FEMURPOSTERIOR: return "FEMURPOSTERIOR";
		case PKAPlanes::TIBIAPROXIMAL: return "TIBIAPROXIMAL";
		default: return "unknown";
		}
	}

	enum class PKAResult
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

	inline const char* to_string(PKAResult t)
	{
		switch (t)
		{
		case PKAResult::f_Varus: return "f_Varus";
		case PKAResult::f_TEAExternal: return "f_External";
		case PKAResult::f_PCAExternal: return "f_Internal";
		case PKAResult::f_flexion: return "f_flexion";
		case PKAResult::f_Distal_MRD: return "f_Distal_MRD";
		case PKAResult::f_Distal_LRD: return "f_Distal_LRD";
		case PKAResult::f_Posterior_MRD: return "f_Posterior_MRD";
		case PKAResult::f_Posterior_LRD: return "f_Posterior_LRD";
		case PKAResult::t_Varus: return "t_Varus";
		case PKAResult::t_External: return "t_External";
		case PKAResult::t_PSlope: return "t_PSlope";
		case PKAResult::t_Proximal_MRD: return "t_Proximal_MRD";
		case PKAResult::t_Proximal_LRD: return "t_Proximal_LRD";
		case PKAResult::ExtensionMedialGap: return "ExtensionMedialGap";
		case PKAResult::ExtensionLateralGap: return "ExtensionLateralGap";
		case PKAResult::flexionMedialGap: return "flexionMedialGap";
		case PKAResult::flexionLateralGap: return "flexionLateralGap";
		case PKAResult::Limb_Flexion: return "Limb_Flexion";
		case PKAResult::Limb_Varus: return "Limb_Varus";
		case PKAResult::Planned_Varus: return "Planned_Varus";
		case PKAResult::Extension: return "Extension";
		case PKAResult::Flexion: return "Flexion";
		default: return "unknown";
		}
	}

	enum class PKAMarker
	{
		PKARobotBaseRF,
		PKARobotEndRF,
		PKAFemurRF, 
		PKAProbe,
		PKATibiaRF,
		PKADrill,
		PKABluntProbe
	};

	inline const char* to_string(PKAMarker p)
	{
		switch (p)
		{
		case lancetAlgorithm::PKAMarker::PKARobotBaseRF:
			return "PKARobotBaseRF";
		case lancetAlgorithm::PKAMarker::PKARobotEndRF:
			return "PKARobotEndRF";
		case lancetAlgorithm::PKAMarker::PKAFemurRF:
			return "PKAFemurRF";
		case lancetAlgorithm::PKAMarker::PKAProbe:
			return "PKAProbe";
		case lancetAlgorithm::PKAMarker::PKATibiaRF:
			return "PKATibiaRF";
		case lancetAlgorithm::PKAMarker::PKADrill:
			return "PKADrill";
		case lancetAlgorithm::PKAMarker::PKABluntProbe:
			return "PKABluntProbe";
		default:
			return "unknow";
		}
	}
	
	inline std::string to_path(PKAMarker p)
	{
		std::string path = std::string(getenv("USERPROFILE")) + "\\Desktop\\PKAModelData\\";
		switch (p)
		{
		case lancetAlgorithm::PKAMarker::PKARobotBaseRF:
			return "unknow";
		case lancetAlgorithm::PKAMarker::PKARobotEndRF:
			return "unknow";
		case lancetAlgorithm::PKAMarker::PKAFemurRF:
			return "unknow";
		case lancetAlgorithm::PKAMarker::PKAProbe:
			return path + to_string(p) + ".stl";
		case lancetAlgorithm::PKAMarker::PKATibiaRF:
			return "unknow";
		case lancetAlgorithm::PKAMarker::PKADrill:
			return path + to_string(p)+".stl";
		case lancetAlgorithm::PKAMarker::PKABluntProbe:
			return path + to_string(p) + ".stl";
		default:
			return "unknow";
		}
	}
}
#endif // !PKAENUM
