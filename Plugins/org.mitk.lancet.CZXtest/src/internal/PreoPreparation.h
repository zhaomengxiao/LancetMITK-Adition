#pragma once
#include <qmetaobject.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <QtConcurrent>
#include "PKADianaAimHardwareDevice.h"
#include "PKAData.h"
#include "CalculationHelper.h"

/// <summary>
/// 术前准备
/// </summary>

namespace lancetAlgorithm
{
	class PreoPreparation : public QObject
	{
	public:
		PreoPreparation(PKADianaAimHardwareDevice* pkaDianaAimHardwareDevice);
		void SelectFemurPositions(QProgressBar* bar, double& error);
		void VerifyProbe(QPushButton* button, QProgressBar* bar, QLabel* label);
	private:
		/// <summary>
		/// 根据股骨导向器的位置在空间中进行球的拟合
		/// </summary>
		/// <returns>髋中心位置在相机下的坐标</returns>
		Eigen::Vector4d CalculateHipCenter(std::vector<Eigen::Vector3d> positions);

		/// <summary>
		/// 根据最大最小值产生随机的三维空间点
		/// </summary>
		/// <param name="minRange">最小值</param>
		/// <param name="maxRange">最大值</param>
		/// <returns></returns>
		Eigen::Vector3d GenerateFumurRFDataSimulated(double minRange, double maxRange);

		

	private:
		std::vector<Eigen::Vector3d> FemurPositions;
		PKADianaAimHardwareDevice* m_PKADianaAimHardwareDevice;
		bool m_IsVerifyProbe = false;
		QTimer* m_ProbeVerifyTimer{ nullptr };
		std::vector<Eigen::Vector3d> m_ProbePosVec;
		int m_ProbeRecordCount = 0;
		QMetaObject::Connection m_ProbeVerifyConnection;
	};
}

