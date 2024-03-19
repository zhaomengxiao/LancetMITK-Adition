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

#include "mitkMatrixConvert.h"

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

bool LhyTest::RecordInitial()
{
	m_initial_robotBaseToFlange = vtkMatrix4x4::New();

	mitk::NavigationData::Pointer nd_robotBaseToFlange = m_StaubliSource->GetOutput(0)->Clone();

	mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), m_initial_robotBaseToFlange);

	return true;
}

bool LhyTest::GoToInitial()
{
	/*double result[9];

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
	m_Controls.textBrowser->append("~~~~~~~~~~~~~~~~~~~~~~~~~~~");*/


	return true;
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
  //connect(m_StaubliTrackingDevice.GetPointer(), &lancet::StaubliRobotDevice::updateToolPosition, this, &LhyTest::OnUpdateToolPosition);
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

void LhyTest::DoImageProcessing()
{
  QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
  if (nodes.empty())
    return;

  mitk::DataNode *node = nodes.front();

  if (!node)
  {
    // Nothing selected. Inform the user and return
    QMessageBox::information(nullptr, "Template", "Please load and select an image before starting image processing.");
    return;
  }

  // here we have a valid mitk::DataNode

  // a node itself is not very useful, we need its data item (the image)
  mitk::BaseData *data = node->GetData();
  if (data)
  {
    // test if this data item is an image or not (could also be a surface or something totally different)
    mitk::Image *image = dynamic_cast<mitk::Image *>(data);
    if (image)
    {
      std::stringstream message;
      std::string name;
      message << "Performing image processing for image ";
      if (node->GetName(name))
      {
        // a property called "name" was found for this DataNode
        message << "'" << name << "'";
      }
      message << ".";
      MITK_INFO << message.str();

      // actually do something here...
    }
  }
}


bool LhyTest::SetPrecisionTestTcp() {
	return false;
}


