#include "LancetHansRobot.h"

LancetHansRobot::LancetHansRobot()
{
	m_InitialPos = vtkSmartPointer<vtkMatrix4x4>::New();
	m_FlangeToTCP = vtkSmartPointer<vtkMatrix4x4>::New();
}

void LancetHansRobot::Connect()
{
	this->SetRobotIpAddress("192.168.0.10");
	unsigned int boxID = 0;
	unsigned short nPort = 10003;
	int nRet = HRIF_Connect(boxID, m_IpAddress, nPort);
}

void LancetHansRobot::PowerOn()
{
	int nGrpEnable = HRIF_GrpEnable(0, 0);
}

void LancetHansRobot::PowerOff()
{
	int nGrpDisable = HRIF_GrpDisable(0, 0);
}

void LancetHansRobot::Translate(double x, double y, double z)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->DeepCopy(GetBaseToTCP());

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);

	transform->Translate(x, y, z);

	vtkSmartPointer<vtkMatrix4x4> retMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->GetMatrix(retMatrix);

	RobotTransformInBase(retMatrix->GetData());
}

void LancetHansRobot::Translate(double* aDirection, double aLength)
{
	this->Translate(aDirection[0] * aLength, aDirection[1] * aLength, aDirection[2] * aLength);
}

void LancetHansRobot::Rotate(double* aDirection, double aAngle)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->DeepCopy(GetBaseToTCP());

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);

	transform->RotateWXYZ(aAngle, aDirection);

	vtkSmartPointer<vtkMatrix4x4> retMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->GetMatrix(retMatrix);

	RobotTransformInBase(retMatrix->GetData());
}

void LancetHansRobot::RecordInitialPos()
{
	m_InitialPos->DeepCopy(GetBaseToTCP());
}

void LancetHansRobot::GoToInitialPos()
{
	auto rotation = this->GetEulerByMatrix(m_InitialPos);
	auto translation = this->GetTranslationPartByMatrix(m_InitialPos);

	// 定义关节目标位置
	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;
	nIsUseJoint = 0;
	// 执行路点运动
	int nRet = HRIF_MoveJ(0, 0, translation[0], translation[1], translation[2], rotation[0], rotation[1], rotation[2], dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}

void LancetHansRobot::SetTCPToFlange()
{
	double dTcp_X = 0;
	double dTcp_Y = 0;
	double dTcp_Z = 0;
	double dTcp_Rx = 0;
	double dTcp_Ry = 0;
	double dTcp_Rz = 0;
	int nRet = HRIF_SetTCP(0, 0, dTcp_X, dTcp_Y, dTcp_Z, dTcp_Rx, dTcp_Ry, dTcp_Rz);
}

bool LancetHansRobot::SetTCP(vtkMatrix4x4* aMatrix)
{
	auto euler = this->GetEulerByMatrix(aMatrix);
	auto trans = this->GetTranslationPartByMatrix(aMatrix);

	m_FlangeToTCP->DeepCopy(aMatrix);

	int nRet = HRIF_SetTCP(0, 0, trans[0], trans[1], trans[2], euler[3], euler[4], euler[5]);
	//set tcp to robot

	if (nRet == 0) {
		std::cout << "line  TCP Set succeed" << std::endl;
		return true;
	}
	else {
		std::cout << "line TCP Set failed" << std::endl;
		return false;
	}
}

std::vector<double> LancetHansRobot::GetJointAngles()
{
	// 定义关节实际位置
	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;
	// 读取关节实际位置
	int nRet = HRIF_ReadActJointPos(0, 0, dJ1, dJ2, dJ3, dJ4, dJ5, dJ6);
	std::vector<double> ret = { dJ1, dJ2, dJ3, dJ4, dJ5, dJ6 };
	
	return ret;
}

void LancetHansRobot::SetJointAngles(double* aJointAngles)
{
	// 定义关节目标位置
	double dX = 0; double dY = 0; double dZ = 0;
	double dRX = 0; double dRY = 0; double dRZ = 0;
	nIsUseJoint = 1;
	// 执行路点运动
	int nRet = HRIF_MoveJ(0, 0, dX, dY, dZ, dRX, dRY, dRZ,
		aJointAngles[0], aJointAngles[1], aJointAngles[2], aJointAngles[3], aJointAngles[4], aJointAngles[5], sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}

vtkSmartPointer<vtkMatrix4x4> LancetHansRobot::GetBaseToTCP()
{
	double dX = 0; double dY = 0; double dZ = 0;
	double dRx = 0; double dRy = 0; double dRz = 0;
	// 基座坐标转换为用户坐标
	int nRet = HRIF_ReadActTcpPos(0, 0, dX, dY, dZ, dRx, dRy, dRz);

	Eigen::Matrix3d rotation = GetRotationMatrixByEuler(dX, dY, dZ);
	Eigen::Vector3d translation(dRx, dRy, dRz);
	return GetMatrixByRotationAndTranslation(rotation, translation);
}

vtkSmartPointer<vtkMatrix4x4> LancetHansRobot::GetFlangeToTCP()
{
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();
	ret->DeepCopy(m_FlangeToTCP);
	return ret;
}

vtkSmartPointer<vtkMatrix4x4> LancetHansRobot::GetBaseToFlange()
{
	auto TBase2TCP = this->GetBaseToTCP();
	auto TTCP2Flange = this->GetFlangeToTCP();
	TTCP2Flange->Invert();
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Multiply4x4(TBase2TCP, TTCP2Flange, ret);
	return ret;
}

void LancetHansRobot::RobotTransformInBase(double* aMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> baseToTarget = vtkSmartPointer<vtkMatrix4x4>::New();
	baseToTarget->DeepCopy(aMatrix);
	auto euler = this->GetEulerByMatrix(baseToTarget);
	auto translation = this->GetTranslationPartByMatrix(baseToTarget);

	double dJ1 = 0; double dJ2 = 0; double dJ3 = 0;
	double dJ4 = 0; double dJ5 = 0; double dJ6 = 0;

	// 执行路点运动
	nIsUseJoint = 0;
	int nRet = HRIF_MoveJ(0, 0, translation[0], translation[1], translation[2], euler[0], euler[1], euler[2], dJ1, dJ2, dJ3, dJ4, dJ5, dJ6, sTcpName, sUcsName,
		dVelocity, dAcc, dRadius, nIsUseJoint, nIsSeek, nIOBit, nIOState, strCmdID);
}

void LancetHansRobot::RobotTransformInTCP(double* aMatrix)
{
	auto baseToTCP = this->GetBaseToTCP();

	vtkSmartPointer<vtkMatrix4x4> tcpToTarget = vtkSmartPointer<vtkMatrix4x4>::New();
	tcpToTarget->DeepCopy(aMatrix);

	vtkSmartPointer<vtkMatrix4x4> baseToTarget = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkMatrix4x4::Multiply4x4(baseToTCP, tcpToTarget, baseToTarget);

	RobotTransformInBase(baseToTarget->GetData());
}

std::vector<std::vector<double>> LancetHansRobot::GetJointAngleLimits()
{
	return std::vector<std::vector<double>>();
}

Eigen::Matrix3d LancetHansRobot::GetRotationMatrixByEuler(double rx, double ry, double rz)
{
	Eigen::Matrix3d rotationMatrix;

	Eigen::AngleAxisd rotationZ(rz, Eigen::Vector3d::UnitZ()); // yaw
	Eigen::AngleAxisd rotationY(ry, Eigen::Vector3d::UnitY()); // pitch
	Eigen::AngleAxisd rotationX(rx, Eigen::Vector3d::UnitX());
	rotationMatrix = rotationZ * rotationY * rotationX;
	return rotationMatrix;
}

vtkSmartPointer<vtkMatrix4x4> LancetHansRobot::GetMatrixByRotationAndTranslation(Eigen::Matrix3d aRotation, Eigen::Vector3d aTranslation)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->Identity();
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			matrix->SetElement(row, col, aRotation(row, col));
		}
	}
	matrix->SetElement(0, 3, aTranslation[0]);
	matrix->SetElement(1, 3, aTranslation[1]);
	matrix->SetElement(2, 3, aTranslation[2]);
	return matrix;
}

Eigen::Vector3d LancetHansRobot::GetEulerByMatrix(vtkMatrix4x4* m)
{
	return this->CalculateZYXEulerByRotation(this->GetRotationPartByMatrix(m));
}

Eigen::Matrix3d LancetHansRobot::GetRotationPartByMatrix(vtkMatrix4x4* m)
{
	auto data = m->GetData();
	Eigen::Matrix3d res;
	res << data[0], data[1], data[2],
		data[4], data[5], data[6],
		data[8], data[9], data[10];
	return res;
}

Eigen::Vector3d LancetHansRobot::GetTranslationPartByMatrix(vtkMatrix4x4* m)
{
	double x = m->GetElement(0, 3);
	double y = m->GetElement(1, 3);
	double z = m->GetElement(2, 3);
	return Eigen::Vector3d(x, y, z);
}

Eigen::Vector3d LancetHansRobot::CalculateZYXEulerByRotation(Eigen::Matrix3d m)
{
	Eigen::Vector3d eulerAngle = m.eulerAngles(2, 1, 0);

	Eigen::Vector3d ret;

	// 将弧度转换为角度并赋值
	ret[0] = 180 * eulerAngle[2] / vtkMath::Pi(); // Z (yaw)
	ret[1] = 180 * eulerAngle[1] / vtkMath::Pi(); // Y (pitch)
	ret[2] = 180 * eulerAngle[0] / vtkMath::Pi(); // X (roll)
	return ret;
}
