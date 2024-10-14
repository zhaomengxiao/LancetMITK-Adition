#include "DianaAimHardwareService.h"

void lancetAlgorithm::DianaAimHardwareService::ConnectCamera()
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

void lancetAlgorithm::DianaAimHardwareService::UpdateCamera()
{
	QString position_text;
	std::vector<std::string> toolidarr;

	auto prlt = GetNewToolData();
	if (rlt == AIMOOE_OK)//判断是否采集成功
	{
		do
		{
			for (auto it = m_ReferenceMap.begin(); it != m_ReferenceMap.end(); ++it)
			{
				UpdateCameraToToolMatrix(prlt, it->first.c_str());
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

void lancetAlgorithm::DianaAimHardwareService::InitToolsName(std::vector<std::string> toolsName, std::vector<QLabel*>* labels)
{
	for (int i = 0; i < toolsName.size(); ++i)
	{
		if (m_ReferenceMap.count(toolsName[i]) > 0)
			continue;
		vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
		Eigen::Vector3d tip(0, 0, 0);
		m_ReferenceMap.insert(std::pair(toolsName[i], m));
		m_ToolTipMap.insert(std::pair(toolsName[i], tip));
		if (labels && i < labels->size())
		{
			m_LabelMap.insert(std::pair(toolsName[i], (*labels)[i]));
		}
	}
}

Eigen::Vector3d lancetAlgorithm::DianaAimHardwareService::GetTipByName(std::string aToolName)
{
	return m_ToolTipMap[aToolName];
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::DianaAimHardwareService::GetMatrixByName(std::string aToolName)
{
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();
	ret->DeepCopy(m_ReferenceMap[aToolName]);
	return ret;
}

void lancetAlgorithm::DianaAimHardwareService::ConnectRobot()
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

void lancetAlgorithm::DianaAimHardwareService::RobotPowerOn()
{
	releaseBrake();
}

void lancetAlgorithm::DianaAimHardwareService::RobotPowerOff()
{
	holdBrake();
}

void lancetAlgorithm::DianaAimHardwareService::SetTCP2Flange()
{
	double pose[6] = { 0,0,0,0,0,0 };
	setDefaultActiveTcpPose(pose, m_RobotIpAddress);

	getTcpPos(pose, m_RobotIpAddress);
	PrintDataHelper::CoutArray(pose, 6, "Current TCP pose is: ");
}

void lancetAlgorithm::DianaAimHardwareService::RecordIntialPos()
{
	getTcpPos(m_InitialPos, m_RobotIpAddress);
}

void lancetAlgorithm::DianaAimHardwareService::Translate(const double x, const double y, const double z)
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
	Eigen::Vector3d moveMent = Eigen::Vector3d(x / 1000, y / 1000, z / 1000);
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

void lancetAlgorithm::DianaAimHardwareService::Translate(const double axis[3], double length)
{
	Translate(axis[0] * length, axis[1] * length, axis[2] * length);
}

void lancetAlgorithm::DianaAimHardwareService::Rotate(double x, double y, double z, double angle)
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
	transform->RotateWXYZ(angle, x, y, z);

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

void lancetAlgorithm::DianaAimHardwareService::Rotate(const double axis[3], double angle)
{
	Rotate(axis[0], axis[1], axis[2], angle);
}

void lancetAlgorithm::DianaAimHardwareService::WaitMove(const char* m_RobotIpAddress)
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

void lancetAlgorithm::DianaAimHardwareService::RobotTransformInTCP(const double* matrix)
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
	vtkMatrix->SetElement(0, 3, vtkMatrix->GetElement(0, 3) / 1000);
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

void lancetAlgorithm::DianaAimHardwareService::RobotTransformInBase(const double* matrix)
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

void lancetAlgorithm::DianaAimHardwareService::SetTCP(vtkMatrix4x4* flangeToTarget)
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	PrintDataHelper::CoutArray(pose, 6, "Initial TCP pose is: ");
	vtkSmartPointer<vtkMatrix4x4> TFlange2TCPNew = vtkSmartPointer<vtkMatrix4x4>::New();
	TFlange2TCPNew->DeepCopy(flangeToTarget);
	PrintDataHelper::CoutMatrix("TFlange2TCPNew: ", TFlange2TCPNew);
	TFlange2TCPNew->SetElement(0, 3, TFlange2TCPNew->GetElement(0, 3) / 1000);
	TFlange2TCPNew->SetElement(1, 3, TFlange2TCPNew->GetElement(1, 3) / 1000);
	TFlange2TCPNew->SetElement(2, 3, TFlange2TCPNew->GetElement(2, 3) / 1000);
	TFlange2TCPNew->Transpose();
	PrintDataHelper::CoutMatrix("TFlange2TCPNew: ", TFlange2TCPNew);

	homogeneous2Pose(TFlange2TCPNew->GetData(), pose);
	PrintDataHelper::CoutArray(pose, 6, "SetTCP pose");
	//int ret = setDefaultActiveTcp(TFlange2TCPNew->GetData());
	int ret = setDefaultActiveTcpPose(pose);
	std::cout << "ret: " << ret << std::endl;
	std::string str = ret < 0 ? "Set TCP Failed" : "Set TCP Success";
	std::cout << str << std::endl;

	getTcpPos(pose, m_RobotIpAddress);
	PrintDataHelper::CoutArray(pose, 6, "Current TCP pose is: ");
}

bool lancetAlgorithm::DianaAimHardwareService::SetVelocity(int vel)
{
	int ret = setVelocityPercentValue(vel, m_RobotIpAddress);
	return ret == 1 ? true : false;
}

int lancetAlgorithm::DianaAimHardwareService::GetVelocity()
{
	//TODO
	return 0;
}

void lancetAlgorithm::DianaAimHardwareService::StopMove()
{
	stop();
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::DianaAimHardwareService::GetRobotBase2RobotEnd()
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
	base2End->SetElement(0, 3, base2End->GetElement(0, 3) * 1000);
	base2End->SetElement(1, 3, base2End->GetElement(1, 3) * 1000);
	base2End->SetElement(2, 3, base2End->GetElement(2, 3) * 1000);
	return base2End;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::DianaAimHardwareService::GetEnd2TCP()
{
	vtkSmartPointer<vtkMatrix4x4> TEnd2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	TEnd2Base->DeepCopy(this->GetRobotBase2RobotEnd());
	TEnd2Base->Invert();
	vtkSmartPointer<vtkMatrix4x4> TBase2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2TCP->DeepCopy(this->GetBase2TCP());

	vtkSmartPointer<vtkMatrix4x4> TEnd2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	TEnd2TCP->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TEnd2Base, TBase2TCP));
	return TEnd2TCP;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::DianaAimHardwareService::GetBase2TCP()
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	printf(" forward succeed! Pose: %f, %f, %f, %f, %f, %f\n ", pose[0], pose[1], pose[2], pose[3], pose[4], pose[5]);
	double matrixArray[16] = {};
	pose2Homogeneous(pose, matrixArray);//轴角转齐次变换矩阵  获得baseToTCP
	vtkSmartPointer<vtkMatrix4x4> TBase2Tcp = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Tcp->DeepCopy(matrixArray);
	TBase2Tcp->Transpose();
	TBase2Tcp->SetElement(0, 3, TBase2Tcp->GetElement(0, 3) * 1000);
	TBase2Tcp->SetElement(1, 3, TBase2Tcp->GetElement(1, 3) * 1000);
	TBase2Tcp->SetElement(2, 3, TBase2Tcp->GetElement(2, 3) * 1000);

	return TBase2Tcp;
}

double* lancetAlgorithm::DianaAimHardwareService::GetRobotImpeda()
{
	double* Impedaa = new double[7];
	double arrStiff[6] = {};
	double dblDamp = 0;

	if (getCartImpeda(arrStiff, &dblDamp) < 0)
	{
		std::cout << "Get CartImpeda Failed" << std::endl;
	}

	for (int i = 0; i < 6; i++)
	{
		Impedaa[i] = arrStiff[i];
	}
	Impedaa[6] = dblDamp;
	return Impedaa;
}

bool lancetAlgorithm::DianaAimHardwareService::SetRobotImpeda(double* aData)
{
	double arrstiff[6] = {};
	double dblDamp = 0;
	for (int i = 0; i < 6; ++i)
	{
		arrstiff[i] = aData[i];
	}
	dblDamp = aData[6];
	int ret = setCartImpeda(arrstiff, dblDamp);

	return ret < 0 ? false : true;
}

bool lancetAlgorithm::DianaAimHardwareService::CleanRobotErrorInfo()
{
	int ret = cleanErrorInfo();
	return ret < 0 ? false : true;
}

std::vector<double> lancetAlgorithm::DianaAimHardwareService::GetJointsElectricCurrent()
{
	double joints[7] = { 0.0 };

	int ret = getJointCurrent(joints);
	std::vector<double> electricCurrent;
	for (int i = 0; i < 7; ++i)
	{
		electricCurrent.push_back(joints[i]);
	}
	return electricCurrent;
}

bool lancetAlgorithm::DianaAimHardwareService::SetPositionMode()
{
	int ret = changeControlMode(T_MODE_POSITION);
	return ret < 0 ? false : true;
}

bool lancetAlgorithm::DianaAimHardwareService::SetJointImpendanceMode()
{
	int ret = changeControlMode(T_MODE_JOINT_IMPEDANCE);
	return ret < 0 ? false : true;
}

bool lancetAlgorithm::DianaAimHardwareService::SetCartImpendanceMode()
{
	int ret = changeControlMode(T_MODE_CART_IMPEDANCE);
	return ret < 0 ? false : true;
}

std::vector<std::vector<double>> lancetAlgorithm::DianaAimHardwareService::GetJointsPositionRange()
{
	double dblMinPos[7] = { 0 }, dblMaxPos[7] = { 0 };
	int ret = getJointsPositionRange(dblMinPos, dblMaxPos);
	std::vector<std::vector<double>> range;
	std::vector<double> minRange;
	std::vector<double> maxRange;
	for (int i = 0; i < 7; ++i)
	{
		minRange.push_back(dblMinPos[i]);
		maxRange.push_back(dblMaxPos[i]);
	}
	range.push_back(minRange);
	range.push_back(maxRange);
	return range;
}

std::vector<double> lancetAlgorithm::DianaAimHardwareService::GetJointAngles()
{
	double pose[6] = {};
	getTcpPos(pose, m_RobotIpAddress);
	double angles[7] = { 0.0 };

	inverse(pose, angles, nullptr, m_RobotIpAddress);
	std::vector<double> angleVec;
	for (int i = 0; i < 7; ++i)
	{
		angleVec.push_back(angles[i]);
	}
	return angleVec;
}

bool lancetAlgorithm::DianaAimHardwareService::SetJointAngles(double* angles)
{
	auto range = this->GetJointsPositionRange();
	for (int i = 0; i < range[0].size(); ++i)
	{
		if (angles[i] < range[0][i] || angles[i] > range[1][i])
		{
			std::cout << "Joint Angle is beyond the limit" << std::endl;
			return false;
		}
	}
	moveJToTarget(angles, 0.2, 0.4);
	return true;
}

void lancetAlgorithm::DianaAimHardwareService::CapturePose(bool translationOnly)
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
	auto tCamera2EndRF = m_ReferenceMap["RobotEndRF"];
	auto tCamera2BaseRF = m_ReferenceMap["RobotBaseRF"];
	AverageNavigationData(tCamera2EndRF, 30, 20, cameraToRoboEndArrayAvg);
	AverageNavigationData(tCamera2BaseRF, 30, 20, cameraToBaseRFarrayAvg);

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

bool lancetAlgorithm::DianaAimHardwareService::AverageNavigationData(vtkMatrix4x4* TCamera2RF, int timeInterval, int intervalNum, double matrixArray[16])
{
	// The frame rate of Vega ST is 60 Hz, so the timeInterval should be larger than 16.7 ms
	if (timeInterval <= 16) {
		std::cout << "Time interval should be larger than 16.7 ms for 60 Hz frame rate." << std::endl;
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

int lancetAlgorithm::DianaAimHardwareService::CaptureRobot()
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
		m_TFlange2EndRF->DeepCopy(TFlange2EndRF);
		vtkSmartPointer<vtkMatrix4x4> TBaseRF2Base = vtkSmartPointer<vtkMatrix4x4>::New();
		m_RobotRegistration.GetRegistraionMatrix(TBaseRF2Base);
		m_TBaseRF2Base->DeepCopy(TBaseRF2Base);

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

int lancetAlgorithm::DianaAimHardwareService::ResetRobotRegistration()
{
	m_RobotRegistration.RemoveAllPose();
	return m_RobotRegistration.PoseCount();
	std::cout << "m_RobotRegistration Count: " << m_RobotRegistration.PoseCount() << std::endl;
}

void lancetAlgorithm::DianaAimHardwareService::GoToInitPos()
{
	double joints_final[7]{};
	inverse(m_InitialPos, joints_final, nullptr, m_RobotIpAddress);

	moveJToTarget(joints_final, 0.2, 0.4);
	WaitMove(m_RobotIpAddress);
}

void lancetAlgorithm::DianaAimHardwareService::RobotAutoRegistration()
{
	this->SetTCP2Flange();
	this->RecordIntialPos();

	double moveMent[][3] = {
		{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0,0,1}
	};

	for (; m_RobotRegistration.PoseCount() < 10; )
	{
		int count = m_RobotRegistration.PoseCount();
		if (count > 0)
		{
			GoToInitPos();
			QThread::msleep(200);
			QApplication::processEvents();
		}
		if (m_RobotRegistration.PoseCount() < 5)
		{
			this->Translate(moveMent[count], 50);
			std::cout << "RobotMove " << count << " Down" << std::endl;
			QThread::msleep(200);
			QApplication::processEvents();

			CapturePose(true);
			std::cout << "Capture " << count << " Down" << std::endl;
			//translation
		}
		else
		{
			this->Rotate(moveMent[count - 5], 15);
			std::cout << "RobotMove " << count << " Down" << std::endl;
			QThread::msleep(200);
			QApplication::processEvents();

			CapturePose(false);
			std::cout << "Capture " << count << " Down" << std::endl;
			//rotation
		}
		QThread::msleep(200);
		QApplication::processEvents();
	}
	//Calculate Registration Data
	vtkNew<vtkMatrix4x4> TFlange2EndRF;
	m_RobotRegistration.GetTCPmatrix(TFlange2EndRF);
	m_TFlange2EndRF->DeepCopy(TFlange2EndRF);
	vtkSmartPointer<vtkMatrix4x4> TBaseRF2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	m_RobotRegistration.GetRegistraionMatrix(TBaseRF2Base);
	m_TBaseRF2Base->DeepCopy(TBaseRF2Base);
	PrintDataHelper::CoutMatrix("TFlange2EndRF: ", TFlange2EndRF);
	PrintDataHelper::CoutMatrix("TBaseRF2Base: ", TBaseRF2Base);

	std::cout << "Registration RMS: " << m_RobotRegistration.RMS() << std::endl;
}

T_AimToolDataResult* lancetAlgorithm::DianaAimHardwareService::GetNewToolData()
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

bool lancetAlgorithm::DianaAimHardwareService::UpdateCameraToToolMatrix(T_AimToolDataResult* ToolData, const std::string Name)
{
	if (strcmp(ToolData->toolname, Name.c_str()) == 0)
	{
		auto label = m_LabelMap[Name];
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
			auto matrix = m_ReferenceMap[Name];
			auto tip = m_ToolTipMap[Name];
			
			matrix->DeepCopy(CalculationHelper::GetMatrixByRotationAndTranslation(camera2ToolRotation, camera2ToolTranslation));
			
			tip[0] = ToolData->tooltip[0];
			tip[1] = ToolData->tooltip[1];
			tip[2] = ToolData->tooltip[2];
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
