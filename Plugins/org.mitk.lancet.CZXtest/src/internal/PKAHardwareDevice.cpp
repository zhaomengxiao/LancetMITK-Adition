#include "PKAHardwareDevice.h"

lancetAlgorithm::PKAKukaVegaHardwareDevice::PKAKukaVegaHardwareDevice(mitk::DataStorage* dataStorage)
{
	m_DataStorage = dataStorage;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::ConnectKuka()
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open Tool Storage"), "/",
		QObject::tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		m_DataStorage);
	PKAData::m_KukaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	PKAData::m_KukaToolStorage->SetName(filename.toStdString());

	//! [UseKuka 1]
	//Here we want to use the Kuka robot as a tracking device. Therefore we instantiate a object of the class
	//KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
	//MITK_INFO << "Kuka tracking";
	//QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
	PKAData::m_KukaTrackingDevice = lancet::KukaRobotDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(PKAData::m_KukaToolStorage, PKAData::m_KukaTrackingDevice);


	PKAData::m_KukaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_KukaVisualizer);


	PKAData::m_KukaSource->Connect();
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::StartKukaTracking(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget)
{
	if (PKAData::m_KukaTrackingDevice->GetState() == mitk::TrackingDevice::TrackingDeviceState::Ready) //ready
	{
		PKAData::m_KukaSource->StartTracking();
		if (toolTrackingStatusWidget)
		{
			ShowKukaToolStatus(toolTrackingStatusWidget);
		}
		//m_Controls.textBrowser->append(QString::number(PKAData::m_KukaTrackingDevice->GetState()));
	}
	else
	{
		MITK_ERROR << "Tracking not start,Device State:" << PKAData::m_KukaTrackingDevice->GetState();
	}
	auto geo = m_DataStorage->ComputeBoundingGeometry3D(m_DataStorage->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::ShowKukaToolStatus(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget)
{
	m_KukaNavigationData.clear();
	for (std::size_t i = 0; i < PKAData::m_KukaSource->GetNumberOfOutputs(); i++)
	{
		m_KukaNavigationData.push_back(PKAData::m_KukaSource->GetOutput(i));
	}
	//initialize widget
	toolTrackingStatusWidget->RemoveStatusLabels();
	toolTrackingStatusWidget->SetShowPositions(true);
	toolTrackingStatusWidget->SetTextAlignment(Qt::AlignLeft);
	toolTrackingStatusWidget->SetNavigationDatas(&m_KukaNavigationData);
	toolTrackingStatusWidget->ShowStatusLabels();
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::SelfCheckBtnClicked()
{
	if (!HandleRobotDisconnection())
		return;
	PKAData::m_KukaTrackingDevice->RequestExecOperate(/*"Robot",*/ "setio", { "20", "20" });
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::HandleRobotDisconnection()
{
	if (!PKAData::m_KukaSource || !PKAData::m_KukaTrackingDevice || PKAData::m_KukaTrackingDevice->GetState() != 2)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("Error"));
		msgBox.setInformativeText(QString::fromLocal8Bit("未连接机械臂"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return false;
	}
	return true;
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::HandlerCameraDisconnection()
{
	if (!PKAData::m_VegaSource || !PKAData::m_VegaSource->IsConnected())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("Error"));
		msgBox.setInformativeText(QString::fromLocal8Bit("未连接相机"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return false;
	}
	return true;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::ConnectNDI(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget)
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open Tool Storage"), "/",
		QObject::tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		m_DataStorage);
	PKAData::m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	PKAData::m_VegaToolStorage->SetName(filename.toStdString());

	//! [UseKuka 1]
	//Here we want to use the Kuka robot as a tracking device. Therefore we instantiate a object of the class
	//KukaRobotDevice and make some settings which are necessary for a proper connection to the device.
	MITK_INFO << "Vega tracking";
	//QMessageBox::warning(nullptr, "Warning", "You have to set the parameters for the NDITracking device inside the code (QmitkIGTTutorialView::OnStartIGT()) before you can use it.");
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer kukaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(PKAData::m_VegaToolStorage, vegaTrackingDevice);

	PKAData::m_VegaSource = kukaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	PKAData::m_VegaSource->SetToolMetaDataCollection(PKAData::m_VegaToolStorage);
	PKAData::m_VegaSource->Connect();

	PKAData::m_VegaSource->StartTracking();
	if (toolTrackingStatusWidget)
	{
		ShowVegaToolStatus(toolTrackingStatusWidget);
	}

	auto geo = m_DataStorage->ComputeBoundingGeometry3D(m_DataStorage->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::ShowVegaToolStatus(QmitkToolTrackingStatusWidget* toolTrackingStatusWidget)
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < PKAData::m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(PKAData::m_VegaSource->GetOutput(i));
	}
	//initialize widget
	toolTrackingStatusWidget->RemoveStatusLabels();
	toolTrackingStatusWidget->SetShowPositions(true);
	toolTrackingStatusWidget->SetTextAlignment(Qt::AlignLeft);
	toolTrackingStatusWidget->SetNavigationDatas(&m_VegaNavigationData);
	toolTrackingStatusWidget->ShowStatusLabels();
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::VerifyProbe(QProgressBar* bar, QPushButton* button, QLabel* rmsLabel)
{
	if (m_IsVerifyProbe)
	{
		m_IsVerifyProbe = false;
		bar->setValue(0);
		button->setText("探针验证");
	}
	else
	{
		m_IsVerifyProbe = true;
		button->setText("停止");
		bar->setValue(0);
		m_ProbeRecordCount = 0;
		m_ProbePosVec.clear();
		m_ProbeVerifyTimer = new QTimer(this);
		m_ProbeVerifyTimer->setInterval(50);
		m_ProbeVerifyTimer->start();

		m_ProbeVerifyConnection = m_ProbeVerifyTimer->callOnTimeout([=]()
			{
				if (!PKAData::m_VegaSource && PKAData::m_VegaSource->IsConnected())
					return;
				Eigen::Vector3d vec;
				auto vegaData = PKAData::m_VegaSource->GetOutput("Probe");
				vec[0] = vegaData->GetAffineTransform3D()->GetOffset()[0];
				vec[1] = vegaData->GetAffineTransform3D()->GetOffset()[1];
				vec[2] = vegaData->GetAffineTransform3D()->GetOffset()[2];

				m_ProbePosVec.push_back(vec);
				m_ProbeRecordCount++;
				bar->setValue(m_ProbeRecordCount);
				if (m_ProbeRecordCount > 100)
				{
					Eigen::Vector3d center{ 0,0,0 };
					for (auto data : m_ProbePosVec)
					{
						center += data;
					}
					center = center / m_ProbePosVec.size();

					double RMS = 0.0;
					//RMS 代表误差值
					for (auto data : m_ProbePosVec)
					{
						RMS += (data - center).norm();
					}
					RMS /= m_ProbePosVec.size();

					rmsLabel->setText(QString::number(fabs(RMS)));
					m_IsVerifyProbe = false;
					button->setText("探针验证");
				}
			});
	}
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::UpdateHardware()
{
	if (m_KukaVisualizer)
		m_KukaVisualizer->Update();
	if (m_VegaVisualizer)
		m_VegaVisualizer->Update();
}

lancet::NavigationObjectVisualizationFilter::Pointer lancetAlgorithm::PKAKukaVegaHardwareDevice::GetRobotVisualizer()
{
	return m_KukaVisualizer;
}

lancet::NavigationObjectVisualizationFilter::Pointer lancetAlgorithm::PKAKukaVegaHardwareDevice::GetCameraVisualizer()
{
	return m_VegaVisualizer;
}

Eigen::Vector3d lancetAlgorithm::PKAKukaVegaHardwareDevice::GetToolInCameraPos(std::string toolName)
{
	mitk::NavigationData::Pointer TCamera2Tool = PKAData::m_VegaSource->GetOutput(toolName);
	auto matrixCamera2Tool = CalculationHelper::GetVTKMatrix4x4(TCamera2Tool);
	Eigen::Vector3d result;
	for (int i = 0; i < 3; ++i)
	{
		result(i) = matrixCamera2Tool->GetElement(i, 3);
	}
	return result;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::KukaSelfCheck()
{
	if (!this->HandleRobotDisconnection())
		return;
	PKAData::m_KukaTrackingDevice->RequestExecOperate(/*"Robot",*/ "setio", { "20", "20" });
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::Translate(const double axis[3], double length, int type)
{
	if (!HandleRobotDisconnection())
		return false;
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double scaledAxis[3];
	for (int i = 0; i < 3; ++i) {
		scaledAxis[i] = axis[i] * length;
	}
	affineTransform->Translate(scaledAxis);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;

	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);
	InterpretMovementAsInBaseSpace(type, rawMovementMatrix, movementMatrixInRobotBase);

	PKAData::m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::InterpretMovementAsInBaseSpace(int moveType, vtkMatrix4x4* rawMovementMatrix, vtkMatrix4x4* movementMatrixInRobotBase)
{
	if (moveType == 1)
	{
		vtkNew<vtkMatrix4x4> matrix_robotBaseToFlange;

		mitk::NavigationData::Pointer nd_robotBaseToFlange = PKAData::m_KukaSource->GetOutput(0)->Clone();

		mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), matrix_robotBaseToFlange);

		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(matrix_robotBaseToFlange);
		tmpTransform->Concatenate(rawMovementMatrix);
		tmpTransform->Update();

		movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());

		//m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		//m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
		//	"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}

	if (moveType == 0)
	{
		vtkNew<vtkMatrix4x4> matrix_robotBaseToFlange;

		mitk::NavigationData::Pointer nd_robotBaseToFlange = PKAData::m_KukaSource->GetOutput(0)->Clone();

		mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), matrix_robotBaseToFlange);

		vtkNew<vtkMatrix4x4> matrix_flangeSpaceToMovementSpace;
		// matrix_flangeSpaceToMovementSpace->DeepCopy(m_matrix_flangeSpaceToMovementSpace);
		matrix_flangeSpaceToMovementSpace->Identity();

		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(rawMovementMatrix);
		tmpTransform->Concatenate(matrix_flangeSpaceToMovementSpace);
		tmpTransform->Concatenate(matrix_robotBaseToFlange);
		tmpTransform->Update();

		movementMatrixInRobotBase->DeepCopy(tmpTransform->GetMatrix());

		//m_Controls.textBrowser->append("Movement matrix in robot base has been updated.");
		//m_Controls.textBrowser->append("Translation: x: " + QString::number(movementMatrixInRobotBase->GetElement(0, 3)) +
		//	"/ y: " + QString::number(movementMatrixInRobotBase->GetElement(1, 3)) + "/ z: " + QString::number(movementMatrixInRobotBase->GetElement(2, 3)));

		return true;
	}

	return false;
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::Rotate(const double axis[3], double degree, int type)
{
	if (!HandleRobotDisconnection())
		return false;
	mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
	affineTransform->SetIdentity();
	double angle = PI * degree / 180;
	affineTransform->Rotate3D(axis, angle);

	vtkNew<vtkMatrix4x4> rawMovementMatrix;
	vtkNew<vtkMatrix4x4> movementMatrixInRobotBase;

	mitk::TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), rawMovementMatrix);

	InterpretMovementAsInBaseSpace(type, rawMovementMatrix, movementMatrixInRobotBase);

	PKAData::m_KukaTrackingDevice->RobotMove(movementMatrixInRobotBase);

	return true;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::DisplayRobotEndDeviation()
{
	if (PKAData::m_KukaSource == nullptr || PKAData::m_VegaSource == nullptr) {
		return;
	}

	vtkNew<vtkMatrix4x4> baseRFtoBaseMatrix;
	mitk::TransferItkTransformToVtkMatrix(PKAData::m_VegaToolStorage->GetToolByName("RobotBaseRF")->GetToolRegistrationMatrix().GetPointer(), baseRFtoBaseMatrix);
	vtkNew<vtkMatrix4x4> endRFtoFlangeMatrix;
	mitk::TransferItkTransformToVtkMatrix(PKAData::m_VegaToolStorage->GetToolByName("RobotEndRF")->GetToolRegistrationMatrix().GetPointer(), endRFtoFlangeMatrix);

	mitk::NavigationData::Pointer nd_baseToFlange = PKAData::m_KukaSource->GetOutput(0);
	mitk::NavigationData::Pointer nd_ndiToRobotEndRF = PKAData::m_VegaSource->GetOutput("RobotEndRF");
	mitk::NavigationData::Pointer nd_ndiToBaseRF = PKAData::m_VegaSource->GetOutput("RobotBaseRF");

	auto baseToFlangeMatrix = CalculationHelper::GetVTKMatrix4x4(nd_baseToFlange);
	auto ndiToBaseRFMatrix = CalculationHelper::GetVTKMatrix4x4(nd_ndiToBaseRF);

	vtkNew<vtkMatrix4x4> vtkFlangeToEndRF;
	vtkFlangeToEndRF->DeepCopy(endRFtoFlangeMatrix);
	vtkFlangeToEndRF->Invert();

	double endRFposition[3] = {
		nd_ndiToRobotEndRF->GetPosition()[0],
		nd_ndiToRobotEndRF->GetPosition()[1],
		nd_ndiToRobotEndRF->GetPosition()[2]
	};

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(vtkFlangeToEndRF);
	tmpTransform->Concatenate(baseToFlangeMatrix);
	tmpTransform->Concatenate(baseRFtoBaseMatrix);
	tmpTransform->Concatenate(ndiToBaseRFMatrix);
	tmpTransform->Update();

	auto vtkNdiToEndRFmatrix = tmpTransform->GetMatrix();
	double endRFposition_1[3] = {
		vtkNdiToEndRFmatrix->GetElement(0, 3),
		vtkNdiToEndRFmatrix->GetElement(1, 3),
		vtkNdiToEndRFmatrix->GetElement(2, 3)
	};

	double error = sqrt(pow(endRFposition_1[0] - endRFposition[0], 2) +
		pow(endRFposition_1[1] - endRFposition[1], 2) +
		pow(endRFposition_1[2] - endRFposition[2], 2));
	// m_Controls.lineEdit_roboRegistrationError->setText(QString::number(error));

	// Calculate the real-time robot registration angle error
	auto calculateAngleError = [](double direct[3], double indirect[3]) -> double {
		const double RAD_TO_DEG = 180 / PI;
		double dotProduct = direct[0] * indirect[0] + direct[1] * indirect[1] + direct[2] * indirect[2];
		return RAD_TO_DEG * acos(dotProduct);
	};

	auto vtkNdiToRoboEndRFmatrix_direct = CalculationHelper::GetVTKMatrix4x4(nd_ndiToRobotEndRF);

	double x_direct[3] = {
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0, 0),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1, 0),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2, 0)
	};

	double x_indirect[3] = {
		vtkNdiToEndRFmatrix->GetElement(0, 0),
		vtkNdiToEndRFmatrix->GetElement(1, 0),
		vtkNdiToEndRFmatrix->GetElement(2, 0)
	};

	double y_direct[3] = {
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0, 1),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1, 1),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2, 1)
	};

	double y_indirect[3] = {
		vtkNdiToEndRFmatrix->GetElement(0, 1),
		vtkNdiToEndRFmatrix->GetElement(1, 1),
		vtkNdiToEndRFmatrix->GetElement(2, 1)
	};

	double z_direct[3] = {
		vtkNdiToRoboEndRFmatrix_direct->GetElement(0, 2),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(1, 2),
		vtkNdiToRoboEndRFmatrix_direct->GetElement(2, 2)
	};

	double z_indirect[3] = {
		vtkNdiToEndRFmatrix->GetElement(0, 2),
		vtkNdiToEndRFmatrix->GetElement(1, 2),
		vtkNdiToEndRFmatrix->GetElement(2, 2)
	};

	double x_angleError = calculateAngleError(x_direct, x_indirect);
	double y_angleError = calculateAngleError(y_direct, y_indirect);
	double z_angleError = calculateAngleError(z_direct, z_indirect);

	// m_Controls.lineEdit_xAngleError->setText(QString::number(x_angleError));
	// m_Controls.lineEdit_yAngleError->setText(QString::number(y_angleError));
	// m_Controls.lineEdit_zAngleError->setText(QString::number(z_angleError));
}

//TODO
void lancetAlgorithm::PKAKukaVegaHardwareDevice::AutoRobotRegistration()
{
	std::cout << "this function is not complete" << std::endl;
	if (!HandleRobotDisconnection())
	{
		return;
	}
	auto moveRobotTask = [this]()
	{
		for (; m_RobotRegistration.PoseCount() < 10;)
		{
			auto affine = RobotAutoMove();
			while (!IsArrivedTarget(affine))
			{
				QThread::msleep(100);
			}
			QThread::msleep(2000);

			QMetaObject::invokeMethod(this, [this]() {
				std::cout << "Robot Moved Done" << std::endl;
				CaptureRobot();
				}, Qt::QueuedConnection);
			QThread::msleep(2000);
		}
	};
	QFuture<void> future = QtConcurrent::run(moveRobotTask);
	RobotMoveTaskWatcher.setFuture(future);
}

mitk::AffineTransform3D::Pointer lancetAlgorithm::PKAKukaVegaHardwareDevice::RobotAutoMove()
{
	if (!this->HandleRobotDisconnection())
		return nullptr;
	// 获取当前机器人的姿态作为参考姿态
	mitk::NavigationData::Pointer nd_robot2flange = PKAData::m_KukaSource->GetOutput(0);
	mitk::AffineTransform3D::Pointer affine = nd_robot2flange->GetAffineTransform3D()->Clone();
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	// 定义平移和旋转参数
	double translations[][3] = {
		{0, 75, 0}, {0, -75, 0}, {0, 0, 75}, {0, 0, -75}
	};

	double rotations[][4] = {
		{1, 0, 0, 15}, {-1, 0, 0, 15}, {1, 0, 0, 11}, {0, 0, 1, 10}, {0, 0, 1, -10}
	};

	vtkNew<vtkTransform> tmpTrans;
	double tcp[6]{ 0 };
	//int poseCount = 0;
	int i = m_RobotRegistration.PoseCount();
	QMetaObject::invokeMethod(this, [this, i]() {
		//m_Controls.textBrowser->append("RobotRegistration PoseCount is: " + QString::number(i));
		}, Qt::QueuedConnection);
	if (i == 1)
	{
		// 设置 TCP
		QMetaObject::invokeMethod(this, [this, tcp]() {
			//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "TCP:", 6, tcp);
			}, Qt::QueuedConnection);

		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("movel",
			QStringList{ QString::number(tcp[0]), QString::number(tcp[1]), QString::number(tcp[2]),
						QString::number(tcp[3]), QString::number(tcp[4]), QString::number(tcp[5]) });
		QThread::msleep(1000);
		//
		PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

		// 将第一个点作为初始位置
		this->RecordInitPos();

		// 平移操作
		affine->Translate(translations[i - 1]);
		mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
		PKAData::m_KukaTrackingDevice->RobotMove(vtkMatrix);
	}
	else if (i > 1 && i <= 9)
	{
		tmpTrans->PostMultiply();

		if (i <= 4) // 平移操作
		{
			tmpTrans->Translate(translations[i - 1]);
		}
		else // 旋转操作
		{
			tmpTrans->RotateWXYZ(rotations[i - 5][3], rotations[i - 5][0], rotations[i - 5][1], rotations[i - 5][2]);
		}

		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
	}
	else
	{
		mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);
		//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "", vtkMatrix);
		//m_KukaTrackingDevice->RobotMove(vtkMatrix);
	}
	return affine;
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::IsArrivedTarget(mitk::AffineTransform3D::Pointer target)
{
	double current = DBL_MAX;
	double targetPoint[3]{ target->GetOffset()[0] ,target->GetOffset()[1] ,target->GetOffset()[2] };
	double currentPoint[3];

	currentPoint[0] = PKAData::m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[0];
	currentPoint[1] = PKAData::m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[1];
	currentPoint[2] = PKAData::m_KukaSource->GetOutput(0)->GetAffineTransform3D()->GetOffset()[2];

	current = sqrt(
		pow(targetPoint[0] - currentPoint[0], 2) +
		pow(targetPoint[1] - currentPoint[1], 2) +
		pow(targetPoint[2] - currentPoint[2], 2)
	);

	if (current < 0.0001)
		return true;

	return false;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::CaptureRobot()
{
	// 增加捕获姿态和更新UI的方法
	auto captureAndUpdateUI = [this](bool isTranslation) {
		CapturePose(isTranslation);
		int m_IndexOfRobotCapture = m_RobotRegistration.PoseCount();
		//m_Controls.CaptureCountLineEdit->setText(QString::number(m_IndexOfRobotCapture));
		MITK_INFO << "OnRobotCapture: " << m_IndexOfRobotCapture;
	};

	if (m_RobotRegistration.PoseCount() < 5) {
		captureAndUpdateUI(true); // 前五次捕获平移姿态
	}
	else if (m_RobotRegistration.PoseCount() < 10) {
		captureAndUpdateUI(false); // 后五次捕获旋转姿态
	}
	else {
		MITK_INFO << "OnRobotCapture finish: " << m_RobotRegistration.PoseCount();
		vtkSmartPointer<vtkMatrix4x4> matrix4x4 = vtkSmartPointer<vtkMatrix4x4>::New();
		m_RobotRegistration.GetRegistraionMatrix(matrix4x4);

		vtkNew<vtkMatrix4x4> robotEndToFlangeMatrix;
		m_RobotRegistration.GetTCPmatrix(robotEndToFlangeMatrix);
		robotEndToFlangeMatrix->Invert();

		//m_Controls.textBrowser->append("Registration RMS: " + QString::number(m_RobotRegistration.RMS()));

		m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();
		auto affineRobotEndRFtoFlangeMatrix = mitk::AffineTransform3D::New();

		mitk::TransferVtkMatrixToItkTransform(matrix4x4, m_RobotRegistrationMatrix.GetPointer());
		mitk::TransferVtkMatrixToItkTransform(robotEndToFlangeMatrix, affineRobotEndRFtoFlangeMatrix.GetPointer());

		// 保存机器人注册矩阵到参考工具
		auto robotBaseTool = PKAData::m_VegaToolStorage->GetToolByName("RobotBaseRF");
		auto robotEndTool = PKAData::m_VegaToolStorage->GetToolByName("RobotEndRF");
		if (robotBaseTool && robotEndTool) {
			robotBaseTool->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);
			robotEndTool->SetToolRegistrationMatrix(affineRobotEndRFtoFlangeMatrix);
		}
		vtkNew<vtkMatrix4x4> vtkT_BaseToBaseRF;
		m_RobotRegistration.GetRegistraionMatrix(vtkT_BaseToBaseRF);
		vtkT_BaseToBaseRF->Invert();
		PKAData::m_TBaseRF2Base->DeepCopy(vtkT_BaseToBaseRF);
		vtkMatrix4x4* vtkT_FlangeToEndRF = vtkMatrix4x4::New();
		m_RobotRegistration.GetTCPmatrix(vtkT_FlangeToEndRF);
		PKAData::m_TFlange2EndRF->DeepCopy(vtkT_FlangeToEndRF);
		//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "TBase2BaseRF: ", PKAData::m_TBase2BaseRF);
		//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "TFlange2EndRF: ", PKAData::m_TFlange2EndRF);

		// 构建 ApplyDeviceRegistrationFilter
		m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
		m_KukaApplyRegistrationFilter->ConnectTo(PKAData::m_KukaSource);
		m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
		m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(PKAData::m_VegaSource->GetOutput("RobotBaseRF"));

		m_KukaVisualizeTimer->stop();
		auto kukaVisualizer = this->GetRobotVisualizer();
		kukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
		m_KukaVisualizeTimer->start();

		// 获取并设置 TCP
		std::array<double, 6> tcp{};
		m_RobotRegistration.GetTCP(tcp);
		//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "TCP: ", tcp);

		// 设置 TCP 到机器人
		auto setRobotWorkMode = [this](const QString& mode) {
			QThread::msleep(1000);
			PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { mode });
		};

		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]), QString::number(tcp[1]), QString::number(tcp[2]), QString::number(tcp[3]), QString::number(tcp[4]), QString::number(tcp[5]) });
		setRobotWorkMode("11");
		setRobotWorkMode("5");
	}
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::OldAutoMove()
{
	mitk::NavigationData::Pointer nd_robot2flange = PKAData::m_KukaSource->GetOutput(0);
	MITK_INFO << "nd_robot2flange";
	MITK_INFO << nd_robot2flange;
	mitk::AffineTransform3D::Pointer affine = mitk::AffineTransform3D::New();
	affine = nd_robot2flange->GetAffineTransform3D()->Clone();
	double axisx[3]{ 1, 0, 0 };
	double axisy[3]{ 0, 1, 0 };
	double axisz[3]{ 0, 0, 1 };
	vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();

	double trans1[3]{ 0, 0, 50 };
	double trans2[3]{ 0, 50, 0 };
	double trans3[3]{ 50, 0, 0 };
	double trans4[3]{ 0, 0, -25 };
	double trans5[3]{ -25, 0, 0 };
	double trans6[3]{ 0, -25, 0 };
	double trans7[3]{ 0, -25, 0 };
	double trans8[3]{ 25, 0, 0 };
	double trans9[3]{ -25, 0, 0 };

	double plusY[3]{ 0, 75, 0 };
	double plusZ[3]{ 0,0,75 };
	double plusX[3]{ 75,0,0 };
	double minusX[3]{ -150,0,0 };

	vtkNew<vtkTransform> tmpTrans;
	double tcp[6]{ 0 };

	switch (m_RobotRegistration.PoseCount())
	{
	case 1: //y+75

		// ensure flange TCP is applied when the robot starts to move

		MITK_INFO << "TCP:" << tcp[0] << "," << tcp[1] << "," << tcp[2] << "," << tcp[3] << "," << tcp[4] << "," << tcp[5];
		//set tcp to robot
		  //set tcp
		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });

		// record the initial position into m_initial_robotBaseToFlange
		RecordInitPos();

		affine->Translate(plusY);
		mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

		PKAData::m_KukaTrackingDevice->RobotMove(vtkMatrix);
		break;

	case 2:
		tmpTrans->PostMultiply();
		tmpTrans->Translate(0, -75, 0);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;

	case 3:
		tmpTrans->PostMultiply();
		tmpTrans->Translate(0, 0, 75);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;


		break;

	case 4:
		tmpTrans->PostMultiply();
		tmpTrans->Translate(0, 0, -75);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;

	case 5:
		tmpTrans->PostMultiply();
		tmpTrans->RotateX(15);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;


	case 6:
		tmpTrans->PostMultiply();
		tmpTrans->RotateX(-15);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;

	case 7:
		tmpTrans->PostMultiply();
		tmpTrans->RotateX(11);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;

	case 8:
		tmpTrans->PostMultiply();
		tmpTrans->RotateZ(10);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;


	case 9:
		tmpTrans->PostMultiply();
		tmpTrans->RotateZ(-10);
		tmpTrans->Concatenate(m_initial_robotBaseToFlange);
		tmpTrans->Update();

		PKAData::m_KukaTrackingDevice->RobotMove(tmpTrans->GetMatrix());
		break;

	default:


		mitk::TransferItkTransformToVtkMatrix(affine.GetPointer(), vtkMatrix);

		PKAData::m_KukaTrackingDevice->RobotMove(vtkMatrix);
		break;
	}
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::RecordInitPos()
{
	m_initial_robotBaseToFlange = vtkMatrix4x4::New();

	mitk::NavigationData::Pointer nd_robotBaseToFlange = PKAData::m_KukaSource->GetOutput(0)->Clone();

	mitk::TransferItkTransformToVtkMatrix(nd_robotBaseToFlange->GetAffineTransform3D().GetPointer(), m_initial_robotBaseToFlange);
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::GoToInitPos()
{
	PKAData::m_KukaTrackingDevice->RobotMove(m_initial_robotBaseToFlange);
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::CapturePose(bool translationOnly)
{
	//Output sequence is the same as AddTool sequence
//get navigation data of flange in robot coords,
	mitk::NavigationData::Pointer nd_robot2flange = PKAData::m_KukaSource->GetOutput(0);
	auto vtkRoboBaseToFlangeMatrix = CalculationHelper::GetVTKMatrix4x4(nd_robot2flange);
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "robot2flange: ", vtkRoboBaseToFlangeMatrix);

	////get navigation data of RobotEndRF in ndi coords,
	mitk::NavigationData::Pointer nd_Ndi2RobotEndRF = PKAData::m_VegaSource->GetOutput("RobotEndRF");
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "Camera2EndRF: ", CalculationHelper::GetVTKMatrix4x4(nd_Ndi2RobotEndRF));
	////get navigation data of RobotBaseRF in ndi coords,
	mitk::NavigationData::Pointer nd_Ndi2RobotBaseRF = PKAData::m_VegaSource->GetOutput("RobotBaseRF");
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "Camera2BaseRF: ", CalculationHelper::GetVTKMatrix4x4(nd_Ndi2RobotBaseRF));
	////get navigation data RobotEndRF in reference frame RobotBaseRF
	mitk::NavigationData::Pointer nd_RobotBaseRF2RobotEndRF = GetNavigationDataInRef(
		nd_Ndi2RobotEndRF, nd_Ndi2RobotBaseRF);

	//// add vtkMatrix as poses to the registration module
	//// Average the NavigationData from the NDI camera
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
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "BaseRF2End: ", vtkBaseRFtoRoboEndMatrix);
	m_RobotRegistration.AddPoseWithVtkMatrix(vtkRoboBaseToFlangeMatrix, vtkBaseRFtoRoboEndMatrix, translationOnly);
}

int lancetAlgorithm::PKAKukaVegaHardwareDevice::ResetRobotRegistration()
{
	m_RobotRegistration.RemoveAllPose();
	return m_RobotRegistration.PoseCount();
}

bool lancetAlgorithm::PKAKukaVegaHardwareDevice::AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval, int intervalNum, double matrixArray[16])
{
	// The frame rate of Vega ST is 60 Hz, so the timeInterval should be larger than 16.7 ms
	if (timeInterval <= 16) {
		qWarning() << "Time interval should be larger than 16.7 ms for 60 Hz frame rate.";
		return false;
	}

	Eigen::Vector3d tmp_x = Eigen::Vector3d::Zero();
	Eigen::Vector3d tmp_y = Eigen::Vector3d::Zero();
	Eigen::Vector3d tmp_translation = Eigen::Vector3d::Zero();

	for (int i = 0; i < intervalNum; ++i) {
		ndPtr->Update();
		auto tmpMatrix = CalculationHelper::GetVTKMatrix4x4(ndPtr);

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

	tmp_x /= intervalNum;
	tmp_y /= intervalNum;
	tmp_translation /= intervalNum;

	Eigen::Vector3d x = tmp_x.normalized();
	Eigen::Vector3d h = tmp_y.normalized();
	Eigen::Vector3d z = x.cross(h).normalized();
	Eigen::Vector3d y = z.cross(x).normalized();

	std::array<double, 16> tmpArray{
		x[0], y[0], z[0], tmp_translation[0],
		x[1], y[1], z[1], tmp_translation[1],
		x[2], y[2], z[2], tmp_translation[2],
		0,    0,    0,    1
	};

	std::copy(tmpArray.begin(), tmpArray.end(), matrixArray);
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "Average Matrix: ", matrixArray);
	return true;
}

mitk::NavigationData::Pointer lancetAlgorithm::PKAKukaVegaHardwareDevice::GetNavigationDataInRef(mitk::NavigationData::Pointer nd, mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::ReuseRobotRegistation()
{
	FileIO::ReadTextFileAsvtkMatrix(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_BaseRFToBase.txt", PKAData::m_TBaseRF2Base);
	FileIO::ReadTextFileAsvtkMatrix(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_FlangeToEndRF.txt", PKAData::m_TFlange2EndRF);
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "TBase2BaseRF: ", PKAData::m_TBase2BaseRF);
	//PrintDataHelper::AppendTextBrowserMatrix(m_Controls.textBrowser, "TFlange2EndRF: ", PKAData::m_TFlange2EndRF);
	vtkSmartPointer<vtkMatrix4x4> matrixBaseRF2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	matrixBaseRF2Base->DeepCopy(PKAData::m_TBaseRF2Base);

	vtkNew<vtkMatrix4x4> MatrixEndRFToFlange;
	MatrixEndRFToFlange->DeepCopy(PKAData::m_TFlange2EndRF);
	MatrixEndRFToFlange->Invert();

	m_RobotRegistrationMatrix = mitk::AffineTransform3D::New();
	auto affineRobotEndRFtoFlangeMatrix = mitk::AffineTransform3D::New();

	mitk::TransferVtkMatrixToItkTransform(matrixBaseRF2Base, m_RobotRegistrationMatrix.GetPointer());
	mitk::TransferVtkMatrixToItkTransform(MatrixEndRFToFlange, affineRobotEndRFtoFlangeMatrix.GetPointer());

	// 保存机器人注册矩阵到参考工具
	auto robotBaseTool = PKAData::m_VegaToolStorage->GetToolByName("RobotBaseRF");
	auto robotEndTool = PKAData::m_VegaToolStorage->GetToolByName("RobotEndRF");
	if (robotBaseTool && robotEndTool) {
		robotBaseTool->SetToolRegistrationMatrix(m_RobotRegistrationMatrix);
		robotEndTool->SetToolRegistrationMatrix(affineRobotEndRFtoFlangeMatrix);
	}

	// 构建 ApplyDeviceRegistrationFilter
	m_KukaApplyRegistrationFilter = lancet::ApplyDeviceRegistratioinFilter::New();
	m_KukaApplyRegistrationFilter->ConnectTo(PKAData::m_KukaSource);
	m_KukaApplyRegistrationFilter->SetRegistrationMatrix(m_RobotRegistrationMatrix);
	m_KukaApplyRegistrationFilter->SetNavigationDataOfRF(PKAData::m_VegaSource->GetOutput("RobotBaseRF"));

	m_KukaVisualizeTimer->stop();
	auto kukaVisualizer = this->GetRobotVisualizer();
	kukaVisualizer->ConnectTo(m_KukaApplyRegistrationFilter);
	m_KukaVisualizeTimer->start();

	// 获取并设置 TCP
	std::array<double, 6> tcp{};
	CalculationHelper::GetXYZEulerByMatrix(PKAData::m_TFlange2EndRF, tcp);

	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "TCP: ", tcp);

	// 设置 TCP 到机器人
	auto setRobotWorkMode = [this](const QString& mode) {
		QThread::msleep(1000);
		PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { mode });
	};

	QThread::msleep(1000);
	PKAData::m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]), QString::number(tcp[1]), QString::number(tcp[2]), QString::number(tcp[3]), QString::number(tcp[4]), QString::number(tcp[5]) });
	setRobotWorkMode("11");
	setRobotWorkMode("5");
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::SaveRobotRegistration()
{
	std::ofstream baseRF2Base(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_BaseRFToBase.txt");
	auto matrixBaseRF2Base = PKAData::m_TBaseRF2Base->GetData();
	for (int i = 0; i < 16; i++) {
		baseRF2Base << matrixBaseRF2Base[i];
		if (i != 15) {
			baseRF2Base << ",";
		}
		else {
			baseRF2Base << ";";
		}
	}
	baseRF2Base << std::endl;
	baseRF2Base.close();

	auto matrixFlange2EndRF = PKAData::m_TFlange2EndRF->GetData();
	std::ofstream flange2EndRFFile(std::string(getenv("USERPROFILE")) + "\\Desktop\\save\\T_FlangeToEndRF.txt");
	for (int i = 0; i < 16; i++) {
		flange2EndRFFile << matrixFlange2EndRF[i];
		if (i != 15) {
			flange2EndRFFile << ",";
		}
		else {
			flange2EndRFFile << ";";
		}
	}
	flange2EndRFFile << std::endl;
	flange2EndRFFile.close();
	//m_Controls.textBrowser->append("saveArmMatrix");
}

void lancetAlgorithm::PKAKukaVegaHardwareDevice::SetTcpToFlange()
{
	double tcp[6]{ 0 };
	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, "TCP:", 6, tcp);
	//set tcp to robot
	QThread::msleep(1000);
	PKAData::m_KukaTrackingDevice->RequestExecOperate("movel", QStringList{ QString::number(tcp[0]),QString::number(tcp[1]),QString::number(tcp[2]),QString::number(tcp[3]),QString::number(tcp[4]),QString::number(tcp[5]) });
	QThread::msleep(1000);
	PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "11" });
	QThread::msleep(1000);
	PKAData::m_KukaTrackingDevice->RequestExecOperate("setworkmode", { "5" });
}
