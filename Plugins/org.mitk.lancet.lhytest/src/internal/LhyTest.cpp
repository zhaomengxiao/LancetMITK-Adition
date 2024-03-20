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
//TeethLhy
#include <StaubliRobotDevice.h>
// Qmitk
#include "LhyTest.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <vtkQuaternion.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkNavigationToolStorageSerializer.h"
#include "QmitkIGTCommonHelper.h"
#include "mitkIGTIOException.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"

namespace mitk
{
	class IGTIOException;
}

const std::string LhyTest::VIEW_ID = "org.mitk.views.lhytest";

void LhyTest::SetFocus()
{
  
}

LhyTest::LhyTest()
{
}

void LhyTest::OnConnected()
{
	m_StaubliTrackingDevice->RequestExecOperate("atimode", { "1" });
	QThread::msleep(300);
	m_StaubliTrackingDevice->RequestExecOperate("atimode", { "8" });
	QThread::msleep(300);
	m_StaubliTrackingDevice->OpenConnection();
	QThread::msleep(300);
	m_StaubliTrackingDevice->RequestExecOperate("start", {});
}

void LhyTest::OnDisConnected()
{
	m_StaubliTrackingDevice->RequestExecOperate("stop", {});
	m_StaubliTrackingDevice->CloseConnection();
}

void LhyTest::OnPowerOn()
{
	m_StaubliTrackingDevice->RequestExecOperate("poweron", {});
}

void LhyTest::OnPowerOff()
{
	m_StaubliTrackingDevice->RequestExecOperate("poweroff", {});
}

void LhyTest::OnSendCommand()
{
	this->m_StaubliTrackingDevice->RequestExecOperate(this->m_Controls.lineEditCommand->text(), this->m_Controls.lineEditParamter->text().split(","));
}

void LhyTest::OnUpdateToolPosition(QString, LToolAttitudeMessage msg)
{
#define Matrix4x4(row, col) \
	this->m_Controls.lineEdit4X4##row##col->setText(QString::number(msg.vectorMatrix4x4()[(row * 4) + col], 'f', 3));
	// 4x4
	Matrix4x4(0, 0); Matrix4x4(0, 1); Matrix4x4(0, 2); Matrix4x4(0, 3);
	Matrix4x4(1, 0); Matrix4x4(1, 1); Matrix4x4(1, 2); Matrix4x4(1, 3);
	Matrix4x4(2, 0); Matrix4x4(2, 1); Matrix4x4(2, 2); Matrix4x4(2, 3);
	Matrix4x4(3, 0); Matrix4x4(3, 1); Matrix4x4(3, 2); Matrix4x4(3, 3);

	// TCP
#define ShowTCPValue(name, index) \
	this->m_Controls.lineEditTCP##name->setText(QString::number(msg.extendedDataField()[index], 'f', 3));

	ShowTCPValue(X, 6);
	ShowTCPValue(Y, 7);
	ShowTCPValue(Z, 8);
	ShowTCPValue(A, 9);
	ShowTCPValue(B, 10);
	ShowTCPValue(C, 11);

	// Joints
#define ShowJointValue(name, index) \
	this->m_Controls.lineEditJoint##name->setText(QString::number(msg.extendedDataField()[index], 'f', 3));

	ShowJointValue(1, 0);
	ShowJointValue(2, 1);
	ShowJointValue(3, 2);
	ShowJointValue(4, 3);
	ShowJointValue(5, 4);
	ShowJointValue(6, 5);
	// IO

	// tool position
	m_RobotPosition->DeepCopy(msg.matrix4x4Date());


	
}



bool LhyTest::InterpretMovementAsInBaseSpace(vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase)
{
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
    // tmpTransform->SetMatrix(m_RobotPosition);
	// tmpTransform->Concatenate(rawMovementMatrix);
	tmpTransform->SetMatrix(rawMovementMatrix);
	tmpTransform->Concatenate(m_RobotPosition);
	tmpTransform->Update();

	movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());
	
	m_Controls.textBrowser->append("----------Initial Matrix---------------");
	for (int i{ 0 }; i < 4; i++) {
		for (int j{ 0 }; j < 4; j++) {
			m_Controls.textBrowser->append(QString::number(m_RobotPosition->GetElement(i, j)));
		}
	}


	m_Controls.textBrowser->append("----------Calculated Matrix---------------");
	for (int i{ 0 }; i < 4; i++) {
		for (int j{ 0 }; j < 4; j++) {
			m_Controls.textBrowser->append(QString::number(movementMatrixInRobotBase->GetElement(i,j)));
		}
	}
	m_Controls.textBrowser->append("-------------------------");

	return true;
	


}

bool LhyTest::TranslateX_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 1,0,0 };
	axis[0] = axis[0] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);
	

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;

	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	return true;
}

bool LhyTest::TranslateX_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 1,0,0 };
	axis[0] = -axis[0] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::TranslateY_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,1,0 };
	axis[1] = axis[1] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::TranslateY_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,1,0 };
	axis[1] = -axis[1] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::TranslateZ_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,0,1 };
	axis[2] = axis[2] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::TranslateZ_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axis[3]{ 0,0,1 };
	axis[2] = -axis[2] * (m_Controls.lineEdit_intuitiveValue->text().toDouble());
	affineTransform->Translate(axis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	return true;
}

bool LhyTest::RotateX_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 1,0,0 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::RotateX_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 1,0,0 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::RotateY_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,1,0 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::RotateY_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,1,0 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::RotateZ_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,0,1 };
	double angle = 3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

bool LhyTest::RotateZ_minus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double axisZ[3]{ 0,0,1 };
	double angle = -3.14159 * (m_Controls.lineEdit_intuitiveValue->text().toDouble()) / 180;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	double result[9];

	m_StaubliTrackingDevice->RobotMove(movementMatrixInRobotBase, result);

	m_Controls.textBrowser->append("XYZ order");
	m_Controls.textBrowser->append(QString::number(result[3]));
	m_Controls.textBrowser->append(QString::number(result[4]));
	m_Controls.textBrowser->append(QString::number(result[5]));

	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	m_Controls.textBrowser->append("ZYX order");
	m_Controls.textBrowser->append(QString::number(result[6]));
	m_Controls.textBrowser->append(QString::number(result[7]));
	m_Controls.textBrowser->append(QString::number(result[8]));
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");


	return true;
}

void LhyTest::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  m_StaubliTrackingDevice = lancet::StaubliRobotDevice::New();

  InitSurfaceSelector(m_Controls.mitkNodeSelectWidget_surface_regis);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_landmark_src);
  InitPointSetSelector(m_Controls.mitkNodeSelectWidget_imageTargetLine);

  connect(m_Controls.pushbtnConnected, &QPushButton::clicked, this, &LhyTest::OnConnected);
  connect(m_Controls.pushbtnDisConnected, &QPushButton::clicked, this, &LhyTest::OnDisConnected);
  connect(m_Controls.pushbtnPowerOn, &QPushButton::clicked, this, &LhyTest::OnPowerOn);
  connect(m_Controls.pushbtnPowerOff, &QPushButton::clicked, this, &LhyTest::OnPowerOff);
  connect(m_Controls.pushbtnSendCommand, &QPushButton::clicked, this, &LhyTest::OnSendCommand);

  connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &LhyTest::TranslateX_minus);
  connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &LhyTest::TranslateX_plus);
  connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &LhyTest::TranslateY_minus);
  connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &LhyTest::TranslateY_plus);
  connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &LhyTest::TranslateZ_minus);
  connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &LhyTest::TranslateZ_plus);
  connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &LhyTest::RotateX_minus);
  connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &LhyTest::RotateX_plus);
  connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &LhyTest::RotateY_minus);
  connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &LhyTest::RotateY_plus);
  connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &LhyTest::RotateZ_minus);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &LhyTest::RotateZ_plus);

  connect(m_StaubliTrackingDevice.GetPointer(), SIGNAL(updateToolPosition(QString, LToolAttitudeMessage)),
	  this, SLOT(OnUpdateToolPosition(QString, LToolAttitudeMessage)));

  connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &LhyTest::on_pushButton_connectVega_clicked);
  connect(m_Controls.pushButton_captureRobot, &QPushButton::clicked, this, &LhyTest::on_pushButton_captureRobot_clicked);
  connect(m_Controls.pushButton_resetRobotReg, &QPushButton::clicked, this, &LhyTest::on_pushButton_resetRobotReg_clicked);
  connect(m_Controls.pushButton_saveRobotRegist, &QPushButton::clicked, this, &LhyTest::on_pushButton_saveRobotRegist_clicked);
  connect(m_Controls.pushButton_usePreRobotRegit, &QPushButton::clicked, this, &LhyTest::on_pushButton_usePreRobotRegit_clicked);
  connect(m_Controls.pushButton_assembleNavigationObject, &QPushButton::clicked, this, &LhyTest::on_pushButton_assembleNavigationObject_clicked);
  connect(m_Controls.pushButton_applyPreImageRegistrationNew, &QPushButton::clicked, this, &LhyTest::on_pushButton_applyPreImageRegistrationNew_clicked);
  connect(m_Controls.pushButton_collectLandmark, &QPushButton::clicked, this, &LhyTest::on_pushButton_collectLandmark_clicked);
  connect(m_Controls.pushButton_collectIcp, &QPushButton::clicked, this, &LhyTest::on_pushButton_collectIcp_clicked);
  connect(m_Controls.pushButton_applyRegistrationNew, &QPushButton::clicked, this, &LhyTest::on_pushButton_applyRegistrationNew_clicked);
  connect(m_Controls.pushButton_saveNdiTools, &QPushButton::clicked, this, &LhyTest::on_pushButton_saveRobotRegist_clicked);
  connect(m_Controls.pushButton_setTcpPrecisionTest, &QPushButton::clicked, this, &LhyTest::on_pushButton_setTcpPrecisionTest_clicked);
  connect(m_Controls.pushButton_confirmImageTargetLine, &QPushButton::clicked, this, &LhyTest::on_pushButton_confirmImageTargetLine_clicked);
  connect(m_Controls.pushButton_startAutoPosition, &QPushButton::clicked, this, &LhyTest::on_pushButton_startAutoPosition_clicked);
  connect(m_Controls.pushButton_showFlange, &QPushButton::clicked, this, &LhyTest::on_pushButton_showFlange_clicked);
  connect(m_Controls.pushButton_showTool, &QPushButton::clicked, this, &LhyTest::on_pushButton_showTool_clicked);
  connect(m_Controls.pushButton_showFlange, &QPushButton::clicked, this, &LhyTest::on_pushButton_showFlange_clicked);
  connect(m_Controls.pushButton_recordInitial, &QPushButton::clicked, this, &LhyTest::on_pushButton_recordInitial_clicked);
  connect(m_Controls.pushButton_goToInitial, &QPushButton::clicked, this, &LhyTest::on_pushButton_goToInitial_clicked);
   
}

void LhyTest::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
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
void LhyTest::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
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


void LhyTest::on_pushButton_recordInitial_clicked()
{
	m_BaseToFlange_target->DeepCopy(m_RobotPosition);
}

void LhyTest::on_pushButton_goToInitial_clicked()
{
	double tmpArray[9];
	m_StaubliTrackingDevice->RobotMove(m_BaseToFlange_target, tmpArray);
}



void LhyTest::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  foreach (mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
    {
      return;
    }
  }
}

void LhyTest::on_pushButton_connectVega_clicked()
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());

	
	MITK_INFO << "Vega tracking";
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
	
	ShowToolStatus_Vega();

	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void LhyTest::OnVegaVisualizeTimer()
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
	}

	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
}

void LhyTest::ShowToolStatus_Vega()
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

vtkMatrix4x4* LhyTest::getVtkMatrix4x4(mitk::NavigationData::Pointer nd)
{
	auto o = nd->GetOrientation();
	double R[3][3];
	double* V = { nd->GetPosition().GetDataPointer() };
	vtkQuaterniond quaterniond{ o.r(), o.x(), o.y(), o.z() };
	quaterniond.ToMatrix3x3(R);

	vtkMatrix4x4* matrix = vtkMatrix4x4::New();
	matrix->SetElement(0, 0, R[0][0]);
	matrix->SetElement(0, 1, R[0][1]);
	matrix->SetElement(0, 2, R[0][2]);
	matrix->SetElement(1, 0, R[1][0]);
	matrix->SetElement(1, 1, R[1][1]);
	matrix->SetElement(1, 2, R[1][2]);
	matrix->SetElement(2, 0, R[2][0]);
	matrix->SetElement(2, 1, R[2][1]);
	matrix->SetElement(2, 2, R[2][2]);

	matrix->SetElement(0, 3, V[0]);
	matrix->SetElement(1, 3, V[1]);
	matrix->SetElement(2, 3, V[2]);

	matrix->Print(std::cout);
	return matrix;
}

bool LhyTest::AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval, int intervalNum, double matrixArray[16])
{
	// The frame rate of Vega ST is 60 Hz, so the timeInterval should be larger than 16.7 ms

	double tmp_x[3]{ 0,0,0 };
	double tmp_y[3]{ 0,0,0 };
	double tmp_translation[3]{ 0,0,0 };

	for (int i{ 0 }; i < intervalNum; i++)
	{
		ndPtr->Update();

		auto tmpMatrix = getVtkMatrix4x4(ndPtr);

		MITK_INFO << "Averaging NavigationData and print the 1st element:" << tmpMatrix->GetElement(0, 0);

		tmp_x[0] += tmpMatrix->GetElement(0, 0);
		tmp_x[1] += tmpMatrix->GetElement(1, 0);
		tmp_x[2] += tmpMatrix->GetElement(2, 0);

		tmp_y[0] += tmpMatrix->GetElement(0, 1);
		tmp_y[1] += tmpMatrix->GetElement(1, 1);
		tmp_y[2] += tmpMatrix->GetElement(2, 1);

		tmp_translation[0] += tmpMatrix->GetElement(0, 3);
		tmp_translation[1] += tmpMatrix->GetElement(1, 3);
		tmp_translation[2] += tmpMatrix->GetElement(2, 3);

		QThread::msleep(timeInterval);
	}

	// Assemble baseRF to EndRF matrix
	Eigen::Vector3d x;
	x[0] = tmp_x[0];
	x[1] = tmp_x[1];
	x[2] = tmp_x[2];
	x.normalize();

	Eigen::Vector3d h;
	h[0] = tmp_y[0];
	h[1] = tmp_y[1];
	h[2] = tmp_y[2];
	h.normalize();

	Eigen::Vector3d z;
	z = x.cross(h);
	z.normalize();

	Eigen::Vector3d y;
	y = z.cross(x);
	y.normalize();

	tmp_translation[0] = tmp_translation[0] / intervalNum;
	tmp_translation[1] = tmp_translation[1] / intervalNum;
	tmp_translation[2] = tmp_translation[2] / intervalNum;

	double tmpArray[16]
	{
	  x[0], y[0], z[0], tmp_translation[0],
	  x[1], y[1], z[1], tmp_translation[1],
	  x[2], y[2], z[2], tmp_translation[2],
	  0,0,0,1
	};

	for (int i{ 0 }; i < 16; i++)
	{
		matrixArray[i] = tmpArray[i];
	}

	return true;
}

mitk::NavigationData::Pointer LhyTest::GetNavigationDataInRef(mitk::NavigationData::Pointer nd, mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}

void LhyTest::CapturePose(bool translationOnly)
{
	//get navigation data of RobotEndRF in ndi coords,
	//auto RobotEndRF = m_VegaToolStorage->GetToolIndexByName("RobotEndRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotEndRF = m_VegaSource->GetOutput("RobotEndRF");
	//get navigation data of RobotBaseRF in ndi coords,
	//auto RobotBaseRF = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	mitk::NavigationData::Pointer nd_Ndi2RobotBaseRF = m_VegaSource->GetOutput("RobotBaseRF");
	//get navigation data RobotEndRF in reference frame RobotBaseRF
	mitk::NavigationData::Pointer nd_RobotBaseRF2RobotEndRF = GetNavigationDataInRef(
		nd_Ndi2RobotEndRF, nd_Ndi2RobotBaseRF);

	//add nd to registration module
	//m_RobotRegistration.AddPose(nd_robot2flange, nd_RobotBaseRF2RobotEndRF, translationOnly);

	// add vtkMatrix as poses to the registration module
	// Average the NavigationData from the NDI camera
	double ndiToRoboEndArrayAvg[16];
	double ndiToBaseRFarrayAvg[16];
	AverageNavigationData(nd_Ndi2RobotEndRF, 30, 20, ndiToRoboEndArrayAvg);
	AverageNavigationData(nd_Ndi2RobotBaseRF, 30, 20, ndiToBaseRFarrayAvg);

	vtkNew<vtkMatrix4x4> vtkNdiToRoboEndMatrix;
	vtkNew<vtkMatrix4x4> vtkBaseRFToNdiMatrix;
	vtkNdiToRoboEndMatrix->DeepCopy(ndiToRoboEndArrayAvg);
	vtkBaseRFToNdiMatrix->DeepCopy(ndiToBaseRFarrayAvg);
	vtkBaseRFToNdiMatrix->Invert();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(ndiToRoboEndArrayAvg);
	tmpTransform->Concatenate(vtkBaseRFToNdiMatrix);
	tmpTransform->Update();
	auto vtkBaseRFtoRoboEndMatrix = tmpTransform->GetMatrix();

	m_RobotRegistration.AddPoseWithVtkMatrix(m_RobotPosition, vtkBaseRFtoRoboEndMatrix, translationOnly);

}

void LhyTest::on_pushButton_resetRobotReg_clicked()
{
	m_RobotRegistration.RemoveAllPose();
	m_IndexOfRobotCapture = 0;
	m_Controls.lineEdit_collectedRoboPose->setText(QString::number(0));
	// m_RobotRegistrationMatrix->Identity();
	m_FlangeToToolMatrix->Identity();
}

void LhyTest::on_pushButton_saveRobotRegist_clicked()
{
	if (m_VegaToolStorage.IsNotNull())
	{
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
}

void LhyTest::on_pushButton_captureRobot_clicked()
{
	if (m_IndexOfRobotCapture < 5) //The first five translations, 
	{
		CapturePose(true);
		//Increase the count each time you click the button
		m_IndexOfRobotCapture++;
		m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));

		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
	}
	else if (m_IndexOfRobotCapture < 10) //the last five rotations
	{
		CapturePose(false);
		//Increase the count each time you click the button
		m_IndexOfRobotCapture++;
		m_Controls.lineEdit_collectedRoboPose->setText(QString::number(m_IndexOfRobotCapture));
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
	}
	else
	{
		MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;

		m_FlangeToToolMatrix->Identity();

		m_RobotRegistration.GetRegistraionMatrix(m_RobotRegistrationMatrix);

		vtkNew<vtkMatrix4x4> robotEndToFlangeMatrix;
		m_RobotRegistration.GetTCPmatrix(robotEndToFlangeMatrix);
		robotEndToFlangeMatrix->Invert();

		m_Controls.textBrowser->append("Registration RMS: " + QString::number(m_RobotRegistration.RMS()));

	
		// m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();

		auto tmpMatrix = mitk::AffineTransform3D::New();

		mitk::TransferVtkMatrixToItkTransform(m_RobotRegistrationMatrix, tmpMatrix.GetPointer());

		auto affineRobotEndRFtoFlangeMatrix = mitk::AffineTransform3D::New();

		// mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());

		mitk::TransferVtkMatrixToItkTransform(robotEndToFlangeMatrix, affineRobotEndRFtoFlangeMatrix.GetPointer());

		//save robot registration matrix into reference tool
		m_VegaToolStorage->GetToolByName("RobotBaseRF")->SetToolRegistrationMatrix(tmpMatrix);
		m_VegaToolStorage->GetToolByName("RobotEndRF")->SetToolRegistrationMatrix(affineRobotEndRFtoFlangeMatrix);


	}

}

void LhyTest::on_pushButton_usePreRobotRegit_clicked()
{
	auto tmpMatrix = m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix();
	mitk::TransferItkTransformToVtkMatrix(tmpMatrix.GetPointer(), m_RobotRegistrationMatrix);

	m_FlangeToToolMatrix->Identity();
}

bool LhyTest::on_pushButton_assembleNavigationObject_clicked()
{
	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = m_Controls.mitkNodeSelectWidget_surface_regis->GetSelectedNode();
	auto landmarkSrcNode = m_Controls.mitkNodeSelectWidget_landmark_src->GetSelectedNode();

	if (surfaceNode == nullptr || landmarkSrcNode == nullptr)
	{
		m_Controls.textBrowser->append("Source surface or source landmarks is not ready!");
		return false;
	}

	m_NavigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if (matrix->IsIdentity() == false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	}

	m_NavigatedImage->SetDataNode(surfaceNode);

	m_NavigatedImage->SetLandmarks(dynamic_cast<mitk::PointSet*>(landmarkSrcNode->GetData()));

	m_NavigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- NavigatedImage has been set up ---");

	return true;
}

bool LhyTest::on_pushButton_applyPreImageRegistrationNew_clicked()
{
	// Apply preexisting surface registration result to all the NDI tools
	m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);

	m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());
	m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();

	auto tmpMatrix = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix().GetPointer(),tmpMatrix);

	m_ImageRegistrationMatrix->DeepCopy(tmpMatrix);

	return true;
}

bool LhyTest::on_pushButton_collectLandmark_clicked()
{
	if (m_NavigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	int landmark_surfaceNum = m_NavigatedImage->GetLandmarks()->GetSize();
	int landmark_rmNum = m_NavigatedImage->GetLandmarks_probe()->GetSize();

	if (landmark_surfaceNum == landmark_rmNum)
	{
		m_Controls.textBrowser->append("--- Enough landmarks have been collected ----");
		return true;
	}

	auto pointSet_probeLandmark = m_NavigatedImage->GetLandmarks_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
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

bool LhyTest::on_pushButton_collectIcp_clicked()
{
	if (m_NavigatedImage == nullptr)
	{
		m_Controls.textBrowser->append("Please setup the navigationObject first!");
		return false;
	}

	auto pointSet_probeIcp = m_NavigatedImage->GetIcpPoints_probe();

	//get navigation data of RobotEndRF in ndi coords,
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("Probe");
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

bool LhyTest::on_pushButton_applyRegistrationNew_clicked()
{
	m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);
	
	m_NavigatedImage->UpdateObjectToRfMatrix();
	m_Controls.textBrowser->append("Avg landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_avgError()));
	m_Controls.textBrowser->append("Max landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_maxError()));

	m_Controls.textBrowser->append("Avg ICP error:" + QString::number(m_NavigatedImage->GetIcpRegis_avgError()));
	m_Controls.textBrowser->append("Max ICP error:" + QString::number(m_NavigatedImage->GetIcpRegis_maxError()));

	m_ImageRegistrationMatrix->DeepCopy(m_NavigatedImage->GetT_Object2ReferenceFrame());

	auto tmpMatrix = mitk::AffineTransform3D::New();

	mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix,tmpMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("ObjectRf")->SetToolRegistrationMatrix(tmpMatrix);

	m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix());

	m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("ObjectRf"));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();

	return true;
}

bool LhyTest::on_pushButton_setTcpPrecisionTest_clicked()
{
	// set TCP for precision test
	// For Test Use, regard ball 2 as the TCP, the pose can be seen on
	// https://gn1phhht53.feishu.cn/wiki/wikcnAYrihLnKdt5kqGYIwmZACh
	//--------------------------------------------------
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
	// x_tcp[0] = 51.91;
	// x_tcp[1] = -55.01;
	// x_tcp[2] = 0.16;
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

	m_FlangeToToolMatrix->Identity();

	for(int i{0}; i < 3; i++)
	{
		m_FlangeToToolMatrix->SetElement(i, 0, x_tcp[i]);
		m_FlangeToToolMatrix->SetElement(i, 1, y_tcp[i]);
		m_FlangeToToolMatrix->SetElement(i, 2, z_tcp[i]);
		m_FlangeToToolMatrix->SetElement(i, 3, p2[i]);
	}
	
	return true;
}

bool LhyTest::on_pushButton_confirmImageTargetLine_clicked()
{
	auto targetLinePoints = dynamic_cast<mitk::PointSet*>(m_Controls.mitkNodeSelectWidget_imageTargetLine->GetSelectedNode()->GetData());
	auto targetPoint_0 = targetLinePoints->GetPoint(0); // TCP frame origin should move to this point
	auto targetPoint_1 = targetLinePoints->GetPoint(1);

	// Interpret targetPoint_0 from image frame to robot (internal) base frame
	// do some average
	double targetPointUnderBase_0[3]{ 0 };
	double targetPointUnderBase_1[3]{ 0 };
	for (int i{ 0 }; i < 20; i++) {
		m_VegaSource->Update();
		auto ndiToObjectRfMatrix = m_VegaSource->GetOutput("ObjectRf")->GetAffineTransform3D();

		auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();

		auto registrationMatrix_surfaceToRF = m_VegaToolStorage->GetToolByName("ObjectRf")->GetToolRegistrationMatrix();
		vtkNew<vtkMatrix4x4> tmpMatrix;
		mitk::TransferItkTransformToVtkMatrix(registrationMatrix_surfaceToRF.GetPointer(), tmpMatrix);
		tmpMatrix->Invert();

		mitk::TransferVtkMatrixToItkTransform(tmpMatrix, rfToSurfaceMatrix.GetPointer());

		auto ndiToTargetMatrix_0 = mitk::AffineTransform3D::New();
		ndiToTargetMatrix_0->SetOffset(targetPoint_0.GetDataPointer());
		ndiToTargetMatrix_0->Compose(rfToSurfaceMatrix);
		ndiToTargetMatrix_0->Compose(ndiToObjectRfMatrix);
		auto targetUnderBase_0 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix_0, targetUnderBase_0);
		auto targetPointUnderBase_tmp0 = targetUnderBase_0->GetOffset();

		auto ndiToTargetMatrix_1 = mitk::AffineTransform3D::New();
		ndiToTargetMatrix_1->SetOffset(targetPoint_1.GetDataPointer());
		ndiToTargetMatrix_1->Compose(rfToSurfaceMatrix);
		ndiToTargetMatrix_1->Compose(ndiToObjectRfMatrix);
		auto targetUnderBase_1 = mitk::AffineTransform3D::New();
		m_VegaSource->TransferCoordsFromTrackingDeviceToTrackedObject("RobotBaseRF", ndiToTargetMatrix_1, targetUnderBase_1);
		auto targetPointUnderBase_tmp1 = targetUnderBase_1->GetOffset();

		targetPointUnderBase_0[0] += targetPointUnderBase_tmp0[0];
		targetPointUnderBase_0[1] += targetPointUnderBase_tmp0[1];
		targetPointUnderBase_0[2] += targetPointUnderBase_tmp0[2];

		targetPointUnderBase_1[0] += targetPointUnderBase_tmp1[0];
		targetPointUnderBase_1[1] += targetPointUnderBase_tmp1[1];
		targetPointUnderBase_1[2] += targetPointUnderBase_tmp1[2];

		MITK_INFO << "tmp target Point:" << targetPointUnderBase_tmp0[0];
	}

	targetPointUnderBase_0[0] = targetPointUnderBase_0[0] / 20;
	targetPointUnderBase_0[1] = targetPointUnderBase_0[1] / 20;
	targetPointUnderBase_0[2] = targetPointUnderBase_0[2] / 20;

	targetPointUnderBase_1[0] = targetPointUnderBase_1[0] / 20;
	targetPointUnderBase_1[1] = targetPointUnderBase_1[1] / 20;
	targetPointUnderBase_1[2] = targetPointUnderBase_1[2] / 20;

	vtkNew<vtkMatrix4x4> vtkCurrentPoseUnderBase;

	auto vtkTrans = vtkTransform::New();
	vtkTrans->PostMultiply();
	vtkTrans->Identity();
	vtkTrans->SetMatrix(m_FlangeToToolMatrix);
	vtkTrans->Concatenate(m_RobotPosition);
	vtkTrans->Update();

	vtkCurrentPoseUnderBase->DeepCopy(vtkTrans->GetMatrix());

	Eigen::Vector3d currentXunderBase;
	currentXunderBase[0] = vtkCurrentPoseUnderBase->GetElement(0, 0);
	currentXunderBase[1] = vtkCurrentPoseUnderBase->GetElement(1, 0);
	currentXunderBase[2] = vtkCurrentPoseUnderBase->GetElement(2, 0);
	currentXunderBase.normalize();
	MITK_INFO << "currentXunderBase" << currentXunderBase;
	
	Eigen::Vector3d targetXunderBase;
	targetXunderBase[0] = targetPointUnderBase_1[0] - targetPointUnderBase_0[0];
	targetXunderBase[1] = targetPointUnderBase_1[1] - targetPointUnderBase_0[1];
	targetXunderBase[2] = targetPointUnderBase_1[2] - targetPointUnderBase_0[2];

	targetXunderBase.normalize();

	MITK_INFO << "targetXunderBase" << targetXunderBase;

	Eigen::Vector3d rotationAxis;
	rotationAxis = currentXunderBase.cross(targetXunderBase);
	rotationAxis;

	double rotationAngle;
	if (currentXunderBase.dot(targetXunderBase) > 0) {
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;
	}
	else {
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(vtkCurrentPoseUnderBase);
	tmpTransform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
	tmpTransform->Update();

	auto testMatrix = tmpTransform->GetMatrix();

	auto targetPoseUnderBase = mitk::AffineTransform3D::New();
	mitk::TransferVtkMatrixToItkTransform(tmpTransform->GetMatrix(), targetPoseUnderBase.GetPointer());

	auto vtkBaseToTool = vtkMatrix4x4::New();
	vtkBaseToTool->DeepCopy(testMatrix);
	vtkBaseToTool->SetElement(0, 3, targetPointUnderBase_0[0]);
	vtkBaseToTool->SetElement(1, 3, targetPointUnderBase_0[1]);
	vtkBaseToTool->SetElement(2, 3, targetPointUnderBase_0[2]);

	m_Controls.textBrowser->append("Result Line target point:" + QString::number(targetPointUnderBase_0[0]) 
		+ "/" + QString::number(targetPointUnderBase_0[1]) 
		+ "/" + QString::number(targetPointUnderBase_0[2]));

	auto vtkToolToFlange = vtkMatrix4x4::New();
	vtkToolToFlange->DeepCopy(m_FlangeToToolMatrix);
	vtkToolToFlange->Invert();

	auto vtkTransform = vtkTransform::New();
	vtkTransform->PostMultiply();
	vtkTransform->Identity();
	vtkTransform->SetMatrix(vtkToolToFlange);
	vtkTransform->Concatenate(vtkBaseToTool);
	vtkTransform->Update();

	m_BaseToFlange_target->DeepCopy(vtkTransform->GetMatrix());

	return true;
}

bool LhyTest::on_pushButton_startAutoPosition_clicked()
{
	double tmpArray[9];
	m_StaubliTrackingDevice->RobotMove(m_BaseToFlange_target, tmpArray);

	return true;
}

void LhyTest::UpdateFlangeVisual()
{
	//get navigation data of RobotEndRF in ndi coords,
	auto robotBaseRFIndex = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (robotBaseRFIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'RobotBaseRF' or 'ObjectRf' in the toolStorage!");
		return;
	}

	mitk::NavigationData::Pointer nd_ndiToRobotBaseRF = m_VegaSource->GetOutput(robotBaseRFIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_objectRfToRobotBaseRF = GetNavigationDataInRef(nd_ndiToRobotBaseRF, nd_ndiToObjectRf);

	// Don't use this line ! auto affineTrans_objectRfToRobotBaseRF = nd_objectRfToRobotBaseRF->GetAffineTransform3D();

	auto vtkMatrix_objectRfToRobotBaseRF = vtkMatrix4x4::New();

	mitk::TransferItkTransformToVtkMatrix(nd_objectRfToRobotBaseRF->GetAffineTransform3D().GetPointer(), vtkMatrix_objectRfToRobotBaseRF);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(m_RobotPosition);
	tmpTrans->Concatenate(m_RobotRegistrationMatrix);
	tmpTrans->Concatenate(vtkMatrix_objectRfToRobotBaseRF);
	tmpTrans->Concatenate(m_ImageRegistrationMatrix);
	tmpTrans->Update();

	if(GetDataStorage()->GetNamedNode("Robot_visual") == nullptr)
	{
		m_Controls.textBrowser->append("Robot_visual is missing");
		return;
	}

	auto robotSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("Robot_visual");

	robotSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

}

void LhyTest::UpdateToolVisual()
{
	//get navigation data of RobotEndRF in ndi coords,
	auto robotBaseRFIndex = m_VegaToolStorage->GetToolIndexByName("RobotBaseRF");
	auto objectRfIndex = m_VegaToolStorage->GetToolIndexByName("ObjectRf");
	if (robotBaseRFIndex == -1 || objectRfIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'RobotBaseRF' or 'ObjectRf' in the toolStorage!");
		return;
	}

	mitk::NavigationData::Pointer nd_ndiToRobotBaseRF = m_VegaSource->GetOutput(robotBaseRFIndex);
	mitk::NavigationData::Pointer nd_ndiToObjectRf = m_VegaSource->GetOutput(objectRfIndex);

	mitk::NavigationData::Pointer nd_objectRfToRobotBaseRF = GetNavigationDataInRef(nd_ndiToRobotBaseRF, nd_ndiToObjectRf);

	// Don't use this line ! auto affineTrans_objectRfToRobotBaseRF = nd_objectRfToRobotBaseRF->GetAffineTransform3D();

	auto vtkMatrix_objectRfToRobotBaseRF = vtkMatrix4x4::New();

	mitk::TransferItkTransformToVtkMatrix(nd_objectRfToRobotBaseRF->GetAffineTransform3D().GetPointer(), vtkMatrix_objectRfToRobotBaseRF);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(m_FlangeToToolMatrix);
	tmpTrans->Concatenate(m_RobotPosition);
	tmpTrans->Concatenate(m_RobotRegistrationMatrix);
	tmpTrans->Concatenate(vtkMatrix_objectRfToRobotBaseRF);
	tmpTrans->Concatenate(m_ImageRegistrationMatrix);
	tmpTrans->Update();

	if (GetDataStorage()->GetNamedNode("Robot_visual") == nullptr)
	{
		m_Controls.textBrowser->append("Robot_visual is missing");
		return;
	}

	auto robotSurface = GetDataStorage()->GetNamedObject<mitk::Surface>("Robot_visual");

	robotSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

}


void LhyTest::on_pushButton_showFlange_clicked()
{
	disconnect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolVisual()));
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateFlangeVisual()));
}

void LhyTest::on_pushButton_showTool_clicked()
{
	connect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateToolVisual()));
	disconnect(m_VegaVisualizeTimer, SIGNAL(timeout()), this, SLOT(UpdateFlangeVisual()));
}

