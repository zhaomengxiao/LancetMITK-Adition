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
#include "RecordAndMove.h"

// Qt
#include <QMessageBox>
#include <QObject>

// mitk image
#include <mitkAffineTransform3D.h>
#include <mitkImage.h>
#include <mitkMatrixConvert.h>

//igt
#include <kukaRobotDevice.h>
#include <lancetApplyDeviceRegistratioinFilter.h>
#include <lancetPathPoint.h>
#include <lancetVegaTrackingDevice.h>
#include <mitkNavigationDataToPointSetFilter.h>

#include <QtWidgets\qfiledialog.h>
#include "lancetTrackingDeviceSourceConfigurator.h"
#include "mitkNavigationToolStorageDeserializer.h"

#include "lancetTreeCoords.h"
#include "mitkIGTIOException.h"
#include "mitkNavigationToolStorageSerializer.h"
#include "QmitkIGTCommonHelper.h"

//image registration needed
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "QmitkDataStorageTreeModel.h"
#include <QmitkSingleNodeSelectionWidget.h>
#include <QTimer>
#include <itkAffineTransform.h>
#include <vtkImplicitPolyDataDistance.h>

#include "lancetTreeCoords.h"

//udp
#include <QTimer>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include "kukaRobotAPI/robotInfoProtocol.h"

#include "ui_RecordAndMoveControls.h"

#include "kukaRobotAPI/defaultProtocol.h"
const std::string RecordAndMove::VIEW_ID = "org.mitk.views.recordandmove";

//=====================================  Helper Function =========================================
mitk::NavigationData::Pointer RecordAndMove::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}
//=====================================  Helper Function =========================================


void RecordAndMove::SetFocus()
{
  m_Controls.pushButton_connectKuka->setFocus();
}

void RecordAndMove::CreateQtPartControl(QWidget *parent)
{
	m_Controls.setupUi(parent);
	InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis);
	InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src);
	InitCTSteelballCenterSelector(m_Controls.mitkNodeSelectWidget_CTSteelballCenterInImage);
	InitCTSteelballCenterSelector(m_Controls.mitkNodeSelectWidget_CTSteelballInImage_2);
	connect(m_Controls.pushButton_assembleNavigationObject, &QPushButton::clicked, this, &RecordAndMove::SetupNavigatedImage);
	connect(m_Controls.pushButton_interpretImagePoint, &QPushButton::clicked, this, &RecordAndMove::InterpretImagePoint);
	connect(m_Controls.pushButton_interpretImageLine, &QPushButton::clicked, this, &RecordAndMove::InterpretImageLine);
	connect(m_Controls.pushButton_moveToTargetPoint, &QPushButton::clicked, this, &RecordAndMove::MoveToTargetPoint);
	connect(m_Controls.pushButton_moveToTargetLine, &QPushButton::clicked, this, &RecordAndMove::MoveToTargetLine);
	connect(m_Controls.pushButton_collectLandmark, &QPushButton::clicked, this, &RecordAndMove::CollectLandmarkProbe);
	connect(m_Controls.pushButton_collectIcp, &QPushButton::clicked, this, &RecordAndMove::CollectIcpProbe);
	connect(m_Controls.pushButton_applyRegistration, &QPushButton::clicked, this, &RecordAndMove::ApplySurfaceRegistration);
	connect(m_Controls.pushButton_connectKuka, &QPushButton::clicked, this, &RecordAndMove::UseKuka);
	connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &RecordAndMove::UseVega);
	connect(m_Controls.pushButton_setAsTarget, &QPushButton::clicked, this, &RecordAndMove::SetAsTarget);
	connect(m_Controls.pushButton_capturePose, &QPushButton::clicked, this, &RecordAndMove::OnRobotCapture);
	connect(m_Controls.pushButton_onAutoMove, &QPushButton::clicked, this, &RecordAndMove::OnAutoMove);
	connect(m_Controls.pushButton_moveToHomePosition, &QPushButton::clicked, this, &RecordAndMove::MoveToHomePosition);
	connect(m_Controls.pushButton_moveAlongA, &QPushButton::clicked, this, &RecordAndMove::MoveAlongA);
	connect(m_Controls.pushButton_moveAlongB, &QPushButton::clicked, this, &RecordAndMove::MoveAlongB);
	connect(m_Controls.pushButton_moveAlongC, &QPushButton::clicked, this, &RecordAndMove::MoveAlongC);
	connect(m_Controls.pushButton_moveAlongX, &QPushButton::clicked, this, &RecordAndMove::MoveAlongX);
	connect(m_Controls.pushButton_moveAlongY, &QPushButton::clicked, this, &RecordAndMove::MoveAlongY);
	connect(m_Controls.pushButton_moveAlongZ, &QPushButton::clicked, this, &RecordAndMove::MoveAlongZ);
	connect(m_Controls.pushButton_onSaveRobotRegistration, &QPushButton::clicked, this, &RecordAndMove::OnSaveRobotRegistration);
	connect(m_Controls.pushButton_onSaveNDIRegistration, &QPushButton::clicked, this, &RecordAndMove::OnSaveRobotRegistration);
	connect(m_Controls.pushButton_onUsePreRobotRegistration, &QPushButton::clicked, this, &RecordAndMove::OnUsePreRobotRegistration);
	connect(m_Controls.pushButton_onUsePreNDIRegistration, &QPushButton::clicked, this, &RecordAndMove::OnUsePreNDIRegistration);
}


// Move test
void RecordAndMove::MoveAlongA() {
	//Move the end effector along self(end effector) z-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[3] += (20.0 / 180.0 * 3.14);
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

void RecordAndMove::MoveAlongB() {
	//Move the end effector along self(end effector) y-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[4] += (20.0 / 180.0 * 3.14);
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

void RecordAndMove::MoveAlongC() {
	//Move the end effector along self(end effector) x-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[5] += (20.0 / 180.0 * 3.14);
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

void RecordAndMove::MoveAlongX() {
	//Move the end effector along robot base x-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[0] += 50;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

void RecordAndMove::MoveAlongY() {
	//Move the end effector along robot base y-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[1] += 50;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

void RecordAndMove::MoveAlongZ() {
	//Move the end effector along robot base z-axis
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	p[2] += 50;
	cout << p[0] << ",  " << p[1] << ",  " << p[2] << ",  " << p[3] << ",  " << p[4] << ",  " << p[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
}

// Line Accuracy Test
bool RecordAndMove::InterpretImageLine()
{
	//read file, get 2 point position in F_Surface
	auto targetLinePointsNode = m_Controls.mitkNodeSelectWidget_CTSteelballInImage_2->GetSelectedNode();
	if (targetLinePointsNode == nullptr)
	{
		cout << "Source surface or source landmarks is not ready!" << endl;
		return false;
	}
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(targetLinePointsNode->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(2); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);

	// Interpret targetPoint_0 from Surface frame to robot (internal) base frame
	// do some average
	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };
	for (int i{ 0 }; i < 20; i++) {
		m_VegaSource->Update();
		auto F_NDI_ObjectRf = m_VegaSource->GetOutput("ObjectRf")->GetAffineTransform3D();
		auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
		vtkNew<vtkMatrix4x4> tmpMatrix;
		mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), tmpMatrix);
		tmpMatrix->Invert();
		auto F_ObjectRf_Surface = mitk::AffineTransform3D::New();
		mitk::TransferVtkMatrixToItkTransform(tmpMatrix, F_ObjectRf_Surface.GetPointer());

		//convert point0 position in Surface frame to point0 position in RobotBaseRF frame
		auto F_NDI_P0 = mitk::AffineTransform3D::New();
		F_NDI_P0->SetIdentity();
		F_NDI_P0->SetOffset(targetPoint_0.GetDataPointer());
		F_NDI_P0->Compose(F_ObjectRf_Surface);
		F_NDI_P0->Compose(F_NDI_ObjectRf);

		auto F_RobotBaseRF_P0 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", F_NDI_P0, F_RobotBaseRF_P0);
		auto targetPointUnderBase_tmp0 = F_RobotBaseRF_P0->GetOffset();

		//convert point1 position in Surface frame to point1 position in RobotBaseRF frame
		auto F_NDI_P1 = mitk::AffineTransform3D::New();
		F_NDI_P1->SetIdentity();
		F_NDI_P1->SetOffset(targetPoint_1.GetDataPointer());
		F_NDI_P1->Compose(F_ObjectRf_Surface);
		F_NDI_P1->Compose(F_NDI_ObjectRf);

		auto F_RobotBaseRF_P1 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF",F_NDI_P1, F_RobotBaseRF_P1);
		auto targetPointUnderBase_tmp1 = F_RobotBaseRF_P1->GetOffset();

		targetPointUnderBase_0[0] += targetPointUnderBase_tmp0[0];
		targetPointUnderBase_0[1] += targetPointUnderBase_tmp0[1];
		targetPointUnderBase_0[2] += targetPointUnderBase_tmp0[2];
		targetPointUnderBase_1[0] += targetPointUnderBase_tmp1[0];
		targetPointUnderBase_1[1] += targetPointUnderBase_tmp1[1];
		targetPointUnderBase_1[2] += targetPointUnderBase_tmp1[2];
	}

	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;
	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	// fine tune the direction
	// dynamic TCP will take effect when the GetOutput function of KukaSource is called;
	// By comparison, GetOutput function of VegaSource will not automatically take the registration matrix of the corresponding tool into consideration
	//auto currentPostureUnderBase = m_KukaSource->GetOutput(0)->GetAffineTransform3D();
	auto currentPostureUnderBase = m_KukaSource->GetOutput(1)->GetAffineTransform3D();
	vtkNew<vtkMatrix4x4> vtkCurrentPoseUnderBase;
	mitk::TransferItkTransformToVtkMatrix(currentPostureUnderBase.GetPointer(), vtkCurrentPoseUnderBase);
	Eigen::Vector3d currentYunderBase;
	currentYunderBase[0] = vtkCurrentPoseUnderBase->GetElement(0, 1);
	currentYunderBase[1] = vtkCurrentPoseUnderBase->GetElement(1, 1);
	currentYunderBase[2] = vtkCurrentPoseUnderBase->GetElement(2, 1);
	currentYunderBase.normalize();
	MITK_INFO << "currentYunderBase" << currentYunderBase;

	Eigen::Vector3d targetYunderBase;
	targetYunderBase[0] = targetPointUnderBase_0[0] - targetPointUnderBase_1[0];
	targetYunderBase[1] = targetPointUnderBase_0[1] - targetPointUnderBase_1[1];
	targetYunderBase[2] = targetPointUnderBase_0[2] - targetPointUnderBase_1[2];
	targetYunderBase.normalize();
	MITK_INFO << "targetYunderBase" << targetYunderBase;

	Eigen::Vector3d rotationAxis;
	rotationAxis = currentYunderBase.cross(targetYunderBase);


	double rotationAngle;
	if (currentYunderBase.dot(targetYunderBase) > 0) {
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;
	}
	else {
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	//Rotate current pose(vtkCurrentPoseUnderBase)
	//Rotation axis(rotationAxis) is cross product: currentYunderBase x targetYunderBase
	//Rotation angle(rotationAngle) is the angle between currentYunderBase and targetYunderBase
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(vtkCurrentPoseUnderBase);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
	tmpTransform->Update();

	auto targetPoseUnderBase = mitk::AffineTransform3D::New();
	mitk::TransferVtkMatrixToItkTransform(tmpTransform->GetMatrix(), targetPoseUnderBase.GetPointer());

	// Assemble m_T_robot
	m_T_robot = mitk::AffineTransform3D::New();
	m_T_robot->SetMatrix(targetPoseUnderBase->GetMatrix());
	m_T_robot->SetOffset(targetPointUnderBase_0);

	vtkMatrix4x4* t = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_T_robot.GetPointer(), t);
	return true;
}

bool RecordAndMove::MoveToTargetLine()
{
	vtkMatrix4x4* t = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_T_robot.GetPointer(), t);
	Eigen::Matrix3d Re;
	Re << t->GetElement(0, 0), t->GetElement(0, 1), t->GetElement(0, 2),
		t->GetElement(1, 0), t->GetElement(1, 1), t->GetElement(1, 2),
		t->GetElement(2, 0), t->GetElement(2, 1), t->GetElement(2, 2);
	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	std::array<double, 6> p;
	p[0] = t->GetElement(0, 3);
	p[1] = t->GetElement(1, 3);
	p[2] = t->GetElement(2, 3);
	p[3] = eulerAngle(0);
	p[4] = eulerAngle(1);
	p[5] = eulerAngle(2);

	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	// Todo: MovePTP(vtkMatrix * t) is an error
	//  m_KukaTrackingDevice->m_RobotApi.MovePTP(t); 
	m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
	return true;
}


// Point Accuracy Test
bool RecordAndMove::InterpretImagePoint()
{
	//read file
	auto CTSteelballCenterPositionInImage = m_Controls.mitkNodeSelectWidget_CTSteelballCenterInImage->GetSelectedNode();
	if (CTSteelballCenterPositionInImage == nullptr)
	{
		cout << "Source surface or source landmarks is not ready!" << endl;
		return false;
	}

	//get one point position in image frame, we select the second point for example.
	auto CTSteelballCenter_Pset = dynamic_cast<mitk::PointSet*>(CTSteelballCenterPositionInImage->GetData());
	auto pointPositionInImage = CTSteelballCenter_Pset->GetPoint(1);

	//check and visualize transformation matrix
	vtkMatrix4x4* F_image_ObjectRf = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_imageRegistrationMatrix.GetPointer(), F_image_ObjectRf);
	vtkMatrix4x4* F_image_ObjectRf_inverse = vtkMatrix4x4::New();;
	mitk::TransferItkTransformToVtkMatrix(m_imageRegistrationMatrix.GetPointer(), F_image_ObjectRf_inverse);
	F_image_ObjectRf_inverse->Invert();
	
	auto ObjectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	mitk::NavigationData::Pointer nd_ndiToObjeRf = m_VegaSource->GetOutput(ObjectRfIndex);
	vtkMatrix4x4* F_NDI_ObjectRf = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_ndiToObjeRf->GetAffineTransform3D().GetPointer(), F_NDI_ObjectRf);
	
	auto RobotBaseRFIndex = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	mitk::NavigationData::Pointer nd_ndiToRobotBaseRF = m_VegaSource->GetOutput(RobotBaseRFIndex);
	vtkMatrix4x4* F_NDI_RobotBaseRF_inverse = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_ndiToRobotBaseRF->GetInverse()->GetAffineTransform3D().GetPointer(), F_NDI_RobotBaseRF_inverse);
	
	vtkMatrix4x4* F_RobotBaseRF_RobotBase = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_RobotRegistrationMatrix.GetPointer(), F_RobotBaseRF_RobotBase);
	vtkMatrix4x4* F_RobotBaseRF_RobotBase_inverse = vtkMatrix4x4::New();;
	mitk::TransferItkTransformToVtkMatrix(m_RobotRegistrationMatrix.GetPointer(), F_RobotBaseRF_RobotBase_inverse);
	F_RobotBaseRF_RobotBase_inverse->Invert();
	
	// Calculate Point in RobotBase
	//Transfer Point3D to vtkMatrix4x4
	auto pointPositionInImage_Matrix = vtkMatrix4x4::New();
	pointPositionInImage_Matrix->Identity();
	pointPositionInImage_Matrix->SetElement(0, 3, pointPositionInImage[0]);
	pointPositionInImage_Matrix->SetElement(1, 3, pointPositionInImage[1]);
	pointPositionInImage_Matrix->SetElement(2, 3, pointPositionInImage[2]);
	
	//Multiply Point Matrix by 4 Matrixs
	//p_robotbase = F_RobotBase_RobotBaseRF * F_RobotBaseRF_NDI * F_NDI_ObjectRf * F_ObjectRf_image * p_image
	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->SetMatrix(pointPositionInImage_Matrix);
	tmpTrans->PostMultiply();
	tmpTrans->Concatenate(F_image_ObjectRf_inverse);
	tmpTrans->Concatenate(F_NDI_ObjectRf);
	tmpTrans->Concatenate(F_NDI_RobotBaseRF_inverse);
	tmpTrans->Concatenate(F_RobotBaseRF_RobotBase_inverse);
	
	//Extract translation part from composed matrix(tmpTrans)
	auto resultMatrix = tmpTrans->GetMatrix();
	CTSteelPointCenterPositionInRobotBaseFrame[0] = resultMatrix->GetElement(0, 3);
	CTSteelPointCenterPositionInRobotBaseFrame[1] = resultMatrix->GetElement(1, 3);
	CTSteelPointCenterPositionInRobotBaseFrame[2] = resultMatrix->GetElement(2, 3);

	//Move TCP to CTSteelPointCenterInRobotBaseFrame
	auto frame_2 = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[2];
	std::array<double, 6> p = frame_2.position;
	CTSteelPointCenterTransformationInRobotBaseFrame = { CTSteelPointCenterPositionInRobotBaseFrame[0],CTSteelPointCenterPositionInRobotBaseFrame[1],
																	CTSteelPointCenterPositionInRobotBaseFrame[2],  2.37, 1.16, -0.80 };
	return true;
}

bool RecordAndMove::MoveToTargetPoint()
{
	cout << CTSteelPointCenterTransformationInRobotBaseFrame[0] << ",  " << CTSteelPointCenterTransformationInRobotBaseFrame[1] << ",  "
		<< CTSteelPointCenterTransformationInRobotBaseFrame[2] << ",  " << CTSteelPointCenterTransformationInRobotBaseFrame[3] << ",  "
		<< CTSteelPointCenterTransformationInRobotBaseFrame[4] << ",  " << CTSteelPointCenterTransformationInRobotBaseFrame[5] << endl;
	m_KukaTrackingDevice->m_RobotApi.SetMotionFrame("RobotEndRF_robot");
	m_KukaTrackingDevice->m_RobotApi.MovePTP(CTSteelPointCenterTransformationInRobotBaseFrame);
	return true;
}


// Image Registration
void RecordAndMove::InitCTSteelballCenterSelector(QmitkSingleNodeSelectionWidget* widget)
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

void RecordAndMove::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
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

void RecordAndMove::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
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

bool RecordAndMove::SetupNavigatedImage()
{
	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = m_Controls.mitkNodeSelectWidget_surface_regis->GetSelectedNode();
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src->GetSelectedNode();

	if (surfaceNode == nullptr || landmarkSrcNode == nullptr)
	{
		m_Controls.textBrowser->append("Source surface or source landmarks is not ready!");
		return false;
	}

	navigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if (matrix->IsIdentity() == false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	}

	navigatedImage->SetDataNode(surfaceNode);

	navigatedImage->SetLandmarks(dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData()));

	navigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- navigatedImage has been set up ---");

	return true;
}

bool RecordAndMove::CollectLandmarkProbe()
{
	if (navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	int landmark_surfaceNum = navigatedImage->GetLandmarks()->GetSize();
	int landmark_rmNum = navigatedImage->GetLandmarks_probe()->GetSize();

	if (landmark_surfaceNum == landmark_rmNum)
	{
		m_Controls.textBrowser->append("--- Enough landmarks have been collected ----");
		return true;
	}

	auto pointSet_probeLandmark = navigatedImage->GetLandmarks_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("ProbeTHA");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);
	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeLandmark->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added landmark: " + QString::number(probeTipPointUnderRf[0]) +
		"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}

bool RecordAndMove::CollectIcpProbe()
{
	if (navigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}
	auto pointSet_probeIcp = navigatedImage->GetIcpPoints_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("ProbeTHA");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (probeIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'Probe' or 'ObjectRf' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_rfToProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToObjectRf);


	mitk::Point3D probeTipPointUnderRf = nd_rfToProbe->GetPosition();

	pointSet_probeIcp->InsertPoint(probeTipPointUnderRf);

	m_Controls.textBrowser->append("Added icp point: " + QString::number(probeTipPointUnderRf[0]) +
		"/ " + QString::number(probeTipPointUnderRf[1]) + "/ " + QString::number(probeTipPointUnderRf[2]));

	return true;
}

bool RecordAndMove::ApplySurfaceRegistration()
{
	//build ApplyDeviceRegistrationFilter
	m_surfaceRegistrationFilter = lancet::ApplySurfaceRegistratioinFilter::New();
	m_surfaceRegistrationFilter->ConnectTo(m_VegaSource);
	m_surfaceRegistrationFilter->SetnavigationImage(navigatedImage);
	m_imageRegistrationMatrix = mitk::AffineTransform3D::New();
	navigatedImage->UpdateObjectToRfMatrix();
	mitk::TransferVtkMatrixToItkTransform(navigatedImage->GetT_Object2ReferenceFrame(), m_imageRegistrationMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(m_imageRegistrationMatrix);

	m_surfaceRegistrationFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());

	auto indexOfObjectRF = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput(indexOfObjectRF));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationFilter);
	m_VegaVisualizeTimer->start();

	return true;
}


// Connect
void RecordAndMove::UseKuka()
{
	m_Controls.pushButton_connectKuka->setText("Kuka Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_KukaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_KukaToolStorage->SetName(filename.toStdString());

	m_KukaTrackingDevice = lancet::KukaRobotDevice_New::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_KukaToolStorage, m_KukaTrackingDevice);

	m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);
	// m_KukaSource->RegisterAsMicroservice();
	if (m_KukaSource.IsNotNull())
	{
		m_KukaSource->Connect();
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
	//Begin:  void SurgicalSimulate::StartTracking()
	if (m_KukaTrackingDevice->GetState() == 1) //ready
	{
		m_KukaSource->StartTracking();
		//update visualize filter by timer
		if (m_KukaVisualizeTimer == nullptr)
		{
			m_KukaVisualizeTimer = new QTimer(this); //create a new timer
		}
		connect(m_KukaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnKukaVisualizeTimer()));
		//connect the timer to the method OnTimer()
		connect(m_KukaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
		//connect the timer to the method OnTimer()
		ShowToolStatus_Kuka();
		m_KukaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps
	}
	else
	{
		MITK_ERROR << "Tracking not start,Device State:" << m_KukaTrackingDevice->GetState();
	}
	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	MITK_INFO << "m_KukaSource connected!";
	m_Controls.pushButton_connectKuka->setText("Kuka Connected Successfully!");
}

void RecordAndMove::UseVega()
{
	m_Controls.pushButton_connectVega->setText("Vega Connecting...");
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/C:/Users/lancet/Desktop/fx",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk  ????????????????????????????????????????????????????????????
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());

	MITK_INFO << "Vega tracking";
	//QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();

	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(OnVegaVisualizeTimer()));
	//connect the timer to the method OnTimer()
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolStatusWidget()));
	//connect the timer to the method OnTimer()
	ShowToolStatus_Vega();
	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	//use navigation scene filter
	cout << "Vega Connected Successfully!" << endl;
	m_Controls.pushButton_connectVega->setText("Vega Connected Successfully!");
}

void RecordAndMove::OnKukaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.
	if (m_KukaVisualizer.IsNotNull())
	{
		m_KukaVisualizer->Update(); //todo Crash When close plugin
		this->RequestRenderWindowUpdate();
	}


	if (m_KukaTrackingDevice->m_RobotApi.GetNumberOfCommandResult() > 0)
	{
		ResultProtocol reply = m_KukaTrackingDevice->m_RobotApi.GetCommandResult();
		cout << "This is reply: " << reply.ToString() << endl;
		//m_Controls.textBrowser->append(QString::fromStdString(reply.ToString()));

		/*if (m_KukaTrackingDevice->m_RobotApi.GetNumberOfCommandResult() > 0)
		{
			reply = m_KukaTrackingDevice->m_RobotApi.GetCommandResult();
			MITK_INFO << "Receive Reply: " << QString::fromStdString(reply.operateType);
		}*/

		/*if (QString::fromStdString(reply.operateType) == "non_precious")
		{
			m_handGuidingOn = false;
			m_Controls.label_handDriveFlag->setText("Hand Drive OFF");
		}*/
		//start handguiding
		if (QString::fromStdString(reply.operateType) == "precious")
		{
			if (preciousHandGuiding_select == true) {
				m_KukaTrackingDevice->m_RobotApi.SendCommandNoPara("HandGuiding");
			}
			else {
				m_KukaTrackingDevice->m_RobotApi.SendCommandNoPara("Test");
			}
		}
	}
}

void RecordAndMove::OnVegaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.
	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		// auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
		// mitk::RenderingManager::GetInstance()->InitializeViews(geo);
		this->RequestRenderWindowUpdate();

		//update probe pose
		auto  probe = m_VegaSource->GetOutput("ProbeTHA")->GetAffineTransform3D();
		// m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
		// m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", probe, m_ProbeRealTimePose);
		  //mitk::AffineTransform3D::Pointer byhand = mitk::AffineTransform3D::New();
		auto registrationMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
		probe->Compose(m_VegaSource->GetOutput("RobotBaseRF")->GetInverse()->GetAffineTransform3D());
		probe->Compose(mitk::NavigationData::New(registrationMatrix)->GetInverse()->GetAffineTransform3D());
		m_ProbeRealTimePose = probe;
	}
}

void RecordAndMove::ShowToolStatus_Vega()
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
	}
	//initialize widget
	m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
	m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();
}

void RecordAndMove::ShowToolStatus_Kuka()
{
	m_KukaNavigationData.clear();
	for (std::size_t i = 0; i < m_KukaSource->GetNumberOfOutputs(); i++)
	{
		m_KukaNavigationData.push_back(m_KukaSource->GetOutput(i));
	}
	//initialize widget
	m_Controls.m_StatusWidgetKukaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetKukaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetKukaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetKukaToolToShow->SetNavigationDatas(&m_KukaNavigationData);
	m_Controls.m_StatusWidgetKukaToolToShow->ShowStatusLabels();
}

void RecordAndMove::SetAsTarget()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	//MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	m_Target = p;
	QString position_text = "Target:    x: " + QString::number(p[0]) + "    y: " + QString::number(p[1]) + "    z: " + QString::number(p[2]) + "    a: " + QString::number(p[3]) + "    b: " + QString::number(p[4]) + "    c: " + QString::number(p[5]);
	m_Controls.label_target->setText(position_text);
}


// Robot Registration
void RecordAndMove::OnAutoMove()
{
	auto frame = m_KukaTrackingDevice->m_RobotApi.GetRobotInfo().frames[0];
	MITK_INFO << frame.name;
	std::array<double, 6> p = frame.position;
	cout << "m_IndexOfRobotCapture: " << m_IndexOfRobotCapture << endl;
	switch (m_IndexOfRobotCapture)
	{
	case 1: //x-50mm	  
		p[0] -= 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;
	case 2: //z-50mm
		p[2] -= 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 3: //x+50mm
		p[0] += 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 4: //y+50
		p[1] += 50;
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 5: //x rotate 10 degree x -25
		p[0] -= 25;
		p[3] += (15.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 6: //x rotate -20 degree y -25
		p[1] -= 25;
		p[3] -= (15.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 7: //y rotate 10 degree y +25
		p[1] += 25;
		p[4] += (15.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 8: //y rotate -20 degree x +25
		p[0] += 25;
		p[4] -= (20.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	case 9: //z rotate 20 degree x -25
		p[0] -= 25;
		p[5] += (20.0 / 180.0 * 3.14);
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;

	default:
		m_KukaTrackingDevice->m_RobotApi.MovePTP(p);
		break;
	}
}

void::RecordAndMove::MoveToHomePosition()
{
	bool isGetHome = m_KukaTrackingDevice->m_RobotApi.MovePTP(m_HomePosition);
}

void RecordAndMove::CapturePose(bool translationOnly)
{
	//Output sequence is the same as AddTool sequence
	//get navigation data of flange in robot coords,
	mitk::NavigationData::Pointer nd_robot2flange = m_KukaSource->GetOutput(0);
	//get navigation data of RobotEndRF in ndi coords,
	auto RobotEndRF = m_VegaToolStorage->GetToolIndexByName("RobotEndRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotEndRF = m_VegaSource->GetOutput(RobotEndRF);
	//get navigation data of RobotBaseRF in ndi coords,
	auto RobotBaseRF = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotBaseRF = m_VegaSource->GetOutput(RobotBaseRF);
	//get navigation data RobotEndRF in reference frame RobotBaseRF
	mitk::NavigationData::Pointer nd_RobotBaseRF2RobotEndRF = GetNavigationDataInRef(
		nd_Ndi2RobotEndRF, nd_Ndi2RobotBaseRF);

	//add nd to registration module
	m_RobotRegistration.AddPose(nd_robot2flange, nd_RobotBaseRF2RobotEndRF, translationOnly);

	//MITK_INFO << nd_robot2flange;
	//MITK_INFO << nd_RobotBaseRF2RobotEndRF;
	cout << "nd_robot2flange: " << endl;
	cout<<nd_robot2flange->GetRotationMatrix() << endl;
	cout << nd_robot2flange->GetPosition() << endl;
	cout << "******************" << endl;
	cout << "nd_RobotBaseRF2RobotEndRF: " << endl;
	cout<<nd_RobotBaseRF2RobotEndRF->GetRotationMatrix() << endl;
	cout << nd_RobotBaseRF2RobotEndRF->GetPosition() << endl;
}

void RecordAndMove::OnRobotCapture()
{
	if (m_IndexOfRobotCapture < 5) //The first five translations, 
	{
		CapturePose(true);
		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture<<"  (Translation: "<< m_IndexOfRobotCapture<<"/5)";
		m_Controls.pushButton_capturePose->setText("Capturing "+ QString::number(m_IndexOfRobotCapture)+"/10");
		return;
	}
	else if (m_IndexOfRobotCapture < 10) //the last five rotations
	{
		CapturePose(false);
		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture << "  (Rotation: " << m_IndexOfRobotCapture-5 << "/5)";
		m_Controls.pushButton_capturePose->setText("Capturing " + QString::number(m_IndexOfRobotCapture) + "/10");
		return;
	}
	if (m_IndexOfRobotCapture==10)
	{

		m_IndexOfRobotCapture++;
		MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
		m_Controls.pushButton_capturePose->setText("OnRobotCapture finish, Calculating registration matrix....");
		vtkMatrix4x4* matrix4x4 = vtkMatrix4x4::New();
		m_RobotRegistration.GetRegistraionMatrix(matrix4x4);  //RobotBase to RobotBaseRF
		m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();   //RobotBase to RobotBaseRF
		mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());   //RobotBase to RobotBaseRF
		//save robot registration matrix into reference tool
		m_VegaToolStorage->GetToolByName("RobotBaseRF")->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);
		MITK_INFO << "******************Robot Registration Matrix : ****************************";
		MITK_INFO << m_RobotRegistrationMatrix;

		//build ApplyDeviceRegistrationFilter
		//m_KukaSource:基于RoboBase
		//m_KukaApplyRegistrationFilter转到NDI坐标系下
		m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
		m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
		m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
		m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("RobotBaseRF"));//must make sure NavigationDataOfRF update somewhere else.
		//m_KukaVisualizeTimer->stop();
		m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
		//m_KukaVisualizeTimer->start();
		//tcp

		std::array<double, 6> tcp{};

		
		//m_RobotRegistration.GetCorrectTCP(tcp);
		m_RobotRegistration.GetTCP(tcp);
		cout << "******************************* TCP *************************************" << endl;
		cout << "TCP: " << tcp[0] << " " << tcp[1] << " " << tcp[2] << " " << tcp[3] << " " << tcp[4] << " " << tcp[5] << endl;
		cout << "******************************* TCP *************************************" << endl;
		//set tcp
		m_KukaTrackingDevice->m_RobotApi.AddFrame("RobotEndRF_robot", tcp);

		//save tcp into robot tool
		m_KukaToolStorage->GetToolByName("RobotEndRF_robot")->SetTCP(tcp.data());
		cout << "tcp.date: " << tcp.data() << endl;
		m_Controls.pushButton_capturePose->setText("OnRobotCapture finish! Calculate finish!");
		return;
	}
}

void RecordAndMove::UpdateToolStatusWidget()
{
	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
	m_Controls.m_StatusWidgetKukaToolToShow->Refresh();
}

void RecordAndMove::OnSaveRobotRegistration()
{
	if (m_VegaToolStorage.IsNotNull())
	{
		cout << "*********  (m_VegaToolStorage.IsNotNull()  ****************" << endl;
		QFileDialog* fileDialog = new QFileDialog;
		fileDialog->setDefaultSuffix("IGTToolStorage");
		QString suffix = "IGT Tool Storage (*.IGTToolStorage)";
		// Set default file name to LastFileSavePath + storage name
		QString defaultFileName = QmitkIGTCommonHelper::GetLastFileSavePath() + "/" + QString::fromStdString(m_VegaToolStorage->GetName());
		QString filename = fileDialog->getSaveFileName(nullptr, tr("Save Navigation Tool Storage"), defaultFileName, suffix, &suffix);

		if (filename.isEmpty()) return; //canceled by the user

		// check file suffix
		QFileInfo file(filename);
		if (file.suffix().isEmpty()) filename += ".IGTToolStorage";

		//serialize tool storage
		mitk::NavigationToolStorageSerializer::Pointer mySerializer = mitk::NavigationToolStorageSerializer::New();

		try
		{
			mySerializer->Serialize(filename.toStdString(), m_VegaToolStorage);
		}
		catch (const mitk::IGTIOException & e)
		{
			m_Controls.textBrowser->append(QString::fromStdString("Error: " + std::string(e.GetDescription())));
			return;
		}
		m_Controls.textBrowser->append(QString::fromStdString(m_VegaToolStorage->GetName() + " saved"));
	}

	if (m_KukaToolStorage.IsNotNull())
	{
		cout << "*********  (m_KukaToolStorage.IsNotNull()  ****************" << endl;
		QFileDialog* fileDialog = new QFileDialog;
		fileDialog->setDefaultSuffix("IGTToolStorage");
		QString suffix = "IGT Tool Storage (*.IGTToolStorage)";
		// Set default file name to LastFileSavePath + storage name
		QString defaultFileName = QmitkIGTCommonHelper::GetLastFileSavePath() + "/" + QString::fromStdString(m_KukaToolStorage->GetName());
		QString filename = fileDialog->getSaveFileName(nullptr, tr("Save Navigation Tool Storage"), defaultFileName, suffix, &suffix);

		if (filename.isEmpty()) return; //canceled by the user

		// check file suffix
		QFileInfo file(filename);
		if (file.suffix().isEmpty()) filename += ".IGTToolStorage";

		//serialize tool storage
		mitk::NavigationToolStorageSerializer::Pointer mySerializer = mitk::NavigationToolStorageSerializer::New();

		try
		{
			mySerializer->Serialize(filename.toStdString(), m_KukaToolStorage);
		}
		catch (const mitk::IGTIOException & e)
		{
			m_Controls.textBrowser->append(QString::fromStdString("Error: " + std::string(e.GetDescription())));
			return;
		}
		m_Controls.textBrowser->append(QString::fromStdString(m_KukaToolStorage->GetName() + " saved"));
	}
}

void RecordAndMove::OnUsePreRobotRegistration()
{
	//build ApplyDeviceRegistrationFilter
	m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
	m_KukaApplyRegistrationFilter->ConnectTo(m_KukaSource);
	m_RobotRegistrationMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
	m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
	m_VegaSource->Update();
	m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("RobotBaseRF"));//must make sure NavigationDataOfRF update somewhere else.

	m_KukaVisualizeTimer->stop();
	m_KukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
	m_KukaVisualizeTimer->start();
}

void RecordAndMove::OnUsePreNDIRegistration()
{
	// Apply preexisting surface registration result
	m_surfaceRegistrationFilter = lancet::ApplySurfaceRegistratioinFilter::New();
	m_surfaceRegistrationFilter->ConnectTo(m_VegaSource);
	m_surfaceRegistrationFilter->SetnavigationImage(navigatedImage);
	m_surfaceRegistrationFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());
	m_surfaceRegistrationFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));

	// save image(surface) registration matrix into its corresponding RF tool
	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_surfaceRegistrationFilter);
	m_VegaVisualizeTimer->start();
}