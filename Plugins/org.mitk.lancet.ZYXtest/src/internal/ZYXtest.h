/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef ZYXtest_h
#define ZYXtest_h

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>
#include "ui_ZYXtestControls.h"
#include "AimPositionDef.h"
#include "AimPositionAPI.h"
#include <HR_Pro.h>
#include "mitkPointSet.h"
#include "mitkTrackingDeviceSource.h"
#include <QmitkPointListModel.h>
#include <mitkPointSetDataInteractor.h>
#include <mitkImageToItk.h>
#include <mitkITKImageImport.h>
#include <mitkDataNode.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>
#include <QKeyEvent>

#include "PrintDataHelper.h"


#include "robotRegistration.h"



/**
  \brief ZYXtest

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class ZYXtest : public QmitkAbstractView
{

	Q_OBJECT
public slots:
	void connectCamera();
	void openCameraQtTimer();
	void getToolInfor();
	void getMatrix();
	void updateCameraData_SPI();//同时把全部的都拿出来
	void updateCameraData_Oral();//同时把全部的都拿出来
	T_AimToolDataResult* ZYXtest::GetNewToolData();
	void UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const char* Name, double* aCamera2Tool, QLabel* label);
	void updataServo();
	void CrashDetechion();
	void updataCrashDetechion();
	void ReadCurrent();
	void stopCrashDetection();

	//随动函数
	//QTimer m_ServoPTimer;

public:
	static const std::string VIEW_ID;

	//init
	void Demo_clean();
	void InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget);
	void InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget);
	//void PrintMatrix(std::string matrixName, double* matrix);

	//table Start-up  and check
	void connectArm();
	void powerOn();
	void powerOff();
	void getBaseToFlangeMatrix();
	void SuddenStop();
	void ScramRecovery();
	double GenerateRandomNumber(double lowerBound, double upperBound, int decimalPoints);





	//table2 Robot registration
	void setTCPToFlange();
	void setInitialPoint();
	void goToInitial();
	void replaceRegistration();
	void saveArmMatrix();
	void reuseArmMatrix();
	void captureRobot();
	void CapturePose(bool translationOnly);
	void CombineRotationTranslation(float Rto[3][3], float Tto[3], vtkMatrix4x4* resultMatrix);
	void PrintToolMatrix();
	void JiontAngleMotion();
	void AutoMoveJ();



	//Robot move
	void xp();
	void yp();
	void zp();
	void xm();
	void ym();
	void zm();
	void rxp();
	void ryp();
	void rzp();
	void rxm();
	void rym();
	void rzm();
	void PrintBaseToFlanger();


	//table3 Start-up  and check
	void collectLandmark();
	void collectICP();
	void landmarkRegistration();
	vtkSmartPointer<vtkMatrix4x4> ComputeHomogeneousTransform_vtk(mitk::PointSet::Pointer points_set1, mitk::PointSet::Pointer points_set2);
	void ICPRegistration();
	void UpdateDeviation();
	void UpdateProbe();
	void UpdateTCP();
	void addICP();
	void ResetImageConfiguration();
	void saveImageMatrix();
	void reuseImageMatrix();

	//table4 Planning bit selection
	bool SetPrecisionTestTcp();
	bool InterpretImageLine();
	void OnAutoPositionStart();
	void getProofPoint();
	void getProofPointagain();




	//table5 
	void setTCP();
	void trackingProbe();
	void trackingProbe1();
	void collectLandmark_SPI();
	void collectICP_SPI();
	void landmarkRegistration_SPI();
	void ICPRegistration_SPI();
	void ResetImageConfiguration_SPI();
	void saveImageMatrix_SPI();
	void saveMetalPoint();
	void addICP_SPI();
	void UpdateDeviation_SPI();
	void Updatedata_SPI();
	void UpdateTCP_SPI();
	void ImportPointSet();
	std::vector<Eigen::Vector3d> ReadTextFileAsPoints(std::string path);




	//table6
	void test_1();
	void test_2();
	void test_3();
	void keyPressEvent();



	void readA();
	void readB();
	void readC();
	void readD();
	void readE();
	void readF();
	void readG();
	void readH();
	void readJ();

	void gotoA();
	void gotoB();
	void gotoC();
	void gotoD();
	void gotoE();
	void gotoF();
	void gotoG();
	void gotoH();
	void gotoJ();

	void AutoRegistration();




	//table7
	void OpenFreeDrag();
	void TurnOffFreeDrag();
	void openToolMotion();
	void closeToolMotion();
	void PauseMotion();
	void KeepMoving();
	void setTestTCP();
	void readTCP();
	void xp1();
	void yp1();
	void zp1();
	void xm1();
	void ym1();
	void zm1();
	void rxp1();
	void ryp1();
	void rzp1();
	void rxm1();
	void rym1();
	void rzm1();
	void SetSpeedrRatio();
	void SetLoad();
	void registrationPoint();
	void goBackPoint();
	void Move_lhy();
	void Move_zyx();
	void Move_lhyinit();
	void EffectiveSpace();
	void logPoint_1();
	void StartServoControl();
	void ComputeAndSendServoCommand();
	void StopServoControl();
	void ComputeNewRobot();



	//table
	void readControllerConnection();
	void readControllerStart();
	void readControllerVersionNumber();
	void readRobotModel();
	void readElectricalBoxInformation();
	void DisplayRobotFrameBtnClicked();
	std::vector<std::vector<double>> GetRobotAnglesBtnClicked();
	void MoveRobotByJointAnglesBtnClicked(std::vector<std::vector<double>> JointAngles);


	//table guideRegistation
	void GetProbeEndPosBtnClicked(int type);
	void ResetProbeEndPosBtnClicked(int type);
	void GetGuiderOriginPosBtnClicked();
	void SetGuiderOriginPosBtnClicked();

	vtkSmartPointer<vtkMatrix4x4> CalculateTFlange2Camera(double* TF2ENDRF, double* TCamera2EndRF);
	vtkSmartPointer<vtkMatrix4x4> GetArray2vtkMatrix(double* array16);
	Eigen::Vector3d CalculatePointInFlangePos(vtkMatrix4x4* matrixFlange2Camera, Eigen::Vector3d posInCamera);
	void CalculateEulerByProbeEndPosClicked();
	void CalculateGuiderTCP2BtnClicked();


	void PrintMatrix(std::string matrixName, double* matrix);
	void Print_Matrix(const std::string& title, vtkMatrix4x4* matrix);
	void PrintTCP_qt(QTextBrowser* browser, double x, double y, double z, double rx, double ry, double rz);
	void PrintTCP_Terminal(double x, double y, double z, double rx, double ry, double rz);
	void PrintResult(QTextBrowser* browser, int nRet, const char* message);
	void PrintArray16ToMatrix(const std::string& title, double* Array);
	void AppendTextBrowerMatrix(QString matrixName, vtkMatrix4x4* matrix);
	void AppendTextBrowerArray(const char* text, std::vector<double> array);
	void AppendTextBrowerArray(const char* text, double* array, int size);
	void AppendTextBrowerArray(const char* text, Eigen::Vector3d array);
	bool IsRobotMoving();
	bool IsRobotArrived();
	double GetRegisrationRMS(mitk::PointSet* points, mitk::Surface* surface, vtkMatrix4x4* matrix);
	vtkSmartPointer<vtkPoints> TransformVTKPoints(vtkPoints* in, vtkMatrix4x4* m);

	void MoveFinished();
	void ReadSaveFileData();

	int ProbEndCountOne = 0;
	int ProbEndCountTwo = 0;
	QFutureWatcher<void> RobotMoveTaskWatcher;
	std::vector<Eigen::Vector3d> probeEndOneVector;
	std::vector<Eigen::Vector3d> probeEndTwoVector;
	std::vector<std::vector<double>> robotJointAngles;
	QTimer* m_AimoeVisualizeTimer{ nullptr };

	//口腔机器人相关功能函数
	//提取金属球
	void on_pushButton_steelballExtract_clicked();
	void UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters);
	double GetPointDistance(const mitk::Point3D p0, const mitk::Point3D p1);
	bool GetCoarseSteelballCenters(double steelballVoxel);
	void IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs);
	void RemoveRedundantCenters();
	void ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs);
	void RearrangeSteelballs(int stdNeighborNum, std::vector<int>& foundIDs);
	void UpdateDrillVisual();
	void TurnOffAllNodesVisibility();

	//图像配准
	void on_pushButton_collectDitch_clicked();//采集患者验证点
	void ResetImageConfiguration_Oral();//清空配准点
	void on_pushButton_imageRegisNew_clicked();//图像配准
	void UpdateTCPAndProbe();//QT开线程在影像空间中刷探针和TCP
	void stopUpdateTCPAndProbe();//关闭QT的刷TCP和探针的数据
	void ConvertVTKPointsToMITKPointSet(vtkSmartPointer<vtkPoints> vtkPoints, mitk::PointSet::Pointer mitkPointSet);
	void SortPointSetByDistance(mitk::PointSet::Pointer inputPointSet, mitk::PointSet::Pointer outputPointSet);
	mitk::Point3D CalculateMassCenter(const mitk::PointSet::Pointer& pointSet);
	std::vector<std::vector<int>> GenerateAllCombinations(int m, int n);
	void GenerateCombinations(int m, int n, int index, std::vector<int>& currentCombination, std::vector<std::vector<int>>& result);
	void on_pushButton_startNavi_clicked();

	// 保存
	void saveRegisMatrix();
	void reuseRegisMatrix();
	void saveTCPCalibrateMatrix();
	void reuseTCPCalibrateMatrix();

	//TCP标定
	void on_pushButton_calibrateDrill_clicked();//使用TCPRF进行配准
	void ZYXtest::setOralTCP();//根据输入框完成TCP的手动输入
	void on_pushButton_calibrateDrill_Robot_clicked();

	//机械臂定位执行
	bool InterpretImageLine_Oral();

	//机械臂力控
	void selectDirectionForceControl();//选择力控的方向
	void openForceControl();//开启力控
	void closeForceControl();//关闭力控


	//口腔机器人变量
	vtkNew<vtkMatrix4x4> m_T_CBCTGeometry_init;
	vtkNew<vtkMatrix4x4> m_T_DrillGeometry_init;
	mitk::PointSet::Pointer m_steelBalls_cmm = mitk::PointSet::New();//提取金属球后的点cmm是球心的意思
	mitk::Surface::Pointer m_splinterSurface = mitk::Surface::New();
	mitk::PointSet::Pointer m_probeDitchPset_cmm; // 硬件工装的探针点
	std::vector<double> stdCenters;
	int realballnumber{ 0 };
	int edgenumber{ 0 };
	std::vector<double> allBallFingerPrint;
	bool m_Stat_patientRFtoImage{ false };//反应配准的是否完成


	//初始化机械配准的类
	RobotRegistration m_RobotRegistration;
	mitk::DataNode* m_IcpSourceSurface{ nullptr };

	///////////////////////////////////机械臂控制变量///////////////////////////////////

	// 空间坐标点
	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	// 关节坐标点
	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;
	// 工具坐标点
	double dTcp_X = 0; double dTcp_Y = 0; double dTcp_Z = 0;
	double dTcp_Rx = 0; double dTcp_Ry = 0; double dTcp_Rz = 0;
	// 用户坐标点
	double dUcs_X = 0; double dUcs_Y = 0; double dUcs_Z = 0;
	double dUcs_Rx = 0; double dUcs_Ry = 0; double dUcs_Rz = 0;
	// 初始的函数
	double g_init_X = 0; double g_init_Y = 0; double g_init_Z = 0;
	double g_init_Rx = 0; double g_init_Ry = 0; double g_init_Rz = 0;
	double dJ1_init = 0; double dJ2_init = 0; double dJ3_init = 0;
	double dJ4_init = 0; double dJ5_init = 0; double dJ6_init = 0;
	//记录点1
	double g_X = 0; double g_Y = 0; double g_Z = 0;
	double g_Rx = 0; double g_Ry = 0; double g_Rz = 0;
	double dJ1_1 = 0; double dJ2_1 = 0; double dJ3_1 = 0;
	double dJ4_1 = 0; double dJ5_1 = 0; double dJ6_1 = 0;

	//记录点A
	double g_X_A = 0; double g_Y_A = 0; double g_Z_A = 0;
	double g_Rx_A = 0; double g_Ry_A = 0; double g_Rz_A = 0;
	double dJ1_A = 0; double dJ2_A = 0; double dJ3_A = 0;
	double dJ4_A = 0; double dJ5_A = 0; double dJ6_A = 0;

	//记录点B
	double g_X_B = 0; double g_Y_B = 0; double g_Z_B = 0;
	double g_Rx_B = 0; double g_Ry_B = 0; double g_Rz_B = 0;
	double dJ1_B = 0; double dJ2_B = 0; double dJ3_B = 0;
	double dJ4_B = 0; double dJ5_B = 0; double dJ6_B = 0;

	//记录点C
	double g_X_C = 0; double g_Y_C = 0; double g_Z_C = 0;
	double g_Rx_C = 0; double g_Ry_C = 0; double g_Rz_C = 0;
	double dJ1_C = 0; double dJ2_C = 0; double dJ3_C = 0;
	double dJ4_C = 0; double dJ5_C = 0; double dJ6_C = 0;

	//记录点D
	double g_X_D = 0; double g_Y_D = 0; double g_Z_D = 0;
	double g_Rx_D = 0; double g_Ry_D = 0; double g_Rz_D = 0;
	double dJ1_D = 0; double dJ2_D = 0; double dJ3_D = 0;
	double dJ4_D = 0; double dJ5_D = 0; double dJ6_D = 0;

	//记录点E
	double g_X_E = 0; double g_Y_E = 0; double g_Z_E = 0;
	double g_Rx_E = 0; double g_Ry_E = 0; double g_Rz_E = 0;
	double dJ1_E = 0; double dJ2_E = 0; double dJ3_E = 0;
	double dJ4_E = 0; double dJ5_E = 0; double dJ6_E = 0;

	//记录点F
	double g_X_F = 0; double g_Y_F = 0; double g_Z_F = 0;
	double g_Rx_F = 0; double g_Ry_F = 0; double g_Rz_F = 0;
	double dJ1_F = 0; double dJ2_F = 0; double dJ3_F = 0;
	double dJ4_F = 0; double dJ5_F = 0; double dJ6_F = 0;

	//记录点G
	double g_X_G = 0; double g_Y_G = 0; double g_Z_G = 0;
	double g_Rx_G = 0; double g_Ry_G = 0; double g_Rz_G = 0;
	double dJ1_G = 0; double dJ2_G = 0; double dJ3_G = 0;
	double dJ4_G = 0; double dJ5_G = 0; double dJ6_G = 0;

	//记录点H
	double g_X_H = 0; double g_Y_H = 0; double g_Z_H = 0;
	double g_Rx_H = 0; double g_Ry_H = 0; double g_Rz_H = 0;
	double dJ1_H = 0; double dJ2_H = 0; double dJ3_H = 0;
	double dJ4_H = 0; double dJ5_H = 0; double dJ6_H = 0;

	//记录点J
	double g_X_J = 0; double g_Y_J = 0; double g_Z_J = 0;
	double g_Rx_J = 0; double g_Ry_J = 0; double g_Rz_J = 0;
	double dJ1_J = 0; double dJ2_J = 0; double dJ3_J = 0;
	double dJ4_J = 0; double dJ5_J = 0; double dJ6_J = 0;

	int nMoveType = 0;
	string sTcpName = "TCP_1";
	string sUcsName = "Base";
	double dVelocity = 10;
	double dAcc = 12;
	double dRadius = 50;
	int nIsUseJoint = 1;
	int nIsSeek = 0;
	int nIOBit = 0;
	int nIOState = 0;
	string strCmdID = "0";
	std::vector<double> dCoord = { dX, dY, dZ, dRx, dRy, dRz };

	///////////////////////////////////导航数据模块///////////////////////////////////
	//机械臂配准数据
	double T_BaseToBaseRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_FlangeToEdnRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_FlangeToTCP[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_calibratorRFToTCP[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_calibratorRFToInputTCP[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_EndRFToCalibratorRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_EndRFToInputTCP[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	// 误差偏差和导航模式
	int m_NaviMode{ 0 }; // 0 is drilling mode; 1 is implantation mode
	double m_AngleError{ 0 };
	double m_DrillTipTotalError{ 0 };
	double m_DrillTipVertiError{ 0 };
	double m_DrillTipHoriError{ 0 };

	double m_EntryTotalError{ 0 };
	double m_EntryVertiError{ 0 };
	double m_EntryHoriError{ 0 };

	double m_ApexTotalError{ 0 };
	double m_ApexVertiError{ 0 };
	double m_ApexHoriError{ 0 };


	//用采集数据的时候转VTK矩阵使用
	float R_tran[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_tran[3] = { 0.0f, 0.0f, 0.0f };

	//相机到机械臂末端Mark
	float R_CamToEndRF[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToEndRF[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToEndRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	//相机到机械基座Mark
	float R_CamToBaseRF[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToBaseRF[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToBaseRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	//相机到探针的Mark
	float R_CamToProbe[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToProbe[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToProbe[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double ProbeTop[4] = { 0.0f, 0.0f, 0.0f, 1.0f };//在相机坐标系下的探针尖端

	double nd_tip_FpatientRF[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	double nd_tip_FImage_icp[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	double ProofPointInit[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	double ProofPoint[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	double T_imageToProbe[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };


	//相机到病人Mark
	float R_CamToPatientRF[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToPatientRF[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToPatientRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	//从病人mark到探针
	double T_PatientRFToProbe[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	//从病人mark到影像空间image
	double T_PatientRFtoImage[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_ImageToImage_icp[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };


	//相机到金属球Mark
	float R_CamToMetalBallRF[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToMetalBallRF[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToMetalBallRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	//相机到TCP
	float R_CamToTCPRF[3][3] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	float t_CamToTCPRF[3] = { 0.0f, 0.0f, 0.0f };
	double T_CamToCalibratorRF[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double finaltcp[6];
	//金属球mark到影像空间image
	double T_CameraToImage[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	double T_ImageToImage_icp_SPI[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_ImageToMetalRF[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_PatientRFtoImage_SPI[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };


	//随动功能
	double T_CameraToProbe1[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_CameraToProbe2[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_Probe1ToProbe2[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_BaseToFlange1[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	double T_CamToBase[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	std::vector<double> dTargetPoint = { 0,0,0,0,0,0 };


	//机械臂执行
	double T_BaseToTarget[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	// 图像到TCP
	double T_ImageToInputTCP[16]{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	//采集的点集
	vtkSmartPointer<vtkPoints> vtkProbeTip_onObjRf = vtkSmartPointer<vtkPoints>::New();//landmark的点集
	vtkSmartPointer<vtkPoints> vtkProbeTip_onObjRf_icp = vtkSmartPointer<vtkPoints>::New();//icp的点集

	vtkSmartPointer<vtkPoints> vtkProbeTip_onObjRf_SPI = vtkSmartPointer<vtkPoints>::New();//landmark的点集(脊柱模型)
	vtkSmartPointer<vtkPoints> vtkProbeTip_onObjRf_icp_SPI = vtkSmartPointer<vtkPoints>::New();//icp的点集(脊柱模型)

	vtkSmartPointer<vtkPoints> vtkProbeTip_Oral = vtkSmartPointer<vtkPoints>::New();//landmark的点集（口腔）
	mitk::PointSet::Pointer m_probeDitchPset_rf; // 探针在PatientRF坐标系下的点集合vtkProbeTip_Oral一样格式不一样
	vtkSmartPointer<vtkPoints> vtkProbeTip_Oral_icp = vtkSmartPointer<vtkPoints>::New();//icp的点集（口腔）

	bool ComparePointsByDistance(const mitk::Point3D& massCenter, const mitk::Point3D& point1, const mitk::Point3D& point2);
	double CalculateDistance(const mitk::Point3D& point1, const mitk::Point3D& point2);
	int callCount = 0; // 添加一个静态变量来记录函数调用次数



protected:
	virtual void CreateQtPartControl(QWidget* parent) override;
	unsigned int m_IndexOfRobotCapture{ 0 };
	unsigned int m_IndexOfRobotCurrents{ 0 };
	unsigned int m_IndexOfLandmark{ 0 };
	unsigned int m_IndexOfICP{ 0 };
	unsigned int move_id{ 0 };
	unsigned int move_id_test{ 0 };
	unsigned int AutoMoveJ_id{ 0 };

	QShortcut* m_shortCut;//快捷键指针
	QTimer* m_ServoPTimer{ nullptr };
	//QTimer* m_AimoeVisualizeTimer{ nullptr };//相机拿数据的QT定时器
	QTimer* m_CrashDetection{ nullptr };//碰撞检测采集电流的数据的QT定时器
	QTimer* m_OralProbeAndTCP{ nullptr };//更新口腔国产机器人的探针和TCP坐标系的更新的QT定时器
	QTimer* m_timer{ nullptr };
	E_ReturnValue rlt;
	virtual void SetFocus() override;
	virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
		const QList<mitk::DataNode::Pointer>& nodes) override;
	Ui::ZYXtestControls m_Controls;




public:


};

#endif // ZYXtest_h
