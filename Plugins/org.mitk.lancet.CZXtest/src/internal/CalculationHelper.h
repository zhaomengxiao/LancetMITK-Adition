#pragma once
#ifndef PKACACLCULAT
#define PKACACLCULAT
#include <eigen3/Eigen/Dense>
#include <mitkPointSet.h>
#include <filesystem>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <mitkNavigationData.h>
#include <vtkQuaternion.h>
#include <mitkDataStorage.h>
#include "org_mitk_lancet_CZXtest_Export.h"

//#define PI 3.14159265358979323846
namespace lancetAlgorithm
{
	class CZXTEST_EXPORT  CalculationHelper
	{
	public:
		/// <summary>
		/// ����������ķ�������
		/// </summary>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <returns></returns>
		static Eigen::Vector3d CalculateDirection(Eigen::Vector3d start, Eigen::Vector3d end);

		static Eigen::Vector3d TransformByMatrix(Eigen::Vector3d in, vtkMatrix4x4* matrix);

		static Eigen::Vector3d CalculateInCoordinate(Eigen::Vector3d in, vtkMatrix4x4* matrix);

		/// <summary>
		/// ��һ�����ƽ���λ�˱任
		/// </summary>
		/// <param name="in"></param>
		/// <param name="matrix"></param>
		/// <param name="out"></param>
		static void TransformPoints(mitk::PointSet* in, vtkMatrix4x4* matrix, mitk::PointSet* out);

		/// <summary>
		///  ����������ƽ�淨����֮�������Ƕ�  basic.cpp�� DirectedAngleBetweenVectorAndPlane();
		/// </summary>
		/// <param name="vector"></param>
		/// <param name="normal"></param>
		/// <param name="radian"></param>
		/// <returns></returns>
		static double CalculateDirectedAngleWithPlaneNormal(double* vector, double* normal, bool radian = false);

		/// <summary>
		/// ���㽫��ͶӰ��ƽ���ϵ�λ��
		/// </summary>
		/// <param name="point">ͶӰ��</param>
		/// <param name="planeOrigin">ƽ���ϵĵ�</param>
		/// <param name="planeNormal">ƽ��ķ�����</param>
		/// <returns>ͶӰ�������</returns>
		static Eigen::Vector3d ProjectPointOntoPlane(const Eigen::Vector3d& point, const Eigen::Vector3d& planeOrigin, const Eigen::Vector3d& planeNormal);

		/// <summary>
		/// ������������֮�������Ƕ�
		/// </summary>
		/// <param name="vector1"></param>
		/// <param name="vector2"></param>
		/// <param name="planeNormal">����ȷ������Ƕȷ���ķ�����</param>
		/// <returns>vector1 �� vector2 ֮�������Ƕȣ����ȣ�</returns>
		static double ComputeSignedAngle(const Eigen::Vector3d& vector1, const Eigen::Vector3d& vector2, const Eigen::Vector3d& planeNormal);


		static double CalculateAngle(const Eigen::Vector3d vec1, const Eigen::Vector3d vec2);
		static double CalculateAngle(const Eigen::Vector2d vec1, const Eigen::Vector2d vec2);

		/// <summary>
		/// ����㵽ƽ��ľ���
		/// </summary>
		/// <param name="point">���λ��</param>
		/// <param name="planeNormal">ƽ��ķ�����</param>
		/// <param name="planePoint">ƽ���ϵ�һ����</param>
		/// <returns>�㵽ƽ��ľ���</returns>
		static double DistanceFromPointToPlane(const Eigen::Vector3d& point, const Eigen::Vector3d& planeNormal, const Eigen::Vector3d& planePoint);

		/// <summary>
		/// �����������������Ƿ���ͬ
		/// </summary>
		/// <param name="vec1"></param>
		/// <param name="vec2"></param>
		/// <returns></returns>
		static 	bool IsSameDirection(const Eigen::Vector3d vec1, const  Eigen::Vector3d vec2);

		static Eigen::Vector3d TwoPointCenter(Eigen::Vector3d point1, Eigen::Vector3d point2);

		static vtkSmartPointer<vtkPoints> TransformVTKPoints(vtkPoints* in, vtkMatrix4x4* m);

		static void ConvertVtkTransformToMitkTransform(vtkTransform* vtkTransform, mitk::AffineTransform3D::Pointer mitkTransform);

		static vtkSmartPointer<vtkMatrix4x4> GetVTKMatrix4x4(mitk::NavigationData::Pointer nd);

		static vtkSmartPointer<vtkMatrix4x4> GetNodeTransform(mitk::DataStorage* dataStorage, const std::string nodeName);
		static vtkSmartPointer<vtkMatrix4x4> GetNodeTransform(mitk::DataNode* node);

		/// <summary>
		/// ����������Ϊһ��ƽ��ķ�����
		/// </summary>
		/// <param name="A"></param>
		/// <param name="B"></param>
		/// <param name="C"></param>
		/// <returns></returns>
		static Eigen::Vector3d ComputeNormal(const Eigen::Vector3d A, const Eigen::Vector3d B, const Eigen::Vector3d C);

		// �����P���߶�AB�Ĵ�������,���ߵ㵽P��
		static Eigen::Vector3d ComputePerpendicular(const Eigen::Vector3d& P, const Eigen::Vector3d& A, const Eigen::Vector3d& B);

		static double CalculateTwoPointsDistance(const Eigen::Vector3d pointA, const Eigen::Vector3d PointB);

		/// <summary>
		/// ���㽫A����ת������B����ƽ����ͬ���RotationMatrix
		/// </summary>
		/// <param name="A"></param>
		/// <param name="B"></param>
		/// <returns></returns>
		static void CalculateRotationMatrix(const double A[3], const double B[3], vtkMatrix4x4* rotationMatrix);

		static Eigen::Vector3d GetTranslationFromMatrix4x4(vtkMatrix4x4* matrix);

		static Eigen::Matrix3d GetRotationFromMatrix4x4(vtkMatrix4x4* matrix);

		static vtkSmartPointer<vtkMatrix4x4> GetMatrixByRotationAndTranslation(Eigen::Matrix3d rotation, Eigen::Vector3d translation);

		static Eigen::Vector3d GetXAxisFromVTKMatrix(vtkMatrix4x4* matrix);
		static Eigen::Vector3d GetYAxisFromVTKMatrix(vtkMatrix4x4* matrix);
		static Eigen::Vector3d GetZAxisFromVTKMatrix(vtkMatrix4x4* matrix);

		static void GetXYZEulerByMatrix(vtkMatrix4x4* inputMatrix, std::array<double, 6>& output);

		static vtkSmartPointer<vtkMatrix4x4> ConvertEigen2vtkMatrix(const Eigen::Matrix4d eigenMatrix);

		static vtkSmartPointer<vtkPolyData> TransformPolyData(vtkPolyData* polyData, vtkMatrix4x4* matrix);
		static vtkSmartPointer<vtkPolyData> TransformPolyData(vtkPolyData* polyData, vtkTransform* transform);

		static double FromDegreeToRadian(double aDegree);
		static double FromRadianToDegree(double aRadian);

		static void SetMatrixXAxis(Eigen::Vector3d X, vtkMatrix4x4* matrix);
		static void SetMatrixYAxis(Eigen::Vector3d Y, vtkMatrix4x4* matrix);
		static void SetMatrixZAxis(Eigen::Vector3d Z, vtkMatrix4x4* matrix);
		static void SetMatrixTranslation(Eigen::Vector3d T, vtkMatrix4x4* matrix);

		template<typename... Args>
		static vtkSmartPointer<vtkMatrix4x4> PreConcatenateMatrixs(vtkMatrix4x4* m1,Args&... args)
		{
			vtkSmartPointer<vtkMatrix4x4> result = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->PreMultiply();
			transform->SetMatrix(m1);
			auto applyMatrices = [&](vtkMatrix4x4* matrix) {
				transform->Concatenate(matrix);
			};

			(applyMatrices(args), ...);
			transform->Update();
			transform->GetMatrix(result);
			return result;
		}
	};
}
#endif // !PKACACLCULAT

