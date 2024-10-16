#pragma once
#include <mitkPointSet.h>
#include <vtkMatrix4x4.h>
#include <mitkDataStorage.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <mitkSurface.h>
#include <vtkOBBTree.h>
#include <vtkIdList.h>
#include <vtkClipClosedSurface.h>
#include <vtkPoints.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkCollisionDetectionFilter.h>
#include <vtkModifiedBSPTree.h>
#include "CalculationHelper.h"
#include "PKAModel.h"
#include "PKAData.h"
#include "PKAEnum.h"
#include "ChunLiXGImplant.h"
#include "PKAPrePlanBoneModel.h"
#include "PKARenderHelper.h"
#include "org_mitk_lancet_CZXtest_Export.h"
#include "ChunLITray.h"

namespace lancetAlgorithm
{
	class CZXTEST_EXPORT AngleCalculationHelper
	{
	public:
		AngleCalculationHelper(mitk::DataStorage* in);
		//virtual void BuildPKAFemur(std::string femurClippedNodeName);
		//virtual void BuildPKATibia(std::string tibiaClippedNodeName);

		/*
		* TODO  需要根据左右和内外  重新判断内外旋 前后翻 左右旋的判断
		*/
		void BuildFemurCalculator(PKAPrePlanBoneModel* bone, ChunLiXGImplant* implant);
		void BuildTibiaCalculator(PKAPrePlanBoneModel* bone, ChunLiTray* tray);
		std::pair<Tilt, double>  CalculateTilt(KneeModel kneeModel);
		std::pair<Va, double> CalculateVa(KneeModel kneeModel);
		std::pair<ProsRotation, double> CalculateRotation(KneeModel kneeModel);

		vtkSmartPointer<vtkMatrix4x4> CalculateTCoordinate2Pros(KneeModel kneeModel);
		vtkSmartPointer<vtkMatrix4x4> SetTiltAngle(Tilt tilt,double angle, KneeModel kneeModel);
		vtkSmartPointer<vtkMatrix4x4> SetVaAngle(Va va, double angle, KneeModel kneeModel);
		vtkSmartPointer<vtkMatrix4x4> SetProsRotationAngle(ProsRotation prosRotation, double angle, KneeModel kneeModel);
		/*****/
		double CalculateProud(KneeModel kneeModel);
		~AngleCalculationHelper();

		void CalculateIntrPlanning();
		std::pair<double, double>  CalculateLimbFlexionAndVarus();
		std::pair<Tilt, double>  CalculateLimbFlexion();
		std::pair<Va, double>  CalculateLimbVarus();
		vtkSmartPointer<vtkMatrix4x4> CalculateTFemur2Tibia();

		void ClipFemur();
		void ClipTibia();

		//胫骨假体在术中移动  那么是在胫骨坐标系下的移动，需要转换到股骨坐标系下
		double CalculateCutPlaneDistance();

		double CalculateIntraplanProsGap();
	private:
		bool CheckVariable();
	private:
		mitk::DataStorage* m_dataStorage;
		PKAPrePlanBoneModel* m_FemurModel;
		PKAPrePlanBoneModel* m_TibiaModel;
		ChunLiXGImplant* m_FemurImplant;
		ChunLiTray* m_TibiaTray;
		std::string m_ProudLine = "AngleCalculationHelperProudLine";
		vtkSmartPointer<vtkIntersectionPolyDataFilter> m_vtkIntersectionPolyDataFilter;
		Eigen::Vector3d CoronalNormal = Eigen::Vector3d(0.0,-1.0,0.0 );
		Eigen::Vector3d TransverseNormal = Eigen::Vector3d( 0.0,0.0,-1.0 );
		Eigen::Vector3d SagittalNormal = Eigen::Vector3d (-1.0,0.0,0.0 );
		vtkSmartPointer<vtkClipClosedSurface> m_vtkClipClosedSurface = nullptr;
	};
}
