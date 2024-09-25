#include "SystemPrecision.h"

lancetAlgorithm::SystemPrecision::SystemPrecision(mitk::DataStorage* aDataStorage, DianaAimHardwareService* aDianaAimHardwareService)
{
	m_dataStorage = aDataStorage;
	m_DianaAimHardwareService = aDianaAimHardwareService;
	m_LandmarkRegistrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_ICPRegistrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_FinalRegistrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_Marker2TCPMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_ICPPoints = mitk::PointSet::New();
	m_LandmarkPoints = mitk::PointSet::New();
	Reset();
	connect(m_DianaAimHardwareService, &lancetAlgorithm::DianaAimHardwareService::CameraUpdateClock, this, &SystemPrecision::Update);
}

void lancetAlgorithm::SystemPrecision::DisplayPrecisionTool()
{
	if (!m_IsDisplayPrecisionTool)
	{
		std::string path = std::string(getenv("USERPROFILE"));
		path += "\\Desktop\\PKAModelData\\SystemPrecision\\PrecisionTool.stl";
		LoadSingleMitkFile(m_dataStorage, path, m_PrecisionToolModelStr);
		m_IsDisplayPrecisionTool = !m_IsDisplayPrecisionTool;
	}
}

void lancetAlgorithm::SystemPrecision::DisplayProbe()
{
	if (!m_IsDisplayProbe)
	{
		std::string path = std::string(getenv("USERPROFILE"));
		path += "\\Desktop\\PKAModelData\\PKAProbe.stl";
		LoadSingleMitkFile(m_dataStorage, path, m_ProbeModelStr);
		m_IsDisplayProbe = !m_IsDisplayProbe;
	}
}

void lancetAlgorithm::SystemPrecision::LandmarkRegistration(mitk::PointSet* target)
{
	vtkSmartPointer<vtkPoints> pSource =
		vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> pTarget =
		vtkSmartPointer<vtkPoints>::New();

	for (int i = 0; i < target->GetSize(); i++)
	{
		auto pointVega = m_LandmarkPoints->GetPoint(i);
		pSource->InsertNextPoint(pointVega[0], pointVega[1], pointVega[2]);
		auto pointSTL = target->GetPoint(i);
		pTarget->InsertNextPoint(pointSTL[0], pointSTL[1], pointSTL[2]);
	}
	vtkSmartPointer<vtkLandmarkTransform> landmarkTransform =
		vtkSmartPointer<vtkLandmarkTransform>::New();

	landmarkTransform->SetSourceLandmarks(pSource);
	landmarkTransform->SetTargetLandmarks(pTarget);
	landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY);
	landmarkTransform->Update();
	
	m_LandmarkRegistrationMatrix->DeepCopy(landmarkTransform->GetMatrix());
	m_FinalRegistrationMatrix->DeepCopy(m_LandmarkRegistrationMatrix);
	PrintDataHelper::CoutMatrix("m_LandmarkRegistrationMatrix: ", m_LandmarkRegistrationMatrix);
}

void lancetAlgorithm::SystemPrecision::ICPRegistration(mitk::Surface* surface)
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	icpRegistrator->SetIcpPoints(m_ICPPoints);
	icpRegistrator->SetSurfaceSrc(surface);
	icpRegistrator->ComputeIcpResult();
	m_ICPRegistrationMatrix->DeepCopy(icpRegistrator->GetResult());
	//Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
	//tmpRegistrationResult.transposeInPlace();
	//
	//for (int row = 0; row < 4; ++row)
	//{
	//	for (int col = 0; col < 4; ++col)
	//	{
	//		m_ICPRegistrationMatrix->SetElement(row, col, tmpRegistrationResult(row, col));
	//	}
	//}
	PrintDataHelper::CoutMatrix("m_ICPRegistrationMatrix: ", m_ICPRegistrationMatrix);
	m_FinalRegistrationMatrix->DeepCopy(PostConcatenateMatrixs(m_LandmarkRegistrationMatrix, m_ICPRegistrationMatrix));  //box2LandMark * landMark2BoxRF
	PrintDataHelper::CoutMatrix("m_FinalRegistrationMatrix: ", m_FinalRegistrationMatrix);
}

void lancetAlgorithm::SystemPrecision::Update()
{
	if (m_IsDisplayPrecisionTool)
		UpdatePrecisionTestKit();

	if (m_IsDisplayProbe)
		UpdateProbe();
}

void lancetAlgorithm::SystemPrecision::UpdatePrecisionTestKit()
{
	vtkSmartPointer<vtkMatrix4x4> TCamera2PrecisionTool = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2PrecisionTool->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_RobotEndRF));

	vtkSmartPointer<vtkMatrix4x4> TPrecisionBoxRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TPrecisionBoxRF2Camera->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_VerificationBlockStr));
	TPrecisionBoxRF2Camera->Invert();
	
	vtkSmartPointer<vtkMatrix4x4> TBox2BoxRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TBox2BoxRF->DeepCopy(m_FinalRegistrationMatrix);
	auto geo = m_dataStorage->GetNamedNode(m_PrecisionToolModelStr)->GetData()->GetGeometry();
	vtkSmartPointer<vtkMatrix4x4> modelMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	if (m_IsUseImageReg)
	{
		modelMatrix->DeepCopy(PreConcatenateMatrixs(TBox2BoxRF, TPrecisionBoxRF2Camera, TCamera2PrecisionTool));
	}
	else
	{
		modelMatrix->DeepCopy(TCamera2PrecisionTool);
	}
	geo->SetIndexToWorldTransformByVtkMatrix(modelMatrix);
}

void lancetAlgorithm::SystemPrecision::UpdateProbe()
{
	vtkSmartPointer<vtkMatrix4x4> TCamera2Probe = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2Probe->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_ProbeModelStr));

	vtkSmartPointer<vtkMatrix4x4> TPrecisionBoxRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TPrecisionBoxRF2Camera->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_VerificationBlockStr));
	TPrecisionBoxRF2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TBox2BoxRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TBox2BoxRF->DeepCopy(m_FinalRegistrationMatrix);
	//TBox2BoxRF->Invert();
	auto geo = m_dataStorage->GetNamedNode(m_ProbeModelStr)->GetData()->GetGeometry();
	vtkSmartPointer<vtkMatrix4x4> modelMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	if (m_IsUseImageReg)
	{
		modelMatrix->DeepCopy(PreConcatenateMatrixs(TBox2BoxRF, TPrecisionBoxRF2Camera, TCamera2Probe));
	}
	else
	{
		modelMatrix->DeepCopy(TCamera2Probe);
	}
	geo->SetIndexToWorldTransformByVtkMatrix(modelMatrix);
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> lancetAlgorithm::SystemPrecision::AveragePointInBase(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB)
{
	vtkSmartPointer<vtkMatrix4x4> TBase2Box = vtkSmartPointer<vtkMatrix4x4>::New();
	
	Eigen::Vector3d retA(0, 0, 0);
	Eigen::Vector3d retB(0, 0, 0);
	for (int i = 0; i < 10; ++i)
	{
		TBase2Box->DeepCopy(CalculateBase2Image());
		retA += TransformByMatrix(aPointA, TBase2Box);
		retB += TransformByMatrix(aPointB, TBase2Box);
		QThread::msleep(50);
		QApplication::processEvents();
	}
	retA = retA / 10;
	retB = retB / 10;
	return std::pair(retA, retB);
}

std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> lancetAlgorithm::SystemPrecision::AveragePointInBase(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB, Eigen::Vector3d aPointC)
{
	vtkSmartPointer<vtkMatrix4x4> TBase2Box = vtkSmartPointer<vtkMatrix4x4>::New();

	Eigen::Vector3d retA(0, 0, 0);
	Eigen::Vector3d retB(0, 0, 0);
	Eigen::Vector3d retC(0, 0, 0);
	for (int i = 0; i < 10; ++i)
	{
		TBase2Box->DeepCopy(CalculateBase2Image());
		retA += TransformByMatrix(aPointA, TBase2Box);
		retB += TransformByMatrix(aPointB, TBase2Box);
		retC += TransformByMatrix(aPointC, TBase2Box);
		QThread::msleep(50);
		QApplication::processEvents();
	}
	retA = retA / 10;
	retB = retB / 10;
	retC = retC / 10;
	return std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d>(retA, retB, retC);
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> lancetAlgorithm::SystemPrecision::AveragePointInTCP(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB)
{
	vtkSmartPointer<vtkMatrix4x4> TBase2Box = vtkSmartPointer<vtkMatrix4x4>::New();

	Eigen::Vector3d retA(0, 0, 0);
	Eigen::Vector3d retB(0, 0, 0);
	for (int i = 0; i < 10; ++i)
	{
		TBase2Box->DeepCopy(CalculateTCP2Image());
		retA += TransformByMatrix(aPointA, TBase2Box);
		retB += TransformByMatrix(aPointB, TBase2Box);
		QThread::msleep(50);
		QApplication::processEvents();
	}
	retA = retA / 10;
	retB = retB / 10;
	return std::pair(retA, retB);
}


/// <summary>
/// 获得探针下的点  然后转换到 boxRF下 或者是 转换后的landmark下
/// </summary>
/// <param name="aPoints"></param>
/// <param name="aMatrix"></param>
void lancetAlgorithm::SystemPrecision::TransformPoints(mitk::PointSet* aPoints, vtkMatrix4x4* aMatrix)
{
	// 检查输入的 PointSet 和 变换矩阵是否为空
	if (!aPoints || !aMatrix)
	{
		std::cerr << "Invalid PointSet or Transform Matrix!" << std::endl;
		return;
	}

	// 遍历 PointSet 中的所有点
	for (unsigned int i = 0; i < aPoints->GetSize(); ++i)
	{
		// 获取原始点
		mitk::Point3D originalPoint = aPoints->GetPoint(i);

		// 定义一个 VTK 的坐标点
		double vtkPoint[4] = { originalPoint[0], originalPoint[1], originalPoint[2], 1.0 };

		// 变换坐标点
		double transformedVtkPoint[4];
		aMatrix->MultiplyPoint(vtkPoint, transformedVtkPoint);

		// 将变换后的坐标应用到 MITK 的 Point3D
		mitk::Point3D transformedPoint;
		transformedPoint[0] = transformedVtkPoint[0];
		transformedPoint[1] = transformedVtkPoint[1];
		transformedPoint[2] = transformedVtkPoint[2];

		// 更新 PointSet 中的点
		aPoints->SetPoint(i, transformedPoint);
	}
}


vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::SystemPrecision::CalculateBase2Image()
{
	vtkSmartPointer<vtkMatrix4x4> TBaseRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBaseRF2Camera->DeepCopy(m_DianaAimHardwareService->GetMatrixByName("RobotBaseRF"));
	TBaseRF2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TBase2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2BaseRF->DeepCopy(m_DianaAimHardwareService->GetBaseRF2BaseMatrix());
	TBase2BaseRF->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2BoxRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2BoxRF->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_VerificationBlockStr));

	vtkSmartPointer<vtkMatrix4x4> TBoxRF2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	TBoxRF2Image->DeepCopy(m_FinalRegistrationMatrix);
	vtkSmartPointer<vtkMatrix4x4> TBase2Image = vtkSmartPointer<vtkMatrix4x4>::New();

	TBase2Image->DeepCopy(PreConcatenateMatrixs(TBase2BaseRF, TBaseRF2Camera, TCamera2BoxRF, TBoxRF2Image));
	return TBase2Image;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::SystemPrecision::CalculateTCP2Image()
{
	vtkSmartPointer<vtkMatrix4x4> TCP2Marker = vtkSmartPointer<vtkMatrix4x4>::New();
	TCP2Marker->DeepCopy(m_Marker2TCPMatrix);
	TCP2Marker->Invert();

	vtkSmartPointer<vtkMatrix4x4> TMarker2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TMarker2Camera->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_PrecisionToolModelStr));
	TMarker2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2BoxRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2BoxRF->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_VerificationBlockStr));

	vtkSmartPointer<vtkMatrix4x4> TBoxRF2Box = vtkSmartPointer<vtkMatrix4x4>::New();
	TBoxRF2Box->DeepCopy(m_FinalRegistrationMatrix);
	TBoxRF2Box->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCP2Box = vtkSmartPointer<vtkMatrix4x4>::New();
	TCP2Box->DeepCopy(PreConcatenateMatrixs(TCP2Marker, TMarker2Camera, TCamera2BoxRF, TBoxRF2Box));
	return TCP2Box;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::SystemPrecision::CalculateBoxRF2Camera()
{
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkSmartPointer<vtkMatrix4x4> TBoxRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBoxRF2Camera->DeepCopy(m_DianaAimHardwareService->GetMatrixByName(m_VerificationBlockStr));
	TBoxRF2Camera->Invert();

	return TBoxRF2Camera;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::SystemPrecision::CalculateLandmartBox2Camera()
{
	vtkSmartPointer<vtkMatrix4x4> TBoxRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBoxRF2Camera->DeepCopy(CalculateBoxRF2Camera());
	vtkSmartPointer<vtkMatrix4x4> boxLandmark2BoxRF = vtkSmartPointer<vtkMatrix4x4>::New();
	boxLandmark2BoxRF->DeepCopy(m_LandmarkRegistrationMatrix);
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();

	ret->DeepCopy(PreConcatenateMatrixs(boxLandmark2BoxRF, TBoxRF2Camera));
	return ret;
}

Eigen::Vector3d lancetAlgorithm::SystemPrecision::TransformByMatrix(Eigen::Vector3d in, vtkMatrix4x4* matrix)
{
	Eigen::Vector4d v_homogeneous(in[0], in[1], in[2], 1.0);
	// 将 vtkMatrix4x4 转换为 Eigen::Matrix4d
	Eigen::Matrix4d M_eigen;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			M_eigen(i, j) = matrix->GetElement(i, j);
		}
	}

	// 执行矩阵乘法
	Eigen::Vector4d result_homogeneous = M_eigen * v_homogeneous;

	// 转换回 3D 向量
	Eigen::Vector3d result(result_homogeneous[0] / result_homogeneous[3],
		result_homogeneous[1] / result_homogeneous[3],
		result_homogeneous[2] / result_homogeneous[3]);

	double vec[4] = { in[0], in[1], in[2], 1.0 };
	vtkSmartPointer<vtkMatrix4x4> mMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	mMatrix->DeepCopy(matrix);
	auto out = mMatrix->MultiplyDoublePoint(vec);
	result = Eigen::Vector3d(out[0], out[1], out[2]);
	return result;
}

int lancetAlgorithm::SystemPrecision::CollectLandMarkPoints()
{
	Eigen::Vector3d point = m_DianaAimHardwareService->GetTipByName(m_ProbeModelStr);
	vtkSmartPointer<vtkMatrix4x4> TBoxRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBoxRF2Camera->DeepCopy(CalculateBoxRF2Camera());
	int count = m_LandmarkPoints->GetSize();
	mitk::PointSet::PointType pt;

	Eigen::Vector3d pointInBoxRF = TransformByMatrix(point, TBoxRF2Camera);
	PrintDataHelper::CoutArray(pointInBoxRF, "pointInBoxRF:");
	pt[0] = pointInBoxRF[0];
	pt[1] = pointInBoxRF[1];
	pt[2] = pointInBoxRF[2];
	m_LandmarkPoints->InsertPoint(count, pt);
	//std::cout << "m_LandmarkPoints->GetSize(): " << m_LandmarkPoints->GetSize() << std::endl;
	return m_LandmarkPoints->GetSize();
}

int lancetAlgorithm::SystemPrecision::CollectICPPoints()
{
	vtkSmartPointer<vtkMatrix4x4> TLandmark2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TLandmark2Camera->DeepCopy(CalculateLandmartBox2Camera());

	Eigen::Vector3d pointInCamera = m_DianaAimHardwareService->GetTipByName(m_ProbeModelStr);

	Eigen::Vector3d pointInLandMark = this->TransformByMatrix(pointInCamera, TLandmark2Camera);
	int count = m_ICPPoints->GetSize();
	mitk::PointSet::PointType pt;
	PrintDataHelper::CoutArray(pointInLandMark, "pointInLandMark:");
	pt[0] = pointInLandMark[0];
	pt[1] = pointInLandMark[1];
	pt[2] = pointInLandMark[2];
	m_ICPPoints->InsertPoint(count, pt);
	
	return m_ICPPoints->GetSize();
}

void lancetAlgorithm::SystemPrecision::Reset()
{
	m_LandmarkPoints->Clear();
	m_ICPPoints->Clear();
	m_LandmarkRegistrationMatrix->Identity();
	m_ICPRegistrationMatrix->Identity();
	m_FinalRegistrationMatrix->Identity();
	m_IsUseImageReg = false;
}

void lancetAlgorithm::SystemPrecision::UpdateDeviation()
{
	m_IsUseImageReg = true;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::SystemPrecision::GetBoxRF2BoxMatrix()
{
	vtkSmartPointer<vtkMatrix4x4> ret = vtkSmartPointer<vtkMatrix4x4>::New();
	ret->DeepCopy(m_FinalRegistrationMatrix);
	return ret;
}

void lancetAlgorithm::SystemPrecision::SetBoxRF2BoxMatrix(vtkMatrix4x4* matrix)
{
	m_FinalRegistrationMatrix->DeepCopy(matrix);
}

void lancetAlgorithm::SystemPrecision::SetGoLinePrecisionTCP(/*Eigen::Vector3d pointA, Eigen::Vector3d pointB*/)
{
	Eigen::Vector3d pointA(-52.508, -43, -23.8);
	Eigen::Vector3d pointB(2.492, -43, 28.2);

	Eigen::Vector3d xDirection = pointB - pointA;
	xDirection.normalize();

	Eigen::Vector3d zFlange(0.0, 0.0, 1.0);

	Eigen::Vector3d yDirection = zFlange.cross(xDirection);
	yDirection.normalize();

	Eigen::Vector3d zDirection = xDirection.cross(yDirection);

	vtkSmartPointer<vtkMatrix4x4> TMarker2PrecisionTCP = vtkSmartPointer<vtkMatrix4x4>::New();
	TMarker2PrecisionTCP->Identity();
	TMarker2PrecisionTCP->SetElement(0, 0, xDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 0, xDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 0, xDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 1, yDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 1, yDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 1, yDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 2, zDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 2, zDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 2, zDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 3, pointA.x());
	TMarker2PrecisionTCP->SetElement(1, 3, pointA.y());
	TMarker2PrecisionTCP->SetElement(2, 3, pointA.z());
	m_Marker2TCPMatrix->DeepCopy(TMarker2PrecisionTCP);

	vtkSmartPointer<vtkMatrix4x4> TFlange2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TEnd2EndRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TEnd2EndRF->DeepCopy(m_DianaAimHardwareService->GetEnd2EndRFMatrix());
	TFlange2TCP->DeepCopy(PreConcatenateMatrixs(TEnd2EndRF, m_Marker2TCPMatrix));
	m_DianaAimHardwareService->SetTCP(TFlange2TCP);
}

void lancetAlgorithm::SystemPrecision::SetGoPlanePrecisionTCP()
{
	Eigen::Vector3d pointA(-52.508, -43, -23.8);
	Eigen::Vector3d pointB(2.492, -43, 28.2);
	Eigen::Vector3d pointC(2.492, -43, -61.8);

	Eigen::Vector3d BA = pointA - pointB;
	Eigen::Vector3d CA = pointA - pointC;

	BA.normalize();
	Eigen::Vector3d xDirection = BA;

	Eigen::Vector3d z_flange(0, 0, 1);

	Eigen::Vector3d yDirection = xDirection.cross(z_flange);
	yDirection.normalize();

	Eigen::Vector3d zDirection = xDirection.cross(yDirection);

	vtkSmartPointer<vtkMatrix4x4> TMarker2PrecisionTCP = vtkSmartPointer<vtkMatrix4x4>::New();
	TMarker2PrecisionTCP->Identity();
	TMarker2PrecisionTCP->SetElement(0, 0, xDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 0, xDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 0, xDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 1, yDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 1, yDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 1, yDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 2, zDirection.x());
	TMarker2PrecisionTCP->SetElement(1, 2, zDirection.y());
	TMarker2PrecisionTCP->SetElement(2, 2, zDirection.z());

	TMarker2PrecisionTCP->SetElement(0, 3, pointA.x());
	TMarker2PrecisionTCP->SetElement(1, 3, pointA.y());
	TMarker2PrecisionTCP->SetElement(2, 3, pointA.z());

	vtkSmartPointer<vtkMatrix4x4> TFlange2TCP = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TEnd2EndRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TEnd2EndRF->DeepCopy(m_DianaAimHardwareService->GetEnd2EndRFMatrix());
	TFlange2TCP->DeepCopy(PreConcatenateMatrixs(TEnd2EndRF, m_Marker2TCPMatrix));
	m_DianaAimHardwareService->SetTCP(TFlange2TCP);
}

void lancetAlgorithm::SystemPrecision::GoLine(Eigen::Vector3d point0, Eigen::Vector3d point1)
{
	auto points = AveragePointInBase(point0, point1);
	
	Eigen::Vector3d targetZUnderBase = points.second - points.first;
	targetZUnderBase.normalize(); 

	vtkSmartPointer<vtkMatrix4x4> TBase2Flange = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2Flange->DeepCopy(m_DianaAimHardwareService->GetRobotBase2RobotEnd());
	auto TBase2FlangeArray = TBase2Flange->GetData();
	Eigen::Vector3d currentZUnderBase(TBase2FlangeArray[2], TBase2FlangeArray[6], TBase2FlangeArray[10]);
	currentZUnderBase.normalize();

	Eigen::Vector3d  rotationAxis;
	rotationAxis = currentZUnderBase.cross(targetZUnderBase);

	double rotationAngle;//定义旋转角
	if (currentZUnderBase.dot(targetZUnderBase) > 0) //如果向量的内积大于0，cos大于0（锐角）
	{
		rotationAngle = 180 * asin(rotationAxis.norm()) / 3.1415926;//求向量的模长（sin[rotationAngle]）,再取反三角
	}
	else //如果向量的内积小于0，cos小于0（钝角）
	{
		rotationAngle = 180 - 180 * asin(rotationAxis.norm()) / 3.1415926;
	}

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	transform->PostMultiply();
	transform->Identity();
	transform->SetMatrix(TBase2Flange);
	// T_TCPtoBase
	transform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);//旋转角度，和旋转向量
	transform->Update();

	vtkSmartPointer<vtkMatrix4x4> pointMatrixInBase = vtkSmartPointer<vtkMatrix4x4>::New();
	pointMatrixInBase = transform->GetMatrix();
	pointMatrixInBase->SetElement(0, 3, points.first.x());
	pointMatrixInBase->SetElement(1, 3, points.first.y());
	pointMatrixInBase->SetElement(2, 3, points.first.z());

	m_DianaAimHardwareService->RobotTransformInBase(pointMatrixInBase->GetData());
}

void lancetAlgorithm::SystemPrecision::GoPlane(Eigen::Vector3d point0, Eigen::Vector3d point1, Eigen::Vector3d point2)
{
	auto points = AveragePointInBase(point0, point1, point2);

	Eigen::Vector3d tmpPoint0 = std::get<0>(points);
	Eigen::Vector3d tmpPoint1 = std::get<1>(points);
	Eigen::Vector3d tmpPoint2 = std::get<2>(points);
	PrintDataHelper::CoutArray(tmpPoint0, "GoPlane tmpPoint0: ");
	PrintDataHelper::CoutArray(tmpPoint1, "GoPlane tmpPoint1: ");
	PrintDataHelper::CoutArray(tmpPoint2, "GoPlane tmpPoint2: ");
	Eigen::Vector3d xDirection = tmpPoint2 - tmpPoint0;
	xDirection.normalize();
	Eigen::Vector3d tmpY = tmpPoint0 - tmpPoint1;
	tmpY.normalize();

	Eigen::Vector3d zDirection = xDirection.cross(tmpY);
	zDirection.normalize();

	Eigen::Vector3d yDirection = zDirection.cross(xDirection);

	vtkSmartPointer<vtkMatrix4x4> targetInBase = vtkSmartPointer<vtkMatrix4x4>::New();
	targetInBase->Identity();
	targetInBase->SetElement(0, 0, xDirection.x());
	targetInBase->SetElement(1, 0, xDirection.y());
	targetInBase->SetElement(2, 0, xDirection.z());

	targetInBase->SetElement(0, 1, yDirection.x());
	targetInBase->SetElement(1, 1, yDirection.y());
	targetInBase->SetElement(2, 1, yDirection.z());

	targetInBase->SetElement(0, 2, zDirection.x());
	targetInBase->SetElement(1, 2, zDirection.y());
	targetInBase->SetElement(2, 2, zDirection.z());

	targetInBase->SetElement(0, 3, tmpPoint0.x());
	targetInBase->SetElement(1, 3, tmpPoint0.y());
	targetInBase->SetElement(2, 3, tmpPoint0.z());

	PrintDataHelper::CoutMatrix("GoPlane targetInBase: ", targetInBase);

	m_DianaAimHardwareService->RobotTransformInBase(targetInBase->GetData());
}

void lancetAlgorithm::SystemPrecision::LoadSingleMitkFile(mitk::DataStorage* dataStorage, std::string filePath, std::string nodeName)
{
	auto Node = dataStorage->GetNamedNode(nodeName);
	if (!Node)
	{
		mitk::DataNode::Pointer node = mitk::DataNode::New();
		node->SetData(mitk::IOUtil::Load(filePath)[0]);

		node->SetName(nodeName);

		dataStorage->Add(node);
	}
	else
	{
		Node->SetData(mitk::IOUtil::Load(filePath)[0]);
	}
}
