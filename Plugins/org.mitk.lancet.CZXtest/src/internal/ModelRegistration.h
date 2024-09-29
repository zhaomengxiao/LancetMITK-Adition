#pragma once
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <mitkSurface.h>
#include <mitkPointSet.h>
#include <vtkPoints.h>
#include <vtkLandmarkTransform.h>
#include <vtkPolyData.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkKdTree.h>
#include "CalculationHelper.h"
#include "PKAData.h"
#include "PrintDataHelper.h"
#include "org_mitk_lancet_CZXtest_Export.h"
namespace lancetAlgorithm
{
	class CZXTEST_EXPORT ModelRegistration
	{
	public:
		ModelRegistration()
		{
			screenPoints = vtkSmartPointer<vtkPoints>::New();
			camera2SurfaceLandmarkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			camera2SurfaceICPMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
			combineICPMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		};
		
		void SetICPInput(mitk::PointSet* sourcePoints, mitk::PointSet* target, mitk::Surface* surface);
		vtkSmartPointer<vtkMatrix4x4> GetICPMatrix();
		vtkSmartPointer<vtkPoints> GetScreenPoint();

		void ComputeFemur(Eigen::Vector3d pointInFemurRF, Eigen::Vector3d targetPoint);
		void ComputeTibia(Eigen::Vector3d MedialMalleolusInTibiaRF, Eigen::Vector3d LateralMalleolusInTibiaRF, Eigen::Vector3d MedialMalleolus,
			Eigen::Vector3d	LateralMalleolus);
		
		double GetRegistrationRMS();

		double VerifyBoneVerifyPoint(KneeModel kneeModel, Eigen::Vector3d probePointInCamera);

	private:
		void Compute();
		void UpdateFemurLandmarkMatrixGroup(Eigen::Vector3d pointInFemurRF, Eigen::Vector3d targetPoint);
		void UpdateTibiaLandmarkMatrixGroup(Eigen::Vector3d MedialMalleolusInTibiaRF, Eigen::Vector3d LateralMalleolusInTibiaRF, Eigen::Vector3d MedialMalleolus,
			Eigen::Vector3d	LateralMalleolus);
		void UpdateLandmarkMatrixGroup();
		void UpdateCamera2SurfaceICP();
		void RemoveBadCamera2SurfacePoint();
		void UpdateRemoveBadCamera2SurfaceICP();
		double CalculateRMS(mitk::PointSet::Pointer source, mitk::PointSet::Pointer target);
		double CalculateRMS(vtkPoints* source, vtkPoints* target);

	private:
		mitk::PointSet* source = nullptr;
		mitk::PointSet* target = nullptr;
		mitk::Surface* surface = nullptr;

		vtkSmartPointer<vtkPoints> screenPoints = nullptr;
		vtkSmartPointer<vtkPoints> targetScreenPoints = nullptr;
		vtkSmartPointer<vtkMatrix4x4> camera2SurfaceLandmarkMatrix = nullptr;
		vtkSmartPointer<vtkMatrix4x4> camera2SurfaceICPMatrix = nullptr;
		vtkSmartPointer<vtkMatrix4x4> combineICPMatrix = nullptr;
		double m_RegistrationRMS = 0.0;
	};
}
