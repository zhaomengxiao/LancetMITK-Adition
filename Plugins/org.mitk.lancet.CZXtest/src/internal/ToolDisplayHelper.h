#pragma once
#ifndef TOOLDISPLAYHELPER
#define TOOLDISPLAYHELPER
#include <QObject>
#include "PKARenderHelper.h"
#include "PKAData.h"
#include "PKAEnum.h"
#include <mitkDataStorage.h>
#include <qtimer.h>
#include <vtkAxesActor.h>
#include "CalculationHelper.h"
#include <mitkIRenderWindowPart.h>
#include <AimPositionAPI.h>
#include "FileIO.h"
#include "PrintDataHelper.h"
#include "PKADianaAimHardwareDevice.h"

namespace lancetAlgorithm
{
	class ToolDisplayHelper : public QObject
	{
		//Q_OBJECT
	public:
		ToolDisplayHelper(mitk::DataStorage* dataStorage, mitk::IRenderWindowPart* iRenderWindowPart, PKADianaAimHardwareDevice* PKADianaAimHardwareDevice);

		void AddModle(PKAMarker p, mitk::IRenderWindowPart* renderWindowPart);
		void RemoveModel(PKAMarker p);
	//public slots:
		void UpdateTool();
		void UsingFemurRegistration();
		void UsingTibiaRegistration();

	private:
		void Start();

		void Stop();
	private:
		QTimer* m_UpdateToolTimer{ nullptr };
		mitk::DataStorage* m_DataStorage;
		std::set<PKAMarker> m_ToolSet;
		std::map<PKAMarker, vtkSmartPointer<vtkAxesActor>> m_AxesMapper;
		vtkSmartPointer<vtkAxesActor> m_DrillEndActor = nullptr;
		vtkSmartPointer<vtkMatrix4x4> m_DrillEndMatrix = nullptr;
		Eigen::Vector3d m_DrillEndPoints = Eigen::Vector3d(230.492, -85, -49.800);
		mitk::IRenderWindowPart* m_IRenderWindowPart;
		bool m_IsUseFemurRegistration = false;
		bool m_IsUseTibiaRegistration = false;
		PKADianaAimHardwareDevice* m_PKADianaAimHardwareDevice;
	};
}

#endif // !TOOLDISPLAYHELPER