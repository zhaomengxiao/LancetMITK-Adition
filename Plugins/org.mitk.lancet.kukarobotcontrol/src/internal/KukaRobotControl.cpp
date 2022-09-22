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
#include "KukaRobotControl.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "usGetModuleContext.h"

//micro service
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usServiceProperties.h>
#include <usModuleContext.h>
#include <usModuleInitialization.h>

#include "mitkMatrixConvert.h"
US_INITIALIZE_MODULE

const std::string KukaRobotControl::VIEW_ID = "org.mitk.views.kukarobotcontrol";

void KukaRobotControl::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void KukaRobotControl::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &KukaRobotControl::DoImageProcessing);
  connect(m_Controls.pushButton_loadToolStorage, &QPushButton::clicked, this, &KukaRobotControl::LoadToolStorage);
  connect(m_Controls.pushButton_connectKuka, &QPushButton::clicked, this, &KukaRobotControl::ConnectKuka);
  connect(m_Controls.pushButton_selfCheck, &QPushButton::clicked, this, &KukaRobotControl::RobotArmSelfCheck);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &KukaRobotControl::RotateZ_plus);

}

bool KukaRobotControl::LoadToolStorage()
{
	// read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/", tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull())
	{
		return false;
	}

	// display the file path in the lineEdit
	m_Controls.lineEdit_toolStoragePath->setText(filename);

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(GetDataStorage());
	m_KukaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_KukaToolStorage->SetName(filename.toStdString());

	return true;
}

bool KukaRobotControl::ConnectKuka()
{
	// try to get Device from Micro service if there's been an existing connection
	us::ModuleContext* context = us::GetModuleContext();
	std::vector<us::ServiceReference<mitk::NavigationDataSource> > refs = context->GetServiceReferences<mitk::NavigationDataSource>();
	if (!refs.empty())
	{
		int totalRefNum = refs.size();

		for (int i{ 0 }; i < totalRefNum; i++)
		{
			mitk::NavigationDataSource* navigationDataSource = context->GetService<
				mitk::NavigationDataSource>(refs.at(i));

			auto deviceSource = dynamic_cast<mitk::TrackingDeviceSource*>(navigationDataSource);

			if (deviceSource != nullptr && deviceSource->GetTrackingDevice().IsNotNull())
			{
				auto device = deviceSource->GetTrackingDevice();
				if (device->GetTrackingDeviceName() == "Kuka")
				{
					m_KukaTrackingDeviceSource = dynamic_cast<mitk::TrackingDeviceSource*>(navigationDataSource);
					m_KukaTrackingDevice = dynamic_cast<lancet::KukaRobotDevice*>(deviceSource->GetTrackingDevice().GetPointer());
				}
			}
		}
	}
	
	if (m_KukaTrackingDevice.IsNull())
	{
		MITK_INFO << "Establishing connection with Kuka...";

		m_KukaTrackingDevice = lancet::KukaRobotDevice::New();

		//Create Navigation Data Source with the factory class, and the visualize filter.
		lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
			lancet::TrackingDeviceSourceConfiguratorLancet::New(m_KukaToolStorage, m_KukaTrackingDevice);

		m_KukaTrackingDeviceSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);

		m_KukaTrackingDeviceSource->RegisterAsMicroservice();

		m_KukaTrackingDeviceSource->Connect(); // TODO: failed connection attempt causes crash
		QThread::msleep(1000);
		m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ "0.0","0.0","0.0","0.0","0.0","0.0" });
		QThread::msleep(1000);
		m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
		QThread::msleep(1000);
		m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

	}

	return true;
	
}

bool KukaRobotControl::RobotArmSelfCheck()
{
	if(m_KukaTrackingDevice->RequestExecOperate(/*"Robot",*/ "setio", { "20", "20" }))
	{
		return true;
	}
	return false;
}

bool KukaRobotControl::ConfigureflangeToMovementSpace()
{
	int tmpMode = m_Controls.comboBox_moveMode->currentIndex();
	m_matrix_flangeSpaceToMovementSpace = vtkMatrix4x4::New();

	switch(tmpMode)
	{
	case 0:
		return true;
		break;

	case 1:
		m_matrix_flangeSpaceToMovementSpace->Identity();
		return true;
		break;

	// case 2:
	// 	m_matrix_flangeSpaceToMovementSpace->DeepCopy(m_matrixArray_flangeToSaw);
	// 	break;
	}

	return false;
}




bool KukaRobotControl::InterpretMovementAsInBaseSpace(vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase)
{
	if(m_Controls.comboBox_moveMode->currentIndex() == 0)
	{
		movementMatrixInRobotBase->DeepCopy(rawMovementMatrix);

		m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
			"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}

	if(ConfigureflangeToMovementSpace())
	{
		vtkNew<vtkMatrix4x4> matrix_robotBaseToFlange;

		mitk::NavigationData::Pointer nd_robotBaseToFlange = m_KukaTrackingDeviceSource->GetOutput(0)->Clone();

		mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), matrix_robotBaseToFlange);

		vtkNew<vtkMatrix4x4> matrix_flangeSpaceToMovementSpace;
		matrix_flangeSpaceToMovementSpace->DeepCopy(m_matrix_flangeSpaceToMovementSpace);

		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(rawMovementMatrix);
		tmpTransform->Concatenate(matrix_flangeSpaceToMovementSpace);
		tmpTransform->Concatenate(matrix_robotBaseToFlange);
		tmpTransform->Update();

		movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());

		m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
			"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}
	
	return false;
	
}

bool KukaRobotControl::RotateZ_plus()
{
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	double axisZ[3]{ 0,0,1 };
	double angle = 180* (m_Controls.lineEdit_intuitiveValue->text().toDouble())/3.14159;
	affineTransform->Rotate3D(axisZ, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;


	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(rawMovementMatrix, movementMatrixInRobotBase);

	m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}



