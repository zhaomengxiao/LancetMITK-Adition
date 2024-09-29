#include "IntraOsteotomy.h"

lancetAlgorithm::IntraOsteotomy::IntraOsteotomy(mitk::DataStorage* dataStorage, PKADianaAimHardwareDevice* pkaDianaAimHardwareDevice)
{
	m_DataStorage = dataStorage;
	m_PKADianaAimHardwareDevice = pkaDianaAimHardwareDevice;
	m_LastRoundMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateTDrillEnd2FemurImage(/*Eigen::Vector3d pointInImage*/)
{
	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	//�ӻ�е�۳���õ�ǰĩ�˵�λ��
	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2Base = vtkSmartPointer<vtkMatrix4x4>::New();  //ĩ��tcp��λ��
	TDrillEnd2Base->DeepCopy(CalculateTBase2DrillEnd());
	TDrillEnd2Base->Invert();
	
	transform->PreMultiply();
	vtkSmartPointer<vtkMatrix4x4> TBase2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2BaseRF->DeepCopy(PKAData::m_TBaseRF2Base);
	TBase2BaseRF->Invert();
	vtkSmartPointer<vtkMatrix4x4> TBaseRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBaseRF2Camera->DeepCopy(PKAData::m_TCamera2BaseRF);
	TBaseRF2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2FemurRF->DeepCopy(PKAData::m_TCamera2FemurRF);

	vtkSmartPointer<vtkMatrix4x4> TFemurRF2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurRF2FemurImage->DeepCopy(PKAData::m_TFemurRF2FemurImage);
	transform->SetMatrix(TDrillEnd2Base);
	transform->Concatenate(TBase2BaseRF);
	transform->Concatenate(TBaseRF2Camera);
	transform->Concatenate(TCamera2FemurRF);
	transform->Concatenate(TFemurRF2FemurImage);

	transform->GetMatrix(TDrillEnd2FemurImage);
	return TDrillEnd2FemurImage;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateTDrillEnd2FemurImage2()
{
	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrillEnd2Drill->Identity();
	TDrillEnd2Drill->SetElement(0, 3, 230.492);
	TDrillEnd2Drill->SetElement(1, 3, -85);
	TDrillEnd2Drill->SetElement(2, 3, -49.800);
	TDrillEnd2Drill->Invert();
	vtkSmartPointer<vtkMatrix4x4> TDrill2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrill2Camera->DeepCopy(PKAData::m_TCamera2Drill);
	TDrill2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2FemurRF->DeepCopy(PKAData::m_TCamera2FemurRF);

	vtkSmartPointer<vtkMatrix4x4> TFemurRF2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurRF2FemurImage->DeepCopy(PKAData::m_TFemurRF2FemurImage);

	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrillEnd2FemurImage->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TDrillEnd2Drill, TDrill2Camera, TCamera2FemurRF, TFemurRF2FemurImage));

	return TDrillEnd2FemurImage;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateTDrillEnd2TibiaImage2()
{
	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrillEnd2Drill->Identity();
	TDrillEnd2Drill->SetElement(0, 3, 230.492);
	TDrillEnd2Drill->SetElement(1, 3, -85);
	TDrillEnd2Drill->SetElement(2, 3, -49.800);
	TDrillEnd2Drill->Invert();
	vtkSmartPointer<vtkMatrix4x4> TDrill2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrill2Camera->DeepCopy(PKAData::m_TCamera2Drill);
	TDrill2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2TibiaRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2TibiaRF->DeepCopy(PKAData::m_TCamera2TibiaRF);

	vtkSmartPointer<vtkMatrix4x4> TTibia2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TTibia2TibiaImage->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);

	vtkSmartPointer<vtkMatrix4x4> TDrillEnd2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TDrillEnd2TibiaImage->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TDrillEnd2Drill, TDrill2Camera, TCamera2TibiaRF, TTibia2TibiaImage));
	
	return TDrillEnd2TibiaImage;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateTBase2DrillEnd()
{
	vtkSmartPointer<vtkMatrix4x4> TBase2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TBase2BaseRF->DeepCopy(PKAData::m_TBaseRF2Base);
	TBase2BaseRF->Invert();
	vtkSmartPointer<vtkMatrix4x4> TBaseRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TBaseRF2Camera->DeepCopy(PKAData::m_TCamera2BaseRF);
	TBaseRF2Camera->Invert();

	//��е��ĩ�˲ο����� ��е����׼�ο�����ͬ
	vtkSmartPointer<vtkMatrix4x4> TCamera2EndRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2EndRF->DeepCopy(PKAData::m_TCamera2Drill);

	vtkSmartPointer<vtkMatrix4x4> TEndRF2DrillEnd = vtkSmartPointer<vtkMatrix4x4>::New();
	TEndRF2DrillEnd->Identity();
	TEndRF2DrillEnd->SetElement(0, 3, 230.492);
	TEndRF2DrillEnd->SetElement(1, 3, -85);
	TEndRF2DrillEnd->SetElement(2, 3, -49.800);
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	transform->PreMultiply();
	transform->SetMatrix(TBase2BaseRF);
	transform->Concatenate(TBaseRF2Camera);
	transform->Concatenate(TCamera2EndRF);
	transform->Concatenate(TEndRF2DrillEnd);
	transform->Update();
	
	vtkSmartPointer<vtkMatrix4x4> TBase2DrillEnd = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->GetMatrix(TBase2DrillEnd);
	return TBase2DrillEnd;
}

/// <summary>
/// 
/// </summary>
/// <param name="pointInPlane">�ɹ�ͼ���еĵ��λ�� </param>
/// <param name="planeNormal">�ɹ�ͼ����ƽ��ķ�����</param>
vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateDrillEndHorizontalPlane(Eigen::Vector3d pointPlaneInImage, Eigen::Vector3d planeNormalInImage)
{
	vtkSmartPointer<vtkMatrix4x4> tDrillEnd2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	tDrillEnd2FemurImage->DeepCopy(this->CalculateTDrillEnd2FemurImage());

	Eigen::Vector3d pointPlaneInDrill = CalculationHelper::TransformByMatrix(pointPlaneInImage, tDrillEnd2FemurImage);
	Eigen::Vector3d planeNormalInDrill = CalculationHelper::TransformByMatrix(planeNormalInImage, tDrillEnd2FemurImage);

	planeNormalInDrill.normalize();

	//��õ�ǰtcp�� xyz rx ry rz
	vtkSmartPointer<vtkMatrix4x4> TBase2DrillEnd = vtkSmartPointer<vtkMatrix4x4>::New();

	Eigen::Vector3d originZAxis(0, 0, 1);
	Eigen::Vector3d rotationAxis = originZAxis.cross(planeNormalInDrill);
	double angle = std::acos(originZAxis.dot(planeNormalInDrill));

	if (rotationAxis.norm() < 1e-6)
	{
		return vtkSmartPointer<vtkMatrix4x4>::New();
	}

	rotationAxis.normalize();

	Eigen::AngleAxisd rotation(angle, rotationAxis);
	Eigen::Matrix3d rotationMatrix = rotation.toRotationMatrix();
	pointPlaneInDrill[0] = pointPlaneInDrill[0] - 20;
	pointPlaneInDrill[1] = pointPlaneInDrill[1] - 20;
	Eigen::Affine3d transform = Eigen::Affine3d::Identity();
	transform.linear() = rotationMatrix;
	transform.translation() = pointPlaneInDrill;
	Eigen::Matrix4d m = transform.matrix();
	return CalculationHelper::ConvertEigen2vtkMatrix(m);
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateFemurDrillEndVerticalPlane(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal)
{
	vtkSmartPointer<vtkMatrix4x4> tDrillEnd2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	tDrillEnd2FemurImage->DeepCopy(this->CalculateTDrillEnd2FemurImage2());
	PrintDataHelper::CoutMatrix("tDrillEnd2FemurImage:", tDrillEnd2FemurImage);
	Eigen::Vector3d pointPlaneInDrill = CalculationHelper::TransformByMatrix(pointInPlane, tDrillEnd2FemurImage);
	Eigen::Vector3d planeNormalInDrill = CalculationHelper::CalculateInCoordinate(planeNormal, tDrillEnd2FemurImage);
	planeNormalInDrill.normalize();
	
	Eigen::Vector3d ZAxis(0, 0, 1);
	Eigen::Vector3d YAxis = ZAxis.cross(planeNormalInDrill);
	Eigen::Vector3d ZNewAxis = planeNormalInDrill.cross(YAxis);
	YAxis.normalize();
	ZNewAxis.normalize();
	planeNormalInDrill.normalize();

	Eigen::Vector3d t = pointPlaneInDrill - 50 * planeNormalInDrill;

	vtkSmartPointer<vtkMatrix4x4> tcpToTarget = vtkSmartPointer<vtkMatrix4x4>::New();
	CalculationHelper::SetMatrixXAxis(planeNormalInDrill, tcpToTarget);
	CalculationHelper::SetMatrixYAxis(YAxis, tcpToTarget);
	CalculationHelper::SetMatrixZAxis(ZNewAxis, tcpToTarget);
	CalculationHelper::SetMatrixTranslation(t, tcpToTarget);

	PrintDataHelper::CoutMatrix("tcpToTarget", tcpToTarget);
	return tcpToTarget;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateTibiaDrillEndVerticalPlane(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal)
{
	vtkSmartPointer<vtkMatrix4x4> tDrillEnd2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
	tDrillEnd2TibiaImage->DeepCopy(this->CalculateTDrillEnd2TibiaImage2());
	PrintDataHelper::CoutMatrix("tDrillEnd2TibiaImage:", tDrillEnd2TibiaImage);
	Eigen::Vector3d pointPlaneInDrill = CalculationHelper::TransformByMatrix(pointInPlane, tDrillEnd2TibiaImage);
	Eigen::Vector3d planeNormalInDrill = CalculationHelper::CalculateInCoordinate(planeNormal, tDrillEnd2TibiaImage);
	planeNormalInDrill.normalize();
	
	Eigen::Vector3d ZAxis(0, 0, 1);
	Eigen::Vector3d YAxis = ZAxis.cross(planeNormalInDrill);
	Eigen::Vector3d ZNewAxis = planeNormalInDrill.cross(YAxis);
	YAxis.normalize();
	ZNewAxis.normalize();
	planeNormalInDrill.normalize();

	Eigen::Vector3d t = pointPlaneInDrill - 50 * planeNormalInDrill;

	vtkSmartPointer<vtkMatrix4x4> tcpToTarget = vtkSmartPointer<vtkMatrix4x4>::New();
	CalculationHelper::SetMatrixXAxis(planeNormalInDrill, tcpToTarget);
	CalculationHelper::SetMatrixYAxis(YAxis, tcpToTarget);
	CalculationHelper::SetMatrixZAxis(ZNewAxis, tcpToTarget);
	CalculationHelper::SetMatrixTranslation(t, tcpToTarget);

	PrintDataHelper::CoutMatrix("tcpToTarget", tcpToTarget);
	return tcpToTarget;
}

/// <summary>
/// ����ɹ�ͼ���ϵĵ�ͷ�����
/// </summary>
/// <param name="pointInPlane"></param>
/// <param name="planeNormal"></param>
/// <returns></returns>
vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::IntraOsteotomy::CalculateFemurDrillEndTipPos( 
	Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal)
{
	// ����������һ���Եõ� X �᷽��
	Eigen::Vector3d xAxis = planeNormal.normalized();

	// ����ֲ�����ϵԭ�㣬�������������ƶ� 50 ����
	Eigen::Vector3d origin = pointInPlane - xAxis * 50.0; // 50 ���� = 0.05 ��

	// ȷ�� Y �᷽��ѡ��һ���뷨������ƽ�е��������в��
	Eigen::Vector3d arbitraryVector(0.0, 1.0, 0.0); // ����һ���������� Y ��
	if (fabs(xAxis.dot(arbitraryVector)) > 0.99) {
		// ����ٶ������뷨�����ӽ�ƽ�У�ѡ����һ������
		arbitraryVector = Eigen::Vector3d(1.0, 0.0, 0.0);
	}

	// ���� Y �᣺ѡ��һ����ƽ��������˵õ�һ����ѡ Z ��
	Eigen::Vector3d zAxis = xAxis.cross(arbitraryVector).normalized();

	// ���� Z ��
	Eigen::Vector3d yAxis = zAxis.cross(xAxis).normalized(); // ͨ���� X ���˵õ����� Y ��

	// ��װ�ֲ�����ϵ����ת����ͱ任����
	vtkSmartPointer<vtkMatrix4x4> localCoordinateSystem = vtkSmartPointer<vtkMatrix4x4>::New();
	localCoordinateSystem->Identity(); // ��ʼ��Ϊ��λ����

	// ������ת���󲿷�
	CalculationHelper::SetMatrixXAxis(xAxis, localCoordinateSystem);
	CalculationHelper::SetMatrixYAxis(yAxis, localCoordinateSystem);
	CalculationHelper::SetMatrixZAxis(zAxis, localCoordinateSystem);
	CalculationHelper::SetMatrixTranslation(origin, localCoordinateSystem);

	return localCoordinateSystem;
}

void lancetAlgorithm::IntraOsteotomy::CalculateBoolean(std::string nodeNameA, MR::Mesh& MeshA,
	MR::Mesh& MeshB, MR::BooleanOperation operationOne, std::string nodeNameB, MR::Mesh& MeshC,
	MR::Mesh& MeshD, MR::BooleanOperation operationTwo, MR::BooleanOperation operationThree)
{
	if (m_DataStorage->GetNamedObject<mitk::Surface>(nodeNameA)->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() <= 0)
		return;

	auto intersectResult = (MR::boolean(MeshA, MeshB, operationOne));

	MR::Mesh intersectMesh = *intersectResult;
	std::cout << "CalculateBoolean:" << intersectMesh.points.vec_.size() << std::endl;
	if (intersectMesh.points.vec_.size() > 0)
	{
		auto diffResult = (MR::boolean(MeshC, MeshD, operationTwo));
		MR::Mesh testMesh = *diffResult;

		if (testMesh.points.vec_.size() > 0)
		{
			auto cutResult = (MR::boolean(MeshC, MeshD, operationThree));
			MR::Mesh shellMesh = *cutResult;
			MeshC = shellMesh;
			MeshC.invalidateCaches();
			MeshD.invalidateCaches();

			vtkSmartPointer<vtkPolyData> ShellPolyData = vtkSmartPointer<vtkPolyData>::New();
			TurnMRMeshIntoPolyData(MeshC, ShellPolyData);
			m_DataStorage->GetNamedObject<mitk::Surface>(nodeNameB)->ReleaseData();
			m_DataStorage->GetNamedObject<mitk::Surface>(nodeNameB)->SetVtkPolyData(ShellPolyData);
		}
	}
}

void lancetAlgorithm::IntraOsteotomy::CalculateBoolean(std::string nodeName, MR::Mesh& MeshA,
	MR::Mesh& MeshB, MR::BooleanOperation operationOne, MR::BooleanOperation operationTwo, int compareSize)
{
	if (m_DataStorage->GetNamedObject<mitk::Surface>(nodeName)->GetVtkPolyData()->GetPoints()->GetNumberOfPoints() <= 0)
		return;

	auto intersectResult = (MR::boolean(MeshA, MeshB, operationOne));

	MR::Mesh intersectMesh = *intersectResult;
	std::cout << "CalculateBoolean:" << intersectMesh.points.vec_.size() << std::endl;
	if (intersectMesh.points.vec_.size() > 0)
	{
		auto diffResult = (MR::boolean(MeshA, MeshB, operationTwo));

		MR::Mesh diffMesh = *diffResult;
		std::cout << "diffMesh points: " << diffMesh.points.vec_.size() << " MeshA points: " << MeshA.points.vec_.size() << std::endl;
		if (diffMesh.points.vec_.size() <= compareSize * MeshA.points.vec_.size())
		{
			std::cout << "diffMesh.points.vec_.size() <= " << compareSize << " * MeshA.points.vec_.size()" << std::endl;
			MeshA = diffMesh;
			MeshA.invalidateCaches();
			MeshB.invalidateCaches();
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			TurnMRMeshIntoPolyData(MeshA, polyData);
			m_DataStorage->GetNamedObject<mitk::Surface>(nodeName)->ReleaseData();
			m_DataStorage->GetNamedObject<mitk::Surface>(nodeName)->SetVtkPolyData(polyData);
		}
	}
}

void lancetAlgorithm::IntraOsteotomy::TurnMRMeshIntoPolyData(MR::Mesh MRMesh, vtkSmartPointer<vtkPolyData> PolyData)
{
	clock_t t_start = clock();

	// all vertices of valid triangles
	const std::vector<std::array<MR::VertId, 3>> triangles = MRMesh.topology.getAllTriVerts();

	size_t cellNum = triangles.size();

	// all point coordinates
	const std::vector<MR::Vector3f>& mrPoints = MRMesh.points.vec_;

	const float* pointsXYZtripples = reinterpret_cast<const float*>(mrPoints.data());

	size_t ptNum = mrPoints.size();

	// triangle vertices as tripples of ints (pointing to elements in points vector)
	const int* vertexIDtripples = reinterpret_cast<const int*>(triangles.data());

	// Step 1: Create points
	vtkNew<vtkPoints> vtkPoints;
	vtkPoints->SetNumberOfPoints(ptNum);

	for (int i{ 0 }; i < ptNum; i++)
	{
		vtkPoints->SetPoint(i, pointsXYZtripples[3 * i], pointsXYZtripples[3 * i + 1], pointsXYZtripples[3 * i + 2]); // fastest
	}

	// faster than above
	vtkNew<vtkCellArray> vtkTriangles;
	vtkNew<vtkIdTypeArray> connectivity;

	// Each triangle has 4 entries: 3 point ids and 1 entry for the number of points in the cell (3)
	connectivity->SetNumberOfValues(cellNum * 4);
	vtkIdType* ptr = connectivity->GetPointer(0);

	for (int i{ 0 }; i < cellNum; i++)
	{
		*ptr++ = 3; // Number of points in this cell
		*ptr++ = vertexIDtripples[i * 3];
		*ptr++ = vertexIDtripples[i * 3 + 1];
		*ptr++ = vertexIDtripples[i * 3 + 2];
	}

	vtkTriangles->SetCells(cellNum, connectivity);

	//Create a polydata object and add the points and triangles to it
	vtkNew<vtkPolyData> newPolyData;
	newPolyData->SetPoints(vtkPoints);
	newPolyData->SetPolys(vtkTriangles);

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputData(newPolyData);
	normals->Update();

	PolyData->DeepCopy(normals->GetOutput());

	clock_t t_end = clock();

	std::cout<<"TurnMRMeshIntoPolyData cost: " + QString::number(t_end - t_start) + " ms"<<std::endl;
}

void lancetAlgorithm::IntraOsteotomy::SaveIntialNodePolyDataWithTransform(std::string nodeName,
	std::string path, vtkTransform* transform)
{
	auto node =m_DataStorage->GetNamedNode(nodeName);
	if (!node)
	{
		std::cout << "Cannot find node: " << nodeName << std::endl;
		return;
	}
	vtkSmartPointer<vtkPolyData> transformedPolyData = vtkSmartPointer<vtkPolyData>::New();
	auto surface = dynamic_cast<mitk::Surface*>(node->GetData());
	auto polyData = surface->GetVtkPolyData();
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->DeepCopy(surface->GetGeometry()->GetVtkMatrix());
	transformedPolyData->DeepCopy(CalculationHelper::TransformPolyData(polyData, matrix));

	if (transform)
	{
		vtkSmartPointer<vtkPolyData> finalTransformedPolyData = vtkSmartPointer<vtkPolyData>::New();
		finalTransformedPolyData->DeepCopy(CalculationHelper::TransformPolyData(transformedPolyData, transform));
		FileIO::WritePolyDataAsSTL(finalTransformedPolyData, path);
		return;
	}
	FileIO::WritePolyDataAsSTL(transformedPolyData, path);
}

void lancetAlgorithm::IntraOsteotomy::Drill()
{
	if (!ValidateRequiredNodes("green", "buffer", "shell", "red", "DrillEndTip"))
	{
		MITK_WARN << "Cutting initialization is not ready";
		return;
	}

	clock_t start = clock();

	//------------ Move m_Cutter_mesh with geometry obtained from the cutter surface ----------------
	auto cutterMatrix = m_DataStorage->GetNamedNode(PKAData::m_DrillEndTipNodeName.toStdString())
		->GetData()->GetGeometry()->GetVtkMatrix();

	double matrixDiff{ 0 };

	for (int i{ 0 }; i < 4; i++)
	{
		for (int j{ 3 }; j < 4; j++)
		{
			if (abs(m_LastRoundMatrix->GetElement(i, j) - cutterMatrix->GetElement(i, j)) > matrixDiff)
			{
				matrixDiff = abs(m_LastRoundMatrix->GetElement(i, j) - cutterMatrix->GetElement(i, j));
			}
		}
	}

	m_LastRoundMatrix->DeepCopy(cutterMatrix);
	if (matrixDiff < 0.01)
	{
		clock_t end = clock();
		std::cout << "Cutting time: " << end - start << std::endl;
		return;
	}

	MR::Vector3f x(cutterMatrix->GetElement(0, 0), cutterMatrix->GetElement(1, 0), cutterMatrix->GetElement(2, 0));
	MR::Vector3f y(cutterMatrix->GetElement(0, 1), cutterMatrix->GetElement(1, 1), cutterMatrix->GetElement(2, 1));
	MR::Vector3f z(cutterMatrix->GetElement(0, 2), cutterMatrix->GetElement(1, 2), cutterMatrix->GetElement(2, 2));
	MR::Vector3f t(cutterMatrix->GetElement(0, 3), cutterMatrix->GetElement(1, 3), cutterMatrix->GetElement(2, 3));
	MR::Matrix3f r(x, y, z);
	MR::AffineXf3f T(r, t);

	m_DrillTipMesh.transform(T);

	//------------ MeshLib Boolean -------------------
	//------------ Green -----------
	CalculateBoolean("green", m_GreenMesh, m_DrillTipMesh, MR::BooleanOperation::Intersection, MR::BooleanOperation::DifferenceAB, 2);

	//------------ buffer -----------
	CalculateBoolean("buffer", m_BufferMesh, m_DrillTipMesh, MR::BooleanOperation::Intersection, MR::BooleanOperation::DifferenceAB, 2);

	//------------- Shell and Red should be considered together ----------------------
	CalculateBoolean("red", m_RedMesh, m_DrillTipMesh, MR::BooleanOperation::Intersection, MR::BooleanOperation::DifferenceAB, 5);
	CalculateBoolean("red", m_RedMesh, m_DrillTipMesh, MR::BooleanOperation::Intersection,
		"shell", m_ShellMesh, m_DrillTipMesh, MR::BooleanOperation::InsideA, MR::BooleanOperation::OutsideA);
	//------------- Move the cutter back to initial --------------
	MR::AffineXf3f T_invert = T.inverse();
	m_DrillTipMesh.transform(T_invert);

	clock_t end = clock();
	std::cout << "Cutting time: " << end - start << std::endl;
}

double lancetAlgorithm::IntraOsteotomy::GetDrillTip2FemurDrillPlaneDistance(Eigen::Vector3d planePoint, Eigen::Vector3d planeNormal, vtkMatrix4x4* TFemur2Tibia)
{
	Eigen::Vector3d tipPos = m_PKADianaAimHardwareDevice->GetDrillEndInCamera();
	vtkSmartPointer<vtkMatrix4x4> TCamera2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TCamera2DrillTip = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2Drill->DeepCopy(PKAData::m_TCamera2Drill);
	auto drillTipRotation = CalculationHelper::GetRotationFromMatrix4x4(TCamera2Drill);
	TCamera2DrillTip->DeepCopy(CalculationHelper::GetMatrixByRotationAndTranslation(drillTipRotation, tipPos));

	vtkSmartPointer<vtkMatrix4x4> TFemur2DrillTip = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TFemur2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemur2FemurRF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
	TFemur2FemurRF->Invert();
	TFemurRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
	TFemurRF2Camera->Invert();

	TFemur2DrillTip->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TFemur2FemurRF, TFemurRF2Camera, TCamera2DrillTip));
	//ת�����ɹ�ͼ������ϵ��
	Eigen::Vector3d tipPosInFemurImage = CalculationHelper::GetTranslationFromMatrix4x4(TFemur2DrillTip);
	Eigen::Vector3d tipPosDirectionInFemurImage = CalculationHelper::GetXAxisFromVTKMatrix(TFemur2DrillTip);
	Eigen::Vector3d planePointInFemur = planePoint;
	Eigen::Vector3d planeNormalInFemur = planeNormal;
	if (TFemur2Tibia)
	{
		planePointInFemur = CalculationHelper::TransformByMatrix(planePointInFemur, TFemur2Tibia);
		planeNormalInFemur = CalculationHelper::CalculateInCoordinate(planeNormalInFemur, TFemur2Tibia);
	}
	double distance = -CalculationHelper::DistanceFromPointToPlane(tipPosInFemurImage, planeNormalInFemur, planePointInFemur);

	return distance;
}

double lancetAlgorithm::IntraOsteotomy::GetDrillTip2TibiaDrillPlaneDistance(Eigen::Vector3d planePoint, Eigen::Vector3d planeNormal)
{
	Eigen::Vector3d tipPos = m_PKADianaAimHardwareDevice->GetDrillEndInCamera();
	vtkSmartPointer<vtkMatrix4x4> TCamera2Drill = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TCamera2DrillTip = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2Drill->DeepCopy(PKAData::m_TCamera2Drill);
	auto drillTipRotation = CalculationHelper::GetRotationFromMatrix4x4(TCamera2Drill);
	TCamera2DrillTip->DeepCopy(CalculationHelper::GetMatrixByRotationAndTranslation(drillTipRotation, tipPos));

	vtkSmartPointer<vtkMatrix4x4> TFemur2DrillTip = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TTibia2TibiaRF = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TTibiaRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TTibia2TibiaRF->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);
	TTibia2TibiaRF->Invert();
	TTibiaRF2Camera->DeepCopy(PKAData::m_TCamera2TibiaRF);
	TTibiaRF2Camera->Invert();

	TFemur2DrillTip->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TTibia2TibiaRF, TTibiaRF2Camera, TCamera2DrillTip));
	//ת�����ɹ�ͼ������ϵ��
	Eigen::Vector3d tipPosInTibiaImage = CalculationHelper::GetTranslationFromMatrix4x4(TFemur2DrillTip);
	Eigen::Vector3d tipPosDirectionInTibiaImage = CalculationHelper::GetXAxisFromVTKMatrix(TFemur2DrillTip);
	Eigen::Vector3d planePointInFemur = planePoint;
	Eigen::Vector3d planeNormalInFemur = planeNormal;

	double distance = -CalculationHelper::DistanceFromPointToPlane(tipPosInTibiaImage, planeNormalInFemur, planePointInFemur);

	return distance;
}

void lancetAlgorithm::IntraOsteotomy::InitalOsteotomyModel(std::string drillEndName, std::string prosNodeName, std::string boneNodeName)
{
		if (!ValidateRequiredNodes(drillEndName, prosNodeName, boneNodeName))
		{
			std::cout << "DrillEndTip, pros, bone is missing" << std::endl;
			return;
		}

		PKARenderHelper::RemoveNodeIsExist(m_DataStorage, "red");
		PKARenderHelper::RemoveNodeIsExist(m_DataStorage, "buffer");
		PKARenderHelper::RemoveNodeIsExist(m_DataStorage, "green");
		PKARenderHelper::RemoveNodeIsExist(m_DataStorage, "shell");

		vtkSmartPointer<vtkPolyData> transformedPolyData = vtkSmartPointer<vtkPolyData>::New();
		std::string drillTipFilePath = m_DesktopPKAIntraOsteotomyFilePath + drillEndName + ".stl";
		std::string prosFilePath = m_DesktopPKAIntraOsteotomyFilePath + prosNodeName + ".stl";
		std::string prosPlusPath = m_DesktopPKAIntraOsteotomyFilePath + prosNodeName + "Plus.stl";
		std::string bonePath = m_DesktopPKAIntraOsteotomyFilePath + boneNodeName + ".stl";

		//����ĥ��ͷ
		auto drillEndTipSurface = dynamic_cast<mitk::Surface*>(m_DataStorage->GetNamedNode(drillEndName)->GetData());
		auto drillEndTipPolyData = drillEndTipSurface->GetVtkPolyData();
		FileIO::WritePolyDataAsSTL(drillEndTipPolyData, drillTipFilePath);

		//�������
		SaveIntialNodePolyDataWithTransform(prosNodeName, prosFilePath);

		//����Ŵ��ļ���
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->Scale(1.1, 1.1, 1.1);
		transform->Update();
		SaveIntialNodePolyDataWithTransform(prosNodeName, prosPlusPath, transform);

		//����ɹ�ģ��
		SaveIntialNodePolyDataWithTransform(boneNodeName, bonePath);

		//��ȡΪMR::Mesh
		MR::Mesh boneMesh = MR::MeshLoad::fromAnySupportedFormat(bonePath).value();
		MR::Mesh prosMesh = MR::MeshLoad::fromAnySupportedFormat(prosFilePath).value();
		MR::Mesh prosPlusMesh = MR::MeshLoad::fromAnySupportedFormat(prosPlusPath).value();
		m_DrillTipMesh = MR::MeshLoad::fromAnySupportedFormat(drillTipFilePath).value();

		//Ԥ����
		m_GreenMesh = *(MR::boolean(boneMesh, prosMesh, MR::BooleanOperation::Intersection)); //��ͷ������ཻ����
		m_RedMesh = *(MR::boolean(boneMesh, prosPlusMesh, MR::BooleanOperation::DifferenceAB)); //A-B
		m_ShellMesh = *(MR::boolean(boneMesh, prosPlusMesh, MR::BooleanOperation::OutsideA));   //��ͷ�����ģ��  �ཻ ��������ͷ�Ĳ���
		MR::Mesh tmp_mesh = *(MR::boolean(boneMesh, prosPlusMesh, MR::BooleanOperation::Intersection)); //��ͷ�����ģ���ཻ�Ĳ���
		m_BufferMesh = *(MR::boolean(tmp_mesh, prosMesh, MR::BooleanOperation::DifferenceAB));   //A-B

		vtkSmartPointer<vtkPolyData> greenPolyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> redPolyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> tmpShellPolyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> shellPolyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> bufferPolyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> drillEndPolyData = vtkSmartPointer<vtkPolyData>::New();

		TurnMRMeshIntoPolyData(m_GreenMesh, greenPolyData);
		TurnMRMeshIntoPolyData(m_RedMesh, redPolyData);
		TurnMRMeshIntoPolyData(m_ShellMesh, tmpShellPolyData);
		TurnMRMeshIntoPolyData(m_DrillTipMesh, drillEndPolyData);

		vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
		normals->SetInputData(tmpShellPolyData);
		normals->SplittingOff();
		normals->Update();

		// Warp using the normals
		vtkSmartPointer<vtkWarpVector> warper = vtkSmartPointer<vtkWarpVector>::New();
		warper->SetInputData(normals->GetOutput());
		warper->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
			vtkDataSetAttributes::NORMALS);
		warper->SetScaleFactor(0.07);
		warper->Update();

		vtkNew<vtkPolyDataNormals> normals_;
		normals_->SetInputData(warper->GetPolyDataOutput());
		normals_->Update();
		shellPolyData->DeepCopy(normals->GetOutput());
		TurnMRMeshIntoPolyData(m_BufferMesh, bufferPolyData);

		auto redNode = PKARenderHelper::AddPolyData2DataStorage(m_DataStorage, "red", redPolyData, 1, 0, 0);
		auto bufferNode = PKARenderHelper::AddPolyData2DataStorage(m_DataStorage, "buffer", bufferPolyData);
		auto greenNode = PKARenderHelper::AddPolyData2DataStorage(m_DataStorage, "green", greenPolyData, 0, 1, 0);
		auto shellNode = PKARenderHelper::AddPolyData2DataStorage(m_DataStorage, "shell", shellPolyData, 1, 1, 1, redNode);
}
