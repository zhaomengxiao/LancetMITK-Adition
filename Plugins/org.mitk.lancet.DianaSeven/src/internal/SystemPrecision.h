#pragma once
#include <qobject.h>
#include <mitkDataStorage.h>
#include <mitkBaseData.h>
#include <mitkIOUtil.h>
#include <mitkBaseGeometry.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkLandmarkTransform.h>
#include <qthread.h>
#include "surfaceregistraion.h"
#include "DianaAimHardwareService.h"
#include <PrintDataHelper.h>

namespace lancetAlgorithm
{
	class SystemPrecision : public QObject
	{
		Q_OBJECT
	public:
		SystemPrecision(mitk::DataStorage* aDataStorage, DianaAimHardwareService* aDianaAimHardwareService);
		void DisplayPrecisionTool();
		void DisplayProbe();
		
		void LandmarkRegistration(mitk::PointSet* target);
		void ICPRegistration(mitk::Surface* surface);
		void SetGoLinePrecisionTCP(/*Eigen::Vector3d pointA, Eigen::Vector3d pointB*/);
		void SetGoPlanePrecisionTCP();
		void GoLine(Eigen::Vector3d point0, Eigen::Vector3d point1);
		void GoPlane(Eigen::Vector3d point0, Eigen::Vector3d point1, Eigen::Vector3d point2);
		int CollectLandMarkPoints();
		int CollectICPPoints();
		void Reset();
		void UpdateDeviation();
		vtkSmartPointer<vtkMatrix4x4> GetBoxRF2BoxMatrix();
		void SetBoxRF2BoxMatrix(vtkMatrix4x4* matrix);
	private slots:
		void Update();
	private:
		static void LoadSingleMitkFile(mitk::DataStorage* dataStorage, std::string filePath, std::string nodeName);
		void UpdatePrecisionTestKit();

		void UpdateProbe();
		std::pair<Eigen::Vector3d, Eigen::Vector3d> AveragePointInBase(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB);
		std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> AveragePointInBase(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB, Eigen::Vector3d aPointC);
		std::pair<Eigen::Vector3d, Eigen::Vector3d> AveragePointInTCP(Eigen::Vector3d aPointA, Eigen::Vector3d aPointB);
		template<typename... Args>
		static vtkSmartPointer<vtkMatrix4x4> PreConcatenateMatrixs(vtkMatrix4x4* m1, Args&... args)
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

		template<typename... Args>
		static vtkSmartPointer<vtkMatrix4x4> PostConcatenateMatrixs(vtkMatrix4x4* m1, Args&... args)
		{
			vtkSmartPointer<vtkMatrix4x4> result = vtkSmartPointer<vtkMatrix4x4>::New();
			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
			transform->PostMultiply();
			transform->SetMatrix(m1);
			auto applyMatrices = [&](vtkMatrix4x4* matrix) {
				transform->Concatenate(matrix);
			};

			(applyMatrices(args), ...);
			transform->Update();
			transform->GetMatrix(result);
			return result;
		}

		void TransformPoints(mitk::PointSet* aPoints, vtkMatrix4x4* aMatrix);

		vtkSmartPointer<vtkMatrix4x4> CalculateBase2Image();
		vtkSmartPointer<vtkMatrix4x4> CalculateTCP2Image();
		vtkSmartPointer<vtkMatrix4x4> CalculateBoxRF2Camera();
		vtkSmartPointer<vtkMatrix4x4> CalculateLandmartBox2Camera();
		static Eigen::Vector3d TransformByMatrix(Eigen::Vector3d in, vtkMatrix4x4* matrix);
	private:
		mitk::DataStorage* m_dataStorage;
		DianaAimHardwareService* m_DianaAimHardwareService;
		std::string m_PrecisionToolModelStr = "PrecisionTool";
		std::string m_RobotEndRF = "RobotEndRF";
		std::string m_RobotBaseRF = "RobotBaseRF";
		std::string m_ProbeModelStr = "Probe";
		bool m_IsUseImageReg = false;

		std::string m_VerificationBlockStr = "VerificationBlock";
		vtkSmartPointer<vtkMatrix4x4> m_LandmarkRegistrationMatrix;  //box2BoxRF
		vtkSmartPointer<vtkMatrix4x4> m_ICPRegistrationMatrix;
		vtkSmartPointer<vtkMatrix4x4> m_FinalRegistrationMatrix;  //boxRF2box
		vtkSmartPointer<vtkMatrix4x4> m_Marker2TCPMatrix;
		bool m_IsDisplayPrecisionTool = false;
		bool m_IsDisplayProbe = false;
		mitk::PointSet::Pointer m_LandmarkPoints;
		mitk::PointSet::Pointer m_ICPPoints;
	};
}

