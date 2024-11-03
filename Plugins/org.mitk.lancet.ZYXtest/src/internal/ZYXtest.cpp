/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "ZYXtest.h"

// Qt
#include  <Qtimer>
#include <QMessageBox>
#include <QInputDialog>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLineEdit>
#include <QKeyEvent>
#include <QApplication>
#include <QShortcut>


// mitk image
#include <mitkImage.h>
#include <mitkAffineTransform3D.h>
#include <mitkMatrixConvert.h>
#include "mitkNodePredicateProperty.h"
#include <mitkDataNode.h>
#include <mitkRenderingManager.h>
#include <mitkBaseRenderer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
//#include "leastsquaresfit.h"
//#include "mitkGizmo.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkPointSet.h"
#include "mitkSurfaceToImageFilter.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"
#include <vtkSphere.h>
#include <vtkLandmarkTransform.h>
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include <mitkImageAccessByItk.h>
#include <vtkIterativeClosestPointTransform.h>
#include "PrintDataHelper.h"
#include <iomanip>
#include <mitkPoint.h>
#include <mitkPointSet.h>



//math

#include "windows.h"
#include "direct.h"
#include "io.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include "stdafx.h"
#include <string>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <random>
#include <vtkKdTree.h>
#include <vtkTransformPolyDataFilter.h>


//Aimooe Camera
#include "AimPositionAPI.h"
#include "AimPositionDef.h"

//DaZu Robot
#include "HR_Pro.h"

//log
#include <fstream>

//口腔机器人
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkConnectivityFilter.h>

//

#include <QFileDialog>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCardinalSpline.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkOBBTree.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkProbeFilter.h>
#include <vtkRendererCollection.h>
#include <vtkSplineFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>
#include "mitkSurfaceVtkMapper3D.h"
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "leastsquaresfit.h"
//#include "mitkGizmo.h"
//#include "mitkGizmo_noscale.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkPointSet.h"
#include "mitkSurfaceToImageFilter.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"
#include <vtkSphere.h>
#include <mitkImageAccessByItk.h>



AimHandle aimHandle = NULL;
E_Interface EI;
T_MarkerInfo markerSt;
T_AimPosStatusInfo statusSt;
using namespace Eigen;
using namespace std;


const std::string ZYXtest::VIEW_ID = "org.mitk.views.zyxtest";

void ZYXtest::SetFocus()
{
	/*m_Controls.buttonPerformImageProcessing->setFocus();*/
}

void ZYXtest::CreateQtPartControl(QWidget* parent)
{
	// create GUI widgets from the Qt Designer's .ui file
	m_Controls.setupUi(parent);

	//Initialize the selection box
	InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis_5); // Select a icp surface
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src_5); // Select a landmark pointset
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine); // Select a line
	InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis_9); // Select a icp SPI surface
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src_9); // Select a landmark  SPI pointset
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine_5); // Select a line
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine_6); // 口腔机器人选择走位规划点
	connect(m_Controls.pushButton_cleantext, &QPushButton::clicked, this, &ZYXtest::Demo_clean);

	//table Start-up  and check
	connect(m_Controls.pushButton_powerOn, &QPushButton::clicked, this, &ZYXtest::powerOn);
	connect(m_Controls.pushButton_powerOff, &QPushButton::clicked, this, &ZYXtest::powerOff);
	connect(m_Controls.pushButton_selfCheck, &QPushButton::clicked, this, &ZYXtest::PrintToolMatrix);
	connect(m_Controls.pushButton_connectCamrea, &QPushButton::clicked, this, &ZYXtest::connectCamera);
	connect(m_Controls.pushButton_updataData, &QPushButton::clicked, this, &ZYXtest::openCameraQtTimer);
	connect(m_Controls.pushButton_SuddenStop, &QPushButton::clicked, this, &ZYXtest::SuddenStop);
	connect(m_Controls.pushButton_connectArm, &QPushButton::clicked, this, &ZYXtest::connectArm);

	//table2 Robot registration
	connect(m_Controls.pushButton_setTCPToFlange, &QPushButton::clicked, this, &ZYXtest::setTCPToFlange);
	connect(m_Controls.pushButton_setInitialPoint, &QPushButton::clicked, this, &ZYXtest::setInitialPoint);
	connect(m_Controls.pushButton_goToInitial, &QPushButton::clicked, this, &ZYXtest::goToInitial);
	connect(m_Controls.pushButton_captureRobot, &QPushButton::clicked, this, &ZYXtest::captureRobot);
	connect(m_Controls.pushButton_replaceRegistration, &QPushButton::clicked, this, &ZYXtest::replaceRegistration);
	connect(m_Controls.pushButton_saveArmMatrix, &QPushButton::clicked, this, &ZYXtest::saveArmMatrix);
	connect(m_Controls.pushButton_reuseArmMatrix, &QPushButton::clicked, this, &ZYXtest::reuseArmMatrix);
	connect(m_Controls.pushButton_JiontAngleMotion, &QPushButton::clicked, this, &ZYXtest::JiontAngleMotion);
	connect(m_Controls.pushButton_AutoMoveJ, &QPushButton::clicked, this, &ZYXtest::AutoMoveJ);

	//Robot move
	connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &ZYXtest::xp);
	connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &ZYXtest::yp);
	connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &ZYXtest::zp);
	connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &ZYXtest::xm);
	connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &ZYXtest::ym);
	connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &ZYXtest::zm);
	connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &ZYXtest::rxp);
	connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &ZYXtest::ryp);
	connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &ZYXtest::rzp);
	connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &ZYXtest::rxm);
	connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &ZYXtest::rym);
	connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &ZYXtest::rzm);

	//table3 
	connect(m_Controls.pushButton_collectLandmark_5, &QPushButton::clicked, this, &ZYXtest::collectLandmark);
	connect(m_Controls.pushButton_collectICP_5, &QPushButton::clicked, this, &ZYXtest::collectICP);
	connect(m_Controls.pushButton_landmarkRegistration_5, &QPushButton::clicked, this, &ZYXtest::landmarkRegistration);
	connect(m_Controls.pushButton_ICPRegistration_5, &QPushButton::clicked, this, &ZYXtest::ICPRegistration);
	connect(m_Controls.pushButton_UpdateDeviation_5, &QPushButton::clicked, this, &ZYXtest::UpdateDeviation);
	connect(m_Controls.pushButton_addICP_2, &QPushButton::clicked, this, &ZYXtest::addICP);
	connect(m_Controls.pushButton_ResetImageConfiguration, &QPushButton::clicked, this, &ZYXtest::ResetImageConfiguration);
	connect(m_Controls.pushButton_saveImageMatrix, &QPushButton::clicked, this, &ZYXtest::saveImageMatrix);
	connect(m_Controls.pushButton_reuseImageMatrix, &QPushButton::clicked, this, &ZYXtest::reuseImageMatrix);

	//table4
	connect(m_Controls.pushButton_setTcpPrecisionTest, &QPushButton::clicked, this, &ZYXtest::SetPrecisionTestTcp);
	connect(m_Controls.pushButton_confirmImageTargetLine, &QPushButton::clicked, this, &ZYXtest::InterpretImageLine);
	connect(m_Controls.pushButton_getProofPoint, &QPushButton::clicked, this, &ZYXtest::getProofPoint);
	connect(m_Controls.pushButton_getProofPointagain, &QPushButton::clicked, this, &ZYXtest::getProofPointagain);

	//table5
	connect(m_Controls.pushButton_setTcpPrecisionTest_7, &QPushButton::clicked, this, &ZYXtest::setTestTCP);
	connect(m_Controls.pushButton_confirmImageTargetLine_5, &QPushButton::clicked, this, &ZYXtest::OnAutoPositionStart);
	connect(m_Controls.pushButton_ResetImageConfiguration_2, &QPushButton::clicked, this, &ZYXtest::ResetImageConfiguration_SPI);
	connect(m_Controls.pushButton_saveImageMatrix_2, &QPushButton::clicked, this, &ZYXtest::saveImageMatrix_SPI);
	connect(m_Controls.pushButton_reuseImageMatrix_2, &QPushButton::clicked, this, &ZYXtest::saveMetalPoint);
	connect(m_Controls.pushButton_collectLandmark_9, &QPushButton::clicked, this, &ZYXtest::collectLandmark_SPI);
	connect(m_Controls.pushButton_collectICP_9, &QPushButton::clicked, this, &ZYXtest::collectICP_SPI);
	connect(m_Controls.pushButton_landmarkRegistration_9, &QPushButton::clicked, this, &ZYXtest::landmarkRegistration_SPI);
	connect(m_Controls.pushButton_ICPRegistration_10, &QPushButton::clicked, this, &ZYXtest::ICPRegistration_SPI);
	connect(m_Controls.pushButton_addICP_3, &QPushButton::clicked, this, &ZYXtest::addICP_SPI);
	connect(m_Controls.pushButton_UpdateDeviation_11, &QPushButton::clicked, this, &ZYXtest::UpdateDeviation_SPI);
	connect(m_Controls.pushButton_getMetalBallPoint, &QPushButton::clicked, this, &ZYXtest::ImportPointSet);

	//table6
	connect(m_Controls.pushButton_1, &QPushButton::clicked, this, &ZYXtest::connectArm);
	connect(m_Controls.pushButton_2, &QPushButton::clicked, this, &ZYXtest::powerOn);
	connect(m_Controls.pushButton_3, &QPushButton::clicked, this, &ZYXtest::powerOff);
	connect(m_Controls.pushButton_4, &QPushButton::clicked, this, &ZYXtest::setTCPToFlange);
	connect(m_Controls.pushButton_9, &QPushButton::clicked, this, &ZYXtest::setInitialPoint);
	connect(m_Controls.pushButton_10, &QPushButton::clicked, this, &ZYXtest::goToInitial);
	connect(m_Controls.pushButton_11, &QPushButton::clicked, this, &ZYXtest::OpenFreeDrag);
	connect(m_Controls.pushButton_12, &QPushButton::clicked, this, &ZYXtest::TurnOffFreeDrag);
	connect(m_Controls.pushButton_17, &QPushButton::clicked, this, &ZYXtest::openToolMotion);
	connect(m_Controls.pushButton_18, &QPushButton::clicked, this, &ZYXtest::closeToolMotion);
	connect(m_Controls.pushButton_19, &QPushButton::clicked, this, &ZYXtest::PauseMotion);
	connect(m_Controls.pushButton_22, &QPushButton::clicked, this, &ZYXtest::KeepMoving);
	connect(m_Controls.pushButton_23, &QPushButton::clicked, this, &ZYXtest::registrationPoint);
	connect(m_Controls.pushButton_24, &QPushButton::clicked, this, &ZYXtest::goBackPoint);
	connect(m_Controls.pushButton_setTCP, &QPushButton::clicked, this, &ZYXtest::setTCP);
	connect(m_Controls.pushButton_setTCP_3, &QPushButton::clicked, this, &ZYXtest::readTCP);
	connect(m_Controls.pushButton_xp_9, &QPushButton::clicked, this, &ZYXtest::xp1);
	connect(m_Controls.pushButton_yp_9, &QPushButton::clicked, this, &ZYXtest::yp1);
	connect(m_Controls.pushButton_zp_9, &QPushButton::clicked, this, &ZYXtest::zp1);
	connect(m_Controls.pushButton_xm_9, &QPushButton::clicked, this, &ZYXtest::xm1);
	connect(m_Controls.pushButton_ym_9, &QPushButton::clicked, this, &ZYXtest::ym1);
	connect(m_Controls.pushButton_zm_9, &QPushButton::clicked, this, &ZYXtest::zm1);
	connect(m_Controls.pushButton_rxp_9, &QPushButton::clicked, this, &ZYXtest::rxp1);
	connect(m_Controls.pushButton_ryp_9, &QPushButton::clicked, this, &ZYXtest::ryp1);
	connect(m_Controls.pushButton_rzp_9, &QPushButton::clicked, this, &ZYXtest::rzp1);
	connect(m_Controls.pushButton_rxm_9, &QPushButton::clicked, this, &ZYXtest::rxm1);
	connect(m_Controls.pushButton_rym_9, &QPushButton::clicked, this, &ZYXtest::rym1);
	connect(m_Controls.pushButton_rzm_9, &QPushButton::clicked, this, &ZYXtest::rzm1);
	connect(m_Controls.pushButton_goToInitial_23, &QPushButton::clicked, this, &ZYXtest::SetSpeedrRatio);
	connect(m_Controls.pushButton_setload, &QPushButton::clicked, this, &ZYXtest::SetLoad);
	connect(m_Controls.pushButton_Move, &QPushButton::clicked, this, &ZYXtest::Move_lhy);
	connect(m_Controls.pushButton_Move_5, &QPushButton::clicked, this, &ZYXtest::Move_lhyinit);
	connect(m_Controls.pushButton_25, &QPushButton::clicked, this, &ZYXtest::EffectiveSpace);
	connect(m_Controls.pushButton_48, &QPushButton::clicked, this, &ZYXtest::logPoint_1);
	connect(m_Controls.pushButton_stopCrashDetection, &QPushButton::clicked, this, &ZYXtest::stopCrashDetection);
	connect(m_Controls.pushButton_CrashDetechion, &QPushButton::clicked, this, &ZYXtest::updataCrashDetechion);


	//随动
	connect(m_Controls.pushButton_servopOn, &QPushButton::clicked, this, &ZYXtest::StartServoControl);
	connect(m_Controls.pushButton_servopEnd, &QPushButton::clicked, this, &ZYXtest::StopServoControl);
	connect(m_Controls.targetPoint, &QPushButton::clicked, this, &ZYXtest::updataServo);


	//table7
	connect(m_Controls.test, &QPushButton::clicked, this, &ZYXtest::test_1);
	connect(m_Controls.test_2, &QPushButton::clicked, this, &ZYXtest::test_2);
	connect(m_Controls.test_3, &QPushButton::clicked, this, &ZYXtest::test_3);
	m_shortCut = new QShortcut(m_Controls.tabWidget);
	m_shortCut->setKey(tr("alt+x"));
	m_shortCut->setAutoRepeat(false);
	connect(m_shortCut, &QShortcut::activated, this, &ZYXtest::keyPressEvent);


	connect(m_Controls.pushButton_A, &QPushButton::clicked, this, &ZYXtest::readA);
	connect(m_Controls.pushButton_B, &QPushButton::clicked, this, &ZYXtest::readB);
	connect(m_Controls.pushButton_C, &QPushButton::clicked, this, &ZYXtest::readC);
	connect(m_Controls.pushButton_D, &QPushButton::clicked, this, &ZYXtest::readD);
	connect(m_Controls.pushButton_E, &QPushButton::clicked, this, &ZYXtest::readE);
	connect(m_Controls.pushButton_F, &QPushButton::clicked, this, &ZYXtest::readF);
	connect(m_Controls.pushButton_G, &QPushButton::clicked, this, &ZYXtest::readG);
	connect(m_Controls.pushButton_H, &QPushButton::clicked, this, &ZYXtest::readH);
	connect(m_Controls.pushButton_J, &QPushButton::clicked, this, &ZYXtest::readJ);
	connect(m_Controls.pushButton_A1, &QPushButton::clicked, this, &ZYXtest::gotoA);
	connect(m_Controls.pushButton_B1, &QPushButton::clicked, this, &ZYXtest::gotoB);
	connect(m_Controls.pushButton_C1, &QPushButton::clicked, this, &ZYXtest::gotoC);
	connect(m_Controls.pushButton_D1, &QPushButton::clicked, this, &ZYXtest::gotoD);
	connect(m_Controls.pushButton_E1, &QPushButton::clicked, this, &ZYXtest::gotoE);
	connect(m_Controls.pushButton_F1, &QPushButton::clicked, this, &ZYXtest::gotoF);
	connect(m_Controls.pushButton_G1, &QPushButton::clicked, this, &ZYXtest::gotoG);
	connect(m_Controls.pushButton_H1, &QPushButton::clicked, this, &ZYXtest::gotoH);
	connect(m_Controls.pushButton_J1, &QPushButton::clicked, this, &ZYXtest::gotoJ);

	//脊柱项目探针标定TCP
	connect(m_Controls.GetProbeEndPosOneBtn, &QPushButton::clicked, this, [=]() {
		GetProbeEndPosBtnClicked(1);
		});
	connect(m_Controls.GetProbeEndPosTwoBtn, &QPushButton::clicked, this, [=]() {
		GetProbeEndPosBtnClicked(2);
		});
	//connect(m_Controls.DisplayRobotFrameBtn, &QPushButton::clicked, this, &ZYXtest::DisplayRobotFrameBtnClicked);
	connect(m_Controls.CalculateGuiderTCPOneBtn, &QPushButton::clicked, this, &ZYXtest::CalculateEulerByProbeEndPosClicked);
	connect(m_Controls.CalculateGuiderTCPTwoBtn, &QPushButton::clicked, this, &ZYXtest::CalculateGuiderTCP2BtnClicked);
	connect(m_Controls.ResetProbeEndPosOneBtn, &QPushButton::clicked, this, [=]() {
		ResetProbeEndPosBtnClicked(1);
		});
	connect(m_Controls.ResetProbeEndPosTwoBtn, &QPushButton::clicked, this, [=]() {
		ResetProbeEndPosBtnClicked(2);
		});
	probeEndOneVector.resize(4);
	probeEndTwoVector.resize(2);

	connect(m_Controls.MoveRobotByJointAnglesBtn, &QPushButton::clicked, this, [=]() {
		std::vector<std::vector<double>> mRobotJointAngles = GetRobotAnglesBtnClicked();
		replaceRegistration();
		int nRet = HRIF_SetOverride(0, 0, 0.3);
		MoveRobotByJointAnglesBtnClicked(mRobotJointAngles);
		});
	connect(&RobotMoveTaskWatcher, &QFutureWatcher<void>::finished, this, &ZYXtest::MoveFinished);
	connect(m_Controls.GetGuiderOriginPosBtn, &QPushButton::clicked, this, &ZYXtest::GetGuiderOriginPosBtnClicked);

	//口腔机器人页面
	connect(m_Controls.pushButton_steelballExtract, &QPushButton::clicked, this, &ZYXtest::on_pushButton_steelballExtract_clicked);//提取金属球
	connect(m_Controls.pushButton_collectDitch, &QPushButton::clicked, this, &ZYXtest::on_pushButton_collectDitch_clicked);//探针采点
	connect(m_Controls.pushButton_resetImageRegis, &QPushButton::clicked, this, &ZYXtest::ResetImageConfiguration_Oral);//重新配准
	connect(m_Controls.pushButton_UpdateTCPAndProbe, &QPushButton::clicked, this, &ZYXtest::UpdateTCPAndProbe);//QT开线程然后进行TCP和探针的导航
	connect(m_Controls.pushButton_stopUpdateTCPAndProbe, &QPushButton::clicked, this, &ZYXtest::stopUpdateTCPAndProbe);//QT开线程然后进行TCP和探针的导航
	//connect(m_Controls.pushButton_setOralTCP, &QPushButton::clicked, this, &ZYXtest::setOralTCP);//手动设置口腔TCP
	connect(m_Controls.pushButton_InterpretImageLine_Oral, &QPushButton::clicked, this, &ZYXtest::InterpretImageLine_Oral);//口腔执行
	connect(m_Controls.pushButton_imageRegisNew, &QPushButton::clicked, this, &ZYXtest::on_pushButton_imageRegisNew_clicked);//口腔影像配准
	connect(m_Controls.pushButton_openForceControl, &QPushButton::clicked, this, &ZYXtest::openForceControl);//开启力控
	connect(m_Controls.pushButton_closeForceControl, &QPushButton::clicked, this, &ZYXtest::closeForceControl);//开启力控
	connect(m_Controls.pushButton_selectDirectionForceControl, &QPushButton::clicked, this, &ZYXtest::selectDirectionForceControl);//开启力控
	connect(m_Controls.pushButton_calibrateDrill, &QPushButton::clicked, this, &ZYXtest::on_pushButton_calibrateDrill_Robot_clicked);
	connect(m_Controls.pushButton_saveImageregisResult, &QPushButton::clicked, this, &ZYXtest::saveRegisMatrix);
	connect(m_Controls.pushButton_reuseImageregisResult, &QPushButton::clicked, this, &ZYXtest::reuseRegisMatrix);
	connect(m_Controls.pushButton_saveTCPResult, &QPushButton::clicked, this, &ZYXtest::saveTCPCalibrateMatrix);
	connect(m_Controls.pushButton_reuseTCPResult, &QPushButton::clicked, this, &ZYXtest::reuseTCPCalibrateMatrix);
	connect(m_Controls.pushButton_Movetest, &QPushButton::clicked, this, &ZYXtest::Move_lhyinit);
	connect(m_Controls.pushButton_startNavi, &QPushButton::clicked, this, &ZYXtest::on_pushButton_startNavi_clicked);
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 当选择发生变化时的处理函数
 * @param part 工作台部件指针
 * @param nodes 数据节点列表
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer>& nodes)
{

	// iterate all selected objects, adjust warning visibility
	foreach(mitk::DataNode::Pointer node, nodes)
	{
		if (node.IsNotNull() && dynamic_cast<mitk::Image*>(node->GetData()))
		{
			/*m_Controls.labelWarning->setVisible(false);*/
			/*m_Controls.buttonPerformImageProcessing->setEnabled(true);*/
			return;
		}
	}

	//m_Controls.labelWarning->setVisible(true);
	/*m_Controls.buttonPerformImageProcessing->setEnabled(false);*/
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 初始化点集选择器
 * @param widget 单节点选择部件指针
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 初始化表面选择器
 * @param widget 单节点选择部件指针
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 清空文本浏览器内容
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::Demo_clean()
{
	QTextBrowser* textBrowser = m_Controls.textBrowser;
	textBrowser->clear();
	//随机函数
	//double randomNum = static_cast<double>(rand()) / RAND_MAX;
	//std::cout << "number: " << std::fixed << std::setprecision(3) << randomNum << std::endl;
	//double a = GenerateRandomNumber(0.0, 1.0, 3);
	//std::cout << "number:" << GenerateRandomNumber(0.0, 1.0, 3) << std::endl;
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 连接机械臂
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::connectArm()
{
	unsigned int boxID = 0;
	char* hostname = "192.168.0.10";
	unsigned short nPort = 10003;
	int nRet = HRIF_Connect(boxID, hostname, nPort);
	PrintResult(m_Controls.textBrowser, nRet, "Robotic arm connection");
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 打开机械臂电源
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::powerOn()
{
	int nGrpEnable = HRIF_GrpEnable(0, 0);
	PrintResult(m_Controls.textBrowser, nGrpEnable, "powerOn");
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 关闭机械臂电源
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::powerOff()
{
	int nGrpDisable = HRIF_GrpDisable(0, 0);
	PrintResult(m_Controls.textBrowser, nGrpDisable, "powerOff");
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取基座到法兰的矩阵
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::getBaseToFlangeMatrix()
{

	//Read TCP get T_BaseToFlanger
	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);
	if (nRet == 0)
	{
		qDebug() << "get BaseToFlange Matrix";
		m_Controls.textBrowser->append("get BaseToFlange Matrix");

		std::cout << "Current TCP Position and Orientation:" << std::endl;
		std::cout << "Position - X: " << dX << ", Y: " << dY << ", Z: " << dZ << std::endl;
		std::cout << "Orientation - Rx: " << dRx << ", Ry: " << dRy << ", Rz: " << dRz << std::endl;

		auto tmpTrans = vtkTransform::New();
		tmpTrans->PostMultiply();
		tmpTrans->RotateZ(dRz);
		tmpTrans->RotateY(dRy);
		tmpTrans->RotateX(dRx);
		tmpTrans->Translate(dX, dY, dZ);
		tmpTrans->Update();

		//VTKT_BaseToFlanger
		vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();
		QVector<double> _vtkMatrix4x4;
		_vtkMatrix4x4 = { VTKT_BaseToFlanger->GetElement(0,0), VTKT_BaseToFlanger->GetElement(0, 1), VTKT_BaseToFlanger->GetElement(0, 2), VTKT_BaseToFlanger->GetElement(0,3),
						  VTKT_BaseToFlanger->GetElement(1, 0),VTKT_BaseToFlanger->GetElement(1, 1), VTKT_BaseToFlanger->GetElement(1, 2), VTKT_BaseToFlanger->GetElement(1,3),
						  VTKT_BaseToFlanger->GetElement(2, 0), VTKT_BaseToFlanger->GetElement(2, 1), VTKT_BaseToFlanger->GetElement(2, 2), VTKT_BaseToFlanger->GetElement(2,3),
						  VTKT_BaseToFlanger->GetElement(3, 0), VTKT_BaseToFlanger->GetElement(3, 1), VTKT_BaseToFlanger->GetElement(3, 2), VTKT_BaseToFlanger->GetElement(3,3)
		};
		std::cout << "VTKT_BaseToFlanger Matrix:" << std::endl << std::endl;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				std::cout << VTKT_BaseToFlanger->GetElement(i, j) << " ";
			}
			std::cout << std::endl;
		}
	}
	else {
		qDebug() << "Failed to read TCP position and orientation.";
		m_Controls.textBrowser->append("Failed to read TCP position and orientation.");
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 连接相机函数
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::connectCamera()
{
	T_AIMPOS_DATAPARA mPosDataPara;
	Aim_API_Initial(aimHandle);
	Aim_SetEthernetConnectIP(aimHandle, 192, 168, 31, 10);
	rlt = Aim_ConnectDevice(aimHandle, I_ETHERNET, mPosDataPara);

	m_Controls.textBrowser->append("-------------------------------------------------------------");
	if (rlt == AIMOOE_OK)
	{
		m_Controls.textBrowser->append("Aimooe Connect Success");
		std::cout << "connect success";
	}
	else {
		m_Controls.textBrowser->append("Aimooe Connect Failed");
		std::cout << "connect failed";
	}

	QString filename = QFileDialog::getExistingDirectory(nullptr, "Select the Tools store folder", "");
	if (filename.isNull()) return;
	filename.append("/");
	std::cout << "The selected folder address :" << filename;
	rlt = Aim_SetToolInfoFilePath(aimHandle, filename.toLatin1().data());

	if (rlt == AIMOOE_OK)
	{
		m_Controls.textBrowser->append("set filenemae success");
		std::cout << "set filenemae success";
	}
	else {
		std::cout << "set filenemae failed";
		m_Controls.textBrowser->append("set filenemae failed");
	}

	int size = 0;
	Aim_GetCountOfToolInfo(aimHandle, size);

	if (size != 0)
	{
		t_ToolBaseInfo* toolarr = new t_ToolBaseInfo[size];

		rlt = Aim_GetAllToolFilesBaseInfo(aimHandle, toolarr);

		if (rlt == AIMOOE_OK)
		{
			for (int i = 0; i < size; i++)
			{
				char* ptool = toolarr[i].name;
				QString toolInfo = QString(ptool);
				m_Controls.textBrowser->append(toolInfo);
			}
		}
		delete[] toolarr;
	}
	else {
		std::cout << "There are no tool identification files in the current directory:";
		m_Controls.textBrowser->append("There are no tool identification files in the current directory:");

	}

	std::cout << "End of connection";
	m_Controls.textBrowser->append("End of connection");

	rlt = AIMOOE_OK;
	m_Controls.textBrowser->append("-------------------------------------------------------------");
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 更新相机数据
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::openCameraQtTimer()
{
	if (m_AimoeVisualizeTimer == nullptr)
	{
		m_AimoeVisualizeTimer = new QTimer(this);
	}
	//connect(m_AimoeVisualizeTimer, SIGNAL(timeout()), this, SLOT(getMatrix()));//	脊柱拿数据第一种方式(一帧只能拿一个数据)
	//connect(m_AimoeVisualizeTimer, SIGNAL(timeout()), this, SLOT(updateCameraData_SPI()));脊柱拿数据第二种方式
	connect(m_AimoeVisualizeTimer, SIGNAL(timeout()), this, SLOT(updateCameraData_Oral())); //口腔拿数据第二种方式
	m_AimoeVisualizeTimer->start(100);
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取工具信息函数（Aimooe自带的demo）
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::getToolInfor()
{
	rlt = Aim_GetMarkerAndStatusFromHardware(aimHandle, I_ETHERNET, markerSt, statusSt);
	if (rlt == AIMOOE_NOT_REFLASH)
	{
		std::cout << "camera get data failed";
	}

	std::vector<int> residualIndex;
	for (int i = 0; i < markerSt.MarkerNumber; i++)
	{
		residualIndex.push_back(i);
	}

	T_AimToolDataResult* mtoolsrlt = new T_AimToolDataResult;
	mtoolsrlt->next = NULL;
	mtoolsrlt->validflag = false;
	rlt = Aim_FindToolInfo(aimHandle, markerSt, mtoolsrlt, 0);
	T_AimToolDataResult* prlt = mtoolsrlt;

	if (rlt == AIMOOE_OK)
	{
		do
		{
			if (prlt->validflag)
			{
				float PI = 3.141592;
				std::cout << "找到工具: " << prlt->toolname << "  平均误差" << prlt->MeanError << " RMS误差" << prlt->Rms;
				std::cout << "工具原点:" << prlt->OriginCoor[0] << "," << prlt->OriginCoor[1] << "," << prlt->OriginCoor[2];
				std::cout << "工具角度:" << prlt->rotationvector[0] * 180 / PI << "," << prlt->rotationvector[1] * 180 / PI << "," << prlt->rotationvector[2] * 180 / PI;
				std::cout << "标记点坐标:";
				for (int i = 0; i < prlt->toolptidx.size(); i++)
				{
					int idx = prlt->toolptidx[i];
					std::vector<int>::iterator iter = residualIndex.begin();
					iter = find(residualIndex.begin(), residualIndex.end(), idx);
					if (iter != residualIndex.end())
					{
						residualIndex.erase(iter);
					}
					if (idx < 0)
						cout << "0 0 0";
					else
						cout << markerSt.MarkerCoordinate[idx * 3 + 0] << " " << markerSt.MarkerCoordinate[idx * 3 + 1] << " " << markerSt.MarkerCoordinate[idx * 3 + 2] << " ";
				}
			}
			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);
		/*cout << endl;*/
		if (residualIndex.size() > 0)
		{
			/*		cout << "in all" << residualIndex.size() << "point：";
					for (int i = 0; i < residualIndex.size(); i++)
					{
						int j = residualIndex[i];
						cout << i << ":" << markerSt.MarkerCoordinate[j * 3 + 0] << "," << markerSt.MarkerCoordinate[j * 3 + 1] << "," << markerSt.MarkerCoordinate[j * 3 + 2];
					}*/
		}
	}
	else
	{
		delete prlt;
	}
	std::cout << "查找结束";
	rlt = AIMOOE_OK;
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取矩阵信息
 * @note：通过筛选的方式来拿数据，问题在于一帧只能拿一个工具的数据
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::getMatrix()
{
	QString position_text;
	std::vector<std::string> toolidarr;
	toolidarr.push_back("Spine_RobotBaseRF");
	toolidarr.push_back("Spine_RobotEndRF");
	toolidarr.push_back("Spine_Probe");
	toolidarr.push_back("Spine_PatientRF");
	toolidarr.push_back("Spine_MetalBallRF");




	rlt = Aim_GetMarkerAndStatusFromHardware(aimHandle, I_ETHERNET, markerSt, statusSt);


	T_AimToolDataResult* mtoolsrlt = new T_AimToolDataResult;
	mtoolsrlt->next = NULL;
	mtoolsrlt->validflag = false;

	rlt = Aim_FindToolInfo(aimHandle, markerSt, mtoolsrlt, 0);


	T_AimToolDataResult* prlt = mtoolsrlt;


	if (prlt->validflag)
	{
		do
		{
			position_text += "Tool name: " + QString(prlt->toolname) + "\n";
			position_text += "Translation: (" + QString::number(prlt->Tto[0]) + ", " +
				QString::number(prlt->Tto[1]) + ", " + QString::number(prlt->Tto[2]) + ")\n";
			position_text += "Rotation matrix:\n";

			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					position_text += QString::number(prlt->Rto[i][j]) + " ";
				}
				position_text += "\n";
			}


			t_tran[0] = prlt->Tto[0];
			t_tran[1] = prlt->Tto[1];
			t_tran[2] = prlt->Tto[2];
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					R_tran[i][j] = prlt->Rto[i][j];
				}
			}
			vtkNew<vtkMatrix4x4> m_T_temp;
			CombineRotationTranslation(R_tran, t_tran, m_T_temp);


			if (strcmp(prlt->toolname, "Spine_RobotBaseRF") == 0)
			{
				memcpy_s(T_CamToBaseRF, sizeof(double) * 16, m_T_temp->GetData(), sizeof(double) * 16);
			}
			else if (strcmp(prlt->toolname, "Spine_Probe") == 0)
			{
				memcpy_s(T_CamToProbe, sizeof(double) * 16, m_T_temp->GetData(), sizeof(double) * 16);
				ProbeTop[0] = prlt->tooltip[0];
				ProbeTop[1] = prlt->tooltip[1];
				ProbeTop[2] = prlt->tooltip[2];

			}
			else if (strcmp(prlt->toolname, "Spine_RobotEndRF") == 0)
			{
				memcpy_s(T_CamToEndRF, sizeof(double) * 16, m_T_temp->GetData(), sizeof(double) * 16);
			}
			else if (strcmp(prlt->toolname, "Spine_PatientRF") == 0)
			{
				memcpy_s(T_CamToPatientRF, sizeof(double) * 16, m_T_temp->GetData(), sizeof(double) * 16);
			}
			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);

		rlt = AIMOOE_OK;
	}
	////PrintToolMatrix();

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 合并旋转和平移矩阵
 * @param Rto 旋转矩阵
 * @param Tto 平移向量
 * @param resultMatrix 结果矩阵指针
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::CombineRotationTranslation(float Rto[3][3], float Tto[3], vtkMatrix4x4* resultMatrix)
{
	// Set rotation part
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			resultMatrix->SetElement(i, j, Rto[i][j]);
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		resultMatrix->SetElement(i, 3, Tto[i]);
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 打印工具矩阵信息
 * @note 打印T_CamToEndRF、T_CamToBaseRF、T_CamToProbe、T_CamToPatientRF、T_CamToMetalBallRF、T_BaseToFlanger
 * @note 其中探针矩阵的xyz部分不是探针尖端、ProbeTop才是探针的尖端
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::PrintToolMatrix()
{

	callCount++;
	std::cout << "Function has been called " << callCount << " times." << std::endl;


	PrintArray16ToMatrix("T_CamToEndRF", T_CamToEndRF);//打印T_CamToEndRF
	PrintArray16ToMatrix("T_CamToBaseRF", T_CamToBaseRF);//打印T_CamToBaseRF
	PrintArray16ToMatrix("T_CamToProbe", T_CamToProbe);//打印T_CamToProbe
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "ProbeTop: " << ProbeTop[0] << "/" << ProbeTop[1] << "/" << ProbeTop[2] << std::endl;//探针针尖
	std::cout << "-------------------------------------------" << std::endl;
	PrintArray16ToMatrix("T_CamToPatientRF", T_CamToPatientRF);//打印T_CamToPatientRF
	PrintArray16ToMatrix("T_CamToMetalBallRF", T_CamToMetalBallRF);//打印T_CamToPatientRF
	PrintArray16ToMatrix("T_BaseToBaseRF", T_BaseToBaseRF);//打印T_CamToPatientRF
	PrintArray16ToMatrix("T_FlangeToEdnRF", T_FlangeToEdnRF);//打印T_FlangeToEdnRF
	PrintArray16ToMatrix("T_CameraToCalibratorRF", T_CamToCalibratorRF);//打印T_FlangeToEdnRF



	//打印机械臂的相关矩阵
	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);
	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();

	//VTKT_BaseToFlanger
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();
	QVector<double> _vtkMatrix4x4;
	_vtkMatrix4x4 = { VTKT_BaseToFlanger->GetElement(0,0), VTKT_BaseToFlanger->GetElement(0, 1), VTKT_BaseToFlanger->GetElement(0, 2), VTKT_BaseToFlanger->GetElement(0,3),
					  VTKT_BaseToFlanger->GetElement(1, 0),VTKT_BaseToFlanger->GetElement(1, 1), VTKT_BaseToFlanger->GetElement(1, 2), VTKT_BaseToFlanger->GetElement(1,3),
					  VTKT_BaseToFlanger->GetElement(2, 0), VTKT_BaseToFlanger->GetElement(2, 1), VTKT_BaseToFlanger->GetElement(2, 2), VTKT_BaseToFlanger->GetElement(2,3),
					  VTKT_BaseToFlanger->GetElement(3, 0), VTKT_BaseToFlanger->GetElement(3, 1), VTKT_BaseToFlanger->GetElement(3, 2), VTKT_BaseToFlanger->GetElement(3,3)
	};
	Print_Matrix("T_BaseToFlanger", VTKT_BaseToFlanger);
	PrintTCP_Terminal(dX, dY, dZ, dRx, dRy, dRz);

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 输入关节角度执行关节角运动
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::JiontAngleMotion()
{
	double DJ1 = m_Controls.lineEdit_DJ1->text().toDouble();
	double DJ2 = m_Controls.lineEdit_DJ2->text().toDouble();
	double DJ3 = m_Controls.lineEdit_DJ3->text().toDouble();
	double DJ4 = m_Controls.lineEdit_DJ4->text().toDouble();
	double DJ5 = m_Controls.lineEdit_DJ5->text().toDouble();
	double DJ6 = m_Controls.lineEdit_DJ6->text().toDouble();

	m_Controls.textBrowser->append("----------------------------------------------------");
	m_Controls.textBrowser->append("JiontAngle");
	m_Controls.textBrowser->append(QString::number(DJ1));
	m_Controls.textBrowser->append(QString::number(DJ2));
	m_Controls.textBrowser->append(QString::number(DJ3));
	m_Controls.textBrowser->append(QString::number(DJ4));
	m_Controls.textBrowser->append(QString::number(DJ5));
	m_Controls.textBrowser->append(QString::number(DJ6));
	m_Controls.textBrowser->append("----------------------------------------------------");

	int nIsUseJoint = 1;
	int nIsSeek = 0;
	int nIOBit = 0;
	int nIOState = 0;
	int result = HRIF_MoveJ(0, 0, g_init_X, g_init_Y, g_init_Z, g_init_Rx, g_init_Ry, g_init_Rz,
		DJ1, DJ2, DJ3, DJ4, DJ5, DJ6, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
	PrintResult(m_Controls.textBrowser, result, "JiontMove");

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 点击按钮会去读save文件夹、在文件夹中有log_Point.txt文件的一组关节角，然后进行关节角运动
 * @note 需要在桌面上有save文件夹、在文件夹中有log_Point.txt文件，里面每一行存的都是一组关节角
 */
 //---------------------------------------------------------------------------------------------------------------

void ZYXtest::AutoMoveJ()
{
	//设定一个全局的变量，每次点击后增加一，并把对应的行数的数据拿出来，给关节角，然后运动

	m_Controls.textBrowser->append("Current AutoMoveJ_id: " + QString::number(AutoMoveJ_id));

	// 打开存储机器人角度的文件
	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\log_Point.txt");
	std::vector<std::vector<double>> jointAngles;
	std::string line;
	while (std::getline(inputFile, line)) {
		std::replace(line.begin(), line.end(), ',', ' ');
		std::istringstream iss(line);
		std::vector<double> angles((std::istream_iterator<double>(iss)), std::istream_iterator<double>());
		if (angles.size() == 6) {
			jointAngles.push_back(angles);
		}
	}
	inputFile.close();

	if (AutoMoveJ_id < jointAngles.size()) {
		std::vector<double> selectedJointAngles = jointAngles[AutoMoveJ_id];
		double DJ1 = selectedJointAngles[0];
		double DJ2 = selectedJointAngles[1];
		double DJ3 = selectedJointAngles[2];
		double DJ4 = selectedJointAngles[3];
		double DJ5 = selectedJointAngles[4];
		double DJ6 = selectedJointAngles[5];

		m_Controls.textBrowser->append("----------------------------------------------------");
		m_Controls.textBrowser->append("JiontAngle");
		m_Controls.textBrowser->append(QString::number(DJ1));
		m_Controls.textBrowser->append(QString::number(DJ2));
		m_Controls.textBrowser->append(QString::number(DJ3));
		m_Controls.textBrowser->append(QString::number(DJ4));
		m_Controls.textBrowser->append(QString::number(DJ5));
		m_Controls.textBrowser->append(QString::number(DJ6));
		m_Controls.textBrowser->append("----------------------------------------------------");

		int nIsUseJoint = 1;
		int nIsSeek = 0;
		int nIOBit = 0;
		int nIOState = 0;
		int result = HRIF_MoveJ(0, 0, g_init_X, g_init_Y, g_init_Z, g_init_Rx, g_init_Ry, g_init_Rz,
			DJ1, DJ2, DJ3, DJ4, DJ5, DJ6, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
			nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
		PrintResult(m_Controls.textBrowser, result, "Goto Point");
		AutoMoveJ_id++;
	}
	else {
		m_Controls.textBrowser->append("AutoMoveJ_id 超出范围");
		AutoMoveJ_id = 0;
	}

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取相机的数据（解决之前一帧只能拿一个工具的数据、完成QT的工具有效性检测）
 * @note 感谢陈钊祥同事优化了我的代码
 * @note 脊柱项目的相机采集函数
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::updateCameraData_SPI()
{
	QString position_text;
	std::vector<std::string> toolidarr;
	toolidarr.push_back("Spine_RobotBaseRF");
	toolidarr.push_back("Spine_RobotEndRF");
	toolidarr.push_back("Spine_Probe");
	toolidarr.push_back("Spine_PatientRF");
	toolidarr.push_back("Spine_MetalBallRF");


	auto prlt = GetNewToolData();

	if (rlt == AIMOOE_OK)//判断是否采集成功
	{
		do
		{
			//获取数据
			UpdateCameraToToolMatrix(prlt, "Spine_RobotBaseRF", T_CamToBaseRF, m_Controls.Spine_RobotBaseRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Spine_RobotEndRF", T_CamToEndRF, m_Controls.Spine_RobotEndRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Spine_PatientRF", T_CamToPatientRF, m_Controls.Spine_PatientRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Spine_Probe", T_CamToProbe, m_Controls.Spine_ProbeDataLabel);
			/*UpdateCameraToToolMatrix(prlt, "Spine_Probe", T_CameraToProbe2, m_Controls.Spine_ProbeDataLabel);*/
			UpdateCameraToToolMatrix(prlt, "Spine_MetalBallRF", T_CamToMetalBallRF, m_Controls.Spine_MetalBallRFDataLabel);

			//获取Spine_Probe数据
			if (strcmp(prlt->toolname, "Spine_Probe") == 0)
			{
				if (prlt->validflag)
				{
					ProbeTop[0] = prlt->tooltip[0];
					ProbeTop[1] = prlt->tooltip[1];
					ProbeTop[2] = prlt->tooltip[2];
				}
			}

			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);
		/*cout << endl;*/

	}
	else
	{
		delete prlt;
	}

}


//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取相机的数据（解决之前一帧只能拿一个工具的数据、完成QT的工具有效性检测）
 * @note 感谢陈钊祥同事优化了我的代码
 * @note 脊柱项目的相机采集函数
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::updateCameraData_Oral()
{

	auto prlt = GetNewToolData();

	if (rlt == AIMOOE_OK)//判断是否采集成功
	{
		do
		{
			//获取数据
			UpdateCameraToToolMatrix(prlt, "Oral_RobotBaseRF", T_CamToBaseRF, m_Controls.Spine_RobotBaseRFDataLabel);
			// UpdateCameraToToolMatrix(prlt, "Oral_RobotEndRF", T_CamToEndRF, m_Controls.Spine_RobotEndRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Oral_PatientRF", T_CamToPatientRF, m_Controls.Spine_PatientRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Oral_Probe", T_CamToProbe, m_Controls.Spine_ProbeDataLabel);
			UpdateCameraToToolMatrix(prlt, "Oral_TCPRF", T_CamToCalibratorRF, m_Controls.Spine_MetalBallRFDataLabel);
			UpdateCameraToToolMatrix(prlt, "Oral_EndRF", T_CamToEndRF, m_Controls.Spine_RobotEndRFDataLabel);

			//获取Spine_Probe数据
			if (strcmp(prlt->toolname, "Oral_Probe") == 0)
			{
				if (prlt->validflag)
				{
					ProbeTop[0] = prlt->tooltip[0];
					ProbeTop[1] = prlt->tooltip[1];
					ProbeTop[2] = prlt->tooltip[2];
				}
			}

			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);
		/*cout << endl;*/

	}
	else
	{
		delete prlt;
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 获取新的工具数据
 *
 * @return T_AimToolDataResult* 指向新工具数据的指针
 */
 //---------------------------------------------------------------------------------------------------------------
T_AimToolDataResult* ZYXtest::GetNewToolData()
{
	rlt = Aim_GetMarkerAndStatusFromHardware(aimHandle, I_ETHERNET, markerSt, statusSt);
	if (rlt == AIMOOE_NOT_REFLASH)
	{
		std::cout << "camera get data failed";
	}
	T_AimToolDataResult* mtoolsrlt = new T_AimToolDataResult;//新建一个值指，将指针清空用于存数据
	mtoolsrlt->next = NULL;
	mtoolsrlt->validflag = false;

	rlt = Aim_FindToolInfo(aimHandle, markerSt, mtoolsrlt, 0);//获取数据
	T_AimToolDataResult* prlt = mtoolsrlt;//将获取完数据的从mtoolsrlt给prlt指针

	return prlt;
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @berief 更新相机到工具矩阵
 *
 * @param ToolData 工具数据指针
 * @param Name 工具名称
 * @param aCamera2Tool 相机到工具矩阵
 * @param label 标签指针
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const char* Name, double* aCamera2Tool, QLabel* label)
{
	/*std::cout << "run UpdateCameraToToolMatrix" << std::endl;*/
	if (strcmp(ToolData->toolname, Name) == 0)
	{
		if (ToolData->validflag)
		{
			//获取相机数据
			t_tran[0] = ToolData->Tto[0];
			t_tran[1] = ToolData->Tto[1];
			t_tran[2] = ToolData->Tto[2];
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					R_tran[i][j] = ToolData->Rto[i][j];
				}
			}
			//拼接矩阵
			vtkNew<vtkMatrix4x4> matrix;
			CombineRotationTranslation(R_tran, t_tran, matrix);
			memcpy_s(aCamera2Tool, sizeof(double) * 16, matrix->GetData(), sizeof(double) * 16);

			//改边QT界面（变色+显示坐标）
			//m_Controls.textBrowser->append(QString("Spine_PatientRF: %1, %2, %3").arg(prlt->Tto[0]).arg(prlt->Tto[1]).arg(prlt->Tto[2]));
		/*	if ("Spine_Probe" == Name)
			{
				ProbeTop[0] = ToolData->tooltip[0];
				ProbeTop[1] = ToolData->tooltip[1];
				ProbeTop[2] = ToolData->tooltip[2];
			}*/
			if (label != nullptr)
			{
				QString str = "x:" + QString::number(ToolData->tooltip[0]) + " "
					+ "y:" + QString::number(ToolData->tooltip[1]) + " "
					+ "z:" + QString::number(ToolData->tooltip[2]);
				label->setText(str);
				label->setStyleSheet("QLabel { color : green; }");
				/*std::cout << str << std::endl;*/
			}
		}
		else
		{
			if (label != nullptr)
			{
				QString str = "x:nan  y:nan   z:nan";
				label->setText(str);
				/*std::cout << str << std::endl;*/
				label->setStyleSheet("QLabel { color : red; }");
			}
		}
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 机器人急停功能
 * @note：仅仅是机械臂暂停运动、机械臂断电操作没有成功，底层权限没有给我们开
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::SuddenStop()
{
	int GrpStop = HRIF_GrpStop(0, 0);
	PrintResult(m_Controls.textBrowser, GrpStop, "Arm stop");
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 机器人紧急恢复函数
 * @note：目前急停底层的一些东西没法操作、目前该函数没用
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::ScramRecovery()
{
	int GrpReset = HRIF_GrpReset(0, 0);
	MITK_INFO << "GrpReset: " << GrpReset;

	// 机器人上电
	int Electrify = HRIF_Electrify(0);
	MITK_INFO << "Electrify: " << Electrify;

	// 判断服务器是否连接
	bool IsConnected = HRIF_IsConnected(0);
	MITK_INFO << "IsConnected: " << IsConnected;
	if (!IsConnected)
	{

		unsigned int boxID = 0;
		char* hostname = "192.168.0.10";
		unsigned short nPort = 10003;
		int Connect = HRIF_Connect(0, hostname, nPort);
	}


	// 连接控制器电箱
	int Connect2Box = HRIF_Connect2Box(0);
	MITK_INFO << "Connect2Box: " << Connect2Box;

	// 连接控制器
	int Connect2Controller = HRIF_Connect2Controller(0);
	MITK_INFO << "Connect2Controller: " << Connect2Controller;

	// 控制器是否启动完成
	// 定义是否启动完成变量
	int nStarted = 0;
	// 判断机器人是否启动完成
	int nRet = HRIF_IsControllerStarted(0, nStarted);
	MITK_INFO << "nStarted: " << nStarted;

	if (Electrify == 0 && Connect2Box == 0 && nStarted == 1)
	{
		MITK_INFO << "Can enable";
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 生成随机数
 *
 * @param lowerBound 下界
 * @param upperBound 上界
 * @param decimalPoints 小数点位数
 * @return double 生成的随机数
 */
 //---------------------------------------------------------------------------------------------------------------
double ZYXtest::GenerateRandomNumber(double lowerBound, double upperBound, int decimalPoints)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(lowerBound, upperBound);
	double randomNum = dis(gen);
	return std::round(randomNum * std::pow(10, decimalPoints)) / std::pow(10, decimalPoints);
}


//+++++++++++++++++++++++++++++++++++++++++Table2 Robotic arm registration++++++++++++++++++++++++++++++++++++++++
 //---------------------------------------------------------------------------------------------------------------
/**
 * @brief 设置TCP到法兰
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::setTCPToFlange()
{
	dTcp_X = 0;
	dTcp_Y = 0;
	dTcp_Z = 0;
	dTcp_Rx = 0;
	dTcp_Ry = 0;
	dTcp_Rz = 0;
	int nRet = HRIF_SetTCP(0, 0, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz);
	PrintResult(m_Controls.textBrowser, nRet, "Set Flange");
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 设置初始点
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::setInitialPoint()
{
	int result = HRIF_ReadActPos(0, 0, g_init_X, g_init_Y, g_init_Z, g_init_Rx, g_init_Ry, g_init_Rz, dJ1_init, dJ2_init, dJ3_init, dJ4_init,
		dJ5_init, dJ6_init, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	PrintResult(m_Controls.textBrowser, result, "SetInitialPoint");
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 回到初始点
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::goToInitial()
{
	int nIsUseJoint = 1;
	int nIsSeek = 0;
	int nIOBit = 0;
	int nIOState = 0;
	//int result = HRIF_MoveJ(0, 0, g_init_X, g_init_Y, g_init_Z, g_init_Rx, g_init_Ry, g_init_Rz,
	//	dJ1_init, dJ2_init, dJ3_init, dJ4_init, dJ5_init, dJ6_init, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
	//	nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
	int result = HRIF_WayPoint(0, 0, nMoveType, g_init_X, g_init_Y, g_init_Z, g_init_Rx, g_init_Ry, g_init_Rz,
		dJ1_init, dJ2_init, dJ3_init, dJ4_init, dJ5_init, dJ6_init, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
	PrintResult(m_Controls.textBrowser, result, "Goto Initial Position");
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 机械臂重新配准
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::replaceRegistration()
{
	m_Controls.textBrowser->append("Replace Registration");
	m_RobotRegistration.RemoveAllPose();
	m_IndexOfRobotCapture = 0;
	m_Controls.lineEdit_collectedRoboPose->setText(QString::number(0));

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 保存机械臂配准矩阵T_BaseToBaseRF、T_FlangeToEndRF
 * @note 保存在桌面save文件夹的T_BaseToBaseRF.txtT_FlangeToEndRF.txt
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::saveArmMatrix()
{
	std::ofstream robotMatrixFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_BaseToBaseRF.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile << T_BaseToBaseRF[i];
		if (i != 15) {
			robotMatrixFile << ",";
		}
		else {
			robotMatrixFile << ";";
		}
	}
	robotMatrixFile << std::endl;
	robotMatrixFile.close();


	std::ofstream robotMatrixFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_FlangeToEndRF.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile1 << T_FlangeToEdnRF[i];
		if (i != 15) {
			robotMatrixFile1 << ",";
		}
		else {
			robotMatrixFile1 << ";";
		}
	}
	robotMatrixFile1 << std::endl;
	robotMatrixFile1.close();
	m_Controls.textBrowser->append("saveArmMatrix");
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 读取机械臂配准矩阵T_BaseToBaseRF、T_FlangeToEndRF
 * @note 读取在桌面save文件夹的T_BaseToBaseRF.txtT_FlangeToEndRF.txt
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::reuseArmMatrix()
{

	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_BaseToBaseRF.txt");
	if (inputFile.is_open()) {
		std::string line;
		if (std::getline(inputFile, line)) {
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ',')) {
				T_BaseToBaseRF[index] = std::stod(token);
				index++;
			}
		}
		inputFile.close();
	}
	else {

		m_Controls.textBrowser->append("无法打开文件:T_BaseToBaseRF.txt");
	}

	PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "T_BaseToBaseRF", T_BaseToBaseRF);
	PrintArray16ToMatrix("T_BaseToBaseRF", T_BaseToBaseRF);



	//导入T_FlangeToEdnRF
	std::ifstream inputFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_FlangeToEndRF.txt");
	if (inputFile2.is_open()) {
		std::string line2;
		if (std::getline(inputFile2, line2)) {
			std::stringstream ss2(line2);
			std::string token2;
			int index2 = 0;
			while (std::getline(ss2, token2, ',')) {
				T_FlangeToEdnRF[index2] = std::stod(token2);
				index2++;
			}
		}
		inputFile2.close();
	}
	else {
		m_Controls.textBrowser->append("无法打开文件：T_FlangeToEndRF.txt");
	}

	//打印T_FlangeToEdnRF
	PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "T_FlangeToEdnRF", T_FlangeToEdnRF);
	PrintArray16ToMatrix("T_FlangeToEdnRF", T_FlangeToEdnRF);

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 机械臂配准点的捕获函数
 * @note 前五个平移、后五个旋转
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::captureRobot()
{
	m_Controls.textBrowser->append("captureRobot");
	if (m_IndexOfRobotCapture < 5) //The first five translations, 
	{
		m_IndexOfRobotCapture++;
		std::cout << "m_IndexOfRobotCapture: " << m_IndexOfRobotCapture << std::endl;
		m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));
		CapturePose(true);


	}
	else if (m_IndexOfRobotCapture < 10) //the last five rotations
	{


		m_IndexOfRobotCapture++;
		m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));
		std::cout << "m_IndexOfRobotCapture: " << m_IndexOfRobotCapture << std::endl;

		CapturePose(false);
	}
	else
	{
		//MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
		vtkNew<vtkMatrix4x4> robotEndToFlangeMatrix;
		m_RobotRegistration.GetTCPmatrix(robotEndToFlangeMatrix);

		vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
		m_RobotRegistration.GetRegistraionMatrix(matrix4x4);


		double x = robotEndToFlangeMatrix->GetElement(0, 3);
		double y = robotEndToFlangeMatrix->GetElement(1, 3);
		double z = robotEndToFlangeMatrix->GetElement(2, 3);
		std::cout << "X: " << x << std::endl;
		std::cout << "Y: " << y << std::endl;
		std::cout << "Z: " << z << std::endl;

		robotEndToFlangeMatrix->Invert();

		m_Controls.textBrowser->append("Registration RMS: " + QString::number(m_RobotRegistration.RMS()));
		std::cout << "Registration RMS: " << m_RobotRegistration.RMS() << std::endl;


		vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
		m_RobotRegistration.GetRegistraionMatrix(vtkT_BaseToBaseRF);
		vtkT_BaseToBaseRF->Invert();
		memcpy_s(T_BaseToBaseRF, sizeof(double) * 16, vtkT_BaseToBaseRF->GetData(), sizeof(double) * 16);


		vtkMatrix4x4* vtkT_FlangeToEndRF = vtkMatrix4x4::New();
		m_RobotRegistration.GetTCPmatrix(vtkT_FlangeToEndRF);
		memcpy_s(T_FlangeToEdnRF, sizeof(double) * 16, vtkT_FlangeToEndRF->GetData(), sizeof(double) * 16);

		//打印T_BaseToBaseRF
		//std::cout << "----------------------------------------" << std::endl;
		//std::cout << "T_BaseRFToBase:" << std::endl;
		//vtkMatrix4x4* vtkT_BaseRFToBase = vtkMatrix4x4::New();
		//m_RobotRegistration.GetRegistraionMatrix(vtkT_BaseRFToBase);
		//for (int i = 0; i < 4; ++i) {
		//	for (int j = 0; j < 4; ++j) {
		//		std::cout << std::fixed << std::setprecision(6) << vtkT_BaseRFToBase->GetElement(i, j) << " ";
		//	}
		//	std::cout << std::endl;
		//}

	}

}

//---------------------------------------------------------------------------------------------------------------
 /**
 * @brief 捕获姿态
 *
 * @param translationOnly 是否旋转 true:平移 flase：旋转
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::CapturePose(bool translationOnly)
{

	//Read TCP get T_BaseToFlanger
	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);


	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();

	//VTKT_BaseToFlanger
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();
	QVector<double> _vtkMatrix4x4;
	_vtkMatrix4x4 = { VTKT_BaseToFlanger->GetElement(0,0), VTKT_BaseToFlanger->GetElement(0, 1), VTKT_BaseToFlanger->GetElement(0, 2), VTKT_BaseToFlanger->GetElement(0,3),
					  VTKT_BaseToFlanger->GetElement(1, 0),VTKT_BaseToFlanger->GetElement(1, 1), VTKT_BaseToFlanger->GetElement(1, 2), VTKT_BaseToFlanger->GetElement(1,3),
					  VTKT_BaseToFlanger->GetElement(2, 0), VTKT_BaseToFlanger->GetElement(2, 1), VTKT_BaseToFlanger->GetElement(2, 2), VTKT_BaseToFlanger->GetElement(2,3),
					  VTKT_BaseToFlanger->GetElement(3, 0), VTKT_BaseToFlanger->GetElement(3, 1), VTKT_BaseToFlanger->GetElement(3, 2), VTKT_BaseToFlanger->GetElement(3,3)
	};

	//std::cout << "VTKT_BaseToFlanger Matrix Contents:" << std::endl;
	//for (int i = 0; i < 4; i++) {
	//	for (int j = 0; j < 4; j++) {
	//		std::cout << _vtkMatrix4x4[i * 4 + j] << "\t";
	//	}
	//	std::cout << std::endl;
	//}

	auto VTKT_CameratoEndRF = vtkMatrix4x4::New();
	VTKT_CameratoEndRF->DeepCopy(T_CamToEndRF);


	auto VTKT_BaseRFToCamera = vtkMatrix4x4::New();
	VTKT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
	VTKT_BaseRFToCamera->Invert();

	//VTKT_BassRFToEndRF
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(VTKT_CameratoEndRF);
	tmpTransform->Concatenate(VTKT_BaseRFToCamera);
	tmpTransform->Update();
	auto vtkBaseRFtoRoboEndRFMatrix = tmpTransform->GetMatrix();

	//打印vtkBaseRFtoRoboEndRFMatrix
	//vtkMatrix4x4* matrix = tmpTransform->GetMatrix();
	//std::cout << "vtkBaseRFtoRoboEndRFMatrix:" << std::endl;
	//for (int i = 0; i < 4; i++) {
	//	for (int j = 0; j < 4; j++) {
	//		std::cout << matrix->GetElement(i, j) << "\t";
	//	}
	//	std::cout << std::endl;
	//}

	//Robotic arm registration
	m_RobotRegistration.AddPoseWithVtkMatrix(VTKT_BaseToFlanger, vtkBaseRFtoRoboEndRFMatrix, translationOnly);



}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的X轴正方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::xp()
{
	m_Controls.textBrowser->append("xp");
	unsigned int boxID = 0;
	unsigned int rbtID = 0;
	int nToolMotion = 1;
	int nAxisID = 0;
	int nDirection = 1;
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));
	int nMoveRelL = HRIF_MoveRelL(boxID, rbtID, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append(QString::number(nMoveRelL));
	m_Controls.textBrowser->append("xp finish");
}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的Y轴正方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::yp()
{
	m_Controls.textBrowser->append("yp");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 1;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));
	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("yp finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的Z轴正方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::zp()
{
	m_Controls.textBrowser->append("zp");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 2;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));
	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("zp finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的X轴负方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::xm()
{
	m_Controls.textBrowser->append("xm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 0;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("xm finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的Y轴负方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::ym()
{
	m_Controls.textBrowser->append("ym");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 1;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("ym finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在Z轴负方向移动
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::zm()
{
	m_Controls.textBrowser->append("zm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 2;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("zm finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在X轴正方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::rxp()
{
	m_Controls.textBrowser->append("rxp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 3;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rxp finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在Y轴正方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::ryp()
{
	m_Controls.textBrowser->append("ryp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 4;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("ryp finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在Z轴正方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::rzp()
{
	m_Controls.textBrowser->append("rzp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 5;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rzp finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在X轴负方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::rxm()
{
	m_Controls.textBrowser->append("rxm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 3;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rxm finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在Y轴负方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::rym()
{
	m_Controls.textBrowser->append("rym");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 4;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rym finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 沿着当前TCP的在Z轴负方向旋转
 * @note
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::rzm()
{
	m_Controls.textBrowser->append("rzm");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 5;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_5->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rzm finish");

}

//++++++++++++++++++++++++++++++++++++table3 image registration+++++++++++++++++++++++++++++++++++++++++++++++++++
 //---------------------------------------------------------------------------------------------------------------
/**
 * @brief 使用探针收集LandMark点
 * @note 本质上是是将探针的尖端点转移到F_PatientRF下
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::collectLandmark()
{
	m_IndexOfLandmark++;
	m_Controls.lineEdit_collectedLandmark->setText(QString::number(m_IndexOfLandmark));
	m_Controls.textBrowser->append(QString(" m_IndexOfLandmark: ") + QString::number(m_IndexOfLandmark));
	//获取T_patientToProbeRF矩阵
	auto vtkT_CameraToProbe = vtkMatrix4x4::New();
	auto vtkT_PatientRFToCamera = vtkMatrix4x4::New();
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);
	vtkT_PatientRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatientRFToCamera->Invert();

	auto transform = vtkTransform::New();
	transform->Identity();
	transform->PostMultiply();
	transform->SetMatrix(vtkT_CameraToProbe);
	transform->Concatenate(vtkT_PatientRFToCamera);
	transform->Update();
	auto T_patientToProbeRF = transform->GetMatrix();

	//保存T_patientrfToProbeRF
	memcpy_s(T_PatientRFToProbe, sizeof(double) * 16, T_patientToProbeRF->GetData(), sizeof(double) * 16);

	//获取probe尖端的点在patinentRF坐标系下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	tmptrans->SetMatrix(vtkT_PatientRFToCamera);
	tmptrans->MultiplyPoint(ProbeTop, nd_tip_FpatientRF);
	vtkProbeTip_onObjRf->InsertNextPoint(nd_tip_FpatientRF[0], nd_tip_FpatientRF[1], nd_tip_FpatientRF[2]);

	m_Controls.textBrowser->append(QString("Probe Point Landmark: (") + QString::number(nd_tip_FpatientRF[0]) + ", " + QString::number(nd_tip_FpatientRF[1]) + ", "
		+ QString::number(nd_tip_FpatientRF[2]) + ")");

}
//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 使用探针收集ICP点
 * @note 本质上是是将探针的尖端点转移到F_Image下
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::collectICP()
{
	//计数
	m_IndexOfICP++;
	m_Controls.lineEdit_collectedICP_2->setText(QString::number(m_IndexOfICP));
	m_Controls.textBrowser->append(QString(" m_IndexOfICP: ") + QString::number(m_IndexOfICP));

	//获取T_patientToProbeRF矩阵
	auto vtkT_cameraToprobeRF = vtkMatrix4x4::New();
	vtkT_cameraToprobeRF->DeepCopy(T_CamToProbe);

	auto vtkT_patientRFTocamera = vtkMatrix4x4::New();
	vtkT_patientRFTocamera->DeepCopy(T_CamToPatientRF);
	vtkT_patientRFTocamera->Invert();

	auto T_ImageTopatientRF_landmark = vtkMatrix4x4::New();
	T_ImageTopatientRF_landmark->DeepCopy(T_PatientRFtoImage);
	T_ImageTopatientRF_landmark->Invert();

	auto transform = vtkTransform::New();
	transform->Identity();
	transform->PostMultiply();
	transform->SetMatrix(vtkT_cameraToprobeRF);
	transform->Concatenate(vtkT_patientRFTocamera);
	transform->Concatenate(T_ImageTopatientRF_landmark);
	transform->Update();
	auto T_ImageToProbeRF = transform->GetMatrix();

	//获取探针在image下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	//tmptrans->SetMatrix(T_ImageToProbeRF);//这里有问题
	tmptrans->SetMatrix(vtkT_patientRFTocamera);
	tmptrans->Concatenate(T_ImageTopatientRF_landmark);
	tmptrans->MultiplyPoint(ProbeTop, nd_tip_FImage_icp);

	vtkProbeTip_onObjRf_icp->InsertNextPoint(nd_tip_FImage_icp[0], nd_tip_FImage_icp[1], nd_tip_FImage_icp[2]);
	m_Controls.textBrowser->append(QString("Probe Point ICP: (") + QString::number(nd_tip_FImage_icp[0]) + ", " + QString::number(nd_tip_FImage_icp[1]) + ", "
		+ QString::number(nd_tip_FImage_icp[2]) + ")");

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief LandMark配准（粗配准、点点配准）
 * @note 重点要注意配的src数据和tar数据，决定了后面配准矩阵的指向
 * @note auto vtkT_patientRFtoImage = ComputeHomogeneousTransform_vtk(Patient坐标系下的点, Image坐标系下的点);
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::landmarkRegistration()
{
	//获取立柱工装的landmark的点
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src_5->GetSelectedNode();
	mitk::PointSet::Pointer pointSet_Src = dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData());
	if (pointSet_Src.IsNotNull())
	{
		unsigned int numberOfPoints = pointSet_Src->GetSize();
		for (unsigned int i = 0; i < numberOfPoints; ++i)
		{
			// get position
			mitk::PointSet::PointType point = pointSet_Src->GetPoint(i);

			QString pointText = QString("Point %1: (%2, %3, %4)\n").arg(i + 1).arg(point[0]).arg(point[1]).arg(point[2]);
			m_Controls.textBrowser->append(pointText);
		}
	}

	//获取采集的点
	mitk::PointSet::Pointer pointSet_Tar = mitk::PointSet::New();
	vtkIdType numPoints = vtkProbeTip_onObjRf->GetNumberOfPoints();
	for (vtkIdType i = 0; i < numPoints; ++i)
	{
		double point[3];
		vtkProbeTip_onObjRf->GetPoint(i, point);

		mitk::PointSet::PointType mitkPoint;
		mitkPoint[0] = point[0];
		mitkPoint[1] = point[1];
		mitkPoint[2] = point[2];
		pointSet_Tar->SetPoint(i, mitkPoint);
	}

	if (pointSet_Tar.IsNotNull())
	{
		unsigned int numberOfPoints = pointSet_Tar->GetSize();

		for (unsigned int i = 0; i < numberOfPoints; ++i)
		{
			mitk::PointSet::PointType point = pointSet_Tar->GetPoint(i);

			QString pointText2 = QString("Point %1: (%2, %3, %4)\n").arg(i + 1).arg(point[0]).arg(point[1]).arg(point[2]);
			m_Controls.textBrowser->append(pointText2);
		}
	}

	//计算Tpatienttoimage
	auto vtkT_patientRFtoImage = ComputeHomogeneousTransform_vtk(pointSet_Src, pointSet_Tar);
	memcpy_s(T_PatientRFtoImage, sizeof(double) * 16, vtkT_patientRFtoImage->GetData(), sizeof(double) * 16);

	// Create a Data Node for the second point set
	// show point in image space
	mitk::DataNode::Pointer pointSetNode2 = mitk::DataNode::New();
	pointSetNode2->SetData(pointSet_Tar);
	pointSetNode2->SetName("Simulated_PointSet_Target");

	GetDataStorage()->Add(pointSetNode2);
	QString matrixText;



}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 使用两组点计算齐次变换矩阵。
 *
 * @param points_set1 在A坐标系的下的点。
 * @param points_set2 在B坐标系下的点。
 * @return 齐次变换矩阵的 T_AToB。
 * @note 重点要注意配的src数据和tar数据，决定了后面配准矩阵的指向
 */
 //---------------------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkMatrix4x4> ZYXtest::ComputeHomogeneousTransform_vtk(mitk::PointSet::Pointer points_set1, mitk::PointSet::Pointer points_set2)
{
	// Creat vtkPointset
	vtkSmartPointer<vtkPoints> vtkPoints1 = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> vtkPoints2 = vtkSmartPointer<vtkPoints>::New();
	// Convert mitk::PointSet to vtkPoints
	for (unsigned int i = 0; i < points_set1->GetSize(); ++i)
	{
		mitk::PointSet::PointType point = points_set1->GetPoint(i);
		vtkPoints1->InsertNextPoint(point[0], point[1], point[2]);
	}

	for (unsigned int i = 0; i < points_set2->GetSize(); ++i)
	{
		mitk::PointSet::PointType point = points_set2->GetPoint(i);
		vtkPoints2->InsertNextPoint(point[0], point[1], point[2]);
	}

	// Create vtkLandmarkTransform
	vtkSmartPointer<vtkLandmarkTransform> tmpTrans = vtkSmartPointer<vtkLandmarkTransform>::New();
	// important
	tmpTrans->SetModeToRigidBody();

	// Set source point and target point into vtkLandmarkTransform
	tmpTrans->SetSourceLandmarks(vtkPoints1);
	tmpTrans->SetTargetLandmarks(vtkPoints2);

	// Calculate Transformation matrix
	tmpTrans->Update();

	// Get Transformation matrix
	auto transformationMatrix = vtkMatrix4x4::New();
	transformationMatrix->DeepCopy(tmpTrans->GetMatrix());

	return transformationMatrix;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 执行源表面和目标点集之间的ICP配准[点面配准]
*@note 配准后的ICP矩阵要右乘到landmark配准矩阵进行补偿
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ICPRegistration()
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	m_IcpSourceSurface = m_Controls.mitkNodeSelectWidget_surface_regis_5->GetSelectedNode();

	auto icpTargetPointset = mitk::PointSet::New();

	for (vtkIdType i = 0; i < vtkProbeTip_onObjRf_icp->GetNumberOfPoints(); ++i)
	{
		double* pointCoords = vtkProbeTip_onObjRf_icp->GetPoint(i);

		mitk::Point3D point;
		point[0] = pointCoords[0];
		point[1] = pointCoords[1];
		point[2] = pointCoords[2];


		icpTargetPointset->InsertPoint(i, point);
	}


	if (m_IcpSourceSurface != nullptr && icpTargetPointset != nullptr)
	{
		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		double rms = GetRegisrationRMS(icpTargetPointset, icpSrcSurface, icpRegistrator->GetResult());
		m_Controls.textBrowser->append("rms" + QString::number(rms));
		Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();

		auto T_imageToImage_icp = vtkMatrix4x4::New();
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				T_imageToImage_icp->SetElement(row, col, tmpRegistrationResult(row, col));
			}
		}

		QString output;

		output.append("Eigen::Matrix4d content:\n");
		for (int row = 0; row < tmpRegistrationResult.rows(); ++row)
		{
			for (int col = 0; col < tmpRegistrationResult.cols(); ++col)
			{
				output.append(QString::number(tmpRegistrationResult(row, col)) + " ");
			}
			output.append("\n");
		}

		m_Controls.textBrowser->append(output);

		memcpy_s(T_ImageToImage_icp, sizeof(double) * 16, T_imageToImage_icp->GetData(), sizeof(double) * 16);
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在MITK里面显示探针
*@note  目前探针的尖端显示正确、探针的位姿还不太正确
*@note  用的时候要把探针的模型拖入MITK，探针的模型名称为probe
*@note  需要的数据：T_CamToProbe、T_CamToPatientRF、T_PatientRFtoImage、ProbeTop
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateProbe()
{
	if (GetDataStorage()->GetNamedNode("drillSurface") == nullptr)
	{
		m_Controls.textBrowser->append("drillSurface is missing");
		return;
	}
	this->RequestRenderWindowUpdate();


	auto vtkT_CameraToProbe = vtkMatrix4x4::New();
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);

	auto vtkT_PatientRFToCamera = vtkMatrix4x4::New();
	vtkT_PatientRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatientRFToCamera->Invert();

	auto vtkT_imageToPatientRF = vtkMatrix4x4::New();
	vtkT_imageToPatientRF->DeepCopy(T_PatientRFtoImage);
	vtkT_imageToPatientRF->Invert();

	//auto vtkT_imageToimage_icp = vtkMatrix4x4::New();
	//vtkT_imageToimage_icp->DeepCopy(T_ImageToImage_icp);
	//vtkT_imageToimage_icp->Invert();

	vtkNew<vtkMatrix4x4> probeTipMatrix; // camera to probe tip matrix
	probeTipMatrix->Identity();
	probeTipMatrix->SetElement(0, 3, ProbeTop[0]);
	probeTipMatrix->SetElement(1, 3, ProbeTop[1]);
	probeTipMatrix->SetElement(2, 3, ProbeTop[2]);

	probeTipMatrix->SetElement(0, 0, vtkT_CameraToProbe->GetElement(0, 0));
	probeTipMatrix->SetElement(0, 1, vtkT_CameraToProbe->GetElement(0, 1));
	probeTipMatrix->SetElement(0, 2, vtkT_CameraToProbe->GetElement(0, 2));
	probeTipMatrix->SetElement(1, 0, vtkT_CameraToProbe->GetElement(1, 0));
	probeTipMatrix->SetElement(1, 1, vtkT_CameraToProbe->GetElement(1, 1));
	probeTipMatrix->SetElement(1, 2, vtkT_CameraToProbe->GetElement(1, 2));
	probeTipMatrix->SetElement(2, 0, vtkT_CameraToProbe->GetElement(2, 0));
	probeTipMatrix->SetElement(2, 1, vtkT_CameraToProbe->GetElement(2, 1));
	probeTipMatrix->SetElement(2, 2, vtkT_CameraToProbe->GetElement(2, 2));



	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(probeTipMatrix);
	tmpTrans->Concatenate(vtkT_PatientRFToCamera);
	tmpTrans->Concatenate(vtkT_imageToPatientRF);
	tmpTrans->Update();

	auto vtkT_T_imageToProbe = tmpTrans->GetMatrix();
	memcpy_s(T_imageToProbe, sizeof(double) * 16, vtkT_T_imageToProbe->GetData(), sizeof(double) * 16);

	GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkT_T_imageToProbe);//导航的影像
	GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->Modified();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在MITK中显示TCP坐标系
*@note  用的时候要把坐标系的模型拖入MITK，探针的模型名称为Coordinate_Label
*@note  需要的数据T_CamToBaseRF、T_CamToPatientRF、T_PatientRFtoImage、T_BaseToBaseRF
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateTCP()//更新显示的数据
{
	if (GetDataStorage()->GetNamedNode("Coordinate_Label") == nullptr)
	{
		m_Controls.textBrowser->append("Coordinate_Label is missing");
		return;
	}
	this->RequestRenderWindowUpdate();


	auto vtkT_CameraToBaseRF = vtkMatrix4x4::New();
	vtkT_CameraToBaseRF->DeepCopy(T_CamToBaseRF);

	auto vtkT_PatientRFToCamera = vtkMatrix4x4::New();
	vtkT_PatientRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatientRFToCamera->Invert();

	auto vtkT_imageToPatientRF = vtkMatrix4x4::New();
	vtkT_imageToPatientRF->DeepCopy(T_PatientRFtoImage);
	vtkT_imageToPatientRF->Invert();


	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);
	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	vtkMatrix4x4* vtkT_BaseRFToBase = vtkMatrix4x4::New();
	vtkT_BaseRFToBase->DeepCopy(T_BaseToBaseRF);
	vtkT_BaseRFToBase->Invert();




	auto tmpTrans1 = vtkTransform::New();
	tmpTrans1->Identity();
	tmpTrans1->PostMultiply();
	tmpTrans1->SetMatrix(VTKT_BaseToFlanger);
	tmpTrans1->Concatenate(vtkT_BaseRFToBase);
	tmpTrans1->Concatenate(vtkT_CameraToBaseRF);
	tmpTrans1->Concatenate(vtkT_PatientRFToCamera);
	tmpTrans1->Concatenate(vtkT_imageToPatientRF);
	tmpTrans1->Update();

	auto vtkT_imageToFlanger = tmpTrans1->GetMatrix();

	GetDataStorage()->GetNamedNode("Coordinate_Label")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkT_imageToFlanger);//导航的影像
	GetDataStorage()->GetNamedNode("Coordinate_Label")->GetData()->GetGeometry()->Modified();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 用于开一个QT定时器刷新探针、和TCP坐标系的显示
*@note  这里用了一个函数改变了探针的颜色为红色
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateDeviation()
{
	/*GetDataStorage()->GetNamedNode("probe")->SetColor(1, 0, 0);
	connect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::UpdateTCP);
	connect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::UpdateProbe);*/

	// planned entry point and apex point in image frame
	auto plan_tip_pts = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("plan_tip_pts")->GetData());
	mitk::Point3D apex_plan = plan_tip_pts->GetPoint(0);//顶点
	mitk::Point3D entry_plan = plan_tip_pts->GetPoint(1);//切入点

	Eigen::Vector3d axis_plan;
	axis_plan[0] = entry_plan[0] - apex_plan[0];
	axis_plan[1] = entry_plan[1] - apex_plan[1];
	axis_plan[2] = entry_plan[2] - apex_plan[2];
	axis_plan.normalize();//表示方向，不表示距离

	// T_imageToProbe->T_imagetoTCP
	auto T_calibratorRFtoTCP = vtkMatrix4x4::New();
	T_calibratorRFtoTCP->DeepCopy(T_calibratorRFToTCP);

	auto T_EndRFtoCalibratorRF = vtkMatrix4x4::New();
	T_EndRFtoCalibratorRF->DeepCopy(T_EndRFToCalibratorRF);

	auto T_CamtoEndRF = vtkMatrix4x4::New();
	T_CamtoEndRF->DeepCopy(T_CamToEndRF);

	auto T_patientRFtoCamera = vtkMatrix4x4::New();
	T_patientRFtoCamera->DeepCopy(T_CamToPatientRF);
	T_patientRFtoCamera->Invert();

	auto T_imageToPatientRF = vtkMatrix4x4::New();
	T_imageToPatientRF->DeepCopy(T_PatientRFtoImage);
	T_imageToPatientRF->Invert();

	auto trans_imagetoTCP = vtkTransform::New();
	trans_imagetoTCP->Identity();
	trans_imagetoTCP->PostMultiply();
	trans_imagetoTCP->SetMatrix(T_calibratorRFtoTCP);
	trans_imagetoTCP->Concatenate(T_EndRFtoCalibratorRF);
	trans_imagetoTCP->Concatenate(T_CamtoEndRF);
	trans_imagetoTCP->Concatenate(T_patientRFtoCamera);
	trans_imagetoTCP->Concatenate(T_imageToPatientRF);
	trans_imagetoTCP->Update();
	auto T_imagetoTCP = trans_imagetoTCP->GetMatrix();

	Eigen::Vector3d axis_probe;//钻头在z轴上的位姿
	axis_probe[0] = T_imagetoTCP->GetElement(0, 2);
	axis_probe[1] = T_imagetoTCP->GetElement(1, 2);
	axis_probe[2] = T_imagetoTCP->GetElement(2, 2);

	double angleDevi = 180 * acos(axis_plan.dot(axis_probe)) / 3.1415926; //计算角度偏差

	m_Controls.lineEdit_angleError->setText(QString::number(angleDevi));

	//******** m_NaviMode = 0 , i.e, Drilling mode, only update the Drill deviation *************
	if (m_NaviMode == 0)
	{
		m_Controls.lineEdit_entryTotalError->setText("NaN");
		m_Controls.lineEdit_entryVertError->setText("NaN");
		m_Controls.lineEdit_entryHoriError->setText("NaN");
		m_Controls.lineEdit_apexTotalError->setText("NaN");
		m_Controls.lineEdit_apexVertError->setText("NaN");
		m_Controls.lineEdit_apexHoriError->setText("NaN");

		// Tip to planned apex
		mitk::Point3D drillTip;
		//drillTip[0] = m_T_imageToInputDrill[3];//源头1
		//drillTip[1] = m_T_imageToInputDrill[7];
		//drillTip[2] = m_T_imageToInputDrill[11];
		drillTip[0] = T_ImageToInputTCP[3];//源头1
		drillTip[1] = T_ImageToInputTCP[7];
		drillTip[2] = T_ImageToInputTCP[11];
		double tipToPlannedApex = GetPointDistance(drillTip, apex_plan);
		m_Controls.lineEdit_drillTipTotalError->setText(QString::number(tipToPlannedApex));

		// Vertical deviation
		Eigen::Vector3d plannedApexToDrillTip;
		plannedApexToDrillTip[0] = drillTip[0] - apex_plan[0];
		plannedApexToDrillTip[1] = drillTip[1] - apex_plan[1];
		plannedApexToDrillTip[2] = drillTip[2] - apex_plan[2];

		double verticalDevi = plannedApexToDrillTip.dot(axis_plan);
		m_Controls.lineEdit_drillTipVertError->setText(QString::number(verticalDevi));

		// Horizontal deviation
		double horizontalDevi = plannedApexToDrillTip.cross(axis_plan).norm();
		m_Controls.lineEdit_drillTipHoriError->setText(QString::number(horizontalDevi));
	}

	//******** m_NaviMode = 1 , i.e, Implantation mode, only update the implant deviation and angle deviation *************
	if (m_NaviMode == 1)
	{
		m_Controls.lineEdit_drillTipTotalError->setText("NaN");
		m_Controls.lineEdit_drillTipVertError->setText("NaN");
		m_Controls.lineEdit_drillTipHoriError->setText("NaN");

		//******* real implant entry ************
		// Total distance to  planned entry
		mitk::Point3D entry_real = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData())->GetPoint(1);
		double totalEntryDis = GetPointDistance(entry_real, entry_plan);
		m_Controls.lineEdit_entryTotalError->setText(QString::number(totalEntryDis));

		// Vertical deviation
		Eigen::Vector3d entry_planToReal;
		entry_planToReal[0] = entry_real[0] - entry_plan[0];
		entry_planToReal[1] = entry_real[1] - entry_plan[1];
		entry_planToReal[2] = entry_real[2] - entry_plan[2];

		double verticalDevi_entry = entry_planToReal.dot(axis_plan);
		m_Controls.lineEdit_entryVertError->setText(QString::number(verticalDevi_entry));

		// Horizontal deviation
		double horizontalDevi_entry = entry_planToReal.cross(axis_plan).norm();
		m_Controls.lineEdit_entryHoriError->setText(QString::number(horizontalDevi_entry));


		//******* real implant apex ************
		// Total distance to  planned apex
		mitk::Point3D apex_real = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("implant_tip_pts")->GetData())->GetPoint(0);
		double totalApexDis = GetPointDistance(apex_real, apex_plan);
		m_Controls.lineEdit_apexTotalError->setText(QString::number(totalApexDis));

		// Vertical deviation
		Eigen::Vector3d apex_planToReal;
		apex_planToReal[0] = apex_real[0] - apex_plan[0];
		apex_planToReal[1] = apex_real[1] - apex_plan[1];
		apex_planToReal[2] = apex_real[2] - apex_plan[2];

		double verticalDevi_apex = apex_planToReal.dot(axis_plan);
		m_Controls.lineEdit_apexVertError->setText(QString::number(verticalDevi_apex));

		// Horizontal deviation
		double horizontalDevi_apex = apex_planToReal.cross(axis_plan).norm();
		m_Controls.lineEdit_apexHoriError->setText(QString::number(horizontalDevi_apex));

	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 用于将ICP配准出来的矩阵配准
*@note  注意Landmark矩阵*ICP矩阵
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::addICP()
{
	auto vtkT_patientRFtoImage = vtkMatrix4x4::New();
	vtkT_patientRFtoImage->DeepCopy(T_PatientRFtoImage);

	auto vtkT_ImageToICP = vtkMatrix4x4::New();
	vtkT_ImageToICP->DeepCopy(T_ImageToImage_icp);

	vtkNew<vtkTransform> Transform;
	Transform->PostMultiply();
	Transform->SetMatrix(vtkT_ImageToICP);
	Transform->Concatenate(vtkT_patientRFtoImage);
	Transform->Update();

	auto vtkT_PatientToICP = Transform->GetMatrix();
	memcpy_s(T_PatientRFtoImage, sizeof(double) * 16, vtkT_PatientToICP->GetData(), sizeof(double) * 16);

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 重新进行图像配准
*@note  清空的东西
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ResetImageConfiguration()
{
	m_IndexOfLandmark = 0;
	m_IndexOfICP = 0;

	m_Controls.lineEdit_collectedLandmark->setText(QString::number(0));
	m_Controls.lineEdit_collectedICP_2->setText(QString::number(0));

	vtkProbeTip_onObjRf->Reset();
	vtkProbeTip_onObjRf_icp->Reset();

	if (vtkProbeTip_onObjRf->GetNumberOfPoints() == 0 && vtkProbeTip_onObjRf_icp->GetNumberOfPoints() == 0)
	{
		m_Controls.textBrowser->append("Replace image configuration");
	}
	else
	{
		m_Controls.textBrowser->append("Replace image configuration failed");
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 保存图像配准矩阵
*@note  保存两个矩阵T_PatientRFtoImage、T_ImageToImage_icp
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::saveImageMatrix()
{
	//存T_PatientRFtoImage
	std::ofstream robotMatrixFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_PatientRFtoImage.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile << T_PatientRFtoImage[i];
		if (i != 15) {
			robotMatrixFile << ",";
		}
		else {
			robotMatrixFile << ";";
		}
	}
	robotMatrixFile << std::endl;
	robotMatrixFile.close();

	//存T_ImageToImage_icp
	std::ofstream robotMatrixFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_ImageToImage_icp.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile1 << T_ImageToImage_icp[i];
		if (i != 15) {
			robotMatrixFile1 << ",";
		}
		else {
			robotMatrixFile1 << ";";
		}
	}
	robotMatrixFile1 << std::endl;
	robotMatrixFile1.close();
	m_Controls.textBrowser->append("saveImageMatrix");

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 复用图像配准矩阵
*@note  复用两个矩阵T_PatientRFtoImage、T_ImageToImage_icp
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::reuseImageMatrix()
{
	//导入T_PatientRFtoImage
	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_PatientRFtoImage.txt");
	if (inputFile.is_open()) {
		std::string line;
		if (std::getline(inputFile, line)) {
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ',')) {
				T_PatientRFtoImage[index] = std::stod(token);
				index++;
			}
		}
		inputFile.close();
	}
	else {

		m_Controls.textBrowser->append("无法打开文件:T_PatientRFtoImage.txt");
	}


	PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "T_PatientRFtoImage", T_PatientRFtoImage);
	PrintArray16ToMatrix("T_PatientRFtoImage", T_PatientRFtoImage);


	std::ifstream inputFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_ImageToImage_icp.txt");
	if (inputFile2.is_open()) {
		std::string line2;
		if (std::getline(inputFile2, line2)) {
			std::stringstream ss2(line2);
			std::string token2;
			int index2 = 0;
			while (std::getline(ss2, token2, ',')) {
				T_ImageToImage_icp[index2] = std::stod(token2);
				index2++;
			}
		}
		inputFile2.close();
	}
	else {
		m_Controls.textBrowser->append("无法打开文件：T_ImageToImage_icp.txt");
	}


	PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "T_ImageToImage_icp", T_ImageToImage_icp);
	PrintArray16ToMatrix("T_ImageToImage_icp", T_ImageToImage_icp);
}

//++++++++++++++++++++++++++++++++++++页面四系统精度执行+++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 设置精度手指TCP参数
*@note  根据硬件的两个点P2、P3,用这个两个点设置为TCP的x轴方向、借用法兰的Z轴叉乘出Y轴和Z轴
*@note  P2点坐标坐标系的X、Y、Z，其中的RX、RY、RZ借用上面的旋转矩阵进行XYZ欧拉角分解
*@note  P2点坐标坐标系的X、Y、Z，其中的RX、RY、RZ借用上面的旋转矩阵进行XYZ欧拉角分解
*@note   精度手指数据文档：https://gn1phhht53.feishu.cn/wiki/X1IYwy2npicJtRkIqWzcVF1ZnAg?from=from_copylink
*/
//---------------------------------------------------------------------------------------------------------------
bool ZYXtest::SetPrecisionTestTcp()
{

	//验证配准结果

	vtkMatrix4x4* vtkT_FlangeToEndRF = vtkMatrix4x4::New();
	vtkT_FlangeToEndRF->DeepCopy(T_FlangeToEdnRF);
	PrintMatrix("vtkT_FlangeToEndRF", vtkT_FlangeToEndRF->GetData());
	Print_Matrix("vtkT_FlangeToEndRF", vtkT_FlangeToEndRF);


	//设置走线TCP
	Eigen::Vector3d p2;
	p2[0] = m_Controls.lineEdit_p2_x->text().toDouble();
	p2[1] = m_Controls.lineEdit_p2_y->text().toDouble();
	p2[2] = m_Controls.lineEdit_p2_z->text().toDouble();

	Eigen::Vector3d p3;
	p3[0] = m_Controls.lineEdit_p3_x->text().toDouble();
	p3[1] = m_Controls.lineEdit_p3_y->text().toDouble();
	p3[2] = m_Controls.lineEdit_p3_z->text().toDouble();

	Eigen::Vector3d x_tcp;
	x_tcp = p3 - p2;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);

	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];


	//TODO:Test
	//Eigen::Matrix4d T;
	//T << Re(0, 0), Re(0, 1), Re(0, 2), p2[0],
	//	Re(1, 0), Re(1, 1), Re(1, 2), p2[1],
	//	Re(2, 0), Re(2, 1), Re(2, 2), p2[2],
	//	0, 0, 0, 1;

	/*std::cout << "T矩阵：" << std::endl;*/
	//std::cout << Re(0, 0) << " " << Re(0, 1) << " " << Re(0, 2) << " " << p2[0] << std::endl;
	//std::cout << Re(1, 0) << " " << Re(1, 1) << " " << Re(1, 2) << " " << p2[1] << std::endl;
	//std::cout << Re(2, 0) << " " << Re(2, 1) << " " << Re(2, 2) << " " << p2[2] << std::endl;
	//std::cout << "0 0 0 1" << std::endl;


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = p2[0];
	tcp[1] = p2[1];
	tcp[2] = p2[2];
	tcp[3] = 180 * eulerAngle(2) / 3.1415;
	tcp[4] = 180 * eulerAngle(1) / 3.1415;
	tcp[5] = 180 * eulerAngle(0) / 3.1415;
	PrintTCP_qt(m_Controls.textBrowser, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	PrintTCP_Terminal(tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);


	int nRet = HRIF_SetTCP(0, 0, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	//set tcp to robot

	if (nRet == 0) {
		std::cout << "line  TCP Set succeed" << std::endl;
	}
	else {
		std::cout << "line TCP Set failed" << std::endl;
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 系统精度走线操作
*@note  将影像中的Image坐标系下的点全部都转到Base坐标系下
*@note  然后求解出T_BaseToFlange,借助该矩阵的第一列X方向的方向向量
*@note  使用轴角的方法求解出期望的x轴与想去到的x轴的旋转向量、然后求出其旋转矩阵
*@note  T_BaseToFlange*上面一步的矩阵=机械臂运动控制矩阵，进行X、Y、Z欧拉角分解
*/
//---------------------------------------------------------------------------------------------------------------
bool ZYXtest::InterpretImageLine()
{
	//确定目标线：会读取线数据导入到P2和P3中
	// targetPoints in F_image
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);
	std::cout << "targetPoint_0: (" << targetPoint_0[0] << ", " << targetPoint_0[1] << ", " << targetPoint_0[2] << ")" << std::endl;
	std::cout << "targetPoint_1: (" << targetPoint_1[0] << ", " << targetPoint_1[1] << ", " << targetPoint_1[2] << ")" << std::endl;


	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };

	for (int i{ 0 }; i < 20; i++)
	{
		//获取机械臂配准矩阵T_BaseToBaseRF
		vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
		vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);


		//获取T_BaseRFToCamera
		auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();
		vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
		vtkT_BaseRFToCamera->Invert();

		//获取T_CameraToPatientRF
		auto vtkT_CameraToPatientRF = vtkMatrix4x4::New();
		vtkT_CameraToPatientRF->DeepCopy(T_CamToPatientRF);

		//获取T_PatientRFToImage
		auto vtkT_PatientRFToImage = vtkMatrix4x4::New();
		vtkT_PatientRFToImage->DeepCopy(T_PatientRFtoImage);

		//计算T_BaseToImage
		vtkNew<vtkTransform> Transform;
		Transform->Identity();
		Transform->PostMultiply();
		Transform->SetMatrix(vtkT_PatientRFToImage);
		Transform->Concatenate(vtkT_CameraToPatientRF);
		Transform->Concatenate(vtkT_BaseRFToCamera);
		Transform->Concatenate(vtkT_BaseToBaseRF);
		Transform->Update();
		auto vtkT_BaseToImage = Transform->GetMatrix();

		//获取P0点的坐标
		auto TargetMatrix_0 = vtkMatrix4x4::New();
		TargetMatrix_0->SetElement(0, 3, targetPoint_0[0]);
		TargetMatrix_0->SetElement(1, 3, targetPoint_0[1]);
		TargetMatrix_0->SetElement(2, 3, targetPoint_0[2]);

		vtkNew<vtkTransform> Trans;
		Trans->Identity();
		Trans->PostMultiply();
		Trans->SetMatrix(TargetMatrix_0);
		Trans->Concatenate(vtkT_BaseToImage);
		Trans->Update();
		auto vtkT_BaseToTarget_0 = Trans->GetMatrix();

		//获取P1点的坐标
		auto TargetMatrix_1 = vtkMatrix4x4::New();
		TargetMatrix_1->SetElement(0, 3, targetPoint_1[0]);
		TargetMatrix_1->SetElement(1, 3, targetPoint_1[1]);
		TargetMatrix_1->SetElement(2, 3, targetPoint_1[2]);

		vtkNew<vtkTransform> Trans1;
		Trans1->Identity();
		Trans1->PostMultiply();
		Trans1->SetMatrix(TargetMatrix_1);
		Trans1->Concatenate(vtkT_BaseToImage);
		Trans1->Update();
		auto vtkT_BaseToTarget_1 = Trans1->GetMatrix();

		//计算20个点
		targetPointUnderBase_0[0] += vtkT_BaseToTarget_0->GetElement(0, 3);
		targetPointUnderBase_0[1] += vtkT_BaseToTarget_0->GetElement(1, 3);
		targetPointUnderBase_0[2] += vtkT_BaseToTarget_0->GetElement(2, 3);

		targetPointUnderBase_1[0] += vtkT_BaseToTarget_1->GetElement(0, 3);
		targetPointUnderBase_1[1] += vtkT_BaseToTarget_1->GetElement(1, 3);
		targetPointUnderBase_1[2] += vtkT_BaseToTarget_1->GetElement(2, 3);

	}
	//取平均
	// Target Points in F_Base
	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	//获取机械臂的T_BaseToFlanger
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	// 得到当前机械臂输出的Base到法兰的欧拉角转换
	int nRet = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);
	// 将欧拉角转换为变换矩阵
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx1);
	tmpTrans->RotateY(dRy1);
	tmpTrans->RotateZ(dRz1);
	tmpTrans->Translate(dX1, dY1, dZ1);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	//借用法兰的z轴方向(第三列)：法兰z轴在Base坐标系下的位置
	Eigen::Vector3d currentZunderBase;
	currentZunderBase[0] = VTKT_BaseToFlanger->GetElement(0, 2);
	currentZunderBase[1] = VTKT_BaseToFlanger->GetElement(1, 2);
	currentZunderBase[2] = VTKT_BaseToFlanger->GetElement(2, 2);
	currentZunderBase.normalize();


	//std::cout << "currentxunderBase: " << currentXunderBase << std::endl;

	//在Base坐标系下目标坐标系z轴的方向向量
	Eigen::Vector3d targetZunderBase;
	targetZunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetZunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetZunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];
	targetZunderBase.normalize();

	//MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d  rotationAxis;
	rotationAxis = currentZunderBase.cross(targetZunderBase);


	double rotationAngle;//定义旋转角
	if (currentZunderBase.dot(targetZunderBase) > 0) //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;//求向量的模长（sin[rotationAngle]）,再取反三角
	}
	else //如果向量的内积小于0，cos小于0（钝角）
	{
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(VTKT_BaseToFlanger);
	// T_TCPtoBase
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);//旋转角度，和旋转向量
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();

	Eigen::Matrix3d Re;

	Re << testMatrix->GetElement(0, 0), testMatrix->GetElement(0, 1), testMatrix->GetElement(0, 2),
		testMatrix->GetElement(1, 0), testMatrix->GetElement(1, 1), testMatrix->GetElement(1, 2),
		testMatrix->GetElement(2, 0), testMatrix->GetElement(2, 1), testMatrix->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double x = targetPointUnderBase_0[0];
	double y = targetPointUnderBase_0[1];
	double z = targetPointUnderBase_0[2];
	double rx = 180 * eulerAngle[2] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[0] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_move");
	m_Controls.textBrowser->append("dx=" + QString::number(x));
	m_Controls.textBrowser->append("dy=" + QString::number(y));
	m_Controls.textBrowser->append("dz=" + QString::number(z));
	m_Controls.textBrowser->append("dRx=" + QString::number(rx));
	m_Controls.textBrowser->append("dRy=" + QString::number(ry));
	m_Controls.textBrowser->append("dRz=" + QString::number(rz));
	("-------------------------------------------------------------------------------------------");

	dX = x;
	dY = y;
	dZ = z;
	dRx = rx;
	dRy = ry;
	dRz = rz;


	int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);//机械臂移动

	if (nTargetPoint == 0) {
		m_Controls.textBrowser->append("MOVE OK");
	}
	else {
		m_Controls.textBrowser->append("MOVE FAILED");
	}


	return true;
}

//table5 test planing
void ZYXtest::setTestTCP()
{

	//设置走线TCP
	Eigen::Vector3d punctureAxisProximal_inFlange;//近端点
	punctureAxisProximal_inFlange[0] = m_Controls.lineEdit_p2_x_7->text().toDouble();
	punctureAxisProximal_inFlange[1] = m_Controls.lineEdit_p2_y_7->text().toDouble();
	punctureAxisProximal_inFlange[2] = m_Controls.lineEdit_p2_z_7->text().toDouble();

	Eigen::Vector3d punctureAxisDistal_inFlange;//远端点
	punctureAxisDistal_inFlange[0] = m_Controls.lineEdit_p3_x_7->text().toDouble();
	punctureAxisDistal_inFlange[1] = m_Controls.lineEdit_p3_y_7->text().toDouble();
	punctureAxisDistal_inFlange[2] = m_Controls.lineEdit_p3_z_7->text().toDouble();

	Eigen::Vector3d x_tcp;
	x_tcp = punctureAxisDistal_inFlange - punctureAxisProximal_inFlange;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);

	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = punctureAxisProximal_inFlange[0];
	tcp[1] = punctureAxisProximal_inFlange[1];
	tcp[2] = punctureAxisProximal_inFlange[2];
	tcp[3] = 180 * eulerAngle(2) / 3.1415;
	tcp[4] = 180 * eulerAngle(1) / 3.1415;
	tcp[5] = 180 * eulerAngle(0) / 3.1415;

	m_Controls.textBrowser->append("dX_tcp=" + QString::number(tcp[0]));
	m_Controls.textBrowser->append("dY_tcp=" + QString::number(tcp[1]));
	m_Controls.textBrowser->append("dZ_tcp=" + QString::number(tcp[2]));
	m_Controls.textBrowser->append("dRx_tcp=" + QString::number(tcp[3]));
	m_Controls.textBrowser->append("dRy_tcp=" + QString::number(tcp[4]));
	m_Controls.textBrowser->append("dRz_tcp=" + QString::number(tcp[5]));


	int nRet = HRIF_SetTCP(0, 0, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	//set tcp to robot

	if (nRet == 0) {
		std::cout << "line  TCP Set succeed" << std::endl;
	}
	else {
		std::cout << "line TCP Set failed" << std::endl;
	}

}

void ZYXtest::OnAutoPositionStart()
{
	// 确定目标线：会读取线数据导入到P2和P3中
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine_5->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);
	std::cout << "targetPoint_0: (" << targetPoint_0[0] << ", " << targetPoint_0[1] << ", " << targetPoint_0[2] << ")" << std::endl;
	std::cout << "targetPoint_1: (" << targetPoint_1[0] << ", " << targetPoint_1[1] << ", " << targetPoint_1[2] << ")" << std::endl;


	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };

	for (int i{ 0 }; i < 20; i++)
	{
		//获取机械臂配准矩阵T_BaseToBaseRF
		vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
		vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);


		//获取T_BaseRFToCamera
		auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();
		vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
		vtkT_BaseRFToCamera->Invert();

		//获取T_CameraToPatientRF
		auto vtkT_CameraToPatientRF = vtkMatrix4x4::New();
		vtkT_CameraToPatientRF->DeepCopy(T_CamToPatientRF);

		//获取T_PatientRFToImage
		auto vtkT_PatientRFToImage = vtkMatrix4x4::New();
		vtkT_PatientRFToImage->DeepCopy(T_PatientRFtoImage);

		//计算T_BaseToImage
		vtkNew<vtkTransform> Transform;
		Transform->Identity();
		Transform->PostMultiply();
		Transform->SetMatrix(vtkT_PatientRFToImage);
		PrintMatrix("T_PatientRFToImage", vtkT_PatientRFToImage->GetData());

		Transform->Concatenate(vtkT_CameraToPatientRF);
		PrintMatrix("T_CameraToPatientRF", vtkT_CameraToPatientRF->GetData());

		Transform->Concatenate(vtkT_BaseRFToCamera);
		PrintMatrix("T_BaseRFToCamera", vtkT_BaseRFToCamera->GetData());

		Transform->Concatenate(vtkT_BaseToBaseRF);
		PrintMatrix("T_BaseToBaseRF", vtkT_BaseToBaseRF->GetData());

		Transform->Update();
		auto vtkT_BaseToImage = Transform->GetMatrix();
		PrintMatrix("T_BaseToImage", vtkT_BaseToImage->GetData());


		//获取P0点的坐标
		auto TargetMatrix_0 = vtkMatrix4x4::New();
		TargetMatrix_0->SetElement(0, 3, targetPoint_0[0]);
		TargetMatrix_0->SetElement(1, 3, targetPoint_0[1]);
		TargetMatrix_0->SetElement(2, 3, targetPoint_0[2]);

		vtkNew<vtkTransform> Trans;
		Trans->Identity();
		Trans->PostMultiply();
		Trans->SetMatrix(TargetMatrix_0);
		Trans->Concatenate(vtkT_BaseToImage);
		Trans->Update();
		auto vtkT_BaseToTarget_0 = Trans->GetMatrix();

		//获取P1点的坐标
		auto TargetMatrix_1 = vtkMatrix4x4::New();
		TargetMatrix_1->SetElement(0, 3, targetPoint_1[0]);
		TargetMatrix_1->SetElement(1, 3, targetPoint_1[1]);
		TargetMatrix_1->SetElement(2, 3, targetPoint_1[2]);

		vtkNew<vtkTransform> Trans1;
		Trans1->Identity();
		Trans1->PostMultiply();
		Trans1->SetMatrix(TargetMatrix_1);
		Trans1->Concatenate(vtkT_BaseToImage);
		Trans1->Update();
		auto vtkT_BaseToTarget_1 = Trans1->GetMatrix();

		//计算20个点
		targetPointUnderBase_0[0] += vtkT_BaseToTarget_0->GetElement(0, 3);
		targetPointUnderBase_0[1] += vtkT_BaseToTarget_0->GetElement(1, 3);
		targetPointUnderBase_0[2] += vtkT_BaseToTarget_0->GetElement(2, 3);

		targetPointUnderBase_1[0] += vtkT_BaseToTarget_1->GetElement(0, 3);
		targetPointUnderBase_1[1] += vtkT_BaseToTarget_1->GetElement(1, 3);
		targetPointUnderBase_1[2] += vtkT_BaseToTarget_1->GetElement(2, 3);

	}
	//取平均
	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;
	std::cout << "targetPointUnderBase_0 [0]" << targetPointUnderBase_0[0] << std::endl;
	std::cout << "targetPointUnderBase_0 [1]" << targetPointUnderBase_0[1] << std::endl;
	std::cout << "targetPointUnderBase_0 [2]" << targetPointUnderBase_0[2] << std::endl;

	std::cout << "targetPointUnderBase_1 [0]" << targetPointUnderBase_1[0] << std::endl;
	std::cout << "targetPointUnderBase_1 [1]" << targetPointUnderBase_1[1] << std::endl;
	std::cout << "targetPointUnderBase_1 [2]" << targetPointUnderBase_1[2] << std::endl;
	//获取机械臂的T_BaseToFlanger
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx1);
	tmpTrans->RotateY(dRy1);
	tmpTrans->RotateZ(dRz1);
	tmpTrans->Translate(dX1, dY1, dZ1);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	//借用法兰的Z轴方向(第三列)
	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = VTKT_BaseToFlanger->GetElement(0, 2);
	currentXunderBase[1] = VTKT_BaseToFlanger->GetElement(1, 2);
	currentXunderBase[2] = VTKT_BaseToFlanger->GetElement(2, 2);
	currentXunderBase.normalize();


	std::cout << "currentxunderBase: " << currentXunderBase << std::endl;

	//在Base坐标系下目标坐标系Z轴的方向向量
	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];
	targetXunderBase.normalize();

	MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d  rotationAxis;
	rotationAxis = currentXunderBase.cross(targetXunderBase);


	double rotationAngle;//定义旋转角
	if (currentXunderBase.dot(targetXunderBase) > 0) //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;//求向量的模长（sin[rotationAngle]）,再取反三角
	}
	else //如果向量的内积小于0，cos小于0（钝角）
	{
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(VTKT_BaseToFlanger);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);//旋转角度，和旋转向量
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();
	PrintMatrix("targetMatrix", testMatrix->GetData());

	Eigen::Matrix3d Re;

	Re << testMatrix->GetElement(0, 0), testMatrix->GetElement(0, 1), testMatrix->GetElement(0, 2),
		testMatrix->GetElement(1, 0), testMatrix->GetElement(1, 1), testMatrix->GetElement(1, 2),
		testMatrix->GetElement(2, 0), testMatrix->GetElement(2, 1), testMatrix->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double x = targetPointUnderBase_0[0];
	double y = targetPointUnderBase_0[1];
	double z = targetPointUnderBase_0[2];
	double rx = 180 * eulerAngle[2] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[0] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_move");
	m_Controls.textBrowser->append("dx=" + QString::number(x));
	m_Controls.textBrowser->append("dy=" + QString::number(y));
	m_Controls.textBrowser->append("dz=" + QString::number(z));
	m_Controls.textBrowser->append("dRx=" + QString::number(rx));
	m_Controls.textBrowser->append("dRy=" + QString::number(ry));
	m_Controls.textBrowser->append("dRz=" + QString::number(rz));
	/*("-------------------------------------------------------------------------------------------");*/

	dX = x;
	dY = y;
	dZ = z;
	dRx = rx;
	dRy = ry;
	dRz = rz;

	//dX = -400
	//dY = -200
	//dZ = -200
	//dRx = 0
	//dRy = 0
	//dRy = 0

	//int nMoveType = 0;

	//// 定义工具坐标变量
	//double dTcp_X = 0; double dTcp_Y = 0; double dTcp_Z = 0;
	//double dTcp_Rx = 0; double dTcp_Ry = 0; double dTcp_Rz = 0;
	//// 定义用户坐标变量
	//double dUcs_X = 0; double dUcs_Y = 0; double dUcs_Z = 0;
	//double dUcs_Rx = 0; double dUcs_Ry = 0; double dUcs_Rz = 0;

	//int nTargetPoint = HRIF_WayPointEx(0, 0, nMoveType, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz, dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit,

	//	dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);//机械臂移动
	int nRet_1 = HRIF_ReadActJointPos(0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6);
	//int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz,
	//	dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
	//	nIsSeek, nIOBit, nIOState, strCmdID);

	int nTargetPoint = HRIF_WayPointEx(0, 0, 1, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, finaltcp[0], finaltcp[1], finaltcp[2], finaltcp[3], finaltcp[4], finaltcp[5], 0, 0, 0, 0, 0, 0, dVelocity, dAcc, dRadius, 0,
			nIsSeek, nIOBit, nIOState, strCmdID);

	std::cout << "nTargetPoint: " << nTargetPoint << std::endl;
	if (nTargetPoint == 0) {
		m_Controls.textBrowser->append("MOVE OK");
	}
	else {
		m_Controls.textBrowser->append("MOVE FAILED");
		std::cout << nTargetPoint << std::endl;
	}
	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");

}

void ZYXtest::getProofPoint()
{
	auto vtkT_PatinetRFToCamera = vtkMatrix4x4::New();
	vtkT_PatinetRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatinetRFToCamera->Invert();

	// 获取探针在image下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	tmptrans->SetMatrix(vtkT_PatinetRFToCamera);
	tmptrans->MultiplyPoint(ProbeTop, ProofPointInit);



	PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "ProofPointInit:", 3, ProofPointInit);
	// Clean up VTK objects to prevent memory leaks
	vtkT_PatinetRFToCamera->Delete();
	tmptrans->Delete();
}

void ZYXtest::getProofPointagain()
{
	// 再次点击
	auto vtkT_PatinetRFToCamera = vtkMatrix4x4::New();
	vtkT_PatinetRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatinetRFToCamera->Invert();

	// 获取探针在image下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	tmptrans->SetMatrix(vtkT_PatinetRFToCamera);
	tmptrans->MultiplyPoint(ProbeTop, ProofPoint);
	PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "ProofPoint:", 3, ProofPoint);
	//m_Controls.textBrowser->append("ProofPoint:\n" + "X: " + QString::number(ProofPoint[0]) + "Y: " + QString::number(ProofPoint[1]) + "Z: " + QString::number(ProofPoint[2]) + "theta: " + QString::number(ProofPoint[3]));

	double dist = std::sqrt(std::pow(ProofPoint[0] - ProofPointInit[0], 2) +
		std::pow(ProofPoint[1] - ProofPointInit[1], 2) +
		std::pow(ProofPoint[2] - ProofPointInit[2], 2));

	m_Controls.textBrowser->append("Distance between ProofPointInit and ProofPoint: " + QString::number(dist));

	if (dist > 1)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("Warning"));
		msgBox.setInformativeText(QString::fromLocal8Bit("ProofPoint not fit with ProofPointInit, need to reproof"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		int ret = msgBox.exec();
	}
}

void ZYXtest::trackingProbe()
{

	//获取机械臂配准矩阵T_BaseToBaseRF
	vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
	vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);

	//获取T_BaseRFToCamera
	auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();
	vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
	vtkT_BaseRFToCamera->Invert();

	//获取T_CameraToProbe
	auto vtkT_CameraToProbe = vtkMatrix4x4::New();
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);

	//获取探针尖端的矩阵
	vtkNew<vtkMatrix4x4> probeTipMatrix; // camera to probe tip matrix
	probeTipMatrix->Identity();
	probeTipMatrix->SetElement(0, 3, ProbeTop[0]);
	probeTipMatrix->SetElement(1, 3, ProbeTop[1]);
	probeTipMatrix->SetElement(2, 3, ProbeTop[2]);


	vtkNew<vtkTransform> Transform;
	Transform->Identity();
	Transform->PostMultiply();
	Transform->SetMatrix(probeTipMatrix);
	Transform->Concatenate(vtkT_BaseRFToCamera);
	Transform->Concatenate(vtkT_BaseToBaseRF);
	Transform->Update();
	auto vtkT_BaseToProbe = Transform->GetMatrix();

	Eigen::Matrix3d Re;
	Re << vtkT_BaseToProbe->GetElement(0, 0), vtkT_BaseToProbe->GetElement(0, 1), vtkT_BaseToProbe->GetElement(0, 2),
		vtkT_BaseToProbe->GetElement(1, 0), vtkT_BaseToProbe->GetElement(1, 1), vtkT_BaseToProbe->GetElement(1, 2),
		vtkT_BaseToProbe->GetElement(2, 0), vtkT_BaseToProbe->GetElement(2, 1), vtkT_BaseToProbe->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	dX = vtkT_BaseToProbe->GetElement(0, 3);
	dY = vtkT_BaseToProbe->GetElement(1, 3);
	dZ = vtkT_BaseToProbe->GetElement(2, 3);

	//读取现在的TCP坐标，将位姿部分按照
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	int nRet1 = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);

	dRx = dRx1;
	dRy = dRy1;
	dRz = dRz1;


	int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);

	if (nTargetPoint == 0) {

		m_Controls.textBrowser->append("move succeed");
	}
	else {

		m_Controls.textBrowser->append("move failed");
		m_Controls.textBrowser->append("nRet1=" + QString::number(nRet1));
	}

}

void ZYXtest::trackingProbe1()
{
	//-------------------------------------------------------------------------------------------

	vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();//获取机械臂配准矩阵T_BaseToBaseRF
	vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);


	auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();//获取T_BaseRFToCamera
	vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
	vtkT_BaseRFToCamera->Invert();


	auto vtkT_CameraToProbe = vtkMatrix4x4::New();//获取T_CameraToProbe（tto点）
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);


	vtkNew<vtkMatrix4x4> probeTipMatrix;//获取探针尖端的矩阵
	probeTipMatrix->Identity();
	probeTipMatrix->SetElement(0, 3, ProbeTop[0]);
	probeTipMatrix->SetElement(1, 3, ProbeTop[1]);
	probeTipMatrix->SetElement(2, 3, ProbeTop[2]);

	//-------------------------------------------------------------------------------------------
	//计算在base坐标系下探针尖端点的坐标
	vtkNew<vtkTransform> Transform_0;
	Transform_0->Identity();
	Transform_0->PostMultiply();
	Transform_0->SetMatrix(probeTipMatrix);
	Transform_0->Concatenate(vtkT_BaseRFToCamera);
	Transform_0->Concatenate(vtkT_BaseToBaseRF);
	Transform_0->Update();
	auto vtkT_BaseToProbe_0 = Transform_0->GetMatrix();

	////计算在base坐标系下探针tto点的坐标
	vtkNew<vtkTransform> Transform_1;
	Transform_1->Identity();
	Transform_1->PostMultiply();
	Transform_1->SetMatrix(vtkT_CameraToProbe);
	Transform_1->Concatenate(vtkT_BaseRFToCamera);
	Transform_1->Concatenate(vtkT_BaseToBaseRF);
	Transform_1->Update();
	auto vtkT_BaseToProbe_1 = Transform_1->GetMatrix();

	//存入向量
	double targetPointUnderBase_0[3]{ vtkT_BaseToProbe_0->GetElement(0, 3), vtkT_BaseToProbe_0->GetElement(1, 3), vtkT_BaseToProbe_0->GetElement(2, 3) };
	double targetPointUnderBase_1[3]{ vtkT_BaseToProbe_1->GetElement(0, 3), vtkT_BaseToProbe_1->GetElement(1, 3), vtkT_BaseToProbe_1->GetElement(2, 3) };

	//-------------------------------------------------------------------------------------------
	//获取机械臂的T_BaseToTCP
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx1);
	tmpTrans->RotateY(dRy1);
	tmpTrans->RotateZ(dRz1);
	tmpTrans->Translate(dX1, dY1, dZ1);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToTCP = tmpTrans->GetMatrix();

	//借用TCP的X轴方向（选轴线一致）借用的是X轴，给的目标也是z轴
	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = VTKT_BaseToTCP->GetElement(0, 0);
	currentXunderBase[1] = VTKT_BaseToTCP->GetElement(1, 0);
	currentXunderBase[2] = VTKT_BaseToTCP->GetElement(2, 0);
	currentXunderBase.normalize();

	//在Base坐标系下目标坐标系X轴的方向向量
	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];

	targetXunderBase.normalize();

	//计算基于base坐标下target矩阵的旋转部分
	Eigen::Vector3d targetZunderBase;
	//targetZunderBase = currentXunderBase.cross(targetXunderBase);//这里还是有问题
	targetZunderBase = targetXunderBase.cross(currentXunderBase);
	targetZunderBase.normalize();

	Eigen::Vector3d targetYunderBase;
	targetYunderBase = targetXunderBase.cross(targetZunderBase);
	targetYunderBase.normalize();


	//-------------------------------------------------------------------------------------------
	Eigen::Matrix3d Re_target;

	Re_target << targetXunderBase[0], targetYunderBase[0], targetZunderBase[0],
		targetXunderBase[1], targetYunderBase[1], targetZunderBase[1],
		targetXunderBase[2], targetYunderBase[2], targetZunderBase[2];

	//使用矩阵转四元数转欧拉角
	Eigen::Quaterniond quaternion_target(Re_target);
	double dQuaW = quaternion_target.w();
	double dQuaX = quaternion_target.x();
	double dQuaY = quaternion_target.y();
	double dQuaZ = quaternion_target.z();
	double dRx_move = 0; double dRy_move = 0; double dRz_move = 0;
	int nRet6 = HRIF_Quaternion2RPY(0, 0, dQuaW, dQuaX, dQuaY, dQuaZ, dRx_move, dRy_move, dRz_move);

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_test");
	m_Controls.textBrowser->append("dRx=" + QString::number(dRx_move));
	m_Controls.textBrowser->append("dRy=" + QString::number(dRy_move));
	m_Controls.textBrowser->append("dRz=" + QString::number(dRz_move));


	//使用xyz欧拉角分解(先绕x轴旋转，然后绕y轴，最后绕z轴)
	Eigen::Vector3d eulerAngle_xyz = Re_target.eulerAngles(0, 1, 2);
	double dRx_xyz = 180 * eulerAngle_xyz[0] / 3.1415;
	double dRy_xyz = 180 * eulerAngle_xyz[1] / 3.1415;
	double dRz_xyz = 180 * eulerAngle_xyz[2] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_xyz");
	m_Controls.textBrowser->append("dRx=" + QString::number(dRx_xyz));
	m_Controls.textBrowser->append("dRy=" + QString::number(dRy_xyz));
	m_Controls.textBrowser->append("dRz=" + QString::number(dRz_xyz));


	//使用zyx欧拉角分解(先绕z轴转、然后绕y轴、然后绕x轴)
	Eigen::Vector3d eulerAngle_zyx = Re_target.eulerAngles(2, 1, 0);
	double dRx_zyx = 180 * eulerAngle_zyx[2] / 3.1415;
	double dRy_zyx = 180 * eulerAngle_zyx[1] / 3.1415;
	double dRz_zyx = 180 * eulerAngle_zyx[0] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_zyx");
	m_Controls.textBrowser->append("dRx=" + QString::number(dRx_zyx));
	m_Controls.textBrowser->append("dRy=" + QString::number(dRy_zyx));
	m_Controls.textBrowser->append("dRz=" + QString::number(dRz_zyx));

	//-------------------------------------------------------------------------------------------
	//控制机械臂的参数
	dX = targetPointUnderBase_0[0];
	dY = targetPointUnderBase_0[1];
	dZ = targetPointUnderBase_0[2];
	dRx = dRx_zyx;
	dRy = dRy_zyx;
	dRz = dRz_zyx;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_set");
	m_Controls.textBrowser->append("dX=" + QString::number(dX));
	m_Controls.textBrowser->append("dY=" + QString::number(dY));
	m_Controls.textBrowser->append("dZ=" + QString::number(dZ));
	m_Controls.textBrowser->append("dRx=" + QString::number(dRx));
	m_Controls.textBrowser->append("dRy=" + QString::number(dRy));
	m_Controls.textBrowser->append("dRz=" + QString::number(dRz));

	//-------------------------------------------------------------------------------------------
	int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);//机械臂移动

	if (nTargetPoint == 0) {

		m_Controls.textBrowser->append("move succeed");
	}
	else {

		m_Controls.textBrowser->append("move failed");

	}

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 使用探针收集LandMark点（软件全流程）
 * @note 本质上是是将探针的尖端点转移到F_PatientRF下
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::collectLandmark_SPI()
{
	m_IndexOfLandmark++;
	m_Controls.lineEdit_collectedLandmark_2->setText(QString::number(m_IndexOfLandmark));
	m_Controls.textBrowser->append(QString(" m_IndexOfLandmark: ") + QString::number(m_IndexOfLandmark));

	//直接获取在相机坐标系下的探针尖端的点

	vtkProbeTip_onObjRf_SPI->InsertNextPoint(ProbeTop[0], ProbeTop[1], ProbeTop[2]);

	m_Controls.textBrowser->append(QString("Probe Point Landmark: (") + QString::number(ProbeTop[0]) + ", " + QString::number(ProbeTop[1]) + ", "
		+ QString::number(ProbeTop[2]) + ")");


}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 使用探针收集ICP点（软件全流程）
 * @note 本质上是是将探针的尖端点转移到F_Image下
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::collectICP_SPI()
{
	//计数
	m_IndexOfICP++;
	m_Controls.lineEdit_collectedICP_3->setText(QString::number(m_IndexOfICP));
	m_Controls.textBrowser->append(QString(" m_IndexOfICP: ") + QString::number(m_IndexOfICP));

	//获取在图像坐标系下的探针尖端点的坐标 T_imageToCamera * 探针尖端点在相机坐标系下的坐标

	auto T_ImageToCamera_landmark = vtkMatrix4x4::New();
	T_ImageToCamera_landmark->DeepCopy(T_CameraToImage);
	T_ImageToCamera_landmark->Invert();


	//获取探针在image下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	tmptrans->SetMatrix(T_ImageToCamera_landmark);
	tmptrans->MultiplyPoint(ProbeTop, nd_tip_FImage_icp);

	vtkProbeTip_onObjRf_icp_SPI->InsertNextPoint(nd_tip_FImage_icp[0], nd_tip_FImage_icp[1], nd_tip_FImage_icp[2]);
	m_Controls.textBrowser->append(QString("Probe Point ICP: (") + QString::number(nd_tip_FImage_icp[0]) + ", " + QString::number(nd_tip_FImage_icp[1]) + ", "
		+ QString::number(nd_tip_FImage_icp[2]) + ")");

}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief LandMark配准（粗配准、点点配准）（软件全流程）
 * @note 重点要注意配的src数据和tar数据，决定了后面配准矩阵的指向
 * @note auto vtkT_patientRFtoImage = ComputeHomogeneousTransform_vtk(Patient坐标系下的点, Image坐标系下的点);
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::landmarkRegistration_SPI()
{
	//获取脊柱表面的点
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src_9->GetSelectedNode();
	mitk::PointSet::Pointer pointSet_Src = dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData());
	if (pointSet_Src.IsNotNull())
	{
		unsigned int numberOfPoints = pointSet_Src->GetSize();
		for (unsigned int i = 0; i < numberOfPoints; ++i)
		{
			// get position
			mitk::PointSet::PointType point = pointSet_Src->GetPoint(i);

			QString pointText = QString("Point %1: (%2, %3, %4)\n").arg(i + 1).arg(point[0]).arg(point[1]).arg(point[2]);
			m_Controls.textBrowser->append(pointText);
		}
	}

	//获取采集的点
	mitk::PointSet::Pointer pointSet_Tar = mitk::PointSet::New();
	vtkIdType numPoints = vtkProbeTip_onObjRf_SPI->GetNumberOfPoints();
	for (vtkIdType i = 0; i < numPoints; ++i)
	{
		double point[3];
		vtkProbeTip_onObjRf_SPI->GetPoint(i, point);

		mitk::PointSet::PointType mitkPoint;
		mitkPoint[0] = point[0];
		mitkPoint[1] = point[1];
		mitkPoint[2] = point[2];
		pointSet_Tar->SetPoint(i, mitkPoint);
	}

	if (pointSet_Tar.IsNotNull())
	{
		unsigned int numberOfPoints = pointSet_Tar->GetSize();

		for (unsigned int i = 0; i < numberOfPoints; ++i)
		{
			mitk::PointSet::PointType point = pointSet_Tar->GetPoint(i);

			QString pointText2 = QString("Point %1: (%2, %3, %4)\n").arg(i + 1).arg(point[0]).arg(point[1]).arg(point[2]);
			m_Controls.textBrowser->append(pointText2);
		}
	}

	//计算T_MetalBallRFtoImage
	auto vtkT_CameraToImage = ComputeHomogeneousTransform_vtk(pointSet_Src, pointSet_Tar);
	memcpy_s(T_CameraToImage, sizeof(double) * 16, vtkT_CameraToImage->GetData(), sizeof(double) * 16);

	//存T_PatientToImage
	//auto vtkT_CameraToImage = vtkMatrix4x4::New();
	//vtkT_CameraToImage->DeepCopy(T_CameraToImage);


	auto vtkT_PatinetRFToCamera = vtkMatrix4x4::New();
	vtkT_PatinetRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatinetRFToCamera->Invert();

	vtkNew<vtkTransform> Transform;
	Transform->PostMultiply();
	Transform->SetMatrix(vtkT_CameraToImage);
	Transform->Concatenate(vtkT_PatinetRFToCamera);
	Transform->Update();

	auto vtkT_PatinetRFToImage = Transform->GetMatrix();
	PrintMatrix("T_PatientRFtoImage_SPI", vtkT_PatinetRFToImage->GetData());
	memcpy_s(T_PatientRFtoImage_SPI, sizeof(double) * 16, vtkT_PatinetRFToImage->GetData(), sizeof(double) * 16);


}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 执行源表面和目标点集之间的ICP配准[点面配准]（软件全流程）
*@note 配准后的ICP矩阵要右乘到landmark配准矩阵进行补偿
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ICPRegistration_SPI()
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	m_IcpSourceSurface = m_Controls.mitkNodeSelectWidget_surface_regis_9->GetSelectedNode();

	auto icpTargetPointset = mitk::PointSet::New();

	for (vtkIdType i = 0; i < vtkProbeTip_onObjRf_icp_SPI->GetNumberOfPoints(); ++i)
	{
		// 获取当前点的坐标
		double* pointCoords = vtkProbeTip_onObjRf_icp_SPI->GetPoint(i);

		// 创建 MITK Point3D 对象
		mitk::Point3D point;
		point[0] = pointCoords[0];
		point[1] = pointCoords[1];
		point[2] = pointCoords[2];

		// 将 Point3D 对象添加到 PointSet 中
		icpTargetPointset->InsertPoint(i, point);
	}

	if (m_IcpSourceSurface != nullptr && icpTargetPointset != nullptr)
	{

		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();

		auto T_imageToImage_icp = vtkMatrix4x4::New();
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				// 逐个复制元素
				T_imageToImage_icp->SetElement(row, col, tmpRegistrationResult(row, col));
			}
		}

		QString output;

		output.append("Eigen::Matrix4d content:\n");
		for (int row = 0; row < tmpRegistrationResult.rows(); ++row)
		{
			for (int col = 0; col < tmpRegistrationResult.cols(); ++col)
			{
				output.append(QString::number(tmpRegistrationResult(row, col)) + " ");
			}
			output.append("\n");
		}

		m_Controls.textBrowser->append(output);

		memcpy_s(T_ImageToImage_icp_SPI, sizeof(double) * 16, T_imageToImage_icp->GetData(), sizeof(double) * 16);
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 重新进行图像配准（软件全流程）
*@note  清空的东西
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ResetImageConfiguration_SPI()
{
	m_IndexOfLandmark = 0;
	m_IndexOfICP = 0;

	m_Controls.lineEdit_collectedLandmark_2->setText(QString::number(0));
	m_Controls.lineEdit_collectedICP_3->setText(QString::number(0));

	vtkProbeTip_onObjRf_SPI->Reset();
	vtkProbeTip_onObjRf_icp_SPI->Reset();

	if (vtkProbeTip_onObjRf_SPI->GetNumberOfPoints() == 0 && vtkProbeTip_onObjRf_icp_SPI->GetNumberOfPoints() == 0)
	{
		m_Controls.textBrowser->append("Replace image configuration");
	}
	else
	{
		m_Controls.textBrowser->append("Replace image configuration failed");
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 保存图像配准矩阵（软件全流程）
*@note  保存两个矩阵T_PatientRFtoImage、T_ImageToImage_icp
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::saveImageMatrix_SPI()
{
	//存CameraToImage
	std::ofstream robotMatrixFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_CameratoImage_SPI.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile << T_CameraToImage[i];
		if (i != 15) {
			robotMatrixFile << ",";
		}
		else {
			robotMatrixFile << ";";
		}
	}
	robotMatrixFile << std::endl;
	robotMatrixFile.close();
	m_Controls.textBrowser->append("saveImageMatrix");

	//存T_ImageToImage_icp
	std::ofstream robotMatrixFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_ImageToImage_icp_SPI.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile1 << T_ImageToImage_icp_SPI[i];
		if (i != 15) {
			robotMatrixFile1 << ",";
		}
		else {
			robotMatrixFile1 << ";";
		}
	}
	robotMatrixFile1 << std::endl;
	robotMatrixFile1.close();
	m_Controls.textBrowser->append("saveImageMatrix");

	//存T_PatientToImage
	auto vtkT_CameraToImage = vtkMatrix4x4::New();
	vtkT_CameraToImage->DeepCopy(T_CameraToImage);


	auto vtkT_PatinetRFToCamera = vtkMatrix4x4::New();
	vtkT_PatinetRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatinetRFToCamera->Invert();

	vtkNew<vtkTransform> Transform;
	Transform->PostMultiply();
	Transform->SetMatrix(vtkT_CameraToImage);
	Transform->Concatenate(vtkT_PatinetRFToCamera);
	Transform->Update();

	auto vtkT_PatinetRFToImage = Transform->GetMatrix();

	memcpy_s(T_PatientRFtoImage_SPI, sizeof(double) * 16, vtkT_PatinetRFToImage->GetData(), sizeof(double) * 16);

	std::ofstream robotMatrixFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_PatientRFToImage_SPI.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile2 << T_PatientRFtoImage_SPI[i];
		if (i != 15) {
			robotMatrixFile2 << ",";
		}
		else {
			robotMatrixFile2 << ";";
		}
	}
	robotMatrixFile2 << std::endl;
	robotMatrixFile2.close();
	m_Controls.textBrowser->append("saveImageMatrix");


}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 用于开一个QT定时器刷新探针、和TCP坐标系的显示（软件全流程）
*@note  这里用了一个函数改变了探针的颜色为红色
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateDeviation_SPI()
{
	connect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::Updatedata_SPI);
	connect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::UpdateTCP_SPI);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在MITK里面显示探针
*@note  目前探针的尖端显示正确、探针的位姿还不太正确（软件全流程）
*@note  用的时候要把探针的模型拖入MITK，探针的模型名称为probe
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::Updatedata_SPI()//更新显示的数据
{
	if (GetDataStorage()->GetNamedNode("probe") == nullptr)
	{
		m_Controls.textBrowser->append("probe is missing");
		return;
	}
	this->RequestRenderWindowUpdate();


	auto vtkT_CameraToProbe = vtkMatrix4x4::New();
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);

	auto vtkT_ImageToCamera = vtkMatrix4x4::New();
	vtkT_ImageToCamera->DeepCopy(T_CameraToImage);
	vtkT_ImageToCamera->Invert();

	vtkNew<vtkMatrix4x4> probeTipMatrix; // camera to probe tip matrix
	probeTipMatrix->Identity();
	probeTipMatrix->SetElement(0, 3, ProbeTop[0]);
	probeTipMatrix->SetElement(1, 3, ProbeTop[1]);
	probeTipMatrix->SetElement(2, 3, ProbeTop[2]);

	probeTipMatrix->SetElement(0, 0, vtkT_CameraToProbe->GetElement(0, 0));
	probeTipMatrix->SetElement(0, 1, vtkT_CameraToProbe->GetElement(0, 1));
	probeTipMatrix->SetElement(0, 2, vtkT_CameraToProbe->GetElement(0, 2));
	probeTipMatrix->SetElement(1, 0, vtkT_CameraToProbe->GetElement(1, 0));
	probeTipMatrix->SetElement(1, 1, vtkT_CameraToProbe->GetElement(1, 1));
	probeTipMatrix->SetElement(1, 2, vtkT_CameraToProbe->GetElement(1, 2));
	probeTipMatrix->SetElement(2, 0, vtkT_CameraToProbe->GetElement(2, 0));
	probeTipMatrix->SetElement(2, 1, vtkT_CameraToProbe->GetElement(2, 1));
	probeTipMatrix->SetElement(2, 2, vtkT_CameraToProbe->GetElement(2, 2));



	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(probeTipMatrix);
	tmpTrans->Concatenate(vtkT_ImageToCamera);

	tmpTrans->Update();

	auto vtkT_T_imageToProbe = tmpTrans->GetMatrix();
	memcpy_s(T_imageToProbe, sizeof(double) * 16, vtkT_T_imageToProbe->GetData(), sizeof(double) * 16);

	GetDataStorage()->GetNamedNode("probe")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkT_T_imageToProbe);//导航的影像
	GetDataStorage()->GetNamedNode("probe")->GetData()->GetGeometry()->Modified();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 用于将ICP配准出来的矩阵配准（软件全流程）
*@note  注意Landmark矩阵*ICP矩阵
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::addICP_SPI()
{

	auto vtkT_CameraToImage = vtkMatrix4x4::New();
	vtkT_CameraToImage->DeepCopy(T_CameraToImage);

	auto vtkT_ImageToICP = vtkMatrix4x4::New();
	vtkT_ImageToICP->DeepCopy(T_ImageToImage_icp_SPI);

	vtkNew<vtkTransform> Transform;
	Transform->PostMultiply();
	Transform->SetMatrix(vtkT_ImageToICP);
	Transform->Concatenate(vtkT_CameraToImage);
	Transform->Update();

	auto vtkT_PatientToICP = Transform->GetMatrix();
	memcpy_s(vtkT_CameraToImage, sizeof(double) * 16, vtkT_PatientToICP->GetData(), sizeof(double) * 16);
	PrintMatrix("vtkT_PatientToICP", vtkT_PatientToICP->GetData());

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 将图像配装矩阵的金属球点全保存到TXT（软件全流程）
*@note  注意点的顺序
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::saveMetalPoint()
{
	//计算T_ImageToMetalRF
	auto vtkT_ImageToCamera = vtkMatrix4x4::New();
	vtkT_ImageToCamera->DeepCopy(T_CameraToImage);
	vtkT_ImageToCamera->Invert();

	auto vtkT_CameraToMetalBallRF = vtkMatrix4x4::New();
	vtkT_CameraToMetalBallRF->DeepCopy(T_CamToMetalBallRF);

	vtkNew<vtkTransform> Transform;
	Transform->PostMultiply();
	Transform->SetMatrix(vtkT_CameraToMetalBallRF);
	Transform->Concatenate(vtkT_ImageToCamera);
	Transform->Update();

	auto vtkT_ImageToMetalBallRF = Transform->GetMatrix();

	memcpy_s(T_ImageToMetalRF, sizeof(double) * 16, vtkT_ImageToMetalBallRF->GetData(), sizeof(double) * 16);

	//存T_ImageToMetalRF
	std::ofstream robotMatrixFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_ImageToMetalRF_SPI.txt");
	for (int i = 0; i < 16; i++) {
		robotMatrixFile << T_ImageToMetalRF[i];
		if (i != 15) {
			robotMatrixFile << ",";
		}
		else {
			robotMatrixFile << ";";
		}
	}
	robotMatrixFile << std::endl;
	robotMatrixFile.close();

	//在金属球坐标系下硬件给的数据


	Eigen::Matrix<double, 4, 7> MetalBallPoint;
	MetalBallPoint <<
		28.531, 17.169, 1.049, -13.127, -7.222, -10.462, -29.41,
		1.558, 20.771, 30.691, 15.19, -30.659, -10.091, -1.063,
		-36.9, -34.9, -36.9, -34.9, -32.9, -34.9, -36.9,
		1, 1, 1, 1, 1, 1, 1;

	Eigen::Matrix<double, 4, 4> T_ImageToMetalBallRF;
	// 将 vtkMatrix4x4 的值赋给 Eigen 矩阵
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			T_ImageToMetalBallRF(i, j) = static_cast<double>(vtkT_ImageToMetalBallRF->GetElement(i, j));
		}
	}

	Eigen::Matrix<double, 4, 7> MetalBall = T_ImageToMetalBallRF * MetalBallPoint;

	m_Controls.textBrowser->append("MetalBall: ");
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			m_Controls.textBrowser->append(QString::number(MetalBall(i, j)) + " ");
		}
		m_Controls.textBrowser->append("");
	}

	std::ofstream file(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MetalBallPoint_SPI.txt");
	if (file.is_open()) {
		for (int j = 0; j < 7; ++j) {
			for (int i = 0; i < 3; ++i) {

				file << MetalBall(i, j) << ",";
			}
			file << "\n";
		}
		file.close();
	}
	else {
		m_Controls.textBrowser->append("failed");


	}


}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在MITK中显示TCP坐标系（软件全流程）
*@note  用的时候要把坐标系的模型拖入MITK，探针的模型名称为Coordinate_Label
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateTCP_SPI()//更新显示的数据
{
	if (GetDataStorage()->GetNamedNode("Coordinate_Label") == nullptr)
	{
		m_Controls.textBrowser->append("Coordinate_Label is missing");
		return;
	}
	this->RequestRenderWindowUpdate();


	auto vtkT_CameraToBaseRF = vtkMatrix4x4::New();
	vtkT_CameraToBaseRF->DeepCopy(T_CamToBaseRF);

	auto vtkT_PatientRFToCamera = vtkMatrix4x4::New();
	vtkT_PatientRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatientRFToCamera->Invert();

	auto vtkT_imageToPatientRF = vtkMatrix4x4::New();
	vtkT_imageToPatientRF->DeepCopy(T_PatientRFtoImage_SPI);
	vtkT_imageToPatientRF->Invert();


	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);
	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	vtkMatrix4x4* vtkT_BaseRFToBase = vtkMatrix4x4::New();
	vtkT_BaseRFToBase->DeepCopy(T_BaseToBaseRF);
	vtkT_BaseRFToBase->Invert();




	auto tmpTrans1 = vtkTransform::New();
	tmpTrans1->Identity();
	tmpTrans1->PostMultiply();
	tmpTrans1->SetMatrix(VTKT_BaseToFlanger);
	tmpTrans1->Concatenate(vtkT_BaseRFToBase);
	tmpTrans1->Concatenate(vtkT_CameraToBaseRF);
	tmpTrans1->Concatenate(vtkT_PatientRFToCamera);
	tmpTrans1->Concatenate(vtkT_imageToPatientRF);
	tmpTrans1->Update();

	auto vtkT_imageToFlanger = tmpTrans1->GetMatrix();

	GetDataStorage()->GetNamedNode("Coordinate_Label")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkT_imageToFlanger);//导航的影像
	GetDataStorage()->GetNamedNode("Coordinate_Label")->GetData()->GetGeometry()->Modified();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 将金属球的点加入MITK中形成一个点击
*@note  注意点的顺序
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ImportPointSet()
{
	mitk::PointSet::Pointer pointSet = mitk::PointSet::New();
	/*auto data = ReadTextFileAsPoints("C:\\Users\\admin\\Desktop\\save\\MetalBallPoint_SPI.txt");*/
	auto data = ReadTextFileAsPoints(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MetalBallPoint_SPI.txt");

	for (int i = 0; i < data.size(); i++)
	{
		mitk::PointSet::PointType pt;
		pt[0] = data[i][0];
		pt[1] = data[i][1];
		pt[2] = data[i][2];
		PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "d", 3, data[i].data());
		pointSet->SetPoint(i, pt);
	}

	mitk::DataNode::Pointer pointSetNode2 = mitk::DataNode::New();
	pointSetNode2->SetData(pointSet);
	pointSetNode2->SetName("ballCenters_image");
	pointSetNode2->SetColor(1.0, 0.0, 0.0);
	pointSetNode2->SetFloatProperty("pointsize", 3);
	GetDataStorage()->Add(pointSetNode2);
	m_Controls.textBrowser->append("already add point");
	auto iRenderWindowPart = GetRenderWindowPart();
	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");
	renderWindow->ResetView();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 读取TXT中的数，形成点集合
*@note  注意点的顺序
*/
//---------------------------------------------------------------------------------------------------------------
std::vector<Eigen::Vector3d> ZYXtest::ReadTextFileAsPoints(std::string path)
{
	std::vector<Eigen::Vector3d> result;
	std::string line;
	std::ifstream inputFile(path);

	if (!inputFile.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return result;
	}

	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		std::string token;
		std::vector<double> row;

		while (std::getline(iss, token, ',')) {
			try {
				row.push_back(std::stod(token)); // Convert the string to a double
			}
			catch (const std::invalid_argument & e) {
				std::cerr << "Invalid number: " << token << std::endl;
				continue;
			}
		}

		if (row.size() >= 3) { // Ensure there are at least 3 elements in the row
			Eigen::Vector3d array(row[0], row[1], row[2]);
			PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "", 3, array.data());
			result.push_back(array);
		}
	}

	inputFile.close();
	return result;
}

//table6  test
void ZYXtest::test_1()
{
	// 定义增量半径
	double dSpiralIncrement = 1;
	// 定义结束半径
	double dSpiralDiameter = 5;
	// 定义运动速度
	double dVelocity = 50;
	// 定义运动加速度
	double dAcc = 50;
	// 定义过渡半径
	double dRadius = 50;
	// 定义路点 ID 
	string strCmdID = "0";
	// 执行螺旋轨迹运动
	/*int nRet = HRIF_MoveS(0, 0, dSpiralIncrement, dSpiralDiameter, ,  dVelocity, dAcc, dRadius,strCmdID);*/
	int nRet = HRIF_MoveS(0, 0, dSpiralIncrement, dSpiralDiameter, dVelocity, dAcc, dRadius, sTcpName, sUcsName, strCmdID);
	PrintResult(m_Controls.textBrowser, nRet, "Move");

}

void ZYXtest::test_2()
{
	//确定目标线：会读取线数据导入到P2和P3中
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);
	std::cout << "targetPoint_0: (" << targetPoint_0[0] << ", " << targetPoint_0[1] << ", " << targetPoint_0[2] << ")" << std::endl;
	std::cout << "targetPoint_1: (" << targetPoint_1[0] << ", " << targetPoint_1[1] << ", " << targetPoint_1[2] << ")" << std::endl;


	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };

	for (int i{ 0 }; i < 20; i++)
	{
		//获取机械臂配准矩阵T_BaseToBaseRF
		vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
		vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);


		//获取T_BaseRFToCamera
		auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();
		vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
		vtkT_BaseRFToCamera->Invert();

		//获取T_CameraToPatientRF
		auto vtkT_CameraToPatientRF = vtkMatrix4x4::New();
		vtkT_CameraToPatientRF->DeepCopy(T_CamToPatientRF);

		//获取T_PatientRFToImage
		auto vtkT_PatientRFToImage = vtkMatrix4x4::New();
		vtkT_PatientRFToImage->DeepCopy(T_PatientRFtoImage);



		//计算T_BaseToImage
		vtkNew<vtkTransform> Transform;
		Transform->Identity();
		Transform->PostMultiply();
		Transform->SetMatrix(vtkT_PatientRFToImage);
		Transform->Concatenate(vtkT_CameraToPatientRF);
		Transform->Concatenate(vtkT_BaseRFToCamera);
		Transform->Concatenate(vtkT_BaseToBaseRF);
		Transform->Update();
		auto vtkT_BaseToImage = Transform->GetMatrix();


		//获取P0点的坐标
		auto TargetMatrix_0 = vtkMatrix4x4::New();
		TargetMatrix_0->SetElement(0, 3, targetPoint_0[0]);
		TargetMatrix_0->SetElement(1, 3, targetPoint_0[1]);
		TargetMatrix_0->SetElement(2, 3, targetPoint_0[2]);

		vtkNew<vtkTransform> Trans;
		Trans->Identity();
		Trans->PostMultiply();
		Trans->SetMatrix(TargetMatrix_0);
		Trans->Concatenate(vtkT_BaseToImage);
		Trans->Update();
		auto vtkT_BaseToTarget_0 = Trans->GetMatrix();

		//获取P1点的坐标
		auto TargetMatrix_1 = vtkMatrix4x4::New();
		TargetMatrix_1->SetElement(0, 3, targetPoint_1[0]);
		TargetMatrix_1->SetElement(1, 3, targetPoint_1[1]);
		TargetMatrix_1->SetElement(2, 3, targetPoint_1[2]);

		vtkNew<vtkTransform> Trans1;
		Trans1->Identity();
		Trans1->PostMultiply();
		Trans1->SetMatrix(TargetMatrix_1);
		Trans1->Concatenate(vtkT_BaseToImage);
		Trans1->Update();
		auto vtkT_BaseToTarget_1 = Trans1->GetMatrix();

		//计算20个点
		targetPointUnderBase_0[0] += vtkT_BaseToTarget_0->GetElement(0, 3);
		targetPointUnderBase_0[1] += vtkT_BaseToTarget_0->GetElement(1, 3);
		targetPointUnderBase_0[2] += vtkT_BaseToTarget_0->GetElement(2, 3);

		targetPointUnderBase_1[0] += vtkT_BaseToTarget_1->GetElement(0, 3);
		targetPointUnderBase_1[1] += vtkT_BaseToTarget_1->GetElement(1, 3);
		targetPointUnderBase_1[2] += vtkT_BaseToTarget_1->GetElement(2, 3);

	}
	//取平均
	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	//获取机械臂的T_BaseToFlanger
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx1);
	tmpTrans->RotateY(dRy1);
	tmpTrans->RotateZ(dRz1);
	tmpTrans->Translate(dX1, dY1, dZ1);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	//借用法兰的X轴方向
	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = VTKT_BaseToFlanger->GetElement(0, 0);
	currentXunderBase[1] = VTKT_BaseToFlanger->GetElement(1, 0);
	currentXunderBase[2] = VTKT_BaseToFlanger->GetElement(2, 0);
	currentXunderBase.normalize();


	std::cout << "currentxunderBase: " << currentXunderBase << std::endl;

	//在Base坐标系下目标坐标系X轴的方向向量
	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];
	targetXunderBase.normalize();

	MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d  rotationAxis;
	rotationAxis = currentXunderBase.cross(targetXunderBase);


	double rotationAngle;//定义旋转角
	if (currentXunderBase.dot(targetXunderBase) > 0) //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;//求向量的模长在取反三角
	}
	else //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(VTKT_BaseToFlanger);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);//旋转角度，和旋转向量
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();

	Eigen::Matrix3d Re;

	Re << testMatrix->GetElement(0, 0), testMatrix->GetElement(0, 1), testMatrix->GetElement(0, 2),
		testMatrix->GetElement(1, 0), testMatrix->GetElement(1, 1), testMatrix->GetElement(1, 2),
		testMatrix->GetElement(2, 0), testMatrix->GetElement(2, 1), testMatrix->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double x = targetPointUnderBase_0[0];
	double y = targetPointUnderBase_0[1];
	double z = targetPointUnderBase_0[2];
	double rx = 180 * eulerAngle[2] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[0] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_move");
	m_Controls.textBrowser->append("dx=" + QString::number(x));
	m_Controls.textBrowser->append("dy=" + QString::number(y));
	m_Controls.textBrowser->append("dz=" + QString::number(z));
	m_Controls.textBrowser->append("dRx=" + QString::number(rx));
	m_Controls.textBrowser->append("dRy=" + QString::number(ry));
	m_Controls.textBrowser->append("dRz=" + QString::number(rz));

	dX = x;
	dY = y;
	dZ = z;
	dRx = rx;
	dRy = ry;
	dRz = rz;


	int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);//机械臂移动

	if (nTargetPoint == 0) {
		m_Controls.textBrowser->append("MOVE OK");
	}
	else {
		m_Controls.textBrowser->append("MOVE FAILED");
	}


}

void ZYXtest::test_3()
{
	QString position_text;
	std::vector<std::string> toolidarr;
	toolidarr.push_back("Spine_RobotBaseRF");
	toolidarr.push_back("Spine_RobotEndRF");
	toolidarr.push_back("Spine_Probe");
	toolidarr.push_back("Spine_PatientRF");

	rlt = Aim_GetMarkerAndStatusFromHardware(aimHandle, I_ETHERNET, markerSt, statusSt);

	T_AimToolDataResult* mtoolsrlt = new T_AimToolDataResult;//新建一个值指，将指针清空用于存数据
	mtoolsrlt->next = NULL;
	mtoolsrlt->validflag = false;

	rlt = Aim_FindToolInfo(aimHandle, markerSt, mtoolsrlt, 0);//获取数据
	T_AimToolDataResult* prlt = mtoolsrlt;//将获取完数据的从mtoolsrlt给prlt指针


	if (rlt == AIMOOE_OK)//判断是否采集成功
	{
		do
		{
			//获取Spine_RobotBaseRF
			if (strcmp(prlt->toolname, "Spine_RobotBaseRF") == 0)
			{
				if (prlt->validflag)
				{
					//获取相机数据
					t_tran[0] = prlt->Tto[0];
					t_tran[1] = prlt->Tto[1];
					t_tran[2] = prlt->Tto[2];
					for (int i = 0; i < 3; ++i)
					{
						for (int j = 0; j < 3; ++j)
						{
							R_tran[i][j] = prlt->Rto[i][j];
						}
					}

					//拼接矩阵
					vtkNew<vtkMatrix4x4> m_T_temp1;
					CombineRotationTranslation(R_tran, t_tran, m_T_temp1);
					memcpy_s(T_CamToBaseRF, sizeof(double) * 16, m_T_temp1->GetData(), sizeof(double) * 16);

					//改边QT界面（变色+显示坐标）

				}
				else
				{
					//  QT界面变红

				}

			}

			//获取Spine_Probe数据
			if (strcmp(prlt->toolname, "Spine_Probe") == 0)
			{
				if (prlt->validflag)
				{
					//获取相机数据
					t_tran[0] = prlt->Tto[0];
					t_tran[1] = prlt->Tto[1];
					t_tran[2] = prlt->Tto[2];
					for (int i = 0; i < 3; ++i)
					{
						for (int j = 0; j < 3; ++j)
						{
							R_tran[i][j] = prlt->Rto[i][j];
						}
					}

					//拼接矩阵
					vtkNew<vtkMatrix4x4> m_T_temp2;
					CombineRotationTranslation(R_tran, t_tran, m_T_temp2);
					memcpy_s(T_CamToProbe, sizeof(double) * 16, m_T_temp2->GetData(), sizeof(double) * 16);
					ProbeTop[0] = prlt->tooltip[0];
					ProbeTop[1] = prlt->tooltip[1];
					ProbeTop[2] = prlt->tooltip[2];

					//改边QT界面（变色+显示坐标）

				}
				else
				{
					//  QT界面变红

				}

			}

			//获取Spine_RobotEndRF数据
			if (strcmp(prlt->toolname, "Spine_RobotEndRF") == 0)
			{
				if (prlt->validflag)
				{
					//获取相机数据
					t_tran[0] = prlt->Tto[0];
					t_tran[1] = prlt->Tto[1];
					t_tran[2] = prlt->Tto[2];
					for (int i = 0; i < 3; ++i)
					{
						for (int j = 0; j < 3; ++j)
						{
							R_tran[i][j] = prlt->Rto[i][j];
						}
					}

					//拼接矩阵
					vtkNew<vtkMatrix4x4> m_T_temp3;
					CombineRotationTranslation(R_tran, t_tran, m_T_temp3);
					memcpy_s(T_CamToEndRF, sizeof(double) * 16, m_T_temp3->GetData(), sizeof(double) * 16);

					//改边QT界面（变色+显示坐标）

				}
				else
				{
					//  QT界面变红

				}

			}

			//获取Spine_PatientRF数据
			if (strcmp(prlt->toolname, "Spine_PatientRF") == 0)
			{
				if (prlt->validflag)
				{
					//获取相机数据
					t_tran[0] = prlt->Tto[0];
					t_tran[1] = prlt->Tto[1];
					t_tran[2] = prlt->Tto[2];
					for (int i = 0; i < 3; ++i)
					{
						for (int j = 0; j < 3; ++j)
						{
							R_tran[i][j] = prlt->Rto[i][j];
						}
					}

					//拼接矩阵
					vtkNew<vtkMatrix4x4> m_T_temp4;
					CombineRotationTranslation(R_tran, t_tran, m_T_temp4);
					memcpy_s(T_CamToPatientRF, sizeof(double) * 16, m_T_temp4->GetData(), sizeof(double) * 16);

					//改边QT界面（变色+显示坐标）

				}
				else
				{
					//  QT界面变红

				}

			}

			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);
		/*cout << endl;*/

	}
	else
	{
		delete prlt;
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 使用快捷键干一些小动作
*@note  这里是将配准矩阵复用进去了
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::keyPressEvent()
{
	m_Controls.textBrowser->append("-----------------------------------------------------------------------------------------");
	//导入T_PatientRFtoImage
	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_PatientRFtoImage.txt");
	if (inputFile.is_open()) {
		std::string line;
		if (std::getline(inputFile, line)) {
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ',')) {
				T_PatientRFtoImage[index] = std::stod(token);
				index++;
			}
		}
		inputFile.close();
	}
	else {

		m_Controls.textBrowser->append("无法打开文件:T_PatientRFtoImage.txt");
	}

	//打印T_BaseToBaseRF
	QString output;
	for (int i = 0; i < 16; i++) {
		output += "T_PatientRFtoImage[" + QString::number(i) + "]: " + QString::number(T_PatientRFtoImage[i]) + " ";
	}
	m_Controls.textBrowser->append(output);

	m_Controls.textBrowser->append("-----------------------------------------------------------------------------------------");

	//导入T_FlangetoEndRF
	std::ifstream inputFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_FlangeToEndRF.txt");
	if (inputFile1.is_open()) {
		std::string line;
		if (std::getline(inputFile1, line)) {
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ',')) {
				T_FlangeToEdnRF[index] = std::stod(token);
				index++;
			}
		}
		inputFile1.close();
	}
	else {

		m_Controls.textBrowser->append("无法打开文件:FlangeToEdnRF.txt");
	}

	//打印FlangeToEdnRF
	QString output1;
	for (int i = 0; i < 16; i++) {
		output1 += "T_FlangeToEdnRF[" + QString::number(i) + "]: " + QString::number(T_FlangeToEdnRF[i]) + " ";
	}
	m_Controls.textBrowser->append(output1);

	m_Controls.textBrowser->append("-----------------------------------------------------------------------------------------");

	//导入T_BaseToBaseRF
	std::ifstream inputFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_BaseToBaseRF.txt");
	if (inputFile2.is_open()) {
		std::string line;
		if (std::getline(inputFile2, line)) {
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ',')) {
				T_BaseToBaseRF[index] = std::stod(token);
				index++;
			}
		}
		inputFile2.close();
	}
	else {

		m_Controls.textBrowser->append("无法打开文件:T_BaseToBaseRF.txt");
	}

	//打印T_BaseToBaseRF
	QString output2;
	for (int i = 0; i < 16; i++) {
		output2 += "T_BaseToBaseRF[" + QString::number(i) + "]: " + QString::number(T_BaseToBaseRF[i]) + " ";
	}
	m_Controls.textBrowser->append(output2);
	m_Controls.textBrowser->append("-----------------------------------------------------------------------------------------");




}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录A点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readA()
{
	m_Controls.textBrowser->append("Set  Point A");

	int result = HRIF_ReadActPos(0, 0, g_X_A, g_Y_A, g_Z_A, g_Rx_A, g_Ry_A, g_Rz_A, dJ1_A, dJ2_A, dJ3_A, dJ4_A,
		dJ5_A, dJ6_A, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_A << ", Y = " << g_Y_A << ", Z = " << g_Z_A
			<< ", Rx = " << g_Rx_A << ", Ry = " << g_Ry_A << ", Rz = " << g_Rz_A << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录B点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readB()
{
	m_Controls.textBrowser->append("Set  Point B");

	int result = HRIF_ReadActPos(0, 0, g_X_B, g_Y_B, g_Z_B, g_Rx_B, g_Ry_B, g_Rz_B, dJ1_B, dJ2_B, dJ3_B, dJ4_B,
		dJ5_B, dJ6_B, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_B << ", Y = " << g_Y_B << ", Z = " << g_Z_B
			<< ", Rx = " << g_Rx_B << ", Ry = " << g_Ry_B << ", Rz = " << g_Rz_B << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录C点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readC()
{
	m_Controls.textBrowser->append("Set  Point C");
	int result = HRIF_ReadActPos(0, 0, g_X_C, g_Y_C, g_Z_C, g_Rx_C, g_Ry_C, g_Rz_C, dJ1_C, dJ2_C, dJ3_C, dJ4_C,
		dJ5_C, dJ6_C, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		std::cout << "Initial Position：X = " << g_X_C << ", Y = " << g_Y_C << ", Z = " << g_Z_C
			<< ", Rx = " << g_Rx_C << ", Ry = " << g_Ry_C << ", Rz = " << g_Rz_C << std::endl;
	}
	else {
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录D点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readD()
{
	m_Controls.textBrowser->append("Set  Point D");
	int result = HRIF_ReadActPos(0, 0, g_X_D, g_Y_D, g_Z_D, g_Rx_D, g_Ry_D, g_Rz_D, dJ1_D, dJ2_D, dJ3_D, dJ4_D,
		dJ5_D, dJ6_D, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_D << ", Y = " << g_Y_D << ", Z = " << g_Z_D
			<< ", Rx = " << g_Rx_D << ", Ry = " << g_Ry_D << ", Rz = " << g_Rz_D << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录E点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readE()
{
	m_Controls.textBrowser->append("Set  Point E");
	int result = HRIF_ReadActPos(0, 0, g_X_E, g_Y_E, g_Z_E, g_Rx_E, g_Ry_E, g_Rz_E, dJ1_E, dJ2_E, dJ3_E, dJ4_E,
		dJ5_E, dJ6_E, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_E << ", Y = " << g_Y_E << ", Z = " << g_Z_E
			<< ", Rx = " << g_Rx_E << ", Ry = " << g_Ry_E << ", Rz = " << g_Rz_E << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录F点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readF()
{
	m_Controls.textBrowser->append("Set  Point F");
	int result = HRIF_ReadActPos(0, 0, g_X_F, g_Y_F, g_Z_F, g_Rx_F, g_Ry_F, g_Rz_F, dJ1_F, dJ2_F, dJ3_F, dJ4_F,
		dJ5_F, dJ6_F, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_F << ", Y = " << g_Y_F << ", Z = " << g_Z_F
			<< ", Rx = " << g_Rx_F << ", Ry = " << g_Ry_F << ", Rz = " << g_Rz_F << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录G点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readG()
{
	m_Controls.textBrowser->append("Set  Point G");
	int result = HRIF_ReadActPos(0, 0, g_X_G, g_Y_G, g_Z_G, g_Rx_G, g_Ry_G, g_Rz_G, dJ1_G, dJ2_G, dJ3_G, dJ4_G,
		dJ5_G, dJ6_G, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_G << ", Y = " << g_Y_G << ", Z = " << g_Z_G
			<< ", Rx = " << g_Rx_G << ", Ry = " << g_Ry_G << ", Rz = " << g_Rz_G << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录H点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readH()
{
	m_Controls.textBrowser->append("Set  Point H");
	int result = HRIF_ReadActPos(0, 0, g_X_H, g_Y_H, g_Z_H, g_Rx_H, g_Ry_H, g_Rz_H, dJ1_H, dJ2_H, dJ3_H, dJ4_H,
		dJ5_H, dJ6_H, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_H << ", Y = " << g_Y_H << ", Z = " << g_Z_H
			<< ", Rx = " << g_Rx_H << ", Ry = " << g_Ry_H << ", Rz = " << g_Rz_H << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂记录J点
*@note  记录关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readJ()
{
	m_Controls.textBrowser->append("Set  Point J");
	int result = HRIF_ReadActPos(0, 0, g_X_J, g_Y_J, g_Z_J, g_Rx_J, g_Ry_J, g_Rz_J, dJ1_J, dJ2_J, dJ3_J, dJ4_J,
		dJ5_J, dJ6_J, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X_J << ", Y = " << g_Y_J << ", Z = " << g_Z_J
			<< ", Rx = " << g_Rx_J << ", Ry = " << g_Ry_J << ", Rz = " << g_Rz_J << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到A点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoA()
{

	m_Controls.textBrowser->append("Goto Point A");

	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态

	HRIF_MoveJ(0, 0, g_X_A, g_Y_A, g_Z_A, g_Rx_A, g_Ry_A, g_Rz_A,
		dJ1_A, dJ2_A, dJ3_A, dJ4_A, dJ5_A, dJ6_A, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到B点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoB()
{
	m_Controls.textBrowser->append("Goto Point B");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_B, g_Y_B, g_Z_B, g_Rx_B, g_Ry_B, g_Rz_B,
		dJ1_B, dJ2_B, dJ3_B, dJ4_B, dJ5_B, dJ6_B, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);


}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到C点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoC()
{
	m_Controls.textBrowser->append("Goto Point C");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态

	HRIF_MoveJ(0, 0, g_X_C, g_Y_C, g_Z_C, g_Rx_C, g_Ry_C, g_Rz_C,
		dJ1_C, dJ2_C, dJ3_C, dJ4_C, dJ5_C, dJ6_C, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到D点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoD()
{
	m_Controls.textBrowser->append("Goto Point D");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_D, g_Y_D, g_Z_D, g_Rx_D, g_Ry_D, g_Rz_D,
		dJ1_D, dJ2_D, dJ3_D, dJ4_D, dJ5_D, dJ6_D, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到E点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoE()
{
	m_Controls.textBrowser->append("Goto Point E");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_E, g_Y_E, g_Z_E, g_Rx_E, g_Ry_E, g_Rz_E,
		dJ1_E, dJ2_E, dJ3_E, dJ4_E, dJ5_E, dJ6_E, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到F点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoF()
{
	m_Controls.textBrowser->append("Goto Point F");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_F, g_Y_F, g_Z_F, g_Rx_F, g_Ry_F, g_Rz_F,
		dJ1_F, dJ2_F, dJ3_F, dJ4_F, dJ5_F, dJ6_F, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到G点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoG()
{
	m_Controls.textBrowser->append("Goto Point G");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_G, g_Y_G, g_Z_G, g_Rx_G, g_Ry_G, g_Rz_G,
		dJ1_G, dJ2_G, dJ3_G, dJ4_G, dJ5_G, dJ6_G, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到H点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoH()
{
	m_Controls.textBrowser->append("Goto Point H");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_H, g_Y_H, g_Z_H, g_Rx_H, g_Ry_H, g_Rz_H,
		dJ1_H, dJ2_H, dJ3_H, dJ4_H, dJ5_H, dJ6_H, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 机械臂回到j点
*@note  读取关节角
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::gotoJ()
{
	m_Controls.textBrowser->append("Goto Point J");
	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态
	HRIF_MoveJ(0, 0, g_X_J, g_Y_J, g_Z_J, g_Rx_J, g_Ry_J, g_Rz_J,
		dJ1_J, dJ2_J, dJ3_J, dJ4_J, dJ5_J, dJ6_J, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}

void ZYXtest::AutoRegistration()
{


}



//---------------------------------------------------------------------------------------------------------------
/**
*@brief 打开自由拖动
*@note  需要将末端IO触发自由拖动关闭
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::OpenFreeDrag()
{
	int nRet = HRIF_GrpOpenFreeDriver(0, 0);
	PrintResult(m_Controls.textBrowser, nRet, "Open free drag");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 关闭自由拖动
*@note  需要将末端IO触发自由拖动关闭
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::TurnOffFreeDrag()
{
	int nRet = HRIF_GrpCloseFreeDriver(0, 0);
	PrintResult(m_Controls.textBrowser, nRet, "Turn off free drag");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 暂停运动
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::PauseMotion()
{
	int nRet = HRIF_GrpInterrupt(0, 0);
	PrintResult(m_Controls.textBrowser, nRet, "The robotic arm pauses");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 继续运动
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::KeepMoving()
{
	int nRet = HRIF_GrpContinue(0, 0);
	PrintResult(m_Controls.textBrowser, nRet, "The robotic arm continues to move");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 开启Tool运动模式
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::openToolMotion()
{
	int nState = 1;
	int nRet = HRIF_SetToolMotion(0, 0, nState);
	PrintResult(m_Controls.textBrowser, nRet, "open Too lMotion");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 关闭Tool运动模式
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::closeToolMotion()
{
	int nState = 0;
	int nRet = HRIF_SetToolMotion(0, 0, nState);
	PrintResult(m_Controls.textBrowser, nRet, "close Tool Motion ");

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 设置TCP
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::setTCP()
{
	dX = m_Controls.TCP_X->text().toDouble();
	dY = m_Controls.TCP_Y->text().toDouble();
	dZ = m_Controls.TCP_Z->text().toDouble();
	dRx = m_Controls.TCP_RX->text().toDouble();
	dRy = m_Controls.TCP_RY->text().toDouble();
	dRz = m_Controls.TCP_RZ->text().toDouble();
	PrintTCP_qt(m_Controls.textBrowser, dX, dY, dZ, dRx, dRy, dRz);
	int nRet1 = HRIF_SetTCP(0, 0, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz);
	PrintResult(m_Controls.textBrowser, nRet1, "TCP Set");

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 读取TCP
*@note   是在Base坐标系下的TCP值
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::readTCP()
{
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);
	PrintResult(m_Controls.textBrowser, nRet, "readTCP");
	PrintTCP_qt(m_Controls.textBrowser, dX, dY, dZ, dRx, dRy, dRz);
}

void ZYXtest::xp1()
{
	m_Controls.textBrowser->append("xp");
	unsigned int boxID = 0;
	unsigned int rbtID = 0;
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 0;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(boxID, rbtID, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append(QString::number(nMoveRelL));

	m_Controls.textBrowser->append("xp finish");

}

void ZYXtest::yp1()
{
	m_Controls.textBrowser->append("yp");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 1;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));
	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("yp finish");

}

void ZYXtest::zp1()
{
	m_Controls.textBrowser->append("zp");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 2;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;
	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));
	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("zp finish");

}

void ZYXtest::xm1()
{
	m_Controls.textBrowser->append("xm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 0;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("xm finish");

}

void ZYXtest::ym1()
{
	m_Controls.textBrowser->append("ym");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 1;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("ym finish");

}

void ZYXtest::zm1()
{
	m_Controls.textBrowser->append("zm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 2;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_11->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("zm finish");

}

void ZYXtest::rxp1()
{
	m_Controls.textBrowser->append("rxp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 3;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rxp finish");

}

void ZYXtest::ryp1()
{
	m_Controls.textBrowser->append("ryp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 4;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("ryp finish");

}

void ZYXtest::rzp1()
{
	m_Controls.textBrowser->append("rzp");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 5;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 1;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rzp finish");

}

void ZYXtest::rxm1()
{
	m_Controls.textBrowser->append("rxm");

	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 3;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rxm finish");

}

void ZYXtest::rym1()
{
	m_Controls.textBrowser->append("rym");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 4;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rym finish");

}

void ZYXtest::rzm1()
{
	m_Controls.textBrowser->append("rzm");
	int nToolMotion = 1;//定义运动坐标类型  0-当前选择的用户坐标运动   1-按Tool坐标运动
	int nAxisID = 5;//定义轴 ID 0:x轴 1：y轴 2：z轴 3：rx轴 4：ry轴 5：rz轴
	int nDirection = 0;//定义运动方向 0：负向 1：正向
	double inputValue;

	inputValue = m_Controls.lineEdit_intuitiveValue_12->text().toDouble();
	double dDistance = inputValue;
	std::string valueString = std::to_string(inputValue);
	m_Controls.textBrowser->append(QString::fromStdString(valueString));

	int nMoveRelL = HRIF_MoveRelL(0, 0, nAxisID, nDirection, dDistance, nToolMotion);
	m_Controls.textBrowser->append("rzm finish");

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 设置速度比率
*@note  范围0-1
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::SetSpeedrRatio()
{
	double dOverride = m_Controls.lineEdit_X_11->text().toDouble();
	int nRet = HRIF_SetOverride(0, 0, dOverride);
	m_Controls.textBrowser->append("Set speed ratio:" + QString::number(dOverride));
	PrintResult(m_Controls.textBrowser, nRet, "Set speed ratio");

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 使用绝对运动接口进行平移
*@note 基于当前位置
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::Move_lhy()
{
	//----------------------------------------------------------------------------------------------------
	if (move_id == 0) {
		std::ofstream clearFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MOVE_Point.txt");
		clearFile.close();
	}

	move_id++;
	m_Controls.textBrowser->append(QString(" move_id: ") + QString::number(move_id));

	//----------------------------------------------------------------------------------------------------
	//记录运动前的TCP
	int point = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);
	std::ofstream outputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MOVE_Point.txt", std::ios::app);
	if (outputFile.is_open())
	{
		outputFile << "move_id: " << move_id << " Source_Point: " << dX << "," << dY << "," << dZ << "," << dRx << "," << dRy << "," << dRz << std::endl;
		outputFile.close();
	}
	else
	{
		m_Controls.textBrowser->append("can not open：MOVE_Point.txt");
	}

	//----------------------------------------------------------------------------------------------------
	//计算当前姿态矩阵
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);
	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> rotationMatrix = tmpTrans->GetMatrix();

	//偏置矩阵
	auto translation = vtkTransform::New();
	translation->Translate(m_Controls.lineEdit_X->text().toDouble(), m_Controls.lineEdit_Y->text().toDouble(), m_Controls.lineEdit_Z->text().toDouble());
	vtkSmartPointer<vtkMatrix4x4> translationMatrix = translation->GetMatrix();

	//目标矩阵
	vtkNew<vtkTransform> TargetMatrix;
	TargetMatrix->PreMultiply();
	TargetMatrix->Concatenate(rotationMatrix);
	TargetMatrix->Concatenate(translationMatrix);
	TargetMatrix->Update();
	auto resultMatrix = TargetMatrix->GetMatrix();


	//----------------------------------------------------------------------------------------------------
	//计算目标的TCP
	double x = resultMatrix->GetElement(0, 3);
	double y = resultMatrix->GetElement(1, 3);
	double z = resultMatrix->GetElement(2, 3);


	Eigen::Matrix3d Re;

	Re << resultMatrix->GetElement(0, 0), resultMatrix->GetElement(0, 1), resultMatrix->GetElement(0, 2),
		resultMatrix->GetElement(1, 0), resultMatrix->GetElement(1, 1), resultMatrix->GetElement(1, 2),
		resultMatrix->GetElement(2, 0), resultMatrix->GetElement(2, 1), resultMatrix->GetElement(2, 2);


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double rx = 180 * eulerAngle[2] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[0] / 3.1415;


	//----------------------------------------------------------------------------------------------------
	//存储计算TCP
	std::ofstream outputFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MOVE_Point.txt", std::ios::app);
	if (outputFile1.is_open())
	{
		outputFile1 << "move_id: " << move_id << " Calulate_Point: " << x << "," << y << "," << z << "," << rx << "," << ry << "," << rz << std::endl;
		outputFile1.close();
	}
	else
	{
		m_Controls.textBrowser->append("can not open：MOVE_Point.txt");
	}

	int nTargetPoint = HRIF_MoveL(0, 0, x, y, z, rx, ry, rz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);

	if (nTargetPoint == 0) {
		m_Controls.textBrowser->append("MOVE ");
	}
	else {
		m_Controls.textBrowser->append("MOVE FAILED");
	}

	//暂停几秒后再采集TCP
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//int point1 = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);
	//std::ofstream outputFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MOVE_Point.txt", std::ios::app);
	//if (outputFile2.is_open())
	//{
	//	outputFile2 << "move_id: " << move_id << " Actual_Point: " << dX << "," << dY << "," << dZ << "," << dRx << "," << dRy << "," << dRz << std::endl;
	//	outputFile2.close();
	//}
	//else
	//{
	//	m_Controls.textBrowser->append("can not open：MOVE_Point.txt");
	//}

	//----------------------------------------------------------------------------------------------------
	//运动完后检查运动状态，并保存现在当前实际的TCP
	//int nMovingState = 0; int nEnableState = 0; int nErrorState = 0; int nErrorCode = 0;
	//int nErrorAxis = 0; int nBreaking = 0; int nPause = 0; int nEmergencyStop = 0;
	//int nSaftyGuard = 0; int nElectrify = 0; int nIsConnectToBox = 0; int nBlendingDone = 0; int nInPos = 0;

	//while (nMovingState != 0 && nInPos != 1)//nMovingState = 0:机器人不处于运动状态 nInPos == 1：运动到位
	//{
	//	int nRet_move = HRIF_ReadRobotState(0, 0, nMovingState, nEnableState,
	//		nErrorState, nErrorCode, nErrorAxis, nBreaking, nPause, nEmergencyStop,
	//		nSaftyGuard, nElectrify, nIsConnectToBox, nBlendingDone, nInPos);

	//	if (nRet_move == 0 && nMovingState == 0 && nInPos == 1)
	//	{
	//		m_Controls.textBrowser->append("MOVE OK");
	//		int point1 = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);
	//		std::ofstream outputFile2(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\MOVE_Point.txt", std::ios::app);
	//		if (outputFile2.is_open())
	//		{
	//			outputFile2 << "move_id: " << move_id << " Actual_Point: " << dX << "," << dY << "," << dZ << "," << dRx << "," << dRy << "," << dRz << std::endl;
	//			outputFile2.close();
	//			break;
	//		}
	//		else
	//		{
	//			m_Controls.textBrowser->append("can not open：MOVE_Point.txt");
	//		}

	//	}
	//}
	//----------------------------------------------------------------------------------------------------

}

void ZYXtest::Move_zyx()
{

	int point = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateZ(dRz);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateX(dRx);
	tmpTrans->Translate(dX, dY, dZ);
	tmpTrans->Update();
	// 获取旋转平移矩阵
	vtkSmartPointer<vtkMatrix4x4> rotationMatrix = tmpTrans->GetMatrix();


	// 打印旋转矩阵信息，小数点后5位
	std::cout << std::fixed << std::setprecision(5);
	rotationMatrix->Print(std::cout);
	//rotationMatrix30->Print(std::cout);


	// 设置偏移量
	auto translation = vtkTransform::New();
	translation->Translate(10, 0, 0);
	// 获取偏移矩阵
	vtkSmartPointer<vtkMatrix4x4> translationMatrix = translation->GetMatrix();

	// 打印偏移矩阵信息
	translationMatrix->Print(std::cout);

	// 创建补偿30度的问题
	vtkSmartPointer<vtkTransform> transformn = vtkSmartPointer<vtkTransform>::New();
	transformn->RotateY(-30.0);
	vtkSmartPointer<vtkMatrix4x4> rotation = transformn->GetMatrix();


	vtkNew<vtkTransform> TargetMatrix;
	TargetMatrix->Identity();
	TargetMatrix->PreMultiply();
	/*TargetMatrix->Concatenate(rotation);*/
	TargetMatrix->SetMatrix(rotationMatrix);
	TargetMatrix->Concatenate(rotation);
	TargetMatrix->Update();
	auto resultMatrix = TargetMatrix->GetMatrix();


	double x = resultMatrix->GetElement(0, 3);
	double y = resultMatrix->GetElement(1, 3);
	double z = resultMatrix->GetElement(2, 3);


	Eigen::Matrix3d Re;
	Re << resultMatrix->GetElement(0, 0), resultMatrix->GetElement(0, 1), resultMatrix->GetElement(0, 2),
		resultMatrix->GetElement(1, 0), resultMatrix->GetElement(1, 1), resultMatrix->GetElement(1, 2),
		resultMatrix->GetElement(2, 0), resultMatrix->GetElement(2, 1), resultMatrix->GetElement(2, 2);


	// R = Rx(dRx) * Ry(dRy) *Rz（dRz）
	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double rx = 180 * eulerAngle[0] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[2] / 3.1415;
	MITK_INFO << "targetPoint" << x << "," << y << "," << z << "," << rx << "," << ry << "," << rz;

	int nState = 1;
	int nRet = HRIF_SetToolMotion(0, 0, nState);

	int nTargetPoint = HRIF_MoveL(0, 0, x, y, z, rx, ry, rz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);
	if (nTargetPoint == 0) {
		m_Controls.textBrowser->append("MOVE OK");
	}
	else {
		m_Controls.textBrowser->append("MOVE FAILED");
	}
	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 设置负载
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::SetLoad()
{

	// 需要设置负载质量
	double dMass = m_Controls.TCP_Z_4->text().toDouble();
	// 需要设置负载质心 X 方向偏移量
	double dX_Mass = m_Controls.TCP_X_3->text().toDouble();
	// 需要设置负载质心 Y 方向偏移量
	double dY_Mass = m_Controls.TCP_Y_3->text().toDouble();
	// 需要设置负载质心 Z 方向偏移量
	double dZ_Mass = m_Controls.TCP_Z_3->text().toDouble();
	// 设置当前负载
	/*int nRet = HRIF_SetPayload(0, 0, dMass, dX_Mass, dY_Mass, dZ_Mass);*/
	m_Controls.textBrowser->append("The robotic set load");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 记录一个点
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::registrationPoint()
{
	m_Controls.textBrowser->append("Set  Point 1");


	int result = HRIF_ReadActPos(0, 0, g_X, g_Y, g_Z, g_Rx, g_Ry, g_Rz, dJ1_1, dJ2_1, dJ3_1, dJ4_1,
		dJ5_1, dJ6_1, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);
	if (result == 0) {
		// 函数调用成功，输出初始位置信息
		std::cout << "Initial Position: X = " << g_X << ", Y = " << g_Y << ", Z = " << g_Z
			<< ", Rx = " << g_Rx << ", Ry = " << g_Ry << ", Rz = " << g_Rz << std::endl;
	}
	else {
		// 函数调用失败，输出错误信息
		std::cerr << "Failed to read initial position. Error code: " << result << std::endl;
	}

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 返回记录的点
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::goBackPoint()
{
	m_Controls.textBrowser->append("Goto Point 1");

	int nIsUseJoint = 1; // 是否使用关节坐标作为目标点，0表示使用迪卡尔坐标
	int nIsSeek = 0; // 是否进行探寻，0表示不进行探寻
	int nIOBit = 0; // IO位
	int nIOState = 0; // IO状态

	HRIF_MoveJ(0, 0, g_X, g_Y, g_Z, g_Rx, g_Ry, g_Rz,
		dJ1_1, dJ2_1, dJ3_1, dJ4_1, dJ5_1, dJ6_1, sTcpName, sUcsName, dVelocity, dAcc, dRadius,
		nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}

void ZYXtest::Move_lhyinit()
{
	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	dX = 0;
	dY = 20;
	dZ = 0;
	dRx = 0;
	dRy = 0;
	dRz = 0;
	dJ1 = 0;
	dJ2 = 0;
	dJ3 = 0;
	dJ4 =0;
	dJ5 = 0;
	dJ6 = 0;
	int nRet = HRIF_MoveJ(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
	std::cout << nRet << std::endl;
	if (nRet == 0) {

		m_Controls.textBrowser->append("ok");
	}
	else {

		m_Controls.textBrowser->append("failed");

	}

}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 通过运动的ID记录每个位置的关节角、机械臂末端mark的点
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::logPoint_1()
{
	//清空文本------------------------------------
	if (move_id_test == 0) {
		std::ofstream clearFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\log_Point.txt");
		clearFile.close();
	}

	if (move_id_test == 0) {
		std::ofstream clearFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\log_CameraPoint.txt");
		clearFile1.close();
	}

	//采集机械臂数据------------------------------------
	move_id_test++;
	m_Controls.textBrowser->append(QString(" move_id: ") + QString::number(move_id_test));

	/*int point = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);*/
	int nRet = HRIF_ReadActJointPos(0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6);

	PrintResult(m_Controls.textBrowser, nRet, "LOGO");

	std::ofstream outputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\log_Point.txt", std::ios::app);
	if (outputFile.is_open())
	{
		/*outputFile << "move_id: " << move_id_test << " TCP_Point: " << dX << "," << dY << "," << dZ << "," << dRx << "," << dRy << "," << dRz << std::endl;*/
		outputFile << dJ1 << "," << dJ2 << "," << dJ3 << "," << dJ4 << "," << dJ5 << "," << dJ6 << std::endl;
		outputFile.close();
	}
	else
	{
		m_Controls.textBrowser->append("can not open：MOVE_Point.txt");
	}

	//采集相机数据------------------------------------
	auto vtkT_CameraToEndRF = vtkMatrix4x4::New();
	vtkT_CameraToEndRF->DeepCopy(T_CamToEndRF);

	double x = vtkT_CameraToEndRF->GetElement(0, 3);
	double y = vtkT_CameraToEndRF->GetElement(1, 3);
	double z = vtkT_CameraToEndRF->GetElement(2, 3);

	std::ofstream outputFile1(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\log_CameraPoint.txt", std::ios::app);
	if (outputFile1.is_open())
	{
		outputFile1 << "move_id: " << move_id_test << " Point: " << x << "," << y << "," << z << std::endl;
		outputFile1.close();
	}
	else
	{
		m_Controls.textBrowser->append("can not open：log_CameraPoint.txt");
	}

}

//table8
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 有效空间的移动、使用路点运动
*@note 没有调试成功
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::EffectiveSpace()
{
	//采集当前的位置(A 点)
	double dX_xzy = 0; double dY_xzy = 0; double dZ_xzy = 0;
	double dRx_xzy = 0; double dRy_xzy = 0; double dRz_xzy = 0;
	int nRet_readpos = HRIF_ReadActTcpPos(0, 0, dX_xzy, dY_xzy, dZ_xzy, dRx_xzy, dRy_xzy, dRz_xzy);

	if (nRet_readpos != 0) {
		std::cout << "Read actual TCP pos failed." << std::endl;
	}


	// 定义空间目标位置
	double WayPoints[7][6] = {
		{dX_xzy, dY_xzy + 100, dZ_xzy, dRx_xzy, dRy_xzy, dRz_xzy},//B
		{dX_xzy, dY_xzy + 100, dZ_xzy + 100, dRx_xzy, dRy_xzy, dRz_xzy},//C
		{dX_xzy, dY_xzy, dZ_xzy + 100, dRx_xzy, dRy_xzy, dRz_xzy},//D
		{dX_xzy + 100, dY_xzy, dZ_xzy + 100, dRx_xzy, dRy_xzy, dRz_xzy},//E
		{dX_xzy + 100, dY_xzy + 100, dZ_xzy + 100, dRx_xzy, dRy_xzy, dRz_xzy},//F
		{dX_xzy + 100, dY_xzy + 100, dZ_xzy, dRx_xzy, dRy_xzy, dRz_xzy},//G
		{dX_xzy + 100, dY_xzy, dZ_xzy, dRx_xzy, dRy_xzy, dRz_xzy}//H 
	};


	//执行路点运动
	for (int i = 0; i < 7; i++)
	{
		//dX_xzy = WayPoints[i][0];
		//dY_xzy = WayPoints[i][1];
		//dZ_xzy = WayPoints[i][2];
		//dRx_xzy = WayPoints[i][3];
		//dRy_xzy = WayPoints[i][4];
		//dRz_xzy = WayPoints[i][5];

		int nMoveType = 1;

		int nRet_action = HRIF_WayPoint(0, 0, nMoveType, WayPoints[i][0], dY_xzy = WayPoints[i][1], dZ_xzy = WayPoints[i][2],
			WayPoints[i][3], WayPoints[i][4], WayPoints[i][5],
			dJ1, dJ2, dJ3, dJ4, dJ5, dJ6,
			sTcpName, sUcsName, dVelocity, dAcc, dRadius,
			nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);

		//QThread::sleep(10); // 休眠 10 秒
		std::this_thread::sleep_for(std::chrono::seconds(10));

		if (nRet_action != 0)
		{
			std::cout << "Move to point " << i + 1 << " failed." << std::endl;
		}
	}


}

void ZYXtest::updataServo()
{
	if (m_ServoPTimer == nullptr)
	{
		m_ServoPTimer = new QTimer(this);
	}
	connect(m_ServoPTimer, &QTimer::timeout, this, &ZYXtest::ComputeAndSendServoCommand);
	m_ServoPTimer->start(100);
}

void ZYXtest::StartServoControl()
{

	// 周期 # 设置伺服周期为 20ms，建议最小不低于 15ms
	double dServoTime = 0.015;
	// 前瞻时间 # 设置前瞻时间，前瞻时间越大，轨迹越平滑，但越滞后。
	double dLookaheadTime = 0.1;
	// 启动机器人在线控制
	int StartServo = HRIF_StartServo(0, 0, dServoTime, dLookaheadTime);

	if (StartServo == 0) {
		m_Controls.textBrowser->append("StartServo");
	}

}

void ZYXtest::ComputeAndSendServoCommand()
{
	clock_t start;
	start = clock();

	ComputeNewRobot();
	std::vector<double> vecUcs = { dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz };
	std::vector<double> vecTcp = { dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz };
	MITK_INFO << "dTargetPoint:" << dTargetPoint[0] << "," << dTargetPoint[1] << "," << dTargetPoint[2] << "," << dTargetPoint[3]
		<< "," << dTargetPoint[4] << "," << dTargetPoint[5];
	// 在线空间位置命令控制
	int ServoP = HRIF_PushServoP(0, 0, dTargetPoint, vecTcp, vecUcs);
	// 处理返回值
	if (ServoP == 0)
	{
		std::cout << "HRIF_PushServoP call succeeded." << std::endl;
	}
	else
	{
		std::cerr << "HRIF_PushServoP call failed with error code: " << ServoP << std::endl;
	}

	std::cout << "while run time " << (clock() - start) / 1000 << std::endl;
}

void ZYXtest::ComputeNewRobot()
{

	int nData = HRIF_ReadActPos(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, dTcp_X, dTcp_Y,
		dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz, dUcs_X, dUcs_Y, dUcs_Z, dUcs_Rx, dUcs_Ry, dUcs_Rz);


	// T_BaseToFlange
	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx);
	tmpTrans->RotateY(dRy);
	tmpTrans->RotateZ(dRz);
	double translation[3] = { dX, dY, dZ };

	tmpTrans->Translate(translation);
	tmpTrans->Update();

	auto vtkT_BaseToFlange = vtkMatrix4x4::New();
	vtkT_BaseToFlange = tmpTrans->GetMatrix();

	PrintMatrix("vtkT_BaseToFlange", vtkT_BaseToFlange->GetData());


	auto VTKT_probe1Tocam = vtkMatrix4x4::New();//老的
	VTKT_probe1Tocam->DeepCopy(T_CameraToProbe1);
	VTKT_probe1Tocam->Invert();
	PrintMatrix("VTKT_probe1Tocam", VTKT_probe1Tocam->GetData());

	auto VTKT_camToprobe2 = vtkMatrix4x4::New();//新的
	VTKT_camToprobe2->DeepCopy(T_CamToProbe);
	memcpy_s(T_CameraToProbe1, sizeof(double) * 16, VTKT_camToprobe2->GetData(), sizeof(double) * 16);
	PrintMatrix("VTKT_camToprobe2", VTKT_camToprobe2->GetData());

	vtkNew<vtkTransform> vtkT_probeTransform;
	vtkT_probeTransform->PostMultiply();
	vtkT_probeTransform->SetMatrix(VTKT_camToprobe2);
	vtkT_probeTransform->Concatenate(VTKT_probe1Tocam);
	vtkT_probeTransform->Update();

	vtkNew<vtkMatrix4x4> vtkT_probe1Toprobe2;
	vtkT_probe1Toprobe2->DeepCopy(vtkT_probeTransform->GetMatrix());
	PrintMatrix("probe1Toprobe2", vtkT_probe1Toprobe2->GetData());

	// 计算robot位置并转化成robot到基于base的坐标
	auto vtkT_BaseToFlange2 = vtkMatrix4x4::New();
	vtkMatrix4x4::Multiply4x4(vtkT_probe1Toprobe2, vtkT_BaseToFlange, vtkT_BaseToFlange2);
	PrintMatrix("vtkT_BaseToFlange2 ", vtkT_BaseToFlange2->GetData());




	Eigen::Matrix3d Re;

	Re << vtkT_BaseToFlange2->GetElement(0, 0), vtkT_BaseToFlange2->GetElement(0, 1), vtkT_BaseToFlange2->GetElement(0, 2),
		vtkT_BaseToFlange2->GetElement(1, 0), vtkT_BaseToFlange2->GetElement(1, 1), vtkT_BaseToFlange2->GetElement(1, 2),
		vtkT_BaseToFlange2->GetElement(2, 0), vtkT_BaseToFlange2->GetElement(2, 1), vtkT_BaseToFlange2->GetElement(2, 2);


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	dTargetPoint[0] = vtkT_BaseToFlange2->GetElement(0, 3);
	dTargetPoint[1] = vtkT_BaseToFlange2->GetElement(1, 3);
	dTargetPoint[2] = vtkT_BaseToFlange2->GetElement(2, 3);
	dTargetPoint[3] = 180 * eulerAngle[2] / 3.1415;
	dTargetPoint[4] = 180 * eulerAngle[1] / 3.1415;
	dTargetPoint[5] = 180 * eulerAngle[0] / 3.1415;


}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 读取各个关节的电流然后存入到save\\RobotCurrents.txt
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ReadCurrent()
{
	// 定义关节实际电流
	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;
	// 读取关节实际电流
	int nRet = HRIF_ReadActJointCur(0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6);

	//m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	//if (nRet == 0) {
	//	m_Controls.textBrowser->append("Read successfully");
	//}
	//else {
	//	m_Controls.textBrowser->append("Read failed ");
	//}
	//m_Controls.textBrowser->append("dJ1_currents: " + QString::number(dJ1));
	//m_Controls.textBrowser->append("dJ2_currents: " + QString::number(dJ2));
	//m_Controls.textBrowser->append("dJ3_currents: " + QString::number(dJ3));
	//m_Controls.textBrowser->append("dJ4_currents: " + QString::number(dJ4));
	//m_Controls.textBrowser->append("dJ5_currents: " + QString::number(dJ5));
	//m_Controls.textBrowser->append("dJ6_currents: " + QString::number(dJ6));
	//m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_IndexOfRobotCurrents++;
	m_Controls.textBrowser->append("m_IndexOfRobotCurrents: " + QString::number(m_IndexOfRobotCurrents));
	if (m_IndexOfRobotCurrents == 1) {
		std::ofstream outputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\RobotCurrents.txt", std::ios_base::trunc);
		outputFile << dJ1 << " " << dJ2 << " " << dJ3 << " " << dJ4 << " " << dJ5 << " " << dJ6 << std::endl;
		outputFile.close();
	}
	else {
		std::ofstream outputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\RobotCurrents.txt", std::ios_base::app);
		outputFile << dJ1 << " " << dJ2 << " " << dJ3 << " " << dJ4 << " " << dJ5 << " " << dJ6 << std::endl;
		outputFile.close();
	}


}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 关闭QT的定时采集关节角电流
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::stopCrashDetection()
{
	if (m_CrashDetection != nullptr && m_CrashDetection->isActive())
	{
		m_CrashDetection->stop();
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 开一个QT的定时采集关节角电流
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::updataCrashDetechion()
{
	if (m_CrashDetection == nullptr)
	{
		m_CrashDetection = new QTimer(this);
	}

	connect(m_CrashDetection, SIGNAL(timeout()), this, SLOT(CrashDetechion()));
	connect(m_CrashDetection, SIGNAL(timeout()), this, SLOT(ReadCurrent()));

	m_CrashDetection->start(100);
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 检测关节角的电流是否高于或者低于阈值
*@note  主要做碰撞检测、目前测试4轴最好
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::CrashDetechion()
{
	//检测机械臂的末端的
	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;
	// 读取关节实际电流
	int nRet = HRIF_ReadActJointCur(0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6);

	/*PrintResult(m_Controls.textBrowser, nRet, "Robotic arm ReadCurrents");*/

	double upper = m_Controls.lineEdit_currentup->text().toDouble();
	double lower = m_Controls.lineEdit_currentdown->text().toDouble();

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("set currents value: " + QString::number(lower));
	m_Controls.textBrowser->append("set currents value: " + QString::number(upper));
	m_Controls.textBrowser->append("real currents value: " + QString::number(dJ4));

	if (dJ4 > upper || dJ4 < lower)
	{
		m_Controls.textBrowser->append("Arm stop");
		int GrpStop = HRIF_GrpStop(0, 0);
		/*int nGrpDisable = HRIF_GrpDisable(0, 0);*/
		PrintResult(m_Controls.textBrowser, GrpStop, " stop");
		if (m_CrashDetection != nullptr && m_CrashDetection->isActive())
		{
			m_CrashDetection->stop();
		}
	}
}

void ZYXtest::StopServoControl()
{
	if (m_ServoPTimer->isActive())
	{
		m_ServoPTimer->stop();
		int GrpStop = HRIF_GrpStop(0, 0);
		if (GrpStop == 0)
		{
			std::cout << "Stop succeeded." << std::endl;
		}
		else
		{
			std::cout << "Stop Failed." << std::endl;
		}

	}
}

void ZYXtest::PrintMatrix(std::string matrixName, double* matrix)
{
	m_Controls.textBrowser->append("---------------------------------------------------");
	m_Controls.textBrowser->append(QString::fromStdString(matrixName + ":"));
	/*std::cout << matrixName + ": " << std::endl;*/
	for (int i = 0; i < 4; ++i)
	{
		std::string row;
		for (int j = 0; j < 4; ++j)
		{
			row += std::to_string(matrix[i * 4 + j]) + " ";
		}
		m_Controls.textBrowser->append(QString::fromStdString(row) + "\n");
	}
	m_Controls.textBrowser->append("---------------------------------------------------");
}

void ZYXtest::Print_Matrix(const std::string& title, vtkMatrix4x4* matrix)
{
	std::cout << title << std::endl;
	std::cout << "+-----------------------------------------------+" << std::endl;
	std::cout << "|       Matrix: " << title << "              |" << std::endl;
	std::cout << "+-----------+-----------+----------+------------+" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "|  ";
		for (int j = 0; j < 4; j++) {
			std::cout << std::fixed << std::setprecision(6) << matrix->GetElement(i, j) << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "+-----------+-----------+----------+------------+" << std::endl;
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在QT中打印TCP
*@note  用法：PrintTCP_qt(m_Controls.textBrowser, dX, dY, dZ, dRx, dRy, dRz);
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::PrintTCP_qt(QTextBrowser* browser, double x, double y, double z, double rx, double ry, double rz)
{
	browser->append("---------------------------------------------------");
	browser->append("dX_tcp=" + QString::number(x));
	browser->append("dY_tcp=" + QString::number(y));
	browser->append("dZ_tcp=" + QString::number(z));
	browser->append("dRx_tcp=" + QString::number(rx));
	browser->append("dRy_tcp=" + QString::number(ry));
	browser->append("dRz_tcp=" + QString::number(rz));
	browser->append("---------------------------------------------------");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 在终端打印TCP
*@note  用法：PrintTCP_Terminal(tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::PrintTCP_Terminal(double x, double y, double z, double rx, double ry, double rz)
{
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "dX_tcp=" << x << std::endl;
	std::cout << "dY_tcp=" << y << std::endl;
	std::cout << "dZ_tcp=" << z << std::endl;
	std::cout << "dRx_tcp=" << rx << std::endl;
	std::cout << "dRy_tcp=" << ry << std::endl;
	std::cout << "dRz_tcp=" << rz << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 打印机械臂函数是否执行成功，不成功返回错误码和错误类型
*@note  用法：PrintResult(m_Controls.textBrowser, n, "你想写的话");
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::PrintResult(QTextBrowser* browser, int nRet, const char* message)
{
	if (nRet == 0) {
		browser->append(QString(message) + " Succeed");
		std::cout << message << " Succeed" << std::endl;
		std::cout << "---------------------------------------------------" << std::endl;
		browser->append("---------------------------------------------------");
	}
	else {
		browser->append(QString(message) + " Failed");
		std::cout << message << " Failed" << std::endl;

		std::cout << "ErrorCode: " << nRet << std::endl;
		string tmpstr;
		int zyx = HRIF_GetErrorCodeStr(0, nRet, tmpstr);
		std::cout << nRet << std::endl;
		/*browser->append("ErrorCode：" + QString::fromStdString(tmpstr));*/
		browser->append("ErrorCode: " + QString::number(nRet));
		//针对一些常见错误做的一些解释
		if (nRet == 39500)
		{
			std::cout << "Please check the connection of the robot arm" << std::endl;
			browser->append("Please check the connection of the robot arm");

		}
		if (nRet == 40025)
		{
			browser->append("The robot is not in a state of readiness" + QString::number(nRet));
		}

		if (nRet >= 40000 && nRet <= 40500) {
			std::cout << "CDS executed command with error: " << nRet << std::endl;
			/*browser->append("CDS executed command with error: " + QString::number(nRet));*/
			browser->append("CDS执行命令时发生错误：" + QString::number(nRet));
		}
		else if (nRet >= 10000 && nRet <= 10015) {
			std::cout << "Robot servo drive reported fault code: " << nRet << std::endl;
			/*browser->append("Robot servo drive reported fault code: " + QString::number(nRet));*/
			browser->append("机器人伺服驱动报告故障代码：" + QString::number(nRet));
		}
		else if (nRet >= 10016 && nRet <= 11000) {
			std::cout << "Robot collaboration algorithm detected fault: " << nRet << std::endl;
			/*browser->append("Robot collaboration algorithm detected fault: " + QString::number(nRet));*/
			browser->append("检测到机器人协作算法故障：" + QString::number(nRet));
		}
		else if (nRet >= 15000 && nRet <= 16000) {
			std::cout << "Robot control module detected fault: " << nRet << std::endl;
			/*browser->append("Robot control module detected fault: " + QString::number(nRet));*/
			/*browser->append("检测到机器人控制模块故障：" + QString::number(nRet));*/
		}
		else if (nRet >= 30001 && nRet <= 30016) {
			std::cout << "Modbus module error during command execution: " << nRet << std::endl;
			/*browser->append("Modbus module error during command execution: " + QString::number(nRet));*/
			/*browser->append("Modbus模块在执行命令时发生错误：" + QString::number(nRet));*/
		}
		else if (nRet >= 20000 && nRet <= 39999) {
			std::cout << "CPS executed command with error: " << nRet << std::endl;
			/*browser->append("CPS executed command with error: " + QString::number(nRet));*/
			/*browser->append("CPS执行命令时发生错误：" + QString::number(nRet));*/
		}

		std::cout << "---------------------------------------------------" << std::endl;
		browser->append("---------------------------------------------------");
	}
}

void ZYXtest::PrintArray16ToMatrix(const std::string& title, double* Array)
{
	std::cout << "+-----------------------------------------------+" << std::endl;
	std::cout << "|               " << title << "              |" << std::endl;
	std::cout << "+-----------+-----------+----------+------------+" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "|  ";
		for (int j = 0; j < 4; j++) {
			std::cout << std::fixed << std::setprecision(6) << Array[i * 4 + j] << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "+-----------+-----------+----------+------------+" << std::endl;
}




//+++++++++++++++++++++++++++++++++++++++++口腔机器人+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::on_pushButton_steelballExtract_clicked()
{
	auto m_CBCTGeo_init = GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->GetData()->GetGeometry()->GetVtkMatrix();
	auto m_DrillGeo_init = GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->GetVtkMatrix();

	m_T_CBCTGeometry_init->DeepCopy(m_CBCTGeo_init);
	m_T_DrillGeometry_init->DeepCopy(m_DrillGeo_init);

	m_Controls.textBrowser->append("------- Started steelball searching -------");

	// Initial preparation
	// Determines the A, B, C splint type
	int splintType = m_Controls.comboBox_plate->currentIndex();

	auto steelBalls_cmm = mitk::PointSet::New();
	auto splintSurface = mitk::Surface::New();
	auto probeDitchPset_cmm = mitk::PointSet::New();

	if (splintType == 0)
	{
		// Check if pointSet data is available
		if (GetDataStorage()->GetNamedNode("steelBalls_cmm_A") != nullptr && GetDataStorage()->GetNamedNode("splintSurface_A") != nullptr
			&& GetDataStorage()->GetNamedNode("probeDitchPset_cmm_A") != nullptr)
		{
			steelBalls_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelBalls_cmm_A")->GetData());
			probeDitchPset_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probeDitchPset_cmm_A")->GetData());
			splintSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("splintSurface_A")->GetData());
		}
	}
	else if (splintType == 1)
	{
		// Check if pointSet data is available
		if (GetDataStorage()->GetNamedNode("steelBalls_cmm_B") != nullptr && GetDataStorage()->GetNamedNode("splintSurface_B") != nullptr
			&& GetDataStorage()->GetNamedNode("probeDitchPset_cmm_B") != nullptr)
		{
			steelBalls_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelBalls_cmm_B")->GetData());
			probeDitchPset_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probeDitchPset_cmm_B")->GetData());
			splintSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("splintSurface_B")->GetData());
		}
	}
	else if (splintType == 2)
	{
		// Check if pointSet data is available
		if (GetDataStorage()->GetNamedNode("steelBalls_cmm_C") != nullptr && GetDataStorage()->GetNamedNode("splintSurface_C") != nullptr
			&& GetDataStorage()->GetNamedNode("probeDitchPset_cmm_C") != nullptr)
		{
			steelBalls_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelBalls_cmm_C")->GetData());
			probeDitchPset_cmm = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probeDitchPset_cmm_C")->GetData());
			splintSurface = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("splintSurface_C")->GetData());
		}

	}

	if (steelBalls_cmm->GetSize() == 0)
	{
		m_Controls.textBrowser->append("Warning! steelBalls_cmm is not available in dataStorage!");
		return;
	}

	if (probeDitchPset_cmm->GetSize() == 0)
	{
		m_Controls.textBrowser->append("Warning! probeDitchPset_cmm is not available in dataStorage!");
		return;
	}

	if (splintSurface->GetVtkPolyData() == nullptr)
	{
		m_Controls.textBrowser->append("Warning! splintSurface is not available in dataStorage!");
		return;
	}

	m_steelBalls_cmm = steelBalls_cmm;
	m_splinterSurface = splintSurface;
	m_probeDitchPset_cmm = probeDitchPset_cmm;
	// 将steelBalls_cmm扩大三倍
	std::vector<double> tmpCenters(3 * steelBalls_cmm->GetSize(), 0.0);

	for (int i{ 0 }; i < 3 * steelBalls_cmm->GetSize(); i++)
	{
		tmpCenters[i] = steelBalls_cmm->GetPoint(i / 3)[i % 3];
	}
	// stdCenter：将金属球点集转化为一个vector数组，例：[(a,b,c),(d,e,f)]转化为[a,b,c,d,e,f]
	stdCenters = tmpCenters;
	realballnumber = steelBalls_cmm->GetSize();
	edgenumber = realballnumber * (realballnumber - 1);

	allBallFingerPrint.resize(edgenumber);
	fill(allBallFingerPrint.begin(), allBallFingerPrint.end(), 0);

	auto standartSteelballCenters = mitk::PointSet::New();

	int stdCenterNum{ realballnumber };
	// 将金属球点集放入standartSteelballCenters点集中
	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	// 计算金属球点集中所有两点间的距离，并储存在全局变量allBallFingerPrint中
	UpdateAllBallFingerPrint(standartSteelballCenters);

	// Get maximal and minimal voxel
	auto inputCtImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");

	auto inputVtkImage = inputCtImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);
	std::cout << "dimension" << std::endl;
	std::cout << dims[0] << std::endl;
	std::cout << dims[1] << std::endl;
	std::cout << dims[2] << std::endl;
	// 图像的bound
	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxVoxel{ 0 };
	double tmpMinVoxel{ 0 };

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxVoxel)
			{
				tmpMaxVoxel = double(*inSI);
			}
			if (double(*inSI) < tmpMinVoxel)
			{
				tmpMinVoxel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}

	int searchIterations{ 7 }; // optimal voxel value
	//tmpMaxVoxel = 15000; // tmp fix  March 03, 2023
	//tmpMinVoxel = 1500; // tmp fix  March 03, 2023


	double voxelThres{ tmpMaxVoxel };
	int foundCleanCenterNum{ 0 };
	int foundCenterNum{ 0 };
	std::vector<int> foundIDs(realballnumber, 0);
	//int foundIDs[7]{ 0 };


	// Search for the best voxel value threshold
	for (int i{ 0 }; i < (searchIterations + 1); i++)
	{
		// double tmpVoxelThreshold = (1 -  double(i) / searchIterations) * (tmpMaxVoxel - tmpMinVoxel) + tmpMinVoxel;
		double tmpVoxelThreshold = tmpMaxVoxel - (tmpMaxVoxel - tmpMinVoxel) * i / searchIterations;

		GetCoarseSteelballCenters(tmpVoxelThreshold);

		foundCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
		m_Controls.textBrowser->append("Current HU value: " + QString::number(tmpVoxelThreshold));
		if (foundCenterNum >= 40)
		{
			break;
		}

		IterativeScreenCoarseSteelballCenters(realballnumber - 3, realballnumber - 1, foundIDs);
		//IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);

		if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() >= foundCleanCenterNum)
		{
			foundCleanCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
			voxelThres = tmpVoxelThreshold;
			if (foundCleanCenterNum >= stdCenterNum)
			{
				IterativeScreenCoarseSteelballCenters(realballnumber - 1, realballnumber - 1, foundIDs);
				//IterativeScreenCoarseSteelballCenters(6, 6, foundIDs);
				if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == stdCenterNum)
				{
					//m_Controls.textBrowser->append("~~All steelballs have been found~~");
					break;
				}

			}
		}

	}

	GetCoarseSteelballCenters(voxelThres);
	//m_Controls.lineEdit_ballGrayValue->setText(QString::number(voxelThres));
	IterativeScreenCoarseSteelballCenters(realballnumber - 3, realballnumber - 1, foundIDs);
	//IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() > realballnumber)
		//if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() > 7)
	{
		IterativeScreenCoarseSteelballCenters(realballnumber - 1, realballnumber - 1, foundIDs);
		//IterativeScreenCoarseSteelballCenters(6, 6, foundIDs);
	}

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == 0)
	{
		GetCoarseSteelballCenters(voxelThres);
		//m_Controls.lineEdit_ballGrayValue->setText(QString::number(voxelThres));
		IterativeScreenCoarseSteelballCenters(realballnumber - 3, realballnumber - 1, foundIDs);
		//IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);
	}


	RearrangeSteelballs(realballnumber - 1, foundIDs);
	//RearrangeSteelballs(6, foundIDs); // this function is redundant ??

	auto partialStdPointset = mitk::PointSet::New();
	for (int q = 0; q < realballnumber; q++) {
		//for (int q{ 0 }; q < 7; q++) {
		if (foundIDs[q] == 1)
		{
			partialStdPointset->InsertPoint(standartSteelballCenters->GetPoint(q));
		}
	}

	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(partialStdPointset);
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData()));

	landmarkRegistrator->ComputeLandMarkResult();
	double maxError = landmarkRegistrator->GetmaxLandmarkError();
	double avgError = landmarkRegistrator->GetavgLandmarkError();

	m_Controls.textBrowser->append("Maximum steelball error: " + QString::number(maxError));
	m_Controls.textBrowser->append("Average steelball error: " + QString::number(avgError));

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == realballnumber)
		//if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == 7)
	{
		m_Controls.textBrowser->append("~~All steelballs have been found!~~");
	}
	else
	{
		m_Controls.textBrowser->append("!!!Warning: Only found " + QString::number(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize())
			+ " steelballs!!!!");
		m_Controls.textBrowser->append("Please compare 'Steelball centers', 'std centers (partial)' and 'std centers (full)' carefully!");
	}

	if (avgError > 1)
	{
		m_Controls.textBrowser->append("!!!Warning: The found centers are highly problematic!!!");
	}

	m_Controls.textBrowser->append("------- End of steelball searching -------");

	auto tmpPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	auto childNode = mitk::DataNode::New();
	childNode->SetName("Steelball centers2");
	childNode->SetData(tmpPointSet);
	GetDataStorage()->Add(childNode);
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	childNode->SetName("steelball_image");


	//************* Move the dental splint surface **********************//
	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image");
	// auto splint_node = GetDataStorage()->GetNamedNode("dentalSplint");
	// auto stdBall_node = GetDataStorage()->GetNamedNode("steelball_rf");

	if (extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_image is missing");
		return;
	}

	// if (splint_node == nullptr || stdBall_node == nullptr)
	// {
	// 	m_Controls.textBrowser->append("dentalSplint or std_steelball is missing");
	// 	return;
	// }

	auto extractedBall_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_image");
	// auto stdball_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_rf");
	int extracted_num = extractedBall_pset->GetSize();

	if (extracted_num < m_steelBalls_cmm->GetSize())
	{
		m_Controls.textBrowser->append("steelball_image extraction incomplete");
		return;
	}

	auto landmarkRegistrator2 = mitk::SurfaceRegistration::New();
	landmarkRegistrator2->SetLandmarksSrc(m_steelBalls_cmm);
	landmarkRegistrator2->SetLandmarksTarget(extractedBall_pset);

	landmarkRegistrator2->ComputeLandMarkResult();

	auto result_matrix = landmarkRegistrator2->GetResult();

	// auto splint_surface = GetDataStorage()->GetNamedObject<mitk::Surface>("dentalSplint");

	splintSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(result_matrix);

	splintSurface->GetGeometry()->Modified();

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters)
{
	int pointNum = stdSteelballCenters->GetSize();

	int currentIndex{ 0 };
	for (int i{ 0 }; i < pointNum; i++)
	{

		int currentColumn{ 0 };
		for (int j{ 0 }; j < pointNum; j++)
		{
			if (i != j)
			{
				allBallFingerPrint[currentIndex] = GetPointDistance(stdSteelballCenters->GetPoint(i), stdSteelballCenters->GetPoint(j));
				//m_Controls.textBrowser->append(QString::number(currentIndex)+": "+ QString::number(allBallFingerPrint[3 * i + currentColumn]));
				currentColumn += 1;
				currentIndex += 1;
			}
		}
	}

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
double ZYXtest::GetPointDistance(const mitk::Point3D p0, const mitk::Point3D p1)
{
	double distance = sqrt(
		pow(p0[0] - p1[0], 2) +
		pow(p0[1] - p1[1], 2) +
		pow(p0[2] - p1[2], 2)
	);

	return distance;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
bool ZYXtest::GetCoarseSteelballCenters(double steelballVoxel)
{
	if (GetDataStorage()->GetNamedNode("Steelball centers") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	}

	// INPUT 1: inputCtImage (MITK image)
	auto inputCtImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");

	auto mitkSteelBallSurfaces = mitk::Surface::New();
	mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

	imageToSurfaceFilter->SetInput(inputCtImage);
	imageToSurfaceFilter->SetThreshold(steelballVoxel);
	mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();
	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("TMP ball node");
	tmpNode->SetData(mitkSteelBallSurfaces);
	//GetDataStorage()->Add(tmpNode);

	if (mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells() > 300000) // To ensure the speed
	{
		auto tmpPointset = mitk::PointSet::New();
		auto nodeSortedSteelballCenters = mitk::DataNode::New();
		nodeSortedSteelballCenters->SetName("Steelball centers");
		// add new node
		nodeSortedSteelballCenters->SetData(tmpPointset);
		GetDataStorage()->Add(nodeSortedSteelballCenters);

		return false;
	}

	// m_Controls.textBrowser->append("cells: "+QString::number(mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells()));

	// Separate steelball surface by examining their connectivity
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(mitkSteelBallSurfaces->GetVtkPolyData());

	vtkConnectivityFilter->SetExtractionModeToAllRegions();
	vtkConnectivityFilter->Update();
	int numberOfPotentialSteelBalls = vtkConnectivityFilter->GetNumberOfExtractedRegions();


	auto mitkSingleSteelballCenterPointset = mitk::PointSet::New(); // store each steelball's center
	double centerOfAllSteelballs[3]{ 0, 0, 0 };                       // the center of all steel balls

	// Iterate over all the potential steelball regions, extract the real ones by assessing their size (facets numbers)
	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();
	for (int m = 0; m < numberOfPotentialSteelBalls; m++)
	{
		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(m);
		vtkConnectivityFilter->Update();
		// m_Controls.textBrowser->append("Connectivity passed");

		auto vtkSingleSteelBallSurface = vtkConnectivityFilter->GetPolyDataOutput();

		auto numberOfCells =
			vtkSingleSteelBallSurface->GetNumberOfCells(); // the total number of cells of a single mesh surface; each cell
														   // stores one facet of the mesh surface

		  // INPUT 3 & 4: facetNumberUpperThreshold (int) & facetNumberUpperThreshold (int)
		int facetNumberUpperThreshold = 2000;// m_Controls.lineEdit_ballMaxCell->text().toInt();
		int facetNumberLowerThreshold = 150; // m_Controls.lineEdit_ballMinCell->text().toInt();

		if (numberOfCells < facetNumberUpperThreshold && numberOfCells > facetNumberLowerThreshold)
		{
			std::vector<double> inp_x(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_y(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_z(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface

			for (int n = 0; n < numberOfCells; n++)
			{
				auto tmpPoint = vtkSingleSteelBallSurface->GetCell(n)->GetPoints()->GetPoint(0);

				inp_x[n] = tmpPoint[0];
				inp_y[n] = tmpPoint[1];
				inp_z[n] = tmpPoint[2];
			}

			// use inp_x, inp_y and inp_z to simulate a sphere
			double cx, cy, cz;
			double R;

			lancetAlgorithm::fit_sphere(inp_x, inp_y, inp_z, cx, cy, cz, R);

			mitk::Point3D mitkTmpCenterPoint3D;
			mitkTmpCenterPoint3D[0] = cx;
			mitkTmpCenterPoint3D[1] = cy;
			mitkTmpCenterPoint3D[2] = cz;
			mitkSingleSteelballCenterPointset->InsertPoint(mitkTmpCenterPoint3D);

			centerOfAllSteelballs[0] = centerOfAllSteelballs[0] + cx;
			centerOfAllSteelballs[1] = centerOfAllSteelballs[1] + cy;
			centerOfAllSteelballs[2] = centerOfAllSteelballs[2] + cz;

		}


	}

	int numberOfActualSteelballs = mitkSingleSteelballCenterPointset->GetSize();

	centerOfAllSteelballs[0] = centerOfAllSteelballs[0] / numberOfActualSteelballs;
	centerOfAllSteelballs[1] = centerOfAllSteelballs[1] / numberOfActualSteelballs;
	centerOfAllSteelballs[2] = centerOfAllSteelballs[2] / numberOfActualSteelballs;

	// Sort the centers of the separate steelballs according to their distances to the group center
	std::vector<double> distancesToPointSetCenter(numberOfActualSteelballs);
	std::vector<int> distanceRanks(numberOfActualSteelballs);

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		distancesToPointSetCenter[i] =
			sqrt(pow(centerOfAllSteelballs[0] - mitkSingleSteelballCenterPointset->GetPoint(i)[0], 2) +
				pow(centerOfAllSteelballs[1] - mitkSingleSteelballCenterPointset->GetPoint(i)[1], 2) +
				pow(centerOfAllSteelballs[2] - mitkSingleSteelballCenterPointset->GetPoint(i)[2], 2));

		distanceRanks[i] = i;
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance before sorting: " << distancesToPointSetCenter[i];
	}

	for (int i = 0; i < numberOfActualSteelballs - 2; i++)
	{
		for (int j = 0; j < numberOfActualSteelballs - 1 - i; j++)
		{
			double temp = 0;
			double temp2 = 0;
			if (distancesToPointSetCenter[j] > distancesToPointSetCenter[j + 1])
			{
				temp = distancesToPointSetCenter[j];
				distancesToPointSetCenter[j] = distancesToPointSetCenter[j + 1];
				distancesToPointSetCenter[j + 1] = temp;

				temp2 = distanceRanks[j];
				distanceRanks[j] = distanceRanks[j + 1];
				distanceRanks[j + 1] = temp2;
			}
		}
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance after sorting: " << distancesToPointSetCenter[i];
	}

	auto mitkSortedSingleSteelballCenterPointset = mitk::PointSet::New();
	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		mitkSortedSingleSteelballCenterPointset->InsertPoint(mitkSingleSteelballCenterPointset->GetPoint(distanceRanks[i]));
	}


	// add sorted steel ball centers
	auto nodeSortedSteelballCenters = mitk::DataNode::New();
	nodeSortedSteelballCenters->SetName("Steelball centers");
	// add new node
	nodeSortedSteelballCenters->SetData(mitkSortedSingleSteelballCenterPointset);
	GetDataStorage()->Add(nodeSortedSteelballCenters);

	return true;
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs)
{
	int oldNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
	int newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	// m_Controls.textBrowser->append("old: " + QString::number(oldNumOfCenters));
	// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
	//
	int limit{ 0 };

	while (newNumOfCenters != oldNumOfCenters)
	{
		// m_Controls.textBrowser->append("old: "+ QString::number(oldNumOfCenters));
		// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
		oldNumOfCenters = newNumOfCenters;
		// m_Controls.textBrowser->append(QString::number(limit));

		ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
		newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
		limit += 1;
		if (limit == 20)
		{
			m_Controls.textBrowser->append("--- Warning: Maximal screening iteration cycle has been reached ---");
			break;
		}
	}

	RemoveRedundantCenters();
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::RemoveRedundantCenters()
{
	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto tmpPointSet = mitk::PointSet::New();

	for (int i{ 0 }; i < inputPointSet->GetSize(); i++)
	{
		bool unique = true;
		for (int j{ 0 }; j < tmpPointSet->GetSize(); j++)
		{
			if (GetPointDistance(tmpPointSet->GetPoint(j), inputPointSet->GetPoint(i)) < 0.05)
			{
				unique = false;
				break;
			}
		}

		if (unique == true)
		{
			tmpPointSet->InsertPoint(inputPointSet->GetPoint(i));
		}
	}


	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPointSet);
	GetDataStorage()->Add(tmpNode);
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, std::vector<int>& foundIDs)
{
	fill(foundIDs.begin(), foundIDs.end(), 0);
	//foundIDs[0] = 0;
	//foundIDs[1] = 0;
	//foundIDs[2] = 0;
	//foundIDs[3] = 0;
	//foundIDs[4] = 0;
	//foundIDs[5] = 0;
	//foundIDs[6] = 0;

	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	int inputPoinSetNum = inputPointSet->GetSize();
	auto steelballCenters = mitk::PointSet::New();

	for (int i = 0; i < inputPoinSetNum; i++)
	{
		MITK_INFO << "raw extracted point " + QString::number(i);
		MITK_INFO << QString::number(inputPointSet->GetPoint(i)[0]) + ","
			+ QString::number(inputPointSet->GetPoint(i)[1]) + ","
			+ QString::number(inputPointSet->GetPoint(i)[2]);
	}

	int lengthOfFingerPrint = stdNeighborNum;
	int numOfTargetSteelballs = stdNeighborNum + 1;

	for (int q{ 0 }; q < numOfTargetSteelballs; q++)
	{
		std::vector<double> fingerPrint(lengthOfFingerPrint, 0);
		for (int i = 0; i < lengthOfFingerPrint; i++)
		{
			fingerPrint[i] = allBallFingerPrint[lengthOfFingerPrint * q + i];
		}
		//double fingerPrint[6]
		//{
		//        allBallFingerPrint[6 * q],
		//        allBallFingerPrint[6 * q + 1],
		//        allBallFingerPrint[6 * q + 2],
		//        allBallFingerPrint[6 * q + 3],
		//        allBallFingerPrint[6 * q + 4],
		//        allBallFingerPrint[6 * q + 5],
		//};


		for (int i{ 0 }; i < inputPoinSetNum; i++)
		{
			int metric{ 0 };

			for (int j{ 0 }; j < lengthOfFingerPrint; j++)
			{
				bool metSingleRequirement = false;
				double standardDistance = fingerPrint[j];

				for (int k{ 0 }; k < inputPoinSetNum; k++)
				{
					double tmpDistance = GetPointDistance(inputPointSet->GetPoint(i), inputPointSet->GetPoint(k));
					if (fabs(tmpDistance - standardDistance) < 1)//to do:error to be confirmed
					{
						metSingleRequirement = true;
						metric += 1;
						break;
					}
				}

				// if (metSingleRequirement == false)
				// {
				//         break;
				// }

			}

			//m_Controls.textBrowser->append("metric: " + QString::number(metric));
			if (metric >= requiredNeighborNum)
			{

				// Add this point to the pointset
				steelballCenters->InsertPoint(inputPointSet->GetPoint(i));
				//break;
				foundIDs[q] = 1;
			}

		}

	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(steelballCenters);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 提取金属球里面的功能函数（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::RearrangeSteelballs(int stdNeighborNum, std::vector<int>& foundIDs)
{
	auto extractedPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto standartSteelballCenters = mitk::PointSet::New();

	int stdCenterNum = realballnumber;
	//int stdCenterNum{ 7 };

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	auto partialStdCenters = mitk::PointSet::New();
	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		if (foundIDs[i] == 1)
		{
			partialStdCenters->InsertPoint(standartSteelballCenters->GetPoint(i));
		}
	}

	// Todo: this array should be generated automatically
	// int a[] = { 0,1,2,3,4,5,6,7,8,9 }; // in case the result of iterative search has pointNum larger than 7

	std::vector<int> a;
	for (int n{ 0 }; n < extractedPointSet->GetSize(); n++)
	{
		a.push_back(n);
	}

	std::sort(a.begin(), a.end());

	double errorSum = 50;
	int permutation_time = 0;
	std::vector<int> b(realballnumber, 0);
	for (int i = 0; i < realballnumber; i++)
	{
		b[i] = i;
	}
	//int b[7]{ 0,1,2,3,4,5,6 };
	do
	{
		auto newPset = mitk::PointSet::New();


		for (int i{ 0 }; i < extractedPointSet->GetSize(); i++)
		{
			// if (a[i] < (extractedPointSet->GetSize()))
			// {
			// 	newPset->InsertPoint(extractedPointSet->GetPoint(a[i]));
			// 	
			// }
			if ((newPset->GetSize()) < partialStdCenters->GetSize())
			{
				newPset->InsertPoint(extractedPointSet->GetPoint(a[i]));
				MITK_INFO << "extracted point " + QString::number(i);
				MITK_INFO << QString::number(newPset->GetPoint(i)[0]) + ","
					+ QString::number(newPset->GetPoint(i)[1]) + ","
					+ QString::number(newPset->GetPoint(i)[2]);

				//MITK_INFO << "partialStdCenters point " + QString::number(i);
				//MITK_INFO << QString::number(partialStdCenters->GetPoint(i)[0]) + ","
				//	+ QString::number(partialStdCenters->GetPoint(i)[1]) + ","
				//	+ QString::number(partialStdCenters->GetPoint(i)[2]);
			}
		}

		auto landmarkRegistrator = mitk::SurfaceRegistration::New();
		landmarkRegistrator->SetLandmarksSrc(newPset);
		landmarkRegistrator->SetLandmarksTarget(partialStdCenters);

		landmarkRegistrator->ComputeLandMarkResult();
		double maxError = landmarkRegistrator->GetmaxLandmarkError();
		double avgError = landmarkRegistrator->GetavgLandmarkError();
		double tmpError = maxError + avgError;

		MITK_INFO << "tmpError: " << tmpError;
		MITK_INFO << "errorSum: " << errorSum;

		if (tmpError < errorSum && tmpError > 0)
		{
			errorSum = tmpError;

			for (int i = 0; i < realballnumber; i++)
			{
				b[i] = a[i];
			}
			//b[0] = a[0];
			//b[1] = a[1];
			//b[2] = a[2];
			//b[3] = a[3];
			//b[4] = a[4];
			//b[5] = a[5];
			//b[6] = a[6];
		}

		if (tmpError < 1)
		{
			break;
		}
		permutation_time++;

	} while (std::next_permutation(a.begin(), a.end()));

	MITK_INFO << "permutation_time:" + QString::number(permutation_time);

	auto tmpPset = mitk::PointSet::New();

	for (int i = 0; i < realballnumber; i++)
		//for (int i{ 0 }; i < 7; i++)
	{
		if (i < partialStdCenters->GetSize())
		{
			tmpPset->InsertPoint(extractedPointSet->GetPoint(b[i]));
		}
	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPset);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 探针采集点（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::on_pushButton_collectDitch_clicked()
{
	if (m_probeDitchPset_rf == nullptr)
	{
		m_probeDitchPset_rf = mitk::PointSet::New();
	}
	m_IndexOfLandmark++;
	m_Controls.lineEdit_collectedLandmark_3->setText(QString::number(m_IndexOfLandmark));
	m_Controls.textBrowser->append(QString(" m_IndexOfLandmark: ") + QString::number(m_IndexOfLandmark));
	//获取T_patientToProbeRF矩阵
	auto vtkT_CameraToProbe = vtkMatrix4x4::New();
	auto vtkT_PatientRFToCamera = vtkMatrix4x4::New();
	vtkT_CameraToProbe->DeepCopy(T_CamToProbe);
	vtkT_PatientRFToCamera->DeepCopy(T_CamToPatientRF);
	vtkT_PatientRFToCamera->Invert();

	auto transform = vtkTransform::New();
	transform->Identity();
	transform->PostMultiply();
	transform->SetMatrix(vtkT_CameraToProbe);
	transform->Concatenate(vtkT_PatientRFToCamera);
	transform->Update();
	auto T_patientToProbeRF = transform->GetMatrix();

	//保存T_patientrfToProbeRF
	/*memcpy_s(T_PatientRFToProbe, sizeof(double) * 16, T_patientToProbeRF->GetData(), sizeof(double) * 16);*/

	//获取probe尖端的点在patinentRF坐标系下的坐标
	auto tmptrans = vtkTransform::New();
	tmptrans->Identity();
	tmptrans->PostMultiply();
	tmptrans->SetMatrix(vtkT_PatientRFToCamera);
	tmptrans->MultiplyPoint(ProbeTop, nd_tip_FpatientRF);
	vtkProbeTip_Oral->InsertNextPoint(nd_tip_FpatientRF[0], nd_tip_FpatientRF[1], nd_tip_FpatientRF[2]);

	m_Controls.textBrowser->append(QString("Probe Point Landmark: (") + QString::number(nd_tip_FpatientRF[0]) + ", " + QString::number(nd_tip_FpatientRF[1]) + ", "
		+ QString::number(nd_tip_FpatientRF[2]) + ")");
}
//---------------------------------------------------------------------------------------------------------------
/**
*@brief 重新进行图像配准(口腔)
*@note  清空的东西
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::ResetImageConfiguration_Oral()
{
	m_IndexOfLandmark = 0;
	m_Controls.lineEdit_collectedLandmark_3->setText(QString::number(0));


	vtkProbeTip_Oral->Reset();
	vtkProbeTip_Oral_icp->Reset();
	m_probeDitchPset_rf = mitk::PointSet::New();
	m_Controls.lineEdit_maxError->setText("NaN");
	m_Controls.lineEdit_avgError->setText("NaN");

	if (vtkProbeTip_Oral->GetNumberOfPoints() == 0 && vtkProbeTip_Oral_icp->GetNumberOfPoints() == 0 && m_probeDitchPset_rf == nullptr)
	{
		m_Controls.textBrowser->append("Replace image configuration");
	}
	else
	{
		m_Controls.textBrowser->append("Replace image configuration failed");
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 图像配准(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::on_pushButton_imageRegisNew_clicked()
{
	if (m_steelBalls_cmm == nullptr || m_probeDitchPset_cmm == nullptr || GetDataStorage()->GetNamedNode("steelball_image") == nullptr)
	{
		m_Controls.textBrowser->append("Image steelBall extraction should be conducted first!");
		return;
	}

	if (m_steelBalls_cmm->GetSize() != dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_image")->GetData())->GetSize())
	{
		m_Controls.textBrowser->append("Image steelBall extraction is not complete!");
		return;
	}
	// Step 1: 金属球从硬件工装到提取出来的金属球的转换矩阵
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	// m_steelBalls_cmm: splint的金属球在初始位置的点集
	landmarkRegistrator->SetLandmarksSrc(m_steelBalls_cmm);
	// 在CBCT中进行金属球提取后的点集
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_image")->GetData()));

	landmarkRegistrator->ComputeLandMarkResult();

	auto tmpMatrix = landmarkRegistrator->GetResult();
	// Step 2: 凹槽点从硬件工装中拿出来
	auto probePoints_cmm = mitk::PointSet::New();

	auto probePoints_image = mitk::PointSet::New();
	// m_probeDitchPset_cmm：splint的凹槽点在初始位置的点集
	for (int i{ 0 }; i < m_probeDitchPset_cmm->GetSize(); i++)
	{
		probePoints_cmm->InsertPoint(m_probeDitchPset_cmm->GetPoint(i));
	}
	// 将凹槽点移动到与提取出的金属球点集匹配位置，记为probePoints_image
	probePoints_cmm->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);
	probePoints_cmm->GetGeometry()->Modified();

	for (int i{ 0 }; i < probePoints_cmm->GetSize(); i++)
	{
		probePoints_image->InsertPoint(probePoints_cmm->GetPoint(i));
	}

	//// Step 3: 把探针采集的点放进去
	//m_probeDitchPset_rf: 在patientRF下的探针采集的凹槽点
	ConvertVTKPointsToMITKPointSet(vtkProbeTip_Oral, m_probeDitchPset_rf);//将点集合的格式转换一下
	if (m_probeDitchPset_rf == nullptr)
	{
		m_Controls.textBrowser->append("No probe ditch point has been captured");
		return;
	}

	if (m_probeDitchPset_rf->GetSize() < 5)
	{
		m_Controls.textBrowser->append("At least 5 probe ditch points should be captured");
		return;
	}
	m_Controls.textBrowser->append("5");
	//// Step 4: 迭代配准
	int collectedPointNum = m_probeDitchPset_rf->GetSize();
	int totalPointNum = probePoints_image->GetSize();

	auto sorted_probeDitchPset_rf = mitk::PointSet::New();
	SortPointSetByDistance(m_probeDitchPset_rf, sorted_probeDitchPset_rf);

	// Generate the tmp landmark_src
	auto tmp_landmark_src = mitk::PointSet::New();
	auto sorted_landmark_src = mitk::PointSet::New();
	double maxError{ 1000 };
	double avgError{ 1000 };

	std::vector<std::vector<int>> combinations = GenerateAllCombinations(totalPointNum - 1, collectedPointNum);

	for (const auto& combination : combinations) {

		tmp_landmark_src->Clear();

		for (int value : combination) {
			tmp_landmark_src->InsertPoint(probePoints_image->GetPoint(value));
		}

		SortPointSetByDistance(tmp_landmark_src, sorted_landmark_src);

		auto tmpLandmarkRegistrator = mitk::SurfaceRegistration::New();
		tmpLandmarkRegistrator->SetLandmarksSrc(sorted_landmark_src);
		tmpLandmarkRegistrator->SetLandmarksTarget(sorted_probeDitchPset_rf);


		m_Controls.textBrowser->append("sorted_landmark_src Pnum: " + QString::number(sorted_landmark_src->GetSize()));

		m_Controls.textBrowser->append("sorted_probeDitchPset_rf Pnum: " + QString::number(sorted_probeDitchPset_rf->GetSize()));

		tmpLandmarkRegistrator->ComputeLandMarkResult();
		double tmpMaxError = tmpLandmarkRegistrator->GetmaxLandmarkError();
		double tmpAvgError = tmpLandmarkRegistrator->GetavgLandmarkError();

		if (tmpMaxError < maxError && tmpAvgError < avgError)
		{
			maxError = tmpMaxError;
			avgError = tmpAvgError;

			m_Controls.lineEdit_maxError->setText(QString::number(tmpMaxError));
			m_Controls.lineEdit_avgError->setText(QString::number(tmpAvgError));

			memcpy_s(T_PatientRFtoImage, sizeof(double) * 16, tmpLandmarkRegistrator->GetResult()->GetData(), sizeof(double) * 16);
		}

		if (maxError < 0.5)
		{
			break;
		}

	}

	if (maxError < 1.5 && avgError < 1.5)
	{
		m_Controls.textBrowser->append("Image registration succeeded");
		m_Stat_patientRFtoImage = true;

	}
	else
	{
		m_Controls.textBrowser->append("Image registration failed, please collect more points or reset!");
		m_Stat_patientRFtoImage = false;

		// Clear m_T_patientRFtoImage
		auto identityMatrix = vtkMatrix4x4::New();
		identityMatrix->Identity();
		memcpy_s(T_PatientRFtoImage, sizeof(double) * 16, identityMatrix->GetData(), sizeof(double) * 16);

	}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief QT开线程刷新探针和TCP坐标系数据(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::UpdateTCPAndProbe()
{

	if (m_OralProbeAndTCP == nullptr)
	{
		m_OralProbeAndTCP = new QTimer(this);
	}

	// GetDataStorage()->GetNamedNode("probe")->SetColor(1, 0, 0);
	// connect(m_OralProbeAndTCP, &QTimer::timeout, this, &ZYXtest::UpdateTCP);
	connect(m_OralProbeAndTCP, &QTimer::timeout, this, &ZYXtest::UpdateProbe);
	m_OralProbeAndTCP->start(100);
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 关闭QT开线程刷新探针和TCP坐标系数据(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::stopUpdateTCPAndProbe()
{
	if (m_OralProbeAndTCP != nullptr && m_OralProbeAndTCP->isActive())
	{
		m_OralProbeAndTCP->stop();
	}
}

//---------------------------------------------------------------------------------------------------------------
/**
 * @brief 将 vtkPoints 对象转换为 mitk::PointSet。(口腔)
 *
 * 该函数接收一个 vtkPoints 对象，并将其点转换为 mitk::PointSet 对象。
 * 它遍历 vtkPoints 对象中的所有点，并将它们插入到 mitk::PointSet 中。
 *
 * @param vtkPoints 一个指向 vtkPoints 对象的 vtkSmartPointer，包含要转换的点。
 * @param mitkPointSet 一个 mitk::PointSet::Pointer，用于存储转换后的点。
 */
 //---------------------------------------------------------------------------------------------------------------
void ZYXtest::ConvertVTKPointsToMITKPointSet(vtkSmartPointer<vtkPoints> vtkPoints, mitk::PointSet::Pointer mitkPointSet)
{
	// Iterate through all points in vtkPoints
	for (vtkIdType i = 0; i < vtkPoints->GetNumberOfPoints(); ++i) {
		double p[3];
		vtkPoints->GetPoint(i, p);

		// Create a MITK Point and set its coordinates
		mitk::PointSet::PointType point;
		point[0] = p[0];
		point[1] = p[1];
		point[2] = p[2];

		// Insert the point into the MITK PointSet
		mitkPointSet->InsertPoint(i, point);
	}
}

//---------------------------------------------------------------------------------------------------------------
void ZYXtest::SortPointSetByDistance(mitk::PointSet::Pointer inputPointSet, mitk::PointSet::Pointer outputPointSet)
{
	// Calculate the mass center
	mitk::Point3D massCenter = CalculateMassCenter(inputPointSet);

	// Get the points container
	// mitk::PointSet::PointsContainer::Pointer pointsContainer = inputPointSet->GetPoints();

	// Convert the points container to a vector for sorting
	std::vector<mitk::Point3D> pointsVector;
	for (int i{ 0 }; i < inputPointSet->GetSize(); i++)
	{
		auto point = inputPointSet->GetPoint(i);
		pointsVector.push_back(point);
	}

	// Sort the points based on their distance from the mass center
	std::sort(pointsVector.begin(), pointsVector.end(),
		[&](const mitk::Point3D& point1, const mitk::Point3D& point2)
		{
			return ComparePointsByDistance(massCenter, point1, point2);
		});

	// Clear existing points in the point set
	outputPointSet->Clear();

	// Add sorted points back to the point set
	for (const auto& sortedPoint : pointsVector)
	{
		outputPointSet->InsertPoint(sortedPoint);
	}
}

bool ZYXtest::ComparePointsByDistance(const mitk::Point3D& massCenter, const mitk::Point3D& point1, const mitk::Point3D& point2)
{
	return CalculateDistance(massCenter, point1) < CalculateDistance(massCenter, point2);
}

double ZYXtest::CalculateDistance(const mitk::Point3D& point1, const mitk::Point3D& point2)
{
	return std::sqrt(std::pow(point1[0] - point2[0], 2) +
		std::pow(point1[1] - point2[1], 2) +
		std::pow(point1[2] - point2[2], 2));
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 算点集的质心(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------

mitk::Point3D ZYXtest::CalculateMassCenter(const mitk::PointSet::Pointer& pointSet)
{
	mitk::Point3D massCenter;

	// Calculate the sum of coordinates
	for (int i{ 0 }; i < pointSet->GetSize(); i++)
	{
		massCenter[0] += pointSet->GetPoint(i)[0];
		massCenter[1] += pointSet->GetPoint(i)[1];
		massCenter[2] += pointSet->GetPoint(i)[2];
	}

	// Divide by the number of points to get the average (mass center)
	int numberOfPoints = pointSet->GetSize();
	massCenter[0] /= numberOfPoints;
	massCenter[1] /= numberOfPoints;
	massCenter[2] /= numberOfPoints;

	return massCenter;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 点集合排序(口腔)
*@note 如果有123个点，会输出123、213、321类似的全部组合
*/
//---------------------------------------------------------------------------------------------------------------
std::vector<std::vector<int>> ZYXtest::GenerateAllCombinations(int m, int n)
{
	std::vector<std::vector<int>> result;
	std::vector<int> currentCombination;
	GenerateCombinations(m, n, 0, currentCombination, result);
	return result;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 点集合排序(功能函数)(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::GenerateCombinations(int m, int n, int index, std::vector<int>& currentCombination, std::vector<std::vector<int>>& result)
{
	if (currentCombination.size() == n) {
		result.push_back(currentCombination);
		return;
	}
	for (int i = index; i <= m; ++i) {
		currentCombination.push_back(i);
		GenerateCombinations(m, n, i + 1, currentCombination, result);
		currentCombination.pop_back();
	}
}


//---------------------------------------------------------------------------------------------------------------
/**
*@brief TCP标定（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::on_pushButton_calibrateDrill_clicked()
{
	//// This function is aimed to calculate m_T_handpieceRFtoDrill

	//// Step 1: calculate T_calibratorRFtoDrill from the
	//// PointSet probe_head_tail_mandible or probe_head_tail_maxilla in the dataStorage
	//if (GetDataStorage()->GetNamedNode("probe_head_tail_mandible") == nullptr ||
	//	GetDataStorage()->GetNamedNode("probe_head_tail_maxilla") == nullptr)
	//{
	//	m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is missing!");
	//	return;
	//}

	//auto probe_head_tail_mandible = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_mandible")->GetData());
	//auto probe_head_tail_maxilla = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_maxilla")->GetData());

	//if (probe_head_tail_mandible->GetSize() != 2 || probe_head_tail_maxilla->GetSize() != 2)
	//{
	//	m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is problematic!");
	//	return;
	//}

	//auto probe_head_tail = mitk::PointSet::New();

	//if (m_Controls.radioButton_maxilla->isChecked())
	//{
	//	probe_head_tail = probe_head_tail_maxilla;
	//}
	//else
	//{
	//	probe_head_tail = probe_head_tail_mandible;
	//}

	//auto probe_head = probe_head_tail->GetPoint(0);
	//auto probe_tail = probe_head_tail->GetPoint(1);

	//Eigen::Vector3d z_probeInCalibratorRF;
	//z_probeInCalibratorRF[0] = probe_tail[0] - probe_head[0];
	//z_probeInCalibratorRF[1] = probe_tail[1] - probe_head[1];
	//z_probeInCalibratorRF[2] = probe_tail[2] - probe_head[2];
	//z_probeInCalibratorRF.normalize();

	//Eigen::Vector3d z_std{ 0,0,1 };

	//Eigen::Vector3d rotAxis = z_std.cross(z_probeInCalibratorRF);

	//rotAxis.normalize();

	//if (rotAxis.norm() < 0.00001) // in case the rotAxis becomes a zero vector
	//{
	//	rotAxis[0] = 1;
	//	rotAxis[1] = 0;
	//	rotAxis[2] = 0;
	//}

	//double rotAngle = 180 * acos(z_std.dot(z_probeInCalibratorRF)) / 3.141592654;

	//auto trans_calibratorRFtoDrill = vtkTransform::New();
	//trans_calibratorRFtoDrill->Identity();
	//trans_calibratorRFtoDrill->PostMultiply();
	//trans_calibratorRFtoDrill->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	//trans_calibratorRFtoDrill->Update();

	//auto T_calibratorRFtoDrill = trans_calibratorRFtoDrill->GetMatrix();
	//T_calibratorRFtoDrill->SetElement(0, 3, probe_head[0]);
	//T_calibratorRFtoDrill->SetElement(1, 3, probe_head[1]);
	//T_calibratorRFtoDrill->SetElement(2, 3, probe_head[2]);

	// for (int i{ 0 }; i < 16; i++)
	// {
	// 	m_T_calibratorRFtoDrill[i] = T_calibratorRFtoDrill->GetData()[i];
	// }

	//memcpy_s(m_T_calibratorRFtoDrill, sizeof(double) * 16, T_calibratorRFtoDrill->GetData(), sizeof(double) * 16);


	//m_Stat_calibratorRFtoDrill = true;

	//// Step 2: Obtain the camera data and assemble the matrix:
	//// T_handpieceRFtoDrill = (T_cameraTohandpieceRF)^-1 * T_cameraToCalibratorRF * T_calibratorRFtoDrill
	//if (m_Stat_cameraToCalibratorRF == false || m_Stat_cameraToHandpieceRF == false)
	//{
	//	m_Controls.textBrowser->append("calibratorRF or handpieceRF is invisible");
	//	return;
	//}


	//if (m_Stat_calibratorRFtoDrill == false)
	//{
	//	m_Controls.textBrowser->append("m_T_calibratorRFtoDrill from hardware design is not ready");
	//	return;
	//}

	//auto T_handpieceRFtoCamera = vtkMatrix4x4::New();
	//auto T_cameraToCalibratorRF = vtkMatrix4x4::New();


	//T_handpieceRFtoCamera->DeepCopy(m_T_cameraToHandpieceRF);
	//T_handpieceRFtoCamera->Invert();

	//T_cameraToCalibratorRF->DeepCopy(m_T_cameraToCalibratorRF);

	//auto trans_handpieceRFtoDrill = vtkTransform::New();
	//trans_handpieceRFtoDrill->Identity();
	//trans_handpieceRFtoDrill->PostMultiply();
	//trans_handpieceRFtoDrill->SetMatrix(T_calibratorRFtoDrill);
	//trans_handpieceRFtoDrill->Concatenate(T_cameraToCalibratorRF);
	//trans_handpieceRFtoDrill->Concatenate(T_handpieceRFtoCamera);
	//trans_handpieceRFtoDrill->Update();

	//// Todo: the matrix below should be averaged for a time span before being stored into m_T_handpieceRFtoDrill
	//auto T_handpieceRFtoDrill = trans_handpieceRFtoDrill->GetMatrix();

	//memcpy_s(m_T_handpieceRFtoDrill, sizeof(double) * 16, T_handpieceRFtoDrill->GetData(), sizeof(double) * 16);

	//m_Stat_handpieceRFtoDrill = true;

	//m_Controls.textBrowser->append("Handpiece calibration succeeded!");

	//m_Controls.textBrowser->append("Drill tip in handpieceRF:"
	//	+ QString::number(m_T_handpieceRFtoDrill[3]) + " / "
	//	+ QString::number(m_T_handpieceRFtoDrill[7]) + " / "
	//	+ QString::number(m_T_handpieceRFtoDrill[11]) + " / "
	//	+ QString::number(m_T_handpieceRFtoDrill[15]));

}


//---------------------------------------------------------------------------------------------------------------
/**
*@brief 手动设置TCP(口腔)
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::setOralTCP()
{
	dX = m_Controls.lineEdit_Oral_x->text().toDouble();
	dY = m_Controls.lineEdit_Oral_y->text().toDouble();
	dZ = m_Controls.lineEdit_Oral_z->text().toDouble();
	dRx = m_Controls.lineEdit_Oral_rx->text().toDouble();
	dRy = m_Controls.lineEdit_Oral_ry->text().toDouble();
	dRz = m_Controls.lineEdit_Oral_rz->text().toDouble();
	PrintTCP_qt(m_Controls.textBrowser, dX, dY, dZ, dRx, dRy, dRz);
	int nRet1 = HRIF_SetTCP(0, 0, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz);
	PrintResult(m_Controls.textBrowser, nRet1, "TCP Set");

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 口腔执行(口腔)
*@note  将影像中的Image坐标系下的点全部都转到Base坐标系下
*@note  然后求解出T_BaseToFlange,借助该矩阵的第一列X方向的方向向量
*@note  使用轴角的方法求解出期望的x轴与想去到的x轴的旋转向量、然后求出其旋转矩阵
*@note  T_BaseToFlange*上面一步的矩阵=机械臂运动控制矩阵，进行X、Y、Z欧拉角分解
*/
//---------------------------------------------------------------------------------------------------------------
bool ZYXtest::InterpretImageLine_Oral()
{
	//确定目标线：会读取线数据导入到P2和P3中
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine_6->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);
	std::cout << "targetPoint_0: (" << targetPoint_0[0] << ", " << targetPoint_0[1] << ", " << targetPoint_0[2] << ")" << std::endl;
	std::cout << "targetPoint_1: (" << targetPoint_1[0] << ", " << targetPoint_1[1] << ", " << targetPoint_1[2] << ")" << std::endl;


	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };

	for (int i{ 0 }; i < 20; i++)
	{
		//获取机械臂配准矩阵T_BaseToBaseRF
		vtkMatrix4x4* vtkT_BaseToBaseRF = vtkMatrix4x4::New();
		vtkT_BaseToBaseRF->DeepCopy(T_BaseToBaseRF);


		//获取T_BaseRFToCamera
		auto vtkT_BaseRFToCamera = vtkMatrix4x4::New();
		vtkT_BaseRFToCamera->DeepCopy(T_CamToBaseRF);
		vtkT_BaseRFToCamera->Invert();

		//获取T_CameraToPatientRF
		auto vtkT_CameraToPatientRF = vtkMatrix4x4::New();
		vtkT_CameraToPatientRF->DeepCopy(T_CamToPatientRF);

		//获取T_PatientRFToImage
		auto vtkT_PatientRFToImage = vtkMatrix4x4::New();
		vtkT_PatientRFToImage->DeepCopy(T_PatientRFtoImage);

		//计算T_BaseToImage
		vtkNew<vtkTransform> Transform;
		Transform->Identity();
		Transform->PostMultiply();
		Transform->SetMatrix(vtkT_PatientRFToImage);
		Transform->Concatenate(vtkT_CameraToPatientRF);
		Transform->Concatenate(vtkT_BaseRFToCamera);
		Transform->Concatenate(vtkT_BaseToBaseRF);
		Transform->Update();
		auto vtkT_BaseToImage = Transform->GetMatrix();

		//获取P0点的坐标
		auto TargetMatrix_0 = vtkMatrix4x4::New();
		TargetMatrix_0->SetElement(0, 3, targetPoint_0[0]);
		TargetMatrix_0->SetElement(1, 3, targetPoint_0[1]);
		TargetMatrix_0->SetElement(2, 3, targetPoint_0[2]);

		vtkNew<vtkTransform> Trans;
		Trans->Identity();
		Trans->PostMultiply();
		Trans->SetMatrix(TargetMatrix_0);
		Trans->Concatenate(vtkT_BaseToImage);
		Trans->Update();
		auto vtkT_BaseToTarget_0 = Trans->GetMatrix();

		//获取P1点的坐标
		auto TargetMatrix_1 = vtkMatrix4x4::New();
		TargetMatrix_1->SetElement(0, 3, targetPoint_1[0]);
		TargetMatrix_1->SetElement(1, 3, targetPoint_1[1]);
		TargetMatrix_1->SetElement(2, 3, targetPoint_1[2]);

		vtkNew<vtkTransform> Trans1;
		Trans1->Identity();
		Trans1->PostMultiply();
		Trans1->SetMatrix(TargetMatrix_1);
		Trans1->Concatenate(vtkT_BaseToImage);
		Trans1->Update();
		auto vtkT_BaseToTarget_1 = Trans1->GetMatrix();

		//计算20个点
		targetPointUnderBase_0[0] += vtkT_BaseToTarget_0->GetElement(0, 3);
		targetPointUnderBase_0[1] += vtkT_BaseToTarget_0->GetElement(1, 3);
		targetPointUnderBase_0[2] += vtkT_BaseToTarget_0->GetElement(2, 3);

		targetPointUnderBase_1[0] += vtkT_BaseToTarget_1->GetElement(0, 3);
		targetPointUnderBase_1[1] += vtkT_BaseToTarget_1->GetElement(1, 3);
		targetPointUnderBase_1[2] += vtkT_BaseToTarget_1->GetElement(2, 3);

	}
	//取平均
	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	//获取机械臂的T_BaseToFlanger
	double dX1 = 0; double dY1 = 0; double dZ1 = 0;
	double dRx1 = 0; double dRy1 = 0; double dRz1 = 0;
	int nRet = HRIF_ReadActTcpPos(0, 0, dX1, dY1, dZ1, dRx1, dRy1, dRz1);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->RotateX(dRx1);
	tmpTrans->RotateY(dRy1);
	tmpTrans->RotateZ(dRz1);
	tmpTrans->Translate(dX1, dY1, dZ1);
	tmpTrans->Update();
	vtkSmartPointer<vtkMatrix4x4> VTKT_BaseToFlanger = tmpTrans->GetMatrix();

	//借用法兰的X轴方向(第一列)
	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = VTKT_BaseToFlanger->GetElement(0, 0);
	currentXunderBase[1] = VTKT_BaseToFlanger->GetElement(1, 0);
	currentXunderBase[2] = VTKT_BaseToFlanger->GetElement(2, 0);
	currentXunderBase.normalize();


	//std::cout << "currentxunderBase: " << currentXunderBase << std::endl;

	//在Base坐标系下目标坐标系X轴的方向向量
	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];
	targetXunderBase.normalize();

	//MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d  rotationAxis;
	rotationAxis = currentXunderBase.cross(targetXunderBase);


	double rotationAngle;//定义旋转角
	if (currentXunderBase.dot(targetXunderBase) > 0) //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;//求向量的模长（sin[rotationAngle]）,再取反三角
	}
	else //如果向量的内积小于0，cos小于0（钝角）
	{
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(VTKT_BaseToFlanger);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);//旋转角度，和旋转向量
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();

	Eigen::Matrix3d Re;

	Re << testMatrix->GetElement(0, 0), testMatrix->GetElement(0, 1), testMatrix->GetElement(0, 2),
		testMatrix->GetElement(1, 0), testMatrix->GetElement(1, 1), testMatrix->GetElement(1, 2),
		testMatrix->GetElement(2, 0), testMatrix->GetElement(2, 1), testMatrix->GetElement(2, 2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	double x = targetPointUnderBase_0[0];
	double y = targetPointUnderBase_0[1];
	double z = targetPointUnderBase_0[2];
	double rx = 180 * eulerAngle[2] / 3.1415;
	double ry = 180 * eulerAngle[1] / 3.1415;
	double rz = 180 * eulerAngle[0] / 3.1415;

	m_Controls.textBrowser->append("-------------------------------------------------------------------------------------------");
	m_Controls.textBrowser->append("TCP_move");
	m_Controls.textBrowser->append("dx=" + QString::number(x));
	m_Controls.textBrowser->append("dy=" + QString::number(y));
	m_Controls.textBrowser->append("dz=" + QString::number(z));
	m_Controls.textBrowser->append("dRx=" + QString::number(rx));
	m_Controls.textBrowser->append("dRy=" + QString::number(ry));
	m_Controls.textBrowser->append("dRz=" + QString::number(rz));
	("-------------------------------------------------------------------------------------------");

	dX = x;
	dY = y;
	dZ = z;
	dRx = rx;
	dRy = ry;
	dRz = rz;

	//机械臂移动
	int nTargetPoint = HRIF_MoveL(0, 0, dX, dY, dZ, dRx, dRy, dRz, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsSeek, nIOBit, nIOState, strCmdID);

	PrintResult(m_Controls.textBrowser, nTargetPoint, "Move");

	return true;
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 力控初始化（口腔）
*@note 力控的初始化：确定力控的方向、确定力控的开启哪个轴
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::selectDirectionForceControl()
{
	//确定力控的坐标系
	int nMode = 1; //1:沿着TCP方向
	int nRet1 = HRIF_SetForceToolCoordinateMotion(0, 0, nMode);
	PrintResult(m_Controls.textBrowser, nRet1, "The force control coordinate system is set to TOOL");

	// 定义力控策略
	int nStrategy = 0;//0：定义为恒力模式  2：定义为越障模式
	int nRet2 = HRIF_SetForceControlStrategy(0, 0, nStrategy);// 设置力控策略为恒力模式
	PrintResult(m_Controls.textBrowser, nRet2, "Set the force control policy to constant force mode");


	//// 开启力控自由驱动
	//bool bEnable = true;
	//int nRet4 = HRIF_SetForceFreeDriveMode(0, 0, bEnable);
	//PrintResult(m_Controls.textBrowser, nRet4, "Start force free drive mode");



	// 定义力控自由驱动自由度状态（那个为1那个轴就是开启状态）
	int nX = 1; int nY = 1; int nZ = 1;
	int nRx = 1; int nRy = 1; int nRz = 1;
	int nRet3 = HRIF_SetFreeDriveMotionFreedom(0, 0, nX, nY, nZ, nRx, nRy, nRz);// 设置力控自由驱动自由度状态
	PrintResult(m_Controls.textBrowser, nRet3, "Set the direction of the force control is along the Z axis");

}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 开启力控（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::openForceControl()
{
	// 开启力控
	int nState = 1;
	int nRet = HRIF_SetForceControlState(0, 0, nState);
	if (!nRet) std::cout << nRet << std::endl;
	nRet = HRIF_SetForceFreeDriveMode(0, 0, 1);
	PrintResult(m_Controls.textBrowser, nRet, "Start force free drive mode");

	selectDirectionForceControl();
	PrintResult(m_Controls.textBrowser, nRet, "Opening force control");
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief 关闭力控（口腔）
*@note
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::closeForceControl()
{
	//关闭力控
	int nState = 0;
	int nRet = HRIF_SetForceControlState(0, 0, nState);
	PrintResult(m_Controls.textBrowser, nRet, "close force control");
}






//============================================脊柱项目探针点TCP模块================================================


//探针点读取
void ZYXtest::ReadSaveFileData()
{
	std::vector<std::vector<double>> data;
	std::string line;
	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\ToolTipData.txt");
	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		std::vector<double> row;
		double value;

		while (iss >> value) {
			row.push_back(value);
		}
		data.push_back(row);
	}

	inputFile.close();
	probeEndOneVector.resize(0);
	for (const auto& row : data) {
		if (row.size() >= 3) { // Ensure there are at least 3 elements in the row
			Eigen::Vector3d vec(row[0], row[1], row[2]);
			m_Controls.textBrowser->append(QString::number(vec[0]) + " " + QString::number(vec[1])
				+ " " + QString::number(vec[2]));
			probeEndOneVector.push_back(vec);
		}
	}
	for (int i = 0; i < probeEndOneVector.size(); i++)
	{
		auto end = probeEndOneVector[i];
		m_Controls.textBrowser->append(QString::number(end[0]) + " " + QString::number(end[1])
			+ " " + QString::number(end[2]));
	}
}


//移动完成
void ZYXtest::MoveFinished()
{
	m_Controls.textBrowser->append("Movement operation completed");
	captureRobot();
}

//判断机械臂到位置
bool ZYXtest::IsRobotArrived()
{
	int nMovingState = 0; int nEnableState = 0; int nErrorState = 0; int nErrorCode = 0;
	int nErrorAxis = 0; int nBreaking = 0; int nPause = 0; int nEmergencyStop = 0;
	int nSaftyGuard = 0; int nElectrify = 0; int nIsConnectToBox = 0; int nBlendingDone = 0; int nInPos = 0;
	int nRet = HRIF_ReadRobotState(0, 0, nMovingState, nEnableState,
		nErrorState, nErrorCode, nErrorAxis, nBreaking, nPause, nEmergencyStop,
		nSaftyGuard, nElectrify, nIsConnectToBox, nBlendingDone, nInPos);
	if (nRet == 0)
	{
		return nInPos;
	}
	m_Controls.textBrowser->append("Failed to call the manipulator move function");
	return false;
}

//判断机械臂是否移动
bool ZYXtest::IsRobotMoving()
{
	int nMovingState = 0; int nEnableState = 0; int nErrorState = 0; int nErrorCode = 0;
	int nErrorAxis = 0; int nBreaking = 0; int nPause = 0; int nEmergencyStop = 0;
	int nSaftyGuard = 0; int nElectrify = 0; int nIsConnectToBox = 0; int nBlendingDone = 0; int nInPos = 0;
	int nRet = HRIF_ReadRobotState(0, 0, nMovingState, nEnableState,
		nErrorState, nErrorCode, nErrorAxis, nBreaking, nPause, nEmergencyStop,
		nSaftyGuard, nElectrify, nIsConnectToBox, nBlendingDone, nInPos);
	if (nRet == 0)
	{
		return nMovingState;
	}
	m_Controls.textBrowser->append("Failed to call the manipulator move function");
	return false;
}

//打印功能函数
void ZYXtest::AppendTextBrowerMatrix(QString matrixName, vtkMatrix4x4* matrix)
{
	m_Controls.textBrowser->append(matrixName);
	auto array = matrix->GetData();
	for (int i = 0; i < 4; ++i)
	{
		QString str = QString::number(array[i * 4]) + " " + QString::number(array[i * 4 + 1]) + " " + QString::number(array[i * 4 + 2]) + " " + QString::number(array[i * 4 + 3]);
		m_Controls.textBrowser->append(str);
	}
}

void ZYXtest::AppendTextBrowerArray(const char* text, std::vector<double> array)
{
	QString str;
	for (int i = 0; i < array.size(); ++i)
	{
		str = str + QString::number(array[i]) + " ";
	}
	str = QString(text) + " " + str;
	m_Controls.textBrowser->append(str);
}

void ZYXtest::AppendTextBrowerArray(const char* text, double* array, int size)
{
	QString str;
	for (int i = 0; i < size; ++i)
	{
		str = str + QString::number(array[i]) + " ";
	}
	str = QString(text) + " " + str;
	m_Controls.textBrowser->append(str);
}

void ZYXtest::AppendTextBrowerArray(const char* text, Eigen::Vector3d array)
{
	QString str;
	for (int i = 0; i < array.size(); ++i)
	{
		str = str + QString::number(array[i]) + " ";
	}
	str = QString(text) + " " + str;
	m_Controls.textBrowser->append(str);
}

//机械臂自动配准
void ZYXtest::MoveRobotByJointAnglesBtnClicked(std::vector<std::vector<double>> JointAngles)
{
	auto moveRobotTask = [this](std::vector<std::vector<double>> JointAngles) {
		for (int i = 0; i < JointAngles.size(); ++i) {
			auto joint = JointAngles[i];
			double dJ1 = joint[0];
			double dJ2 = joint[1];
			double dJ3 = joint[2];
			double dJ4 = joint[3];
			double dJ5 = joint[4];
			double dJ6 = joint[5];


			int nRet = HRIF_MoveJ(0, 0, 0, 0, 0, 0, 0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
				dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
			QMetaObject::invokeMethod(this, [this]() {
				m_Controls.textBrowser->append("Start Move Robot");
				}, Qt::QueuedConnection);
			while (IsRobotMoving()) {
				QThread::sleep(1);
			}
			QThread::sleep(3);
			QMetaObject::invokeMethod(this, [this]() {
				m_Controls.textBrowser->append("Robot Moved Done");
				m_Controls.textBrowser->append("Start Capure Robot ");
				captureRobot();
				m_Controls.textBrowser->append("Capure Robot Done");
				}, Qt::QueuedConnection);
			QThread::sleep(3);
		}
	};

	QFuture<void> future = QtConcurrent::run(moveRobotTask, JointAngles);
	RobotMoveTaskWatcher.setFuture(future);
}

//读取机械臂关节角
std::vector<std::vector<double>> ZYXtest::GetRobotAnglesBtnClicked()
{
	std::vector<std::vector<double>> data;
	std::string line;
	std::ifstream inputFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\RobotAngles.txt");
	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		std::vector<double> row;
		double value;

		while (iss >> value) {
			row.push_back(value);
		}

		data.push_back(row);
	}

	inputFile.close();
	std::cout << std::fixed << std::setprecision(6);
	for (const auto& row : data) {
		for (double value : row) {
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}
	robotJointAngles = data;
	return data;
}

//两点计算TCP
void ZYXtest::CalculateGuiderTCP2BtnClicked()
{
	//设置走线TCP
	auto point2PosInCamera = probeEndTwoVector[0];
	auto point3PosInCamera = probeEndTwoVector[1];
	auto Tflange2camera = CalculateTFlange2Camera(T_FlangeToEdnRF, T_CamToEndRF);
	auto p2 = CalculatePointInFlangePos(Tflange2camera, point2PosInCamera);
	auto p3 = CalculatePointInFlangePos(Tflange2camera, point3PosInCamera);

	Eigen::Vector3d x_tcp;
	x_tcp = p3 - p2;
	x_tcp.normalize();

	Eigen::Vector3d z_flange;// = probeEndTwoVector[2];
	z_flange[0] = 0.0;
	z_flange[1] = 0.0;
	z_flange[2] = 1;

	Eigen::Vector3d y_tcp;
	y_tcp = z_flange.cross(x_tcp);

	y_tcp.normalize();

	Eigen::Vector3d z_tcp;
	z_tcp = x_tcp.cross(y_tcp);

	Eigen::Matrix3d Re;

	Re << x_tcp[0], y_tcp[0], z_tcp[0],
		x_tcp[1], y_tcp[1], z_tcp[1],
		x_tcp[2], y_tcp[2], z_tcp[2];


	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = m_Controls.GuiderLineEditX->text().toDouble();
	tcp[1] = m_Controls.GuiderLineEditY->text().toDouble();
	tcp[2] = m_Controls.GuiderLineEditZ->text().toDouble();
	tcp[3] = vtkMath::DegreesFromRadians(eulerAngle(2));
	tcp[4] = vtkMath::DegreesFromRadians(eulerAngle(1));
	tcp[5] = vtkMath::DegreesFromRadians(eulerAngle(0));
	m_Controls.textBrowser->append("tcp x y z is:");
	m_Controls.textBrowser->append(QString::number(tcp[0]) + " /" + QString::number(tcp[1]) + "/ " + QString::number(tcp[2]));

	m_Controls.textBrowser->append("tcp rx ry rz is:");
	m_Controls.textBrowser->append(QString::number(tcp[3]) + " /" + QString::number(tcp[4]) + " /" + QString::number(tcp[5]));

	int nRet = HRIF_SetTCP(0, 0, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	if (nRet != 0)
	{
		m_Controls.textBrowser->append("set tcp failed");
	}
}


//四个点配准机械臂
void ZYXtest::CalculateEulerByProbeEndPosClicked()
{
	auto Tflange2camera = CalculateTFlange2Camera(T_FlangeToEdnRF, T_CamToEndRF);
	std::vector<Eigen::Vector3d> VectorEigen;
	for (int i = 0; i < probeEndOneVector.size(); ++i)
	{
		//probeEndOneVector[i] = CalculatePointInFlangePos(Tflange2camera, probeEndOneVector[i]);
		VectorEigen.push_back(CalculatePointInFlangePos(Tflange2camera, probeEndOneVector[i]));
	}
	Eigen::Vector3d p1 = VectorEigen[0];
	Eigen::Vector3d p2 = VectorEigen[1];
	Eigen::Vector3d p3 = VectorEigen[2];
	Eigen::Vector3d p4 = VectorEigen[3];
	Eigen::Vector3d RX = Eigen::Vector3d(p2[0] - p3[0], p2[1] - p3[1], p2[2] - p3[2]);
	RX.normalize();
	Eigen::Vector3d RY = Eigen::Vector3d(p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
	RY.normalize();
	Eigen::Vector3d RZ = RX.cross(RY);
	RZ.normalize();

	Eigen::Matrix3d Re;

	Re << RX[0], RY[0], RZ[0],
		RX[1], RY[1], RZ[1],
		RX[2], RY[2], RZ[2];

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	//------------------------------------------------
	double tcp[6];
	tcp[0] = p4[0];
	tcp[1] = p4[1];
	tcp[2] = p4[2];
	tcp[3] = vtkMath::DegreesFromRadians(eulerAngle(2));
	tcp[4] = vtkMath::DegreesFromRadians(eulerAngle(1));
	tcp[5] = vtkMath::DegreesFromRadians(eulerAngle(0));
	m_Controls.textBrowser->append("tcp x y z is:");
	m_Controls.textBrowser->append(QString::number(tcp[0]) + " /" + QString::number(tcp[1]) + "/ " + QString::number(tcp[2]));

	m_Controls.textBrowser->append("tcp rx ry rz is:");
	m_Controls.textBrowser->append(QString::number(tcp[3]) + " /" + QString::number(tcp[4]) + " /" + QString::number(tcp[5]));

	int nRet = HRIF_SetTCP(0, 0, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	if (nRet != 0)
	{
		m_Controls.textBrowser->append("set tcp failed");
	}
}

//计算在相机坐标系下的点到法兰下
Eigen::Vector3d ZYXtest::CalculatePointInFlangePos(vtkMatrix4x4* matrixFlange2Camera, Eigen::Vector3d posInCamera)
{
	vtkSmartPointer<vtkMatrix4x4> Flange2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	Flange2Camera->DeepCopy(matrixFlange2Camera);
	AppendTextBrowerArray("", posInCamera);
	double point[4] = { posInCamera[0], posInCamera[1],posInCamera[2],1 };
	double m[4];
	Flange2Camera->MultiplyPoint(point, m);
	AppendTextBrowerArray("Pos in Flange is", m, 3);

	return Eigen::Vector3d(m[0], m[1], m[2]);
}

//将数值转化未vtk
vtkSmartPointer<vtkMatrix4x4>ZYXtest::GetArray2vtkMatrix(double* array16)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	// 将元素赋值给vtkMatrix4x4
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix->SetElement(i, j, array16[i * 4 + j]);
		}
	}
	return matrix;
}

/// <summary>
/// 计算法兰到相机的转换矩阵
/// 法兰到相机 == 法兰到末端工具 * 相机到末端工具的逆
/// </summary>
/// <param name="TF2ENDRF">法兰到末端工具位置关系</param>
/// <param name="TCamera2EndRF">相机到末端工具的位置关系</param>
/// <returns></returns>

vtkSmartPointer<vtkMatrix4x4> ZYXtest::CalculateTFlange2Camera(double* TF2ENDRF, double* TCamera2EndRF)
{
	auto matrixF2ENDRF = GetArray2vtkMatrix(TF2ENDRF);
	//AppendTextBrowerMatrix("matrixF2ENDRF", matrixF2ENDRF);

	vtkSmartPointer<vtkMatrix4x4> matrixCamera2EndRF = GetArray2vtkMatrix(TCamera2EndRF);
	//AppendTextBrowerMatrix("matrixCamera2EndRF", matrixCamera2EndRF);

	vtkSmartPointer<vtkMatrix4x4> matrixEndRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	matrixEndRF2Camera->DeepCopy(matrixCamera2EndRF);
	matrixEndRF2Camera->Invert();
	//AppendTextBrowerMatrix("matrixEndRF2Camera", matrixEndRF2Camera);

	vtkSmartPointer<vtkMatrix4x4> result = vtkSmartPointer<vtkMatrix4x4>::New();

	// 执行矩阵乘法
	vtkMatrix4x4::Multiply4x4(matrixF2ENDRF, matrixEndRF2Camera, result);
	//AppendTextBrowerMatrix("CalculateTFlange2Camera", result);
	return result;
}

//显示机械臂模型
//void ZYXtest::DisplayRobotFrameBtnClicked()
//{
//	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
//	std::string path = "E:\\RobotFrame";
//	std::filesystem::path rootPath = "E:\\RobotFrame";
//	std::vector<std::filesystem::path> fileNames;
//	std::vector<std::string> surfaceNodeName;
//	for (const auto& entry : std::filesystem::directory_iterator(path)) {
//		if (entry.is_regular_file()) {
//			surfaceNodeName.push_back(entry.path().filename().string());
//			std::filesystem::path fullPath = rootPath / entry.path().filename();
//			fileNames.push_back(fullPath);
//		}
//	}
//	for (const auto& fileName : fileNames) {
//		std::cout << fileName << std::endl;
//	}
//	std::vector<vtkSmartPointer<vtkPolyData>> robotFrameVector;
//
//	for (const auto& fileName : fileNames)
//	{
//		reader->SetFileName(fileName.string().c_str());
//		reader->Update();
//		auto robotframe = reader->GetOutput();
//		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer < vtkPolyData >::New();
//		polyData->DeepCopy(robotframe);
//		robotFrameVector.push_back(polyData);
//	}
//
//	for (int i = 0; i < robotFrameVector.size(); ++i)
//	{
//		mitk::Surface::Pointer surface = mitk::Surface::New();
//		surface->SetVtkPolyData(robotFrameVector[i]);
//		mitk::DataNode::Pointer surfaceNode = mitk::DataNode::New();
//		surfaceNode->SetData(surface);
//		surfaceNode->SetName(surfaceNodeName[i].c_str());
//		GetDataStorage()->Add(surfaceNode);
//	}
//	auto iRenderWindowPart = GetRenderWindowPart();
//	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");
//	renderWindow->ResetView();
//}

//设置四点配准的第四个点【坐标系原点】
void ZYXtest::GetGuiderOriginPosBtnClicked()
{
	//auto newData = GetNewToolData();
	////获取Spine_Probe数据
	//UpdateCameraToToolMatrix(newData, "Spine_Probe", T_CamToProbe, nullptr);
	//Eigen::Vector3d v(newData->tooltip[0], newData->tooltip[1], newData->tooltip[2]);
	//AppendTextBrowerArray("Origin Pos: ", v);
	//probeEndOneVector[3] = v;
	probeEndOneVector[3] = Eigen::Vector3d(ProbeTop);
	AppendTextBrowerArray("Origin is: ", probeEndOneVector[3]);
}

//重置末端点
void ZYXtest::ResetProbeEndPosBtnClicked(int type)
{
	if (type == 1)
	{
		for (int i = 0; i < probeEndOneVector.size(); ++i)
		{
			this->probeEndOneVector[i].setZero();
		}
		ProbEndCountOne = 0;
		m_Controls.ProbeEndOneCountLineEdit->setText(QString::number(ProbEndCountOne));
		return;
	}
	for (int i = 0; i < probeEndTwoVector.size(); ++i)
	{
		this->probeEndTwoVector[i].setZero();
	}
	ProbEndCountTwo = 0;
	m_Controls.ProbeEndTwoCountLineEdit->setText(QString::number(ProbEndCountTwo));
}

//根据输入的类型往传参数 1是四点 2是三点
void ZYXtest::GetProbeEndPosBtnClicked(int type)
{
	// ReadSaveFileData();
	int& currentCount = (type == 1) ? ProbEndCountOne : ProbEndCountTwo;
	int maxCount = (type == 1) ? 3 : 2;
	QLineEdit* countLineEdit = (type == 1) ? m_Controls.ProbeEndOneCountLineEdit : m_Controls.ProbeEndTwoCountLineEdit;
	std::vector<Eigen::Vector3d>& probeEndVector = (type == 1) ? probeEndOneVector : probeEndTwoVector;

	if (currentCount == maxCount)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("waring"));
		msgBox.setInformativeText(QString::fromLocal8Bit("The collection quantity has reached the upper limit, please reset"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return;
	}

	currentCount++;
	countLineEdit->setText(QString::number(currentCount));

	//auto newData = GetNewToolData();
	// 获取Spine_Probe数据
	//UpdateCameraToToolMatrix(newData, "Spine_Probe", T_CamToProbe, nullptr);

	//Eigen::Vector3d v(newData->tooltip[0], newData->tooltip[1], newData->tooltip[2]);
	std::string str = "Probe End Pos " + std::to_string(currentCount) + ": ";
	AppendTextBrowerArray(str.c_str(), ProbeTop, 3);

	probeEndVector[currentCount - 1] = Eigen::Vector3d(ProbeTop);
}

vtkSmartPointer<vtkPoints> ZYXtest::TransformVTKPoints(vtkPoints* in, vtkMatrix4x4* m)
{
	vtkSmartPointer<vtkPoints> tmpPoints = vtkSmartPointer<vtkPoints>::New();
	tmpPoints->DeepCopy(in);
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(tmpPoints);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkMatrix4x4> tmpMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	tmpMatrix->DeepCopy(m);
	vtkSmartPointer<vtkTransform> trans = vtkSmartPointer<vtkTransform>::New();
	trans->SetMatrix(tmpMatrix);
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(trans);
	transformFilter->Update();
	vtkSmartPointer<vtkPoints> result = vtkSmartPointer<vtkPoints>::New();
	result->DeepCopy(transformFilter->GetOutput()->GetPoints());
	return result;
}

double ZYXtest::GetRegisrationRMS(mitk::PointSet* points, mitk::Surface* surface, vtkMatrix4x4* matrix)
{
	vtkSmartPointer<vtkPoints> pSource =
		vtkSmartPointer<vtkPoints>::New();

	for (int i = 0; i < points->GetSize(); i++)
	{
		auto pointVega = points->GetPoint(i);
		pSource->InsertNextPoint(pointVega[0], pointVega[1], pointVega[2]);
	}
	vtkSmartPointer<vtkPoints> transformedPoints =
		vtkSmartPointer<vtkPoints>::New();
	transformedPoints->DeepCopy(TransformVTKPoints(pSource, matrix));

	vtkSmartPointer<vtkKdTree> KDTree = vtkSmartPointer<vtkKdTree>::New();
	KDTree->BuildLocatorFromPoints(surface->GetVtkPolyData()->GetPoints());
	double sum = 0.0;
	for (int i = 0; i < transformedPoints->GetNumberOfPoints(); ++i)
	{
		double distance;
		double pos[3];
		transformedPoints->GetPoint(i, pos);
		KDTree->FindClosestPoint(pos, distance);
		sum += distance;
	}

	double result = std::sqrt(sum / transformedPoints->GetNumberOfPoints());
	return result;
}

void ZYXtest::on_pushButton_calibrateDrill_Robot_clicked()
{
	// EndRF: 机械臂末端追踪阵列
	// CalibratorRF: TCP标定板
	// TCP: 牙科钻针

	// This function is aimed to calculate T_EndRFRFtoTCP

	// Step 1: calculate T_calibratorRFtoTCP from the
	// PointSet probe_head_tail_mandible or probe_head_tail_maxilla in the dataStorage
	if (GetDataStorage()->GetNamedNode("probe_head_tail_mandible") == nullptr ||
		GetDataStorage()->GetNamedNode("probe_head_tail_maxilla") == nullptr)
	{
		m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is missing!");
		return;
	}

	// probe_head_tail_mandible下颌标定点集
	// probe_head_tail_mandible下颌标定点集

	auto probe_head_tail_mandible = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_mandible")->GetData());
	auto probe_head_tail_maxilla = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_maxilla")->GetData());

	if (probe_head_tail_mandible->GetSize() != 2 || probe_head_tail_maxilla->GetSize() != 2)
	{
		m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is problematic!");
		return;
	}

	auto probe_head_tail = mitk::PointSet::New();

	if (m_Controls.radioButton_maxilla->isChecked())
	{
		probe_head_tail = probe_head_tail_maxilla;
	}
	else
	{
		probe_head_tail = probe_head_tail_mandible;
	}

	// 计算法兰到EndRF的转换矩阵T_FlangeRFToEndRF
	auto T_EndRFtoCamera = vtkMatrix4x4::New();
	auto T_CameratoCalibratorRF = vtkMatrix4x4::New();
	auto T_FlangetoEndRF = vtkMatrix4x4::New();

	T_EndRFtoCamera->DeepCopy(T_CamToEndRF);
	T_EndRFtoCamera->Invert();
	T_CameratoCalibratorRF->DeepCopy(T_CamToCalibratorRF);
	T_FlangetoEndRF->DeepCopy(T_FlangeToEdnRF);
	//auto tmpTrans = vtkTransform::New();
	//tmpTrans->Identity();
	//tmpTrans->PostMultiply();
	//tmpTrans->SetMatrix(T_CameraToEndRF);

	auto probe_head = probe_head_tail->GetPoint(0);
	auto probe_tail = probe_head_tail->GetPoint(1);

	Eigen::Vector3d z_probeInCalibratorRF;
	z_probeInCalibratorRF[0] = probe_tail[0] - probe_head[0];
	z_probeInCalibratorRF[1] = probe_tail[1] - probe_head[1];
	z_probeInCalibratorRF[2] = probe_tail[2] - probe_head[2];
	z_probeInCalibratorRF.normalize();

	Eigen::Vector3d z_std{ 0,0,1 };

	Eigen::Vector3d rotAxis = z_std.cross(z_probeInCalibratorRF);

	rotAxis.normalize();

	if (rotAxis.norm() < 0.00001) // in case the rotAxis becomes a zero vector
	{
		rotAxis[0] = 1;
		rotAxis[1] = 0;
		rotAxis[2] = 0;
	}

	double rotAngle = 180 * acos(z_std.dot(z_probeInCalibratorRF)) / 3.141592654;

	auto trans_calibratorRFtoTCP = vtkTransform::New();
	trans_calibratorRFtoTCP->Identity();
	trans_calibratorRFtoTCP->PostMultiply();
	trans_calibratorRFtoTCP->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	trans_calibratorRFtoTCP->Update();

	auto T_CalibratorRFtoTCP = trans_calibratorRFtoTCP->GetMatrix();
	T_CalibratorRFtoTCP->SetElement(0, 3, probe_head[0]);
	T_CalibratorRFtoTCP->SetElement(1, 3, probe_head[1]);
	T_CalibratorRFtoTCP->SetElement(2, 3, probe_head[2]);

	memcpy_s(T_calibratorRFToTCP, sizeof(double) * 16, T_CalibratorRFtoTCP->GetData(), sizeof(double) * 16);

	// for (int i{ 0 }; i < 16; i++)
	// {
	// 	m_T_calibratorRFtoDrill[i] = T_calibratorRFtoDrill->GetData()[i];
	// }

	//计算 T_calibratorRFtoTCP
	auto trans_EndRFtoCalibratorRF = vtkTransform::New();
	trans_EndRFtoCalibratorRF->Identity();
	trans_EndRFtoCalibratorRF->PostMultiply();
	trans_EndRFtoCalibratorRF->SetMatrix(T_CameratoCalibratorRF);
	trans_EndRFtoCalibratorRF->Concatenate(T_EndRFtoCamera);
	trans_EndRFtoCalibratorRF->Update();
	auto T_EndRFtoCalibratorRF = trans_EndRFtoCalibratorRF->GetMatrix();

	memcpy_s(T_EndRFToCalibratorRF, sizeof(double) * 16, T_EndRFtoCalibratorRF->GetData(), sizeof(double) * 16);




	// Step 2: Obtain the camera data and assemble the matrix:
	// T_handpieceRFtoDrill = (T_cameraTohandpieceRF)^-1 * T_cameraToCalibratorRF * T_calibratorRFtoDrill


	auto trans_FlangetoTCP = vtkTransform::New();
	trans_FlangetoTCP->Identity();
	trans_FlangetoTCP->PostMultiply();
	trans_FlangetoTCP->SetMatrix(T_CalibratorRFtoTCP);
	trans_FlangetoTCP->Concatenate(T_CameratoCalibratorRF);
	trans_FlangetoTCP->Concatenate(T_EndRFtoCamera);
	trans_FlangetoTCP->Concatenate(T_FlangetoEndRF);
	trans_FlangetoTCP->Update();

	// Todo: the matrix below should be averaged for a time span before being stored into m_T_handpieceRFtoDrill
	auto T_FlanegtoTCP = trans_FlangetoTCP->GetMatrix();

	memcpy_s(T_FlangeToTCP, sizeof(double) * 16, T_FlanegtoTCP->GetData(), sizeof(double) * 16);
	Eigen::Vector3d RX;
	Eigen::Vector3d RY;
	Eigen::Vector3d RZ;
	Eigen::Matrix3d Re;

	RX[0] = T_FlanegtoTCP->GetElement(0, 0);
	RX[1] = T_FlanegtoTCP->GetElement(1, 0);
	RX[2] = T_FlanegtoTCP->GetElement(2, 0);

	RY[0] = T_FlanegtoTCP->GetElement(0, 1);
	RY[1] = T_FlanegtoTCP->GetElement(1, 1);
	RY[2] = T_FlanegtoTCP->GetElement(2, 1);

	RZ[0] = T_FlanegtoTCP->GetElement(0, 2);
	RZ[1] = T_FlanegtoTCP->GetElement(1, 2);
	RZ[2] = T_FlanegtoTCP->GetElement(2, 2);

	Re << RX[0], RY[0], RZ[0],
		RX[1], RY[1], RZ[1],
		RX[2], RY[2], RZ[2];

	// 欧拉角顺序zyx
	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	
	finaltcp[0] = T_FlanegtoTCP->GetElement(0, 3);
	finaltcp[1] = T_FlanegtoTCP->GetElement(1, 3);
	finaltcp[2] = T_FlanegtoTCP->GetElement(2, 3);
	finaltcp[3] = vtkMath::DegreesFromRadians(eulerAngle(2));
	finaltcp[4] = vtkMath::DegreesFromRadians(eulerAngle(1));
	finaltcp[5] = vtkMath::DegreesFromRadians(eulerAngle(0));
	m_Controls.textBrowser->append("tcp x y z is:");
	m_Controls.textBrowser->append(QString::number(finaltcp[0]) + " /" + QString::number(finaltcp[1]) + "/ " + QString::number(finaltcp[2]));

	m_Controls.textBrowser->append("tcp rx ry rz is:");
	m_Controls.textBrowser->append(QString::number(finaltcp[3]) + " /" + QString::number(finaltcp[4]) + " /" + QString::number(finaltcp[5]));

	m_Controls.textBrowser->append("Robot Calibration Succeeded!");


	PrintTCP_qt(m_Controls.textBrowser, finaltcp[0], finaltcp[1], finaltcp[2], finaltcp[3], finaltcp[4], finaltcp[5]);
	int nRet = HRIF_SetTCPByName(0, 0, sTcpName);
	int nRet1 = HRIF_SetTCP(0, 0, finaltcp[0], finaltcp[1], finaltcp[2], finaltcp[3], finaltcp[4], finaltcp[5]);
	PrintResult(m_Controls.textBrowser, nRet1, "TCP Set");
	if (nRet1 == 0) {
		m_Controls.textBrowser->append(" set TCP Succeeded!");
	}
	else {
		m_Controls.textBrowser->append("set TCP  Failed!");
	}
}
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
void ZYXtest::saveRegisMatrix()
{
	try
	{
		std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\DIRobot\\ImageRegis.txt";

		std::filesystem::path dir = std::filesystem::path(desktopPath).parent_path();
		if (!std::filesystem::exists(dir))
		{
			std::filesystem::create_directories(dir);
		}

		std::ofstream robotMatrixFile(desktopPath);
		if (!robotMatrixFile.is_open())
		{
			throw std::ios_base::failure("Failed to open file for writing.");
		}

		for (int i = 0; i < 16; i++)
		{
			robotMatrixFile << T_PatientRFtoImage[i];
			if (i != 15)
			{
				robotMatrixFile << ",";
			}
			else
			{
				robotMatrixFile << ";";
			}
		}
		robotMatrixFile << std::endl;
		robotMatrixFile.close();

		m_Controls.textBrowser->append("registration result saved");
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		m_Controls.textBrowser->append("Failed to save registration result");
	}
}

void ZYXtest::reuseRegisMatrix()
{
	try
	{

		std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\DIRobot\\ImageRegis.txt";

		std::filesystem::path dir = std::filesystem::path(desktopPath).parent_path();
		if (!std::filesystem::exists(dir))
		{
			throw std::ios_base::failure("Directory does not exist.");
		}

		std::ifstream inputFile(desktopPath);
		if (!inputFile.is_open())
		{
			throw std::ios_base::failure("Failed to open file for reading.");
		}

		std::string line;
		if (std::getline(inputFile, line))
		{
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ','))
			{
				if (index < 16)
				{
					T_PatientRFtoImage[index] = std::stod(token);
					index++;
				}
				else
				{
					break;
				}
			}
		}
		inputFile.close();

		// 显示读取的矩阵数据
		QString output;
		for (int i = 0; i < 16; i++)
		{
			output += "Registration Matrix[" + QString::number(i) + "]: " + QString::number(T_PatientRFtoImage[i]) + " ";
		}
		m_Controls.textBrowser->append(output);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		m_Controls.textBrowser->append("Failed to load registration result");
	}
}

void ZYXtest::saveTCPCalibrateMatrix()
{
	try
	{
		std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\DIRobot\\TCPCalibrate.txt";

		std::filesystem::path dir = std::filesystem::path(desktopPath).parent_path();
		if (!std::filesystem::exists(dir))
		{
			std::filesystem::create_directories(dir);
		}

		std::ofstream robotMatrixFile(desktopPath);
		if (!robotMatrixFile.is_open())
		{
			throw std::ios_base::failure("Failed to open file for writing.");
		}

		for (int i = 0; i < 16; i++)
		{
			robotMatrixFile << T_FlangeToTCP[i];
			if (i != 15)
			{
				robotMatrixFile << ",";
			}
			else
			{
				robotMatrixFile << ";";
			}
		}
		robotMatrixFile << std::endl;
		robotMatrixFile.close();

		m_Controls.textBrowser->append("registration result saved");
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		m_Controls.textBrowser->append("Failed to save registration result");
	}
}

void ZYXtest::reuseTCPCalibrateMatrix()
{
	try
	{

		std::string desktopPath = std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\DIRobot\\TCPCalibrate.txt";

		std::filesystem::path dir = std::filesystem::path(desktopPath).parent_path();
		if (!std::filesystem::exists(dir))
		{
			throw std::ios_base::failure("Directory does not exist.");
		}

		std::ifstream inputFile(desktopPath);
		if (!inputFile.is_open())
		{
			throw std::ios_base::failure("Failed to open file for reading.");
		}

		std::string line;
		if (std::getline(inputFile, line))
		{
			std::stringstream ss(line);
			std::string token;
			int index = 0;
			while (std::getline(ss, token, ','))
			{
				if (index < 16)
				{
					T_FlangeToTCP[index] = std::stod(token);
					index++;
				}
				else
				{
					break;
				}
			}
		}
		inputFile.close();

		// 显示读取的矩阵数据
		QString output;
		for (int i = 0; i < 16; i++)
		{
			output += "Registration Matrix[" + QString::number(i) + "]: " + QString::number(T_FlangeToTCP[i]) + " ";
		}
		m_Controls.textBrowser->append(output);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		m_Controls.textBrowser->append("Failed to load registration result");
	}
	vtkSmartPointer <vtkMatrix4x4> T_FlanegtoTCP;
	T_FlanegtoTCP->DeepCopy(T_FlangeToTCP);
	
	Eigen::Vector3d RX;
	Eigen::Vector3d RY;
	Eigen::Vector3d RZ;
	Eigen::Matrix3d Re;

	RX[0] = T_FlanegtoTCP->GetElement(0, 0);
	RX[1] = T_FlanegtoTCP->GetElement(1, 0);
	RX[2] = T_FlanegtoTCP->GetElement(2, 0);

	RY[0] = T_FlanegtoTCP->GetElement(0, 1);
	RY[1] = T_FlanegtoTCP->GetElement(1, 1);
	RY[2] = T_FlanegtoTCP->GetElement(2, 1);

	RZ[0] = T_FlanegtoTCP->GetElement(0, 2);
	RZ[1] = T_FlanegtoTCP->GetElement(1, 2);
	RZ[2] = T_FlanegtoTCP->GetElement(2, 2);

	Re << RX[0], RY[0], RZ[0],
		RX[1], RY[1], RZ[1],
		RX[2], RY[2], RZ[2];

	// 欧拉角顺序zyx
	//Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);

	/*double tcp[6];
	tcp[0] = T_FlanegtoTCP->GetElement(0, 3);
	tcp[1] = T_FlanegtoTCP->GetElement(1, 3);
	tcp[2] = T_FlanegtoTCP->GetElement(2, 3);
	tcp[3] = vtkMath::DegreesFromRadians(eulerAngle(2));
	tcp[4] = vtkMath::DegreesFromRadians(eulerAngle(1));
	tcp[5] = vtkMath::DegreesFromRadians(eulerAngle(0));
	m_Controls.textBrowser->append("tcp x y z is:");
	m_Controls.textBrowser->append(QString::number(tcp[0]) + " /" + QString::number(tcp[1]) + "/ " + QString::number(tcp[2]));

	m_Controls.textBrowser->append("tcp rx ry rz is:");
	m_Controls.textBrowser->append(QString::number(tcp[3]) + " /" + QString::number(tcp[4]) + " /" + QString::number(tcp[5]));

	m_Controls.textBrowser->append("Robot Calibration Succeeded!");


	PrintTCP_qt(m_Controls.textBrowser, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);*/
	//int nRet1 = HRIF_SetTCPByName(0, 0, "TCP_1");
	//if (nRet1 == 0) {
	//	m_Controls.textBrowser->append(" set TCP Succeeded!");
	//}
	//else {
	//	m_Controls.textBrowser->append("set TCP  Failed!");
	//}
	//nRet1 = HRIF_SetTCP(0, 0, tcp[0], tcp[1], tcp[2], tcp[3], tcp[4], tcp[5]);
	//PrintResult(m_Controls.textBrowser, nRet1, "TCP Set");
	//if (nRet1 == 0) {
	//	m_Controls.textBrowser->append(" set TCP Succeeded!");
	//}
	//else {
	//	m_Controls.textBrowser->append("set TCP  Failed!");
	//}
}

//---------------------------------------------------------------------------------------------------------------
/**
*@brief QT开线程刷新探针和TCP坐标系数据(口腔)
*@note 修改
*/
//---------------------------------------------------------------------------------------------------------------
void ZYXtest::on_pushButton_startNavi_clicked()
{
	qDebug()<<"---- - navigation start------";
	m_Controls.textBrowser->append("-----navigation start------");

	disconnect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::UpdateDrillVisual);

	// m_NaviMode = 0;
	//根据输入长度和 m_T_handpieceRFtoInputDrill 修改钻头/探针表面的长度和 m_T_handpieceRFtoInputDrill
	double inputDrillLength = m_Controls.lineEdit_drillLength->text().toDouble();//钻头长度（实际）
	auto probe_head_tail_mandible = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_mandible")->GetData());
	double probeDrillLength = GetPointDistance(probe_head_tail_mandible->GetPoint(0), probe_head_tail_mandible->GetPoint(1));//探针长度（设定）
	double z_scale = inputDrillLength / probeDrillLength;//缩放系数

	auto T_probeDrilltoInputDrill = vtkMatrix4x4::New();
	T_probeDrilltoInputDrill->Identity();
	T_probeDrilltoInputDrill->SetElement(2, 2, z_scale);//设置第三行第三列z轴的值为缩放系数
	T_probeDrilltoInputDrill->SetElement(2, 3, probeDrillLength - inputDrillLength);//设置第三行第四列z轴的平移

	//QString matrixString = MatrixToString(T_probeDrilltoInputDrill);
	//m_Controls.textBrowser->append("T_probeDrilltoInputDrill:\n"+matrixString);
	//auto T_handpieceRFtoDrill_init = vtkMatrix4x4::New();

	auto T_calibratorRFtoTCP = vtkMatrix4x4::New();
	T_calibratorRFtoTCP->DeepCopy(T_calibratorRFToInputTCP);

	auto T_EndRFtoCalibratorRF = vtkMatrix4x4::New();
	T_EndRFtoCalibratorRF->DeepCopy(T_EndRFToCalibratorRF);

	//计算到实际drill的转化矩阵
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(T_probeDrilltoInputDrill);//maybe problem
	tmpTrans->Concatenate(T_calibratorRFtoTCP);
	tmpTrans->Concatenate(T_EndRFtoCalibratorRF);
	tmpTrans->Update();

	//auto tmpTrans = vtkTransform::New();
	//tmpTrans->PostMultiply();
	//tmpTrans->SetMatrix(T_probeDrilltoInputDrill);
	//tmpTrans->Concatenate(T_handpieceRFtoDrill_init);
	//tmpTrans->Update();

	auto T_EndRFtoDrill_new = tmpTrans->GetMatrix();

	//memcpy_s(m_T_handpieceRFtoInputDrill, sizeof(double) * 16, T_EndRFtoDrill_new->GetData(), sizeof(double) * 16);
	memcpy_s(T_EndRFToInputTCP, sizeof(double) * 16, T_EndRFtoDrill_new->GetData(), sizeof(double) * 16);

	TurnOffAllNodesVisibility();
	GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("drillSurface")->SetVisibility(true);

	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(false);

	connect(m_AimoeVisualizeTimer, &QTimer::timeout, this, &ZYXtest::UpdateDrillVisual);

}
void ZYXtest::TurnOffAllNodesVisibility()
{
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}

	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);
}

void ZYXtest::UpdateDrillVisual()
{
	qDebug() << "---- - Get TCP from Camera------";
	m_Controls.textBrowser->append("----Get TCP from Camera------");

	if (GetDataStorage()->GetNamedNode("drillSurface") == nullptr)
	{
		m_Controls.textBrowser->append("drillSurface is missing");
		return;
	}

	//------TCP from Camera Begin-------

/*	auto T_calibratorRFtoTCP = vtkMatrix4x4::New();
	T_calibratorRFtoTCP->DeepCopy(T_calibratorRFToInputTCP);*///源头2

	auto T_EndRFtoInputTCP = vtkMatrix4x4::New();
	T_EndRFtoInputTCP->DeepCopy(T_EndRFToInputTCP);

	auto T_CamtoEndRF = vtkMatrix4x4::New();
	T_CamtoEndRF->DeepCopy(T_CamToEndRF);

	auto T_patientRFtoCamera = vtkMatrix4x4::New();
	T_patientRFtoCamera->DeepCopy(T_CamToPatientRF);
	T_patientRFtoCamera->Invert();

	auto T_imageToPatientRF = vtkMatrix4x4::New();
	T_imageToPatientRF->DeepCopy(T_PatientRFtoImage);
	T_imageToPatientRF->Invert();

	auto trans_imagetoTCP = vtkTransform::New();
	trans_imagetoTCP->Identity();
	trans_imagetoTCP->PostMultiply();
	trans_imagetoTCP->SetMatrix(T_EndRFToInputTCP);
	trans_imagetoTCP->Concatenate(T_CamtoEndRF);
	trans_imagetoTCP->Concatenate(T_patientRFtoCamera);
	trans_imagetoTCP->Concatenate(T_imageToPatientRF);
	trans_imagetoTCP->Update();
	auto T_imagetoTCP = trans_imagetoTCP->GetMatrix();

	memcpy_s(T_ImageToInputTCP, sizeof(double) * 16, T_imagetoTCP->GetData(), sizeof(double) * 16);


	//------TCP from Camera End-------

	

	//------TCP from Robotic arm Begin-------
	//auto T_FlangetoTCP = vtkMatrix4x4::New();
	//T_FlangetoTCP->DeepCopy(T_FlangeToTCP);

	//auto T_CamtoEndRF = vtkMatrix4x4::New();
	//T_CamtoEndRF->DeepCopy(T_CamToEndRF);

	//auto T_EndRFtoFlange = vtkMatrix4x4::New();
	//T_EndRFtoFlange->DeepCopy(T_FlangeToEdnRF);
	//T_EndRFtoFlange->Invert();


	//auto T_patientRFtoCamera = vtkMatrix4x4::New();
	//T_patientRFtoCamera->DeepCopy(T_CamToPatientRF);
	//T_patientRFtoCamera->Invert();

	//auto T_imageToPatientRF = vtkMatrix4x4::New();
	//T_imageToPatientRF->DeepCopy(T_PatientRFtoImage);
	//T_imageToPatientRF->Invert();

	//auto tmpTrans = vtkTransform::New();
	//tmpTrans->Identity();
	//tmpTrans->PostMultiply();
	//tmpTrans->SetMatrix(T_FlangetoTCP);
	//tmpTrans->Concatenate(T_EndRFtoFlange);
	//tmpTrans->Concatenate(T_CamtoEndRF);
	//tmpTrans->Concatenate(T_patientRFtoCamera);
	//tmpTrans->Concatenate(T_imageToPatientRF);
	//tmpTrans->Update();
	//auto T_imagetoTCP = tmpTrans->GetMatrix();

	//memcpy_s(T_ImageToTCP, sizeof(double) * 16, T_imagetoTCP->GetData(), sizeof(double) * 16);

	//------TCP from Robotic arm End-------

	GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(T_imagetoTCP);
	GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->Modified();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	UpdateDeviation();
}