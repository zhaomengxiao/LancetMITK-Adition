#include "PKAPrePlanBoneModel.h"

using namespace lancetAlgorithm;

lancetAlgorithm::PKAPrePlanBoneModel::PKAPrePlanBoneModel(mitk::DataStorage* dataStorage)
{
	m_BoneTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_BoneCooridnate = vtkSmartPointer<vtkMatrix4x4>::New();
	m_AxesActorMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	m_DataStorage = dataStorage;
}

void PKAPrePlanBoneModel::BuildFemurModel()
{
	//��������
	CalculateFemurMechanicalAxis(m_DataStorage);
	CalculateFemuralAcrocondylarAxis(m_DataStorage);
	
	//��������
	Eigen::Vector3d femoralCenter = ((PKAData::m_MedialFemuralEpicondyle + PKAData::m_LateralFemuralEpicondyle) / 2);
	PrintDataHelper::CoutArray(femoralCenter, "BuildFemurModel::femoralCenter");
	//�ֲ�����ϵZ��Ϊcenter��hipCenter
	Eigen::Vector3d zAxis = CalculationHelper::CalculateDirection(femoralCenter, PKAData::m_HipCenter);
	PrintDataHelper::CoutArray(zAxis, "BuildFemurModel::zAxis");
	Eigen::Vector3d yAxis = CalculationHelper::ComputeNormal(PKAData::m_HipCenter, PKAData::m_MedialFemuralEpicondyle, PKAData::m_LateralFemuralEpicondyle);
	PrintDataHelper::CoutArray(yAxis, "BuildFemurModel::yAxis");
	// ���� X ���������� kneeCenter �� lateralEpicondyle��
	Eigen::Vector3d xAxis = yAxis.cross(zAxis);
	xAxis.normalize();

	// ���� Z ���������� MITK ��������ϵ Z ��ͬ��

	for (int i = 0; i < 3; ++i) {
		m_BoneCooridnate->SetElement(i, 0, xAxis[i]);
		m_BoneCooridnate->SetElement(i, 1, yAxis[i]);
		m_BoneCooridnate->SetElement(i, 2, zAxis[i]);
		m_BoneCooridnate->SetElement(i, 3, femoralCenter[i]);  // ƽ�Ʋ���
	}
	m_BoneCooridnate->SetElement(3, 3, 1.0);  // ���һ������Ϊ (0, 0, 0, 1) ��Ϊ��ת�����һ����

	PrintDataHelper::CoutMatrix("PKAPrePlanBoneModel::BuildFemurModel: ", m_BoneCooridnate);
	m_AxesActor = vtkSmartPointer<vtkAxesActor>::New();
	m_AxesActor = PKARenderHelper::GenerateAxesActor();
	m_AxesActorMatrix->DeepCopy(this->GetBoneModelCoordinate());
	m_AxesActor->SetUserMatrix(m_AxesActorMatrix);
}

void PKAPrePlanBoneModel::BuildTibiaModel()
{
	//��������
	CalculateTibiaMechanicalAxis(m_DataStorage);
	CalculateTibialAcrocondylarAxis(m_DataStorage);

	//����Tibia �ֲ�����ϵ  TODO
	Eigen::Vector3d tibiaEpicondyleCenter = CalculationHelper::TwoPointCenter(PKAData::m_MedialTibialEpicondyle, PKAData::m_LateralTibialEpicondyle);
	Eigen::Vector3d tibialMalleolusCenter = CalculationHelper::TwoPointCenter(PKAData::m_MedialMalleolus, PKAData::m_LateralMalleolus);
	//�ֲ�����ϵZ��Ϊcenter��hipCenter
	Eigen::Vector3d zAxis = CalculationHelper::CalculateDirection(tibiaEpicondyleCenter, tibialMalleolusCenter);

	Eigen::Vector3d yAxis = CalculationHelper::ComputeNormal(tibiaEpicondyleCenter, PKAData::m_MedialMalleolus, PKAData::m_LateralMalleolus);

	// ���� X ���������� kneeCenter �� lateralEpicondyle��
	Eigen::Vector3d xAxis = yAxis.cross(zAxis);
	xAxis.normalize();

	for (int i = 0; i < 3; ++i) {
		m_BoneCooridnate->SetElement(i, 0, xAxis[i]);
		m_BoneCooridnate->SetElement(i, 1, yAxis[i]);
		m_BoneCooridnate->SetElement(i, 2, zAxis[i]);
		m_BoneCooridnate->SetElement(i, 3, tibiaEpicondyleCenter[i]);  // ƽ�Ʋ���
	}
	m_BoneCooridnate->SetElement(3, 3, 1.0);  // ���һ������Ϊ (0, 0, 0, 1) ��Ϊ��ת�����һ����
	PrintDataHelper::CoutMatrix("PKAPrePlanBoneModel::BuildTibiaModel: ", m_BoneCooridnate);
	m_AxesActor = vtkSmartPointer<vtkAxesActor>::New();
	m_AxesActor = PKARenderHelper::GenerateAxesActor();
	m_AxesActorMatrix->DeepCopy(this->GetBoneModelCoordinate());
	m_AxesActor->SetUserMatrix(m_AxesActorMatrix);
}

/// <summary>
/// ����ɹǲ���Ҫ�任
/// </summary>
/// <returns></returns>
vtkSmartPointer<vtkMatrix4x4> PKAPrePlanBoneModel::GetBoneModelCoordinate()
{
	return m_BoneCooridnate;
}

vtkAxesActor* lancetAlgorithm::PKAPrePlanBoneModel::GetBoneAxes()
{
	return m_AxesActor;
}

void PKAPrePlanBoneModel::CalculateTibiaMechanicalAxis(mitk::DataStorage* dataStorage)
{
	auto tibiaKneeCenterPointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_TibiaKneeCenterNodeName.toStdString())->GetData());

	auto medialMalleolusPointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_MedialMalleolusNodeName.toStdString())->GetData());

	auto lateralMalleolusPointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_LateralMalleolusNodeName.toStdString())->GetData());


	auto tibiaCenterPoint = tibiaKneeCenterPointSet->GetPoint(0);
	auto medialMalleolusPoint = medialMalleolusPointSet->GetPoint(0);
	auto lateralMalleolusPoint = lateralMalleolusPointSet->GetPoint(0);

	PKAData::m_TibiaKneeCenter = Eigen::Vector3d(tibiaCenterPoint[0], tibiaCenterPoint[1], tibiaCenterPoint[2]);
	PKAData::m_MedialMalleolus = Eigen::Vector3d(medialMalleolusPoint[0], medialMalleolusPoint[1], medialMalleolusPoint[2]);
	PKAData::m_LateralMalleolus = Eigen::Vector3d(lateralMalleolusPoint[0], lateralMalleolusPoint[1], lateralMalleolusPoint[2]);

	PKAData::m_MalleolusCenter = CalculationHelper::TwoPointCenter(PKAData::m_MedialMalleolus, PKAData::m_LateralMalleolus);

	PKAData::m_TibiaMechanicalAxis = CalculationHelper::CalculateDirection(PKAData::m_MalleolusCenter, PKAData::m_TibiaKneeCenter);

	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, PKAData::m_TibiaMechanicalAxis, "TibiaMechanicalAxis: ");
}

void PKAPrePlanBoneModel::CalculateFemurMechanicalAxis(mitk::DataStorage* dataStorage)
{
	auto femurKneeCenterPointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_FemurKneeCenterNodeName.toStdString())->GetData());

	auto hipCenterPointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_HipCenterNodeName.toStdString())->GetData());

	auto femurCenterPoint = femurKneeCenterPointSet->GetPoint(0);
	auto hipCenterPoint = hipCenterPointSet->GetPoint(0);
	PKAData::m_FemuralKneeCenter = Eigen::Vector3d(femurCenterPoint[0], femurCenterPoint[1], femurCenterPoint[2]);
	PKAData::m_HipCenter = Eigen::Vector3d(hipCenterPoint[0], hipCenterPoint[1], hipCenterPoint[2]);
	PKAData::m_FemurMechanicalAxis = CalculationHelper::CalculateDirection(PKAData::m_FemuralKneeCenter, PKAData::m_HipCenter);
	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, PKAData::m_FemurMechanicalAxis, "FemurMechanicalAxis: ");
}

void PKAPrePlanBoneModel::CalculateFemuralAcrocondylarAxis(mitk::DataStorage* dataStorage)
{
	auto lateralEpicondylePointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_LateralFemuralEpicondyleNodeName.toStdString())->GetData());
	auto medialEpicondylePointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_MedialFemuralEpicondyleNodeName.toStdString())->GetData());

	auto lateralEpicondylePoint = lateralEpicondylePointSet->GetPoint(0);
	auto medialEpicondylePoint = medialEpicondylePointSet->GetPoint(0);

	PKAData::m_LateralFemuralEpicondyle = Eigen::Vector3d(lateralEpicondylePoint[0], lateralEpicondylePoint[1], lateralEpicondylePoint[2]);
	PKAData::m_MedialFemuralEpicondyle = Eigen::Vector3d(medialEpicondylePoint[0], medialEpicondylePoint[1], medialEpicondylePoint[2]);
	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, PKAData::m_LateralFemuralEpicondyle, "LateralFemuralEpicondyle: ");
	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, PKAData::m_MedialFemuralEpicondyle, "MedialFemuralEpicondyle: ");
	PKAData::m_FemuralAcrocondylarAxis = CalculationHelper::CalculateDirection(PKAData::m_MedialFemuralEpicondyle, PKAData::m_LateralFemuralEpicondyle);
	//PrintDataHelper::AppendTextBrowserArray(m_Controls.textBrowser, PKAData::m_FemuralAcrocondylarAxis, "FemuralAcrocondylarAxis: ");
}

void PKAPrePlanBoneModel::CalculateTibialAcrocondylarAxis(mitk::DataStorage* dataStorage)
{
	auto lateralEpicondylePointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_LateralTibialEpicondyleNodeName.toStdString())->GetData());
	auto medialEpicondylePointSet = dynamic_cast<mitk::PointSet*>(dataStorage->GetNamedNode(PKAData::m_MedialTibialEpicondyleNodeName.toStdString())->GetData());
	auto lateralEpicondylePoint = lateralEpicondylePointSet->GetPoint(0);
	auto medialEpicondylePoint = medialEpicondylePointSet->GetPoint(0);

	PKAData::m_LateralTibialEpicondyle = Eigen::Vector3d(lateralEpicondylePoint[0], lateralEpicondylePoint[1], lateralEpicondylePoint[2]);
	PKAData::m_MedialTibialEpicondyle = Eigen::Vector3d(medialEpicondylePoint[0], medialEpicondylePoint[1], medialEpicondylePoint[2]);
	PKAData::m_TibialAcrocondylarAxis = CalculationHelper::CalculateDirection(PKAData::m_MedialTibialEpicondyle, PKAData::m_LateralTibialEpicondyle);
}

Eigen::Vector3d lancetAlgorithm::PKAPrePlanBoneModel::GetBoneSagittalPlaneNormal()
{
	Eigen::Vector3d sagittalNormalPlane = CalculationHelper::GetXAxisFromVTKMatrix(this->GetBoneModelCoordinate());
	return sagittalNormalPlane;
}

Eigen::Vector3d lancetAlgorithm::PKAPrePlanBoneModel::GetMechanicalAxis()
{
	Eigen::Vector3d mechanical = CalculationHelper::GetYAxisFromVTKMatrix(this->GetBoneModelCoordinate());
	return mechanical;
}

Eigen::Vector3d lancetAlgorithm::PKAPrePlanBoneModel::GetMedialPosteriorPoint()
{
	return Eigen::Vector3d();
}

Eigen::Vector3d lancetAlgorithm::PKAPrePlanBoneModel::GetLateralPosteriorPoint()
{
	return Eigen::Vector3d();
}

vtkMatrix4x4* lancetAlgorithm::PKAPrePlanBoneModel::GetBoneTransform()
{
	return m_BoneTransformMatrix;
}

void lancetAlgorithm::PKAPrePlanBoneModel::UpdateBoneAxesActor(vtkMatrix4x4* matrix)
{
	m_AxesActorMatrix->DeepCopy(matrix);
	m_AxesActor->SetUserMatrix(m_AxesActorMatrix);
}

