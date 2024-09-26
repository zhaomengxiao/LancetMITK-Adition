#pragma once
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <mitkDataStorage.h>
#include <vtkNew.h>
#include <mitkApplyTransformMatrixOperation.h>
#include <mitkOperation.h>
#include "PKAData.h"
#include "CalculationHelper.h"
#include "PKAEnum.h"
#include "PKARenderHelper.h"
#include <vtkRenderer.h>
#include <vtkCamera.h>

namespace lancetAlgorithm
{
	//该类用来计算术中骨头之间的角度
	class IntraBone
	{
	public:
		IntraBone(mitk::DataStorage* dataStorage);
		void UpdateMechanical();

		/// <summary>
		/// 根据术中股骨和胫骨的力线  计算力线夹角
		/// </summary>
		/// <returns></returns>
		double CalculateAngle();

		void UpdateDisplayModel();
		void InitPros();

		void UpdateTibiaProsMatrix();
		void DrawMechanicalAxis(vtkRenderer* femurRenderer, vtkRenderer* tibiaRenderer, vtkMatrix4x4* tibiaMatrix);

	private:
		Eigen::Matrix3d RotationFemur2Implant;
		Eigen::Vector3d TranslationFemur2Implant;

		Eigen::Matrix3d RotationTibia2Tray;
		Eigen::Vector3d TranslationTibia2Tray;
		mitk::DataStorage* m_DataStorage;
		vtkSmartPointer<vtkMatrix4x4> m_FemurProsMatrix;
		vtkSmartPointer<vtkMatrix4x4> m_TibiaTrayMatrix;
		vtkSmartPointer<vtkMatrix4x4> m_TibiaInsertMatrix;
		Eigen::Vector3d FemurMechanicalStart;
		Eigen::Vector3d FemurMechanicalEnd;
		Eigen::Vector3d TibiaMechanicalStart;
		Eigen::Vector3d TibiaMechanicalEnd;

		vtkSmartPointer<vtkActor> tibiaLeftActor;
		vtkSmartPointer<vtkActor> femurLeftActor;
		vtkRenderer* leftRenderer;
		vtkRenderer* rightRenderer;
		std::string femurProsDataPath;
		std::string tibiaTrayDataPath;
		std::string tibiaInsertDataPath;
		std::string tempTibiaTrayNodeName;
		std::string tempFemurProsNodeName;
		std::string tempTibiaInsertNodeName;
		std::string tempTibiaClippedNodeName;
		std::string tempFibulaClippedNodeName;
	};
}

