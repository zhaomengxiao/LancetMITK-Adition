#ifndef PKADATA_H
#define PKADATA_H

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <QString>
#include <eigen3/Eigen/Dense>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <mitkTrackingDeviceSource.h>
#include <lancetVegaTrackingDevice.h>
#include <kukaRobotDevice.h>
#include "PKAEnum.h"

namespace lancetAlgorithm
{
	class PKAData
	{
	public:
		static lancet::KukaRobotDevice::Pointer m_KukaTrackingDevice;
		static mitk::TrackingDeviceSource::Pointer m_VegaSource;
		static mitk::TrackingDeviceSource::Pointer m_KukaSource;
		static mitk::NavigationToolStorage::Pointer m_KukaToolStorage;
		static mitk::NavigationToolStorage::Pointer m_VegaToolStorage;

		static Tilt FemurTilt;
		static Tilt TibiaTilt;
		static double m_FemurTiltAngle;
		static double m_TibiaTiltAngle;
		static Va FemurVa;
		static Va TibiaVa;
		static double m_FemurVaAngle;
		static double m_TibiaVaAngle;
		static ProsRotation FemurProsRotation;
		static ProsRotation TibiaProsRotation;
		static double m_FemurProsRotationAngle;
		static double m_TibiaProsRotationAngle;
		static double m_FemurProudDistance;
		static double m_TibiaProudDistance;
		static PKASurgicalSide m_SurgicalSide;
		static MedioLateral m_MedioLateral;
		static SurgicalType m_SurgicalType;
		static Tilt m_LimbTilt;
		static Va m_LimbVa;
		static double m_LimbTiltAngle;
		static double m_LimbVaAngle;

		/*static vtkSmartPointer<vtkMatrix4x4> m_TCamera2BluntProbe;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera1FitPlane;*/

		static vtkSmartPointer<vtkMatrix4x4> m_FemurRegistrationMatrix;
		static vtkSmartPointer<vtkMatrix4x4> m_TibiaRegistrationMatrix;

		//髋关节中心
		static Eigen::Vector3d m_HipCenter;
		static Eigen::Vector3d m_IntraHipCenterInFemurRF;
		static QString m_HipCenterNodeName;
		static QString m_IntraHipCenterNodeName;

		//股骨上髁
		static Eigen::Vector3d m_MedialFemuralEpicondyle;
		static QString m_MedialFemuralEpicondyleNodeName;
		static Eigen::Vector3d m_IntraMedialFemoralEpicondyle;

		static Eigen::Vector3d m_LateralFemuralEpicondyle;
		static QString m_LateralFemuralEpicondyleNodeName;
		static Eigen::Vector3d m_IntraLateralFemuralEpicondyle;

		static Eigen::Vector3d m_MedialTibialEpicondyle;
		static QString m_MedialTibialEpicondyleNodeName;

		static Eigen::Vector3d m_LateralTibialEpicondyle;
		static QString m_LateralTibialEpicondyleNodeName;

		static Eigen::Vector3d m_FemuralKneeCenter;

		static Eigen::Vector3d m_TibiaKneeCenter;
		static QString m_TibiaKneeCenterNodeName;

		static Eigen::Vector3d m_RotationalLandmark;
		static QString m_RotationalLandmarkNodeName;

		static Eigen::Vector3d m_TibiaMechanicalAxis;
		static Eigen::Vector3d m_IntraTibiaMechanicalAxis;
		static Eigen::Vector3d m_FemurMechanicalAxis;
		static Eigen::Vector3d m_IntraFemurMechanicalAxis;

		static Eigen::Vector3d m_FemuralAcrocondylarAxis;
		static Eigen::Vector3d m_TibialAcrocondylarAxis;

		//探针模型节点
		static QString m_ProbeNodeName;
		static QString m_DrillNodeName;

		/******************************************************************/
		//股骨表面
		static QString m_FemurSurfaceNodeName;

		//股骨切割后的表面
		static QString m_FemurClippedSurfaceNodeName;

		//股骨配准点
		static QString m_FemurRegistrationNodeName;

		//股骨验证点
		static QString m_FemurVerifyPointNodeName;
		static Eigen::Vector3d m_FemurVerifyPointInFemurRF;
		static QString m_FemurRegistrationVerifyNodeName;

		//股骨假体 
		static QString m_FemurImplantNodeName;
		static vtkSmartPointer<vtkMatrix4x4> G_FemurProsthesisMatrix;

		//股骨上髁
		static QString m_FemurEpicondyleNodeName;

		//股骨后髁
		static QString m_FemurPosteriorNodeName;

		//股骨远端点
		static QString m_FemurDistalNodeName;

		//股骨端点
		static QString m_FemurProximalNodeName;

		//股骨膝关节中心
		static QString m_FemurKneeCenterNodeName;

		/********************************* Cut Line ******************************/

		//胫骨表面
		static QString m_TibiaSurfaceNodeName;

		//胫骨切割后的表面
		static QString m_TibiaClippedSurfaceNodeName;

		//胫骨配准点
		static QString m_TibiaRegistrationNodeName;

		//胫骨验证点
		static QString m_TibiaVerifyPointNodeName;
		static Eigen::Vector3d m_TibiaVerifyPointInCamera;
		static Eigen::Vector3d m_TibiaVerifyPointInTibiaRF;
		static QString m_TibiaRegistrationVerifyNodeName;

		//胫骨假体 
		static QString m_TibiaProsthesisNodeName;
		static vtkSmartPointer<vtkMatrix4x4> G_TibiaTrayMatrix;

		//托盘
		static QString m_TibiaTrayNodeName;

		//胫骨 垫片
		static QString m_TibiaInsertNodeName;

		//胫骨假体平台法线
		static QString m_TibiaProsthesisProximalNodeName;

		//胫骨假体对称轴
		static QString m_TibiaProsthesisSymmetryNodeName;

		//胫骨近端点
		static QString m_TibiaProximalNodeName;

		/********************/

		//前切平面
		static QString m_AnteriorCutNodeName;

		// 前斜切平面（前倒角切平面）
		static QString m_AnteriorChamferCutNodeName;

		//远端切平面
		static QString m_DistalCutNodeName;

		//后斜切平面（后倒角切平面）
		static QString m_PosteriorChamferCutNodeName;

		//股骨下切平面法线
		static QString m_PostHoleMedialNodeName;

		//后切平面
		static QString m_PosteriorCutNodeName;

		//后十字韧带(PCL)中心
		static QString m_PCLCenterNodeName;

		//胫骨结节内侧1/3处
		static QString m_TubercleMedialNodeName;

		//踝关节中心
		static Eigen::Vector3d m_MedialMalleolus;
		static QString m_MedialMalleolusNodeName;

		static Eigen::Vector3d m_LateralMalleolus;
		static QString m_LateralMalleolusNodeName;

		static Eigen::Vector3d m_MalleolusCenter;
		static QString m_MalleolusCenterNodeName;

		static Eigen::Vector3d m_IntraMedialMalleolusInTibiaRF;
		static QString m_IntraMedialMalleolusNodeName;

		static Eigen::Vector3d m_IntraLateralMalleolusInTibiaRF;
		static QString m_IntraLateralMalleolusNodeName;

		/**********************************/
		//春立假体设计的用于配准的14个点
		static QString m_ChunliProsthesisRegistrationPointNodeName;

		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2Probe;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2EndRF;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2FemurRF;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2TibiaRF;
		static vtkSmartPointer<vtkMatrix4x4> m_TBaseRF2Base;
		static vtkSmartPointer<vtkMatrix4x4> m_TFlange2EndRF;
		static vtkSmartPointer<vtkMatrix4x4> m_TFemurRF2FemurImage;
		static vtkSmartPointer<vtkMatrix4x4> m_TTibiaRF2TibiaImage;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2BaseRF;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2Drill;
		static vtkSmartPointer<vtkMatrix4x4> m_TCamera2BluntProbe;

		static bool m_IsRegistrationTibia;
		static bool m_IsRegistrationFemur;

		static QString m_DrillEndTipNodeName;
	};

	enum class PrePointCaptureType
	{
		HipCenter,
		MedialFemuralEpicondyle,
		LateralFemuralEpicondyle,
		FemurKneeCenter,
		MedialMalleolus,
		LateralMalleolus,
		RotationalLandmark,
		TibiaKneeCenter,
		MedialTibialEpicondyle,
		LateralTibialEpicondyle
	};

	inline std::string to_string(PrePointCaptureType t)
	{
		switch (t)
		{
		case PrePointCaptureType::HipCenter: return PKAData::m_HipCenterNodeName.toStdString();
		case PrePointCaptureType::MedialFemuralEpicondyle: return PKAData::m_MedialFemuralEpicondyleNodeName.toStdString();
		case PrePointCaptureType::LateralFemuralEpicondyle: return PKAData::m_LateralFemuralEpicondyleNodeName.toStdString();
		case PrePointCaptureType::FemurKneeCenter: return PKAData::m_FemurKneeCenterNodeName.toStdString();
		case PrePointCaptureType::MedialMalleolus: return PKAData::m_MedialMalleolusNodeName.toStdString();
		case PrePointCaptureType::LateralMalleolus: return PKAData::m_LateralMalleolusNodeName.toStdString();
		case PrePointCaptureType::RotationalLandmark: return PKAData::m_RotationalLandmarkNodeName.toStdString();
		case PrePointCaptureType::TibiaKneeCenter: return PKAData::m_TibiaKneeCenterNodeName.toStdString();
		case PrePointCaptureType::MedialTibialEpicondyle: return PKAData::m_MedialTibialEpicondyleNodeName.toStdString();
		case PrePointCaptureType::LateralTibialEpicondyle: return PKAData::m_LateralTibialEpicondyleNodeName.toStdString();
		default: return "unknown";
		}
	}
}
#endif // ! PKADATA