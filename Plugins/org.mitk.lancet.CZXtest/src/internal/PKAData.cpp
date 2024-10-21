#include "PKAData.h"
#include "PKAEnum.h"

using namespace lancetAlgorithm;
//PKASurgicalSide PKAData::mSurgicalSide = PKASurgicalSide::Left;

lancet::KukaRobotDevice::Pointer PKAData::m_KukaTrackingDevice;
mitk::TrackingDeviceSource::Pointer PKAData::m_VegaSource;
mitk::TrackingDeviceSource::Pointer PKAData::m_KukaSource;
mitk::NavigationToolStorage::Pointer PKAData::m_KukaToolStorage;
mitk::NavigationToolStorage::Pointer PKAData::m_VegaToolStorage;

Tilt PKAData::FemurTilt = Tilt::BackTilt;
Tilt PKAData::TibiaTilt = Tilt::BackTilt;
double PKAData::m_FemurTiltAngle = 0.0;
double PKAData::m_TibiaTiltAngle = 0.0;
Va PKAData::FemurVa = Va::Valgus;
Va PKAData::TibiaVa = Va::Valgus;
double PKAData::m_FemurVaAngle = 0.0;
double PKAData::m_TibiaVaAngle = 0.0;
ProsRotation PKAData::FemurProsRotation = ProsRotation::ExternalRotation;
ProsRotation PKAData::TibiaProsRotation = ProsRotation::ExternalRotation;
double PKAData::m_FemurProsRotationAngle = 0.0;
double PKAData::m_TibiaProsRotationAngle = 0.0;

PKASurgicalSide PKAData::m_SurgicalSide = PKASurgicalSide::Left;
MedioLateral PKAData::m_MedioLateral = MedioLateral::Medial;
SurgicalType PKAData::m_SurgicalType = SurgicalType::LL;
Tilt PKAData::m_LimbTilt = Tilt::FrontTilt;
Va PKAData::m_LimbVa = Va::Valgus;
double PKAData::m_LimbTiltAngle = 0.0;
double PKAData::m_LimbVaAngle = 0.0;

double PKAData::m_FemurProudDistance = 0.0;
double PKAData::m_TibiaProudDistance = 0.0;

QString PKAData::m_TibiaProsthesisProximalNodeName = "TibiaProsthesisNormal";

QString PKAData::m_TibiaProsthesisSymmetryNodeName = "TibiaProsthesisSymmetry";

QString PKAData::m_FemurClippedSurfaceNodeName = "femurSurface";//"FemurClippedSurface";

QString PKAData::m_FemurEpicondyleNodeName = "FemurEpicondyle";

QString PKAData::m_FemurPosteriorNodeName = "FemurPosterior";

QString PKAData::m_FemurDistalNodeName = "FemurDistal";

QString PKAData::m_HipCenterNodeName = "HipCenter";

QString PKAData::m_FemurKneeCenterNodeName = "FemurKneeCenter";

QString PKAData::m_AnteriorCutNodeName = "AnteriorCut";

QString PKAData::m_AnteriorChamferCutNodeName = "AnteriorChamferCut";

QString PKAData::m_DistalCutNodeName = "DistalCut";
QString PKAData::m_PosteriorChamferCutNodeName = "PosteriorChamferCut";

QString PKAData::m_PosteriorCutNodeName = "PosteriorCut";

QString PKAData::m_ChunliProsthesisRegistrationPointNodeName = "ChunliProsthesisRegistrationPoint";

QString PKAData::m_TibiaClippedSurfaceNodeName = "tibiaSurface";//"TibiaClippedSurface";

QString PKAData::m_PCLCenterNodeName = "PCLCenter";

QString PKAData::m_TibiaKneeCenterNodeName = "TibiaKneeCenter";

QString PKAData::m_TubercleMedialNodeName = "TubercleMedial";

QString PKAData::m_MalleolusCenterNodeName = "MalleolusCenter";

QString PKAData::m_TibiaProximalNodeName = "TibiaProximal";

QString PKAData::m_LateralFemuralEpicondyleNodeName = "LateralFemuralEpicondyle";

QString PKAData::m_MedialMalleolusNodeName = "MedialMalleolus";

QString PKAData::m_MedialFemuralEpicondyleNodeName = "MedialFemuralEpicondy";

QString PKAData::m_LateralMalleolusNodeName = "LateralMalleolus";

QString PKAData::m_RotationalLandmarkNodeName = "RotationalLandmark";

QString PKAData::m_MedialTibialEpicondyleNodeName = "MedialTibialEpicondyle";

QString PKAData::m_LateralTibialEpicondyleNodeName = "LateralTibialEpicondyle";

QString PKAData::m_TibiaTrayNodeName = "TibiaTray";
QString PKAData::m_TibiaInsertNodeName = "TibiaInsert";
QString PKAData::m_FemurImplantNodeName = "FemurImplant";

QString PKAData::m_TibiaRegistrationNodeName = "TibiaRegistrationPoints";
QString PKAData::m_FemurRegistrationNodeName = "FemurRegistrationPoints";
QString PKAData::m_TibiaVerifyPointNodeName = "TibiaVerifyPoint";
QString PKAData::m_FemurVerifyPointNodeName = "FemurVerifyPoint";

QString PKAData::m_FemurSurfaceNodeName = "FemurSurface";
QString PKAData::m_TibiaSurfaceNodeName = "TibiaSurface";
QString PKAData::m_TibiaRegistrationVerifyNodeName = "TibiaRegistrationVerifyNodeName";
QString PKAData::m_FemurRegistrationVerifyNodeName = "FemurRegistrationVerifyNodeName";

//data
Eigen::Vector3d PKAData::m_FemurMechanicalAxis;
Eigen::Vector3d PKAData::m_MalleolusCenter;
Eigen::Vector3d PKAData::m_TibiaMechanicalAxis;

Eigen::Vector3d PKAData::m_FemuralAcrocondylarAxis;
Eigen::Vector3d PKAData::m_TibialAcrocondylarAxis;

Eigen::Vector3d PKAData::m_LateralFemuralEpicondyle;

Eigen::Vector3d PKAData::m_LateralTibialEpicondyle;

Eigen::Vector3d PKAData::m_MedialFemuralEpicondyle;
Eigen::Vector3d PKAData::m_MedialTibialEpicondyle;

Eigen::Vector3d PKAData::m_FemuralKneeCenter;
Eigen::Vector3d PKAData::m_IntraHipCenterInFemurRF;
Eigen::Vector3d PKAData::m_HipCenter;

Eigen::Vector3d PKAData::m_TibiaKneeCenter;
Eigen::Vector3d PKAData::m_MedialMalleolus;
Eigen::Vector3d PKAData::m_LateralMalleolus;

Eigen::Vector3d PKAData::m_IntraMedialMalleolusInTibiaRF;
Eigen::Vector3d PKAData::m_IntraLateralMalleolusInTibiaRF;

vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2Probe = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2EndRF = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2TibiaRF = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TBaseRF2Base = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TFlange2EndRF = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TFemurRF2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TTibiaRF2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::G_FemurProsthesisMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::G_TibiaTrayMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
vtkSmartPointer<vtkMatrix4x4> PKAData::m_TCamera2BluntProbe = vtkSmartPointer<vtkMatrix4x4>::New();

Eigen::Vector3d PKAData::m_FemurVerifyPointInFemurRF;
Eigen::Vector3d PKAData::m_TibiaVerifyPointInTibiaRF;

bool PKAData::m_IsRegistrationTibia = false;
bool PKAData::m_IsRegistrationFemur = false;

QString PKAData::m_DrillEndTipNodeName = "DrillEndTip";