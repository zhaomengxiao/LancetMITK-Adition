#include "CalculationHelper.h"
using namespace lancetAlgorithm;
Eigen::Vector3d CalculationHelper::CalculateDirection(Eigen::Vector3d start, Eigen::Vector3d end)
{
	Eigen::Vector3d direction = end - start;
	direction.normalize();
	return direction;
}

Eigen::Vector3d CalculationHelper::TransformByMatrix(Eigen::Vector3d in, vtkMatrix4x4* matrix)
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

Eigen::Vector3d CalculationHelper::CalculateInCoordinate(Eigen::Vector3d in, vtkMatrix4x4 * matrix)
{
	// 将 vtkMatrix4x4 转换为 Eigen::Matrix4d
	Eigen::Matrix3d M_eigen;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j< 3; ++j) {
			M_eigen(i, j) = matrix->GetElement(i, j);
			//std::cout <<"matrix" << i << " " << j << " " << matrix->GetElement(i, j) << std::endl;
		}
	}
	Eigen::Vector3d result = M_eigen/*.inverse()*/ * in;
	return result;
}

void CalculationHelper::TransformPoints(mitk::PointSet* in, vtkMatrix4x4* matrix, mitk::PointSet* out)
{
	for (int i = 0; i < in->GetSize(); i++)
	{
		auto point = in->GetPoint(i);
		double point1[4] = { point[0], point[1], point[2], 1 };
		double* r = matrix->MultiplyDoublePoint(point1);
		out->InsertPoint(i, r);
	}
}

double CalculationHelper::CalculateDirectedAngleWithPlaneNormal(double* vector, double* normal, bool radian)
{
	auto DegreesFromRadians = [](double x) { return x * 57.2957795131; };
	auto RadiansFromDegrees = [](double x) { return x * 0.017453292; };

	auto AngleBetweenVectors = [](const Eigen::Vector3d& v1, const Eigen::Vector3d& v2) {
		return atan2(v1.cross(v2).norm(), v1.dot(v2));
	};

	Eigen::Vector3d LineVector(vector);
	Eigen::Vector3d PlaneNormal(normal);

	// 计算向量与平面法向量之间的夹角（弧度）
	double radianAngle = AngleBetweenVectors(LineVector, PlaneNormal);
	double degreeAngle = DegreesFromRadians(radianAngle);

	// 计算结果角度
	double result = 0.0;
	if (fabs(degreeAngle - 90) < 1e-6) {
		// 向量与平面平行
		result = 0.0;
	}
	else {
		result = (degreeAngle > 90) ? (degreeAngle - 90) : (90 - degreeAngle);
	}

	// 判断向量与平面法向量的方向关系
	if (LineVector.dot(PlaneNormal) < 0) {
		result = -result;
	}

	// 返回角度或弧度
	return radian ? RadiansFromDegrees(result) : result;
}

Eigen::Vector3d CalculationHelper::ProjectPointOntoPlane(const Eigen::Vector3d& point, const Eigen::Vector3d& planeOrigin, const Eigen::Vector3d& planeNormal) {
	Eigen::Vector3d normalizedNormal = planeNormal.normalized();
	Eigen::Vector3d pointToOrigin = point - planeOrigin;
	double projectionLength = pointToOrigin.dot(normalizedNormal);
	Eigen::Vector3d projectedPoint = point - projectionLength * normalizedNormal;
	return projectedPoint;
}

/// <summary>
/// 得到垂直于V1 V2的向量，计算v1 v2之间的夹角并根据垂直向量与平面向量判断正负
/// </summary>
/// <param name="vector1"></param>
/// <param name="vector2"></param>
/// <param name="planeNormal"></param>
/// <returns></returns>
double CalculationHelper::ComputeSignedAngle(const Eigen::Vector3d& vector1, const Eigen::Vector3d& vector2, const Eigen::Vector3d& planeNormal)
{
	Eigen::Vector3d crossProduct = vector1.cross(vector2);
	double angle = atan2(crossProduct.norm(), vector1.dot(vector2));
	if (crossProduct.dot(planeNormal) < 0) {
		angle = -angle;
	}
	return angle;
}

double CalculationHelper::CalculateAngle(const Eigen::Vector3d vec1, const Eigen::Vector3d vec2)
{
	double dotProduct = vec1.dot(vec2);
	double norms = vec1.norm() * vec2.norm();
	double cosTheta = dotProduct / norms;
	return std::acos(cosTheta);
}

double lancetAlgorithm::CalculationHelper::CalculateAngle(const Eigen::Vector2d vec1, const Eigen::Vector2d vec2)
{
	double dotProduct = vec1.dot(vec2);
	double norms = vec1.norm() * vec2.norm();
	double cosTheta = dotProduct / norms;
	return std::acos(cosTheta);
}

double CalculationHelper::DistanceFromPointToPlane(const Eigen::Vector3d& point, const Eigen::Vector3d& planeNormal, const Eigen::Vector3d& planePoint)
{
	Eigen::Vector3d vectorFromPlane = point - planePoint;
	return planeNormal.dot(vectorFromPlane) / planeNormal.norm();
}

bool CalculationHelper::IsSameDirection(const Eigen::Vector3d vec1, const Eigen::Vector3d vec2)
{
	if (vec1.dot(vec2) < 0)
		return false;
	else
		return true;
}

Eigen::Vector3d CalculationHelper::TwoPointCenter(Eigen::Vector3d point1, Eigen::Vector3d point2)
{
	Eigen::Vector3d pointSum;
	pointSum.setZero();
	pointSum = point1 + point2;

	return pointSum / 2;
}

vtkSmartPointer<vtkPoints> CalculationHelper::TransformVTKPoints(vtkPoints* in, vtkMatrix4x4* m)
{
	vtkSmartPointer<vtkPoints> tmpPoints = vtkSmartPointer<vtkPoints>::New();
	tmpPoints->DeepCopy(in);
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(tmpPoints);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkMatrix4x4> tmpMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	tmpMatrix->DeepCopy(m);
	vtkSmartPointer<vtkTransform> trans = vtkSmartPointer<vtkTransform>::New();
	trans->SetMatrix(tmpMatrix);
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(trans);
	transformFilter->Update();
	vtkSmartPointer<vtkPoints> result = vtkSmartPointer<vtkPoints>::New();
	result->DeepCopy(transformFilter->GetOutput()->GetPoints());
	return result;
}

void lancetAlgorithm::CalculationHelper::ConvertVtkTransformToMitkTransform(vtkTransform* vtkTransform, mitk::AffineTransform3D::Pointer mitkTransform)
{
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkTransform->GetMatrix();
	mitk::AffineTransform3D::MatrixType mitkMatrix;
	mitk::AffineTransform3D::OffsetType mitkOffset;

	for (unsigned int i = 0; i < 3; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			mitkMatrix[i][j] = vtkMatrix->GetElement(i, j);
		}
		mitkOffset[i] = vtkMatrix->GetElement(i, 3);
	}

	mitkTransform->SetMatrix(mitkMatrix);
	mitkTransform->SetOffset(mitkOffset);
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::CalculationHelper::GetVTKMatrix4x4(mitk::NavigationData::Pointer nd)
{
	auto o = nd->GetOrientation();
	double R[3][3];
	double* V = { nd->GetPosition().GetDataPointer() };
	vtkQuaterniond quaterniond{ o.r(), o.x(), o.y(), o.z() };
	quaterniond.ToMatrix3x3(R);

	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
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
	return matrix;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::CalculationHelper::GetNodeTransform(mitk::DataStorage* dataStorage, const std::string nodeName)
{
	vtkSmartPointer<vtkMatrix4x4> result = vtkSmartPointer<vtkMatrix4x4>::New();

	auto matrix = dataStorage->GetNamedNode(nodeName)->GetData()->GetGeometry()->GetVtkMatrix();

	result->DeepCopy(matrix);
	return result;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::CalculationHelper::GetNodeTransform(mitk::DataNode* node)
{
	vtkSmartPointer<vtkMatrix4x4> result = vtkSmartPointer<vtkMatrix4x4>::New();

	auto matrix = node->GetData()->GetGeometry()->GetVtkMatrix();

	result->DeepCopy(matrix);
	return result;
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::ComputeNormal(const Eigen::Vector3d A, const Eigen::Vector3d B, const Eigen::Vector3d C)
{
	// 计算向量AB和AC
	Eigen::Vector3d AB = B - A;
	Eigen::Vector3d AC = C - A;

	// 计算叉积AB x AC
	Eigen::Vector3d normal = AB.cross(AC);

	// 归一化法向量
	normal.normalize();

	return normal;
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::ComputePerpendicular(const Eigen::Vector3d& P, const Eigen::Vector3d& A, const Eigen::Vector3d& B)
{
	// 计算向量AP和AB
	Eigen::Vector3d AP = P - A;
	Eigen::Vector3d AB = B - A;

	// 计算向量AB的单位向量
	Eigen::Vector3d AB_normalized = AB.normalized();

	// 计算向量AP在向量AB上的投影
	double projection_length = AP.dot(AB_normalized);
	Eigen::Vector3d projection = projection_length * AB_normalized;

	// 计算垂线的终点
	Eigen::Vector3d perpendicular_end = A + projection;

	// 计算垂线向量
	Eigen::Vector3d perpendicular = P - perpendicular_end ;
	perpendicular.normalize();
	return perpendicular;
}

double lancetAlgorithm::CalculationHelper::CalculateTwoPointsDistance(const Eigen::Vector3d pointA, const Eigen::Vector3d PointB)
{
	return (pointA - PointB).norm();
}

void lancetAlgorithm::CalculationHelper::CalculateRotationMatrix(const double A[3], const double B[3], vtkMatrix4x4* rotationMatrix)
{
	double crossProduct[3];
	vtkMath::Cross(A, B, crossProduct);
	vtkMath::Normalize(crossProduct);

	double dotProduct = vtkMath::Dot(A, B);
	double angle = std::acos(dotProduct / (vtkMath::Norm(A) * vtkMath::Norm(B))) * 180.0 / vtkMath::Pi();
	std::cout << "CalculateRotationMatrix: " << angle << std::endl;
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateWXYZ(angle, crossProduct);
	transform->GetMatrix(rotationMatrix);
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::GetTranslationFromMatrix4x4(vtkMatrix4x4* matrix)
{
	Eigen::Vector3d res;

	res << matrix->GetElement(0, 3), matrix->GetElement(1, 3), matrix->GetElement(2, 3);
	return res;
}

Eigen::Matrix3d lancetAlgorithm::CalculationHelper::GetRotationFromMatrix4x4(vtkMatrix4x4* matrix)
{
	auto data = matrix->GetData();
	Eigen::Matrix3d res;
	res << data[0], data[1], data[2],
		data[4], data[5], data[6],
		data[8], data[9], data[10];
	return res;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::CalculationHelper::GetMatrixByRotationAndTranslation(Eigen::Matrix3d rotation, Eigen::Vector3d translation)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->Identity();
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			matrix->SetElement(row, col, rotation(row, col));
		}
	}
	matrix->SetElement(0, 3, translation[0]);
	matrix->SetElement(1, 3, translation[1]);
	matrix->SetElement(2, 3, translation[2]);
	return matrix;
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::GetXAxisFromVTKMatrix(vtkMatrix4x4* matrix)
{
	Eigen::Vector3d result;
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	m->DeepCopy(matrix);
	result[0] = matrix->GetElement(0, 0); // m01
	result[1] = matrix->GetElement(1, 0); // m11
	result[2] = matrix->GetElement(2, 0); // m21
	return result;
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::GetYAxisFromVTKMatrix(vtkMatrix4x4* matrix)
{
	Eigen::Vector3d result;
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	m->DeepCopy(matrix);
	result[0] = matrix->GetElement(0, 1); // m01
	result[1] = matrix->GetElement(1, 1); // m11
	result[2] = matrix->GetElement(2, 1); // m21
	return result;
}

Eigen::Vector3d lancetAlgorithm::CalculationHelper::GetZAxisFromVTKMatrix(vtkMatrix4x4* matrix)
{
	Eigen::Vector3d result;
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	m->DeepCopy(matrix);
	result[0] = matrix->GetElement(0, 2); // m02
	result[1] = matrix->GetElement(1, 2); // m12
	result[2] = matrix->GetElement(2, 2); // m22
	return result;
}

void lancetAlgorithm::CalculationHelper::GetXYZEulerByMatrix(vtkMatrix4x4* inputMatrix, std::array<double, 6>& output)
{
	Eigen::Matrix3d rotationMatrix;
	Eigen::Vector3d translationVector;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			rotationMatrix(i, j) = inputMatrix->GetElement(i, j);
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		translationVector(i) = inputMatrix->GetElement(i, 3);
	}

	output[0] = translationVector(0);
	output[1] = translationVector(1);
	output[2] = translationVector(2);

	Eigen::Vector3d eulerAngle = rotationMatrix.eulerAngles(2, 1, 0);
	output[3] = eulerAngle(0);
	output[4] = eulerAngle(1);
	output[5] = eulerAngle(2);
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::CalculationHelper::ConvertEigen2vtkMatrix(const Eigen::Matrix4d eigenMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			vtkMatrix->SetElement(i, j, eigenMatrix(i, j));
		}
	}

	return vtkMatrix;
}

vtkSmartPointer<vtkPolyData> lancetAlgorithm::CalculationHelper::TransformPolyData(vtkPolyData* polyData, vtkMatrix4x4* matrix)
{
	vtkSmartPointer<vtkTransformPolyDataFilter> filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkPolyData> result = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	m->DeepCopy(matrix);
	transform->SetMatrix(m);
	transform->Update();
	filter->SetInputData(polyData);
	filter->SetTransform(transform);
	filter->Update();
	result->DeepCopy(filter->GetOutput());
	return result;
}

vtkSmartPointer<vtkPolyData> lancetAlgorithm::CalculationHelper::TransformPolyData(vtkPolyData* polyData, vtkTransform* transform)
{
	vtkSmartPointer<vtkTransformPolyDataFilter> filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkPolyData> result = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkTransform> mTransform = vtkSmartPointer<vtkTransform>::New();
	mTransform->DeepCopy(transform);
	mTransform->Update();

	filter->SetInputData(polyData);
	filter->SetTransform(mTransform);
	filter->Update();
	result->DeepCopy(filter->GetOutput());
	return result;
}

double lancetAlgorithm::CalculationHelper::FromDegreeToRadian(double aDegree)
{
	return aDegree * vtkMath::Pi() / 180;
}

double lancetAlgorithm::CalculationHelper::FromRadianToDegree(double aRadian)
{
	return aRadian * 180 / vtkMath::Pi();
}

void lancetAlgorithm::CalculationHelper::SetMatrixXAxis(Eigen::Vector3d X, vtkMatrix4x4* matrix)
{
	matrix->SetElement(0, 0, X[0]);
	matrix->SetElement(1, 0, X[1]);
	matrix->SetElement(2, 0, X[2]);
}

void lancetAlgorithm::CalculationHelper::SetMatrixYAxis(Eigen::Vector3d Y, vtkMatrix4x4* matrix)
{
	matrix->SetElement(0, 1, Y(0));  // Y轴分量
	matrix->SetElement(1, 1, Y(1));
	matrix->SetElement(2, 1, Y(2));
}

void lancetAlgorithm::CalculationHelper::SetMatrixZAxis(Eigen::Vector3d Z, vtkMatrix4x4* matrix)
{
	matrix->SetElement(0, 2, Z(0));  // Z轴分量
	matrix->SetElement(1, 2, Z(1));
	matrix->SetElement(2, 2, Z(2));
}

void lancetAlgorithm::CalculationHelper::SetMatrixTranslation(Eigen::Vector3d T, vtkMatrix4x4* matrix)
{
	matrix->SetElement(0, 3, T(0));  // 平移分量
	matrix->SetElement(1, 3, T(1));
	matrix->SetElement(2, 3, T(2));
}
