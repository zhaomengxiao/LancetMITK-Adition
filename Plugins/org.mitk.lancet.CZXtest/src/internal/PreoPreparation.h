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
/// ��ǰ׼��
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
		/// ���ݹɹǵ�������λ���ڿռ��н���������
		/// </summary>
		/// <returns>������λ��������µ�����</returns>
		Eigen::Vector4d CalculateHipCenter(std::vector<Eigen::Vector3d> positions);

		/// <summary>
		/// ���������Сֵ�����������ά�ռ��
		/// </summary>
		/// <param name="minRange">��Сֵ</param>
		/// <param name="maxRange">���ֵ</param>
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

