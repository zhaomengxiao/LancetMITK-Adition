#include "PKADianaAimHardwareDevice.h"


void lancetAlgorithm::PKADianaAimHardwareDevice::ConnectCamera()
{
	T_AIMPOS_DATAPARA mPosDataPara;
	Aim_API_Initial(m_AimHandle);
	Aim_SetEthernetConnectIP(m_AimHandle, 192, 168, 31, 10);
	rlt = Aim_ConnectDevice(m_AimHandle, I_ETHERNET, mPosDataPara);

	if (rlt == AIMOOE_OK)
	{
		std::cout << "connect success";
	}
	else {

		std::cout << "connect failed" << std::endl;
		//return;
	}

	QString filename = QFileDialog::getExistingDirectory(nullptr, "Select the Tools store folder", "");
	if (filename.isNull()) return;
	filename.append("/");
	std::cout << "The selected folder address :" << filename.toStdString() << std::endl;;
	rlt = Aim_SetToolInfoFilePath(m_AimHandle, filename.toLatin1().data());

	if (rlt == AIMOOE_OK)
	{

		std::cout << "set filenemae success" << std::endl;
	}
	else {

		std::cout << "set filenemae failed" << std::endl;
		//return;
	}

	int size = 0;
	Aim_GetCountOfToolInfo(m_AimHandle, size);

	if (size != 0)
	{
		t_ToolBaseInfo* toolarr = new t_ToolBaseInfo[size];

		rlt = Aim_GetAllToolFilesBaseInfo(m_AimHandle, toolarr);

		if (rlt == AIMOOE_OK)
		{
			for (int i = 0; i < size; i++)
			{
				/*		char* ptool = toolarr[i].name;
						QString toolInfo = QString("Tool Name：") + QString::fromLocal8Bit(ptool);
						m_Controls.textBrowser->append(toolInfo);*/
			}
		}
		delete[] toolarr;
	}
	else {
		std::cout << "There are no tool identification files in the current directory:";
	}

	std::cout << "End of connection";
	rlt = AIMOOE_OK;
}

void lancetAlgorithm::PKADianaAimHardwareDevice::UpdateCamera()
{
	QString position_text;
	std::vector<std::string> toolidarr;

	auto prlt = GetNewToolData();
	if (rlt == AIMOOE_OK)//判断是否采集成功
	{
		do
		{
			//获取数据
			UpdateCameraToToolMatrix(prlt, "PKARobotBaseRF", PKAData::m_TCamera2BaseRF, m_Labels[0]);
			//UpdateCameraToToolMatrix(prlt, "PKARobotEndRF", PKAData::m_TCamera2EndRF, m_Labels[1]);
			//UpdateCameraToToolMatrix(prlt, "PKADrill", PKAData::m_TCamera2EndRF, m_Labels[1]);
			m_FemurRFValidity = UpdateCameraToToolMatrix(prlt, "PKAFemurRF", PKAData::m_TCamera2FemurRF, m_Labels[2]);
			m_ProbeValidity = UpdateCameraToToolMatrix(prlt, "PKAProbe", PKAData::m_TCamera2Probe, m_Labels[3]);
			m_TibiaRFValidity = UpdateCameraToToolMatrix(prlt, "PKATibiaRF", PKAData::m_TCamera2TibiaRF, m_Labels[4]);
			UpdateCameraToToolMatrix(prlt, "PKADrill", PKAData::m_TCamera2Drill, m_Labels[5]);
			UpdateCameraToToolMatrix(prlt, "PKABluntProbeNew", PKAData::m_TCamera2BluntProbe, nullptr);
			//获取PKAProbe数据
			if (strcmp(prlt->toolname, "PKAProbe") == 0)
			{
				if (prlt->validflag)
				{
					m_ProbeTip[0] = prlt->tooltip[0];
					m_ProbeTip[1] = prlt->tooltip[1];
					m_ProbeTip[2] = prlt->tooltip[2];
				}
			}

			if (strcmp(prlt->toolname, "PKADrill") == 0)
			{
				if (prlt->validflag)
				{
					PKAData::m_TCamera2EndRF->DeepCopy(PKAData::m_TCamera2Drill);
				}
			}

			T_AimToolDataResult* pnext = prlt->next;
			delete prlt;
			prlt = pnext;
		} while (prlt != NULL);
	}
	else
	{
		delete prlt;
	}
}

void lancetAlgorithm::PKADianaAimHardwareDevice::InitQLabels(std::vector<QLabel*> labels)
{
	m_Labels = labels;
}

void lancetAlgorithm::PKADianaAimHardwareDevice::ConnectRobot()
{
	srv_net_st* pinfo = new srv_net_st();
	memset(pinfo->SrvIp, 0x00, sizeof(pinfo->SrvIp));
	memcpy(pinfo->SrvIp, m_RobotIpAddress, strlen(m_RobotIpAddress));
	pinfo->LocHeartbeatPort = 0;
	pinfo->LocRobotStatePort = 0;
	pinfo->LocSrvPort = 0;
	int ret = initSrv(nullptr, nullptr, pinfo);
	if (ret < 0)
	{
		std::cout << "Couldn't connect Robot, please ping 192.168.10.75 first" << std::endl;
	}
	if (pinfo)
	{
		delete pinfo;
		pinfo = nullptr;
	}
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RobotPowerOn()
{
	releaseBrake();
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RobotPowerOff()
{
	holdBrake();
	//stop(m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::SetTCP2Flange()
{
	double pose[6] = {0,0,0,0,0,0};
	setDefaultActiveTcpPose(pose, m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RecordIntialPos()
{
	getTcpPos(m_InitialPos, m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::Translate(const double x, const double y, const double z)
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
	double matrix[16] = {};
	pose2Homogeneous(pose, matrix);//轴角转齐次变换矩阵

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix->DeepCopy(matrix);
	vtkMatrix->Transpose();
	Eigen::Vector3d moveMent = Eigen::Vector3d(x/1000, y/1000, z/1000);
	transform->SetMatrix(vtkMatrix);
	transform->Translate(moveMent.data());

	vtkSmartPointer<vtkMatrix4x4> setMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->Update();
	transform->GetMatrix(setMatrix);
	setMatrix->Transpose();
	homogeneous2Pose(setMatrix->GetData(), pose);
	double joints_final[7]{};
	inverse(pose, joints_final, nullptr, m_RobotIpAddress);

	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::Translate(const double axis[3], double length)
{
	Translate(axis[0]* length, axis[1]* length, axis[2]* length);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::Rotate(double x, double y, double z, double angle)
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
	double matrix[16] = {};
	pose2Homogeneous(pose, matrix);//轴角转齐次变换矩阵

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix->DeepCopy(matrix);
	vtkMatrix->Transpose();
	transform->SetMatrix(vtkMatrix);
	transform->RotateWXYZ(angle, x,y,z);

	vtkSmartPointer<vtkMatrix4x4> setMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->Update();
	transform->GetMatrix(setMatrix);
	setMatrix->Transpose();
	homogeneous2Pose(setMatrix->GetData(), pose);
	double joints_final[7]{};
	inverse(pose, joints_final, nullptr, m_RobotIpAddress);

	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::Rotate(const double axis[3], double angle)
{
	Rotate(axis[0], axis[1], axis[2], angle);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::WaitMove(const char* m_RobotIpAddress)
{
	QThread::msleep(20);
	while (true)
	{
		const char state = getRobotState(m_RobotIpAddress);
		if (state != 0)
		{
			break;
		}
		else
		{
			QThread::msleep(1);
			QApplication::processEvents();
		}
	}
	stop();
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RobotTransformInTCP(const double* matrix)
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
	double matrixArray[16] = {};
	pose2Homogeneous(pose, matrixArray);//轴角转齐次变换矩阵  获得baseToTCP
	vtkSmartPointer<vtkMatrix4x4> TBase2Tcp = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Tcp->DeepCopy(matrixArray);
	TBase2Tcp->Transpose();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PreMultiply();
	transform->SetMatrix(TBase2Tcp);
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	
	vtkMatrix->DeepCopy(matrix);
	vtkMatrix->SetElement(0,3,vtkMatrix->GetElement(0, 3) / 1000) ;
	vtkMatrix->SetElement(1, 3, vtkMatrix->GetElement(1, 3) / 1000);
	vtkMatrix->SetElement(2, 3, vtkMatrix->GetElement(2, 3) / 1000);
	transform->Concatenate(vtkMatrix);

	vtkSmartPointer<vtkMatrix4x4> newBase2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->GetMatrix(newBase2TCP);
	newBase2TCP->Transpose();
	homogeneous2Pose(newBase2TCP->GetData(), pose);
	double joints_final[7]{};
	inverse(pose, joints_final, nullptr, m_RobotIpAddress);

	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RobotTransformInBase(const double* matrix)
{
	double pose[6] = {};
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix->DeepCopy(matrix);
	vtkMatrix->SetElement(0, 3, vtkMatrix->GetElement(0, 3) / 1000);
	vtkMatrix->SetElement(1, 3, vtkMatrix->GetElement(1, 3) / 1000);
	vtkMatrix->SetElement(2, 3, vtkMatrix->GetElement(2, 3) / 1000);
	vtkMatrix->Transpose();
	homogeneous2Pose(vtkMatrix->GetData(), pose);
	double joints_final[7]{};
	inverse(pose, joints_final, nullptr, m_RobotIpAddress);
	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::SetVelocity(int vel)
{
	int ret = setVelocityPercentValue(vel, m_RobotIpAddress);
	return ret == 1 ? true : false;
}

int lancetAlgorithm::PKADianaAimHardwareDevice::GetVelocity()
{
	//TODO
	return 0;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::PKADianaAimHardwareDevice::GetRobotBase2End()
{
	double joints[7] = { 0.0 };
	int ret = getJointPos(joints, m_RobotIpAddress);
	double pose[6] = {};
	forward(joints, pose);
	double matrix[16] = {};
	pose2Homogeneous(pose, matrix);
	vtkSmartPointer<vtkMatrix4x4> base2End = vtkSmartPointer<vtkMatrix4x4>::New();
	base2End->DeepCopy(matrix);
	base2End->Transpose();
	return base2End;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::PKADianaAimHardwareDevice::GetFlange2TCP()
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	double matrix[16] = {};
	pose2Homogeneous(pose, matrix);
	vtkSmartPointer<vtkMatrix4x4> TFlange2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	TFlange2TCP->DeepCopy(matrix);
	TFlange2TCP->Transpose();
	return TFlange2TCP;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::PKADianaAimHardwareDevice::GetBase2TCP()
{
	vtkSmartPointer<vtkMatrix4x4> TBase2End = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TEnd2TCP = vtkSmartPointer<vtkMatrix4x4>::New();

	TBase2End->DeepCopy(GetRobotBase2End());
	TEnd2TCP->DeepCopy(GetFlange2TCP());
	vtkSmartPointer<vtkMatrix4x4> TBase2Tcp = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Tcp->DeepCopy(CalculationHelper::ConcatenateMatrixs(TBase2End, TEnd2TCP));

	return TBase2Tcp;
}

void lancetAlgorithm::PKADianaAimHardwareDevice::CapturePose(bool translationOnly)
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	double matrix[16] = {};
	pose2Homogeneous(pose, matrix);
	vtkSmartPointer<vtkMatrix4x4> TBase2Flange = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Flange->DeepCopy(matrix);
	TBase2Flange->Transpose();
	PrintDataHelper::CoutMatrix("TBase2Flange ", TBase2Flange);
	double x = TBase2Flange->GetElement(0, 3);
	double y = TBase2Flange->GetElement(1, 3);
	double z = TBase2Flange->GetElement(2, 3);
	TBase2Flange->SetElement(0, 3, x * 1000);
	TBase2Flange->SetElement(1, 3, y * 1000);
	TBase2Flange->SetElement(2, 3, z * 1000);
	PrintDataHelper::CoutMatrix("TBase2Flange ", TBase2Flange);

	double cameraToRoboEndArrayAvg[16];
	double cameraToBaseRFarrayAvg[16];
	AverageNavigationData(PKAData::m_TCamera2EndRF, 30, 20, cameraToRoboEndArrayAvg);
	AverageNavigationData(PKAData::m_TCamera2BaseRF, 30, 20, cameraToBaseRFarrayAvg);

	vtkNew<vtkMatrix4x4> vtkNdiToRoboEndMatrix;
	vtkNew<vtkMatrix4x4> vtkBaseRFToNdiMatrix;
	vtkNdiToRoboEndMatrix->DeepCopy(cameraToRoboEndArrayAvg);
	vtkBaseRFToNdiMatrix->DeepCopy(cameraToBaseRFarrayAvg);
	vtkBaseRFToNdiMatrix->Invert();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(cameraToRoboEndArrayAvg);
	tmpTransform->Concatenate(vtkBaseRFToNdiMatrix);
	tmpTransform->Update();
	auto vtkBaseRFtoRoboEndMatrix = tmpTransform->GetMatrix();
	PrintDataHelper::CoutMatrix("BaseRF2End: ", vtkBaseRFtoRoboEndMatrix);
	m_RobotRegistration.AddPoseWithVtkMatrix(TBase2Flange, vtkBaseRFtoRoboEndMatrix, translationOnly);
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::AverageNavigationData(vtkMatrix4x4* TCamera2RF, int timeInterval, int intervalNum, double matrixArray[16])
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
		//ndPtr->Update();
		vtkSmartPointer<vtkMatrix4x4> camera2RF = vtkSmartPointer<vtkMatrix4x4>::New();
		camera2RF->DeepCopy(TCamera2RF);

		tmp_x[0] += camera2RF->GetElement(0, 0);
		tmp_x[1] += camera2RF->GetElement(1, 0);
		tmp_x[2] += camera2RF->GetElement(2, 0);

		tmp_y[0] += camera2RF->GetElement(0, 1);
		tmp_y[1] += camera2RF->GetElement(1, 1);
		tmp_y[2] += camera2RF->GetElement(2, 1);

		tmp_translation[0] += camera2RF->GetElement(0, 3);
		tmp_translation[1] += camera2RF->GetElement(1, 3);
		tmp_translation[2] += camera2RF->GetElement(2, 3);

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
	return true;
}

int lancetAlgorithm::PKADianaAimHardwareDevice::CaptureRobot()
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
		//MITK_INFO << "OnRobotCapture finish: " << m_IndexOfRobotCapture;
		vtkNew<vtkMatrix4x4> TFlange2EndRF;
		m_RobotRegistration.GetTCPmatrix(TFlange2EndRF);
		PKAData::m_TFlange2EndRF->DeepCopy(TFlange2EndRF);
		vtkSmartPointer<vtkMatrix4x4> TBase2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
		m_RobotRegistration.GetRegistraionMatrix(TBase2BaseRF);
		PKAData::m_TBase2BaseRF->DeepCopy(TBase2BaseRF);

		double x = TFlange2EndRF->GetElement(0, 3);
		double y = TFlange2EndRF->GetElement(1, 3);
		double z = TFlange2EndRF->GetElement(2, 3);
		std::cout << "X: " << x << std::endl;
		std::cout << "Y: " << y << std::endl;
		std::cout << "Z: " << z << std::endl;

		std::cout << "Registration RMS: " << m_RobotRegistration.RMS() << std::endl;
	}
	return m_RobotRegistration.PoseCount();
}

int lancetAlgorithm::PKADianaAimHardwareDevice::ResetRobotRegistration()
{
	m_RobotRegistration.RemoveAllPose();
	return m_RobotRegistration.PoseCount();
	std::cout << "m_RobotRegistration Count: " << m_RobotRegistration.PoseCount() << std::endl;
}

void lancetAlgorithm::PKADianaAimHardwareDevice::GoToInitPos()
{
	double joints_final[7]{};
	inverse(m_InitialPos, joints_final, nullptr, m_RobotIpAddress);

	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

void lancetAlgorithm::PKADianaAimHardwareDevice::RobotAutoMove()
{
	// 获取当前机器人的姿态作为参考姿态
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	vtkSmartPointer<vtkMatrix4x4> TBase2Flange = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Flange->Transpose();
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

		// 将第一个点作为初始位置
		this->RecordIntialPos();
		
		// 平移操作
		this->Translate(translations[i - 1][0], translations[i-1][1],translations[i-1][2]);
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
		double matrixArray[16] = {};
		pose2Homogeneous(m_InitialPos, matrixArray);
		vtkSmartPointer<vtkMatrix4x4> homogeneous = vtkSmartPointer<vtkMatrix4x4>::New();
		homogeneous->DeepCopy(matrixArray);
		homogeneous->Transpose();
		tmpTrans->Concatenate(m_InitialPos);
		tmpTrans->Update();
		vtkSmartPointer<vtkMatrix4x4> robotTransform = vtkSmartPointer<vtkMatrix4x4>::New();
		tmpTrans->GetMatrix(robotTransform);
		this->RobotTransformInTCP(robotTransform->GetData());
	}
}

T_AimToolDataResult* lancetAlgorithm::PKADianaAimHardwareDevice::GetNewToolData()
{
	rlt = Aim_GetMarkerAndStatusFromHardware(m_AimHandle, I_ETHERNET, markerSt, statusSt);
	if (rlt == AIMOOE_NOT_REFLASH)
	{
		std::cout << "camera get data failed";
	}
	T_AimToolDataResult* mtoolsrlt = new T_AimToolDataResult;//新建一个值指，将指针清空用于存数据
	mtoolsrlt->next = NULL;
	mtoolsrlt->validflag = false;

	rlt = Aim_FindToolInfo(m_AimHandle, markerSt, mtoolsrlt, 0);//获取数据
	T_AimToolDataResult* prlt = mtoolsrlt;//将获取完数据的从mtoolsrlt给prlt指针

	return prlt;
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const char* Name, vtkMatrix4x4* matrix, QLabel* label)
{
	if (strcmp(ToolData->toolname, Name) == 0)
	{
		if (ToolData->validflag)
		{
			//获取相机数据
			Eigen::Vector3d camera2ToolTranslation;
			Eigen::Matrix3d camera2ToolRotation;
			camera2ToolTranslation[0] = ToolData->Tto[0];
			camera2ToolTranslation[1] = ToolData->Tto[1];
			camera2ToolTranslation[2] = ToolData->Tto[2];
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					camera2ToolRotation(i, j) = (double)ToolData->Rto[i][j];
				}
			}
			//拼接矩阵
			vtkNew<vtkMatrix4x4> m;
			m->DeepCopy(CalculationHelper::GetMatrixByRotationAndTranslation(camera2ToolRotation, camera2ToolTranslation));
			matrix->DeepCopy(m);

			//memcpy_s(aCamera2Tool, sizeof(double) * 16, matrix->GetData(), sizeof(double) * 16);
			if (label != nullptr)
			{
				QString str = "x:" + QString::number(ToolData->tooltip[0]) + "\n "
					+ "y:" + QString::number(ToolData->tooltip[1]) + " \n "
					+ "z:" + QString::number(ToolData->tooltip[2]);
				label->setText(str);
				label->setStyleSheet("QLabel { color : green; }");
			}
			return true;
		}
		else
		{
			if (label != nullptr)
			{
				QString str = "x:nan  y:nan   z:nan";
				label->setText(str);
				label->setStyleSheet("QLabel { color : red; }");
			}
			return false;
		}
	}
	return false;
}

Eigen::Vector3d lancetAlgorithm::PKADianaAimHardwareDevice::GetProbeTip()
{
	//if (this->IsProbeValid())
	//{
		//PrintDataHelper::CoutArray(m_ProbeTip, "Probe is Valid: ");
		return m_ProbeTip;
	//}
	//Eigen::Vector3d result;
	//result.Zero();
	//PrintDataHelper::CoutArray(result, "GetProbeTip: ");
	//return result;
}

Eigen::Vector3d lancetAlgorithm::PKADianaAimHardwareDevice::GetDrillEndInCamera()
{
	vtkSmartPointer<vtkMatrix4x4> TCamera2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
	Eigen::Vector3d drillEnd = CalculationHelper::TransformByMatrix(m_DrillEndPoints, PKAData::m_TCamera2Drill);
	return drillEnd;
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::IsFemurRFValid()
{
	return m_FemurRFValidity;
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::IsProbeValid()
{
	return m_ProbeValidity;
}

bool lancetAlgorithm::PKADianaAimHardwareDevice::IsTibiaRFValid()
{
	return m_TibiaRFValidity;
}
