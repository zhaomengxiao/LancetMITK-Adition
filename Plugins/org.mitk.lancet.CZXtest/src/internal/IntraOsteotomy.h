#pragma once
#include <qobject.h>
#include <mitkImage.h>
#include <mitkDataStorage.h>
#include <mitkSurface.h>
#include <vtkWarpVector.h>
#include <vtkDataSetAttributes.h>
#include <vtkCellLocator.h>
#include "org_mitk_lancet_CZXtest_Export.h"
#include "CalculationHelper.h"
#include "PrintDataHelper.h"
#include "AngleCalculationHelper.h"
#include "PKAData.h"
//#include "PKADianaAimHardwareDevice.h"
#include "ChunLiXGImplant.h"
#include "ChunLITray.h"
#include "PKAEnum.h"
#include <LancetAimCamera.h>

//meshlib
#include <MRMesh/MRMesh.h>
#include <MRMesh/MRMeshLoad.h>
#include <MRMesh/MRUVSphere.h>
#include <MRMesh/MRMeshBoolean.h>
#include <MRMesh/MRMeshSave.h>
#include <MRMesh/MRMeshDecimate.h>
namespace lancetAlgorithm
{
	class CZXTEST_EXPORT IntraOsteotomy : public QObject
	{
		//Q_OBJECT
	public:
		IntraOsteotomy(mitk::DataStorage* dataStorage, LancetAimCamera* aCamera,
			ChunLiXGImplant* aChunLiXGImplant, ChunLiTray* aChunLITray);

		vtkSmartPointer<vtkMatrix4x4>  CalculateTDrillEnd2FemurImage(/*Eigen::Vector3d pointInImage*/);
		vtkSmartPointer<vtkMatrix4x4> CalculateTDrillEnd2FemurImage2();
		vtkSmartPointer<vtkMatrix4x4> CalculateTDrillEnd2TibiaImage2();
		//vtkSmartPointer<>
		vtkSmartPointer<vtkMatrix4x4> CalculateTBase2DrillEnd();

		/// <summary>
		/// ���㽫ĥ��ĩ���ƶ���ƽ���ϵĵ㣬����ĥ���XYƽ����ƽ�淨������ֱ
		/// TODO ĥ��ĩ�����ƶ�����ʼλ��  Ӧ��ʹ��X��Y�����20mm
		/// ���ַ����£�ֻ��ҪXY�����ƶ�ĥ�꣬��ر�Z��������   ��ʱ����
		/// </summary>
		/// <param name="pointInPlane"></param>
		/// <param name="planeNormal"></param>
		/// <returns></returns>
		vtkSmartPointer<vtkMatrix4x4>  CalculateDrillEndHorizontalPlane(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal);

		/// <summary>
		/// ���㽫ĥ��ĩ���ƶ���ƽ��ĵ��ϣ�����ĥ��X����ƽ�淨����ƽ��
		/// TODO ĥ��ĩ�����ƶ�����ʼλ�� ��ҪX�� -20mm
		/// ���ַ�ʽ��  ĥ����Ҫ��ĥ��Ȼ��ĥ���һ��λ��֮�󣬿���YZ���������
		/// </summary>
		/// <param name="pointInPlane"></param>
		/// <param name="planeNormal"></param>
		/// <returns></returns>
		vtkSmartPointer<vtkMatrix4x4> CalculateFemurDrillEndVerticalPlane(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal);

		vtkSmartPointer<vtkMatrix4x4> CalculateTibiaDrillEndVerticalPlane(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal);


		/// <summary>
		/// ���� ��ʼ��ĥ��ͷ��ָ��λ��
		/// </summary>
		/// <param name="pointInPlane"></param>
		/// <param name="planeNormal"></param>
		/// <returns></returns>
		vtkSmartPointer<vtkMatrix4x4> CalculateFemurDrillEndTipPos(Eigen::Vector3d pointInPlane, Eigen::Vector3d planeNormal);

		void CalculateBoolean(std::string nodeNameA, MR::Mesh& MeshA, MR::Mesh& MeshB, MR::BooleanOperation operationOne,
			std::string nodeNameB, MR::Mesh& MeshC, MR::Mesh& MeshD,
			MR::BooleanOperation operationTwo, MR::BooleanOperation operationThree);

		void CalculateBoolean(std::string nodeName, MR::Mesh& MeshA, MR::Mesh& MeshB,
			MR::BooleanOperation operationOne, MR::BooleanOperation operationTwo, int compareSize);


		void TurnMRMeshIntoPolyData(MR::Mesh MRMesh, vtkSmartPointer<vtkPolyData> PolyData);

		void InitalOsteotomyModel(std::string drillEndName, std::string prosNodeName, std::string boneNodeName);

		void SaveIntialNodePolyDataWithTransform(std::string nodeName, std::string path, vtkTransform* transform = nullptr);

		void Drill(CutPlane aCutPlane);

		double GetDrillTip2FemurDrillPlaneDistance(Eigen::Vector3d planePoint, Eigen::Vector3d planeNormal, vtkMatrix4x4* TFemur2Tibia = nullptr);
		double GetDrillTip2TibiaDrillPlaneDistance(Eigen::Vector3d planePoint, Eigen::Vector3d planeNormal);
		bool IsDrillInSecurityBoundary(CutPlane aCutPlane);
		bool IsDrillInSecurityDepth(CutPlane aCutPlane);

	private:
		template<typename... Args>
		bool ValidateRequiredNodes(const std::string& nodeName1, const Args&... args)
		{
			if (!m_DataStorage->GetNamedNode(nodeName1))
			{
				return false;
			}

			// Recursively check the remaining nodes
			if constexpr (sizeof...(args) > 0)
			{
				return ValidateRequiredNodes(args...);
			}

			return true;
		}

	private:
		mitk::DataStorage* m_DataStorage;
		LancetAimCamera* m_Camera;
		MR::Mesh m_GreenMesh;
		MR::Mesh m_BufferMesh;
		MR::Mesh m_ShellMesh;
		MR::Mesh m_RedMesh;
		MR::Mesh m_DrillTipMesh;
		vtkSmartPointer<vtkMatrix4x4> m_LastRoundMatrix;
		std::string m_DesktopPKAIntraOsteotomyFilePath = std::string(getenv("USERPROFILE")) + "\\Desktop\\PKAModelData\\IntraOsteotomy\\";

		ChunLiTray* m_ChunLiTray;
		ChunLiXGImplant* m_ChunLiXGImplant;
	};
}

