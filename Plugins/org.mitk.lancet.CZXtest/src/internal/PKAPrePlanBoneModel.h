#pragma once
#ifndef PKAPREPLANBONEMODEL
#define PKAPREPLANBONEMODEL

#include <eigen3/Eigen/Dense>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
//#include "PKADataBase.h"
#include "CalculationHelper.h"
#include "PKAData.h"
#include <mitkDataStorage.h>
#include "PrintDataHelper.h"
#include "PKARenderHelper.h"
#include "org_mitk_lancet_CZXtest_Export.h"
namespace lancetAlgorithm
{
	class CZXTEST_EXPORT PKAPrePlanBoneModel //: public PKADataBase
	{
	public:
		PKAPrePlanBoneModel(mitk::DataStorage* dataStorage);
		void BuildFemurModel();
		void BuildTibiaModel();
		vtkSmartPointer<vtkMatrix4x4> GetBoneModelCoordinate();
		vtkAxesActor* GetBoneAxes();
		Eigen::Vector3d GetBoneSagittalPlaneNormal();
		Eigen::Vector3d GetMechanicalAxis();
		Eigen::Vector3d GetMedialPosteriorPoint();
		Eigen::Vector3d GetLateralPosteriorPoint();
		vtkMatrix4x4* GetBoneTransform();
		void UpdateBoneAxesActor(vtkMatrix4x4* matrix);
	private:
		/// <summary>
		/// 踝关节中心  和胫骨中心 计算胫骨力线
		/// </summary>
		void CalculateTibiaMechanicalAxis(mitk::DataStorage* dataStorage);

		/// <summary>
		/// 通过股骨中心和髋关节中心计算股骨力线
		/// </summary>
		void CalculateFemurMechanicalAxis(mitk::DataStorage* dataStorage);

		/// <summary>
		/// 通过股骨内外髁 计算股骨通髁轴
		/// </summary>
		void CalculateFemuralAcrocondylarAxis(mitk::DataStorage* dataStorage);

		/// <summary>
		/// 通过胫骨内外髁  计算胫骨通髁轴
		/// </summary>
		void CalculateTibialAcrocondylarAxis(mitk::DataStorage* dataStorage);

	private:
		vtkSmartPointer<vtkMatrix4x4> m_BoneCooridnate;
		mitk::DataStorage* m_DataStorage;
		vtkSmartPointer<vtkAxesActor> m_AxesActor;
		vtkSmartPointer<vtkMatrix4x4> m_AxesActorMatrix;
		Eigen::Vector3d CoronalNormal = Eigen::Vector3d( 0.0,-1.0,0.0 );
		Eigen::Vector3d TransverseNormal = Eigen::Vector3d( 0.0,0.0,-1.0);
		Eigen::Vector3d SagittalNormal =   Eigen::Vector3d( -1.0,0.0,0.0);
		vtkSmartPointer<vtkMatrix4x4> m_BoneTransformMatrix;
	};
}
#endif // !PKAPREPLANBONEMODEL

