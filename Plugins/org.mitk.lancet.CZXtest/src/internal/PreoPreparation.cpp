#include "PreoPreparation.h"

lancetAlgorithm::PreoPreparation::PreoPreparation(PKADianaAimHardwareDevice* pkaDianaAimHardwareDevice)
{
	m_PKADianaAimHardwareDevice = pkaDianaAimHardwareDevice;
}

/// <summary>
/// 计算得到相机坐标系下的髋关节中心  即PhipcenterInCamera
/// </summary>
/// <param name="bar">进度条</param>
/// <param name="error">计算得到的误差</param>
void lancetAlgorithm::PreoPreparation::SelectFemurPositions(QProgressBar* bar,double& error)
{
	FemurPositions.clear();
	auto addFemurPositionTask = [this,bar, &error]() {
		// Collect 100 positions asynchronously
		while (FemurPositions.size() < 100) {
			Eigen::Vector3d pos = CalculationHelper::GetTranslationFromMatrix4x4(PKAData::m_TCamera2FemurRF);
			bool flag = false;

			// Ensure the generated position is unique
			for (const auto& existingPos : FemurPositions) {
				if (pos.isApprox(existingPos, 1e-1)) {
					flag = true;
					break;
				}
			}

			if (!flag) {
				FemurPositions.push_back(pos);
				int posCount = FemurPositions.size();
				QMetaObject::invokeMethod(this, [this, bar, posCount]() {
					bar->setValue(posCount);
					}, Qt::QueuedConnection);
			}
			QThread::msleep(50); // Add a small delay to avoid blocking
		}

		// Show message box when done
		QMetaObject::invokeMethod(this, []() {
			QMessageBox msgBox;
			msgBox.setText(QString::fromLocal8Bit("Tip"));
			msgBox.setInformativeText(QString::fromLocal8Bit("采集完成"));
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			msgBox.exec();
			}, Qt::QueuedConnection);
		QMetaObject::invokeMethod(this, [this, &error]() {
			 Eigen::Vector4d center4d = CalculateHipCenter(FemurPositions);
			 Eigen::Vector3d center = Eigen::Vector3d(center4d[0], center4d[1], center4d[2]);
			 vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camaera = vtkSmartPointer<vtkMatrix4x4>::New();
			 TFemurRF2Camaera->DeepCopy(PKAData::m_TCamera2FemurRF);
			 TFemurRF2Camaera->Invert();
			 PKAData::m_IntraHipCenterInFemurRF = CalculationHelper::TransformByMatrix(center, TFemurRF2Camaera);
			double r = std::sqrt(center4d[0] * center4d[0] + center4d[1] * center4d[1]+ center4d[2]* center4d[2] + 1);
			error = 0;
			for (const auto& p : FemurPositions) {
				double dist = std::sqrt((p.x() - center.x()) * (p.x() - center.x()) +
					(p.y() - center.y()) * (p.y() - center.y()) +
					(p.z() - center.z()) * (p.z() - center.z()));
				error += (dist - r) * (dist - r);
			}
			error = std::sqrt(error / FemurPositions.size());
			}, Qt::QueuedConnection);
	};

	QtConcurrent::run(addFemurPositionTask);
}

Eigen::Vector4d lancetAlgorithm::PreoPreparation::CalculateHipCenter(std::vector<Eigen::Vector3d> positions)
{
	int n = positions.size();
	Eigen::MatrixXd A(n, 4);
	Eigen::VectorXd b(n);
	for (int i = 0; i < n; ++i)
	{
		const Eigen::Vector3d p = positions[i];
		A(i, 0) = 2 * p.x();
		A(i, 1) = 2 * p.y();
		A(i, 2) = 2 * p.z();
		A(i, 3) = 1;
		b(i) = p.squaredNorm();
	}
	Eigen::Vector4d x = A.colPivHouseholderQr().solve(b);
	return x;
}

Eigen::Vector3d lancetAlgorithm::PreoPreparation::GenerateFumurRFDataSimulated(double minRange, double maxRange)
{
	std::vector<Eigen::Vector3d> points;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(minRange, maxRange);

	Eigen::Vector3d point(dis(gen), dis(gen), dis(gen));
	return point;
}

void lancetAlgorithm::PreoPreparation::VerifyProbe(QPushButton* button, QProgressBar* bar, QLabel* label)
{
	if (m_IsVerifyProbe)
	{
		m_IsVerifyProbe = false;
		bar->setValue(0);
		button->setText(QString::fromLocal8Bit("探针验证"));
	}
	else
	{
		m_IsVerifyProbe = true;
		button->setText(QString::fromLocal8Bit("停止"));
		bar->setValue(0);
		m_ProbeRecordCount = 0;
		m_ProbePosVec.clear();
		if(!m_ProbeVerifyTimer)
		{
			m_ProbeVerifyTimer = new QTimer(this);
		}
		
		m_ProbeVerifyTimer->setInterval(50);
		m_ProbeVerifyTimer->start();

		m_ProbeVerifyConnection = m_ProbeVerifyTimer->callOnTimeout([=]()
			{
				if (!PKAData::m_VegaSource && PKAData::m_VegaSource->IsConnected())
					return;
				Eigen::Vector3d vec;
				auto vegaData = PKAData::m_VegaSource->GetOutput("Probe");
				vec[0] = vegaData->GetAffineTransform3D()->GetOffset()[0];
				vec[1] = vegaData->GetAffineTransform3D()->GetOffset()[1];
				vec[2] = vegaData->GetAffineTransform3D()->GetOffset()[2];

				m_ProbePosVec.push_back(vec);
				m_ProbeRecordCount++;
				bar->setValue(m_ProbeRecordCount);
				if (m_ProbeRecordCount > 100)
				{
					Eigen::Vector3d center{ 0,0,0 };
					for (auto data : m_ProbePosVec)
					{
						center += data;
					}
					center = center / m_ProbePosVec.size();

					double RMS = 0.0;
					//RMS 代表误差值
					for (auto data : m_ProbePosVec)
					{
						RMS += (data - center).norm();
					}
					RMS /= m_ProbePosVec.size();

					label->setText(QString::number(fabs(RMS)));
					m_IsVerifyProbe = false;
					button->setText(QString::fromLocal8Bit("探针验证"));
				}
			});
	}
}
