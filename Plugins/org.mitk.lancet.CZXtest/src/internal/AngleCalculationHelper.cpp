#include "AngleCalculationHelper.h"

using namespace lancetAlgorithm;
AngleCalculationHelper::AngleCalculationHelper(mitk::DataStorage* in)
{
	m_dataStorage = in;
	m_vtkIntersectionPolyDataFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
}

void lancetAlgorithm::AngleCalculationHelper::BuildFemurCalculator(PKAPrePlanBoneModel* bone, ChunLiXGImplant* implant)
{
	m_FemurModel = bone;
	m_FemurImplant = implant;
}

void lancetAlgorithm::AngleCalculationHelper::BuildTibiaCalculator(PKAPrePlanBoneModel* bone, ChunLiTray* tray)
{
	m_TibiaModel = bone;
	m_TibiaTray = tray;
}

std::pair<Tilt, double> AngleCalculationHelper::CalculateTilt(KneeModel kneeModel)
{
	vtkSmartPointer<vtkMatrix4x4> TLocal2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	Eigen::Vector3d localV;
	Eigen::Vector2d vYZ;
	double angle = 0.0;
	if (kneeModel == KneeModel::Femur)
	{
		TLocal2Image->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		//将图像坐标系下的力线向量转换到局部坐标系下 只用转matrix
		localV = CalculationHelper::CalculateInCoordinate(m_FemurImplant->GetMechanicalAxis(), TLocal2Image);
		PrintDataHelper::CoutArray(m_FemurImplant->GetMechanicalAxis(), "m_Implant->GetMechanicalAxis(): ");
	}
	else
	{
		TLocal2Image->DeepCopy(m_TibiaModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		//将图像坐标系下的力线向量转换到局部坐标系下 只用转matrix
		localV = CalculationHelper::CalculateInCoordinate(m_TibiaTray->GetProximalDirection(), TLocal2Image);
		//PrintDataHelper::CoutArray(m_TibiaModel->GetFemurMechanicalAxis(), "m_TibiaModel->GetMechanicalAxis(): ");
	}
	PrintDataHelper::CoutArray(localV, "CalculateTilt localV: ");
	localV.normalize();
	// 向量在YZ平面的投影
	vYZ = Eigen::Vector2d(localV.y(), localV.z());
	// 计算投影向量与Z轴的夹角
	angle = std::atan2(vYZ.x(), vYZ.y());
	std::cout << "CalculateTilt Angle:" << angle << std::endl;

	// 将弧度转换为角度
	angle = angle * 180.0 / PI;
	Tilt tilt = (vYZ.x() >= 0) ? Tilt::FrontTilt : Tilt::BackTilt;
	return { tilt,std::abs(angle) };
}

std::pair<Va, double> lancetAlgorithm::AngleCalculationHelper::CalculateVa(KneeModel kneeModel)
{
	Eigen::Vector3d localV;
	vtkSmartPointer<vtkMatrix4x4> TLocal2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	if (kneeModel == KneeModel::Femur)
	{
		TLocal2Image->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		localV = CalculationHelper::CalculateInCoordinate(m_FemurImplant->GetMechanicalAxis(), TLocal2Image);
	}
	else
	{
		TLocal2Image->DeepCopy(m_TibiaModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		localV = CalculationHelper::CalculateInCoordinate(m_TibiaTray->GetProximalDirection(), TLocal2Image);
	}
	localV.normalize();
	// 向量在XZ平面的投影
	Eigen::Vector2d vXZ(localV.x(), localV.z());

	// 计算投影向量与Z轴的夹角
	double angle = std::atan2(vXZ.x(), vXZ.y());

	// 将角度转换为度数（可选）
	angle = angle * 180.0 / PI;
	Va va = (vXZ.x() > 0) ? Va::Valgus : Va::Varus;
	return { va,std::abs(angle) };
}

std::pair<ProsRotation, double> lancetAlgorithm::AngleCalculationHelper::CalculateRotation(KneeModel kneeModel)
{
	Eigen::Vector3d localV;
	vtkSmartPointer<vtkMatrix4x4> TLocal2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	if (kneeModel == KneeModel::Femur)
	{
		TLocal2Image->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		localV = CalculationHelper::CalculateInCoordinate(m_FemurImplant->GetOrientation(), TLocal2Image);
	}
	else
	{
		TLocal2Image->DeepCopy(m_TibiaModel->GetBoneModelCoordinate());
		TLocal2Image->Invert();
		localV = CalculationHelper::CalculateInCoordinate(m_TibiaTray->GetOrientation(), TLocal2Image);
	}
	localV.normalize();
	// 向量在XY平面的投影
	Eigen::Vector2d vXY(localV.x(), localV.y());

	// 计算投影向量与X轴的夹角
	double angle = std::atan2(vXY.y(), vXY.x());

	// 将角度转换为度数（可选）
	angle = angle * 180.0 / PI;
	ProsRotation prosRotation = vXY.x() > 0 ? ProsRotation::IntenalRotation : ProsRotation::ExternalRotation;
	return { prosRotation, std::abs(angle) };
}

double lancetAlgorithm::AngleCalculationHelper::CalculateIntraplanProsGap()
{
	//获得胫骨proud在图像坐标系下的位置
	vtkSmartPointer<vtkMatrix4x4> TImage2Tibia = vtkSmartPointer<vtkMatrix4x4>::New();
	TImage2Tibia->DeepCopy(CalculateTFemur2Tibia());
	
	Eigen::Vector3d tibiaProudInImage = CalculationHelper::TransformByMatrix(m_TibiaTray->GetProudPoint(),TImage2Tibia);

	//获得股骨gap points 在图像坐标系下的位置 图像坐标系下胫骨的位置时I
	auto points = m_FemurImplant->GetGapPoints();
	double minDistance = std::numeric_limits<double>::max();
	double index = 0;
	for (int i = 0; i < points.size(); ++i)
	{
		//double distance =  std::abs(tibiaProudInImage.z()- points[i].z())/2;
		double distance = CalculationHelper::CalculateTwoPointsDistance(tibiaProudInImage, points[i]);
		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}
	//Eigen::Vector3d 
	if (tibiaProudInImage.z() > points[index].z())
	{
		minDistance = -minDistance;
	}
	return minDistance;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::AngleCalculationHelper::CalculateTCoordinate2Pros(KneeModel kneeModel)
{
	vtkSmartPointer<vtkMatrix4x4> TImage2Local = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TPros2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PreMultiply();
	vtkSmartPointer<vtkMatrix4x4> TCoordinate2Pros = vtkSmartPointer<vtkMatrix4x4>::New();
	if (KneeModel::Femur == kneeModel)
	{
		TImage2Local->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
		TPros2Image->DeepCopy(m_FemurImplant->GetImplantMatrix());
		TPros2Image->Invert();
		transform->SetMatrix(TPros2Image);
		transform->Concatenate(TImage2Local);
	}
	else
	{
		TImage2Local->DeepCopy(m_TibiaModel->GetBoneModelCoordinate());
		TPros2Image->DeepCopy(m_TibiaTray->GetTrayMatrix());
		TPros2Image->Invert();
		transform->SetMatrix(TPros2Image);
		transform->Concatenate(TImage2Local);
	}
	transform->Update();
	transform->GetMatrix(TCoordinate2Pros);
	TCoordinate2Pros->Invert();
	//PrintDataHelper::CoutMatrix("CalculateTCoordinate2Pros:", TCoordinate2Pros);
	return TCoordinate2Pros;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::AngleCalculationHelper::SetTiltAngle(Tilt tilt, double angle, KneeModel kneeModel)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> TImage2NewPros = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->PostMultiply();
	transform->Identity();
	Eigen::Vector3d direction; 
	Eigen::Vector3d center;
	if (kneeModel == KneeModel::Femur)
	{
		direction = CalculationHelper::GetXAxisFromVTKMatrix(m_FemurModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_FemurImplant->GetImplantMatrix());
		Eigen::Vector3d center = CalculationHelper::GetTranslationFromMatrix4x4(m_FemurImplant->GetImplantMatrix());
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	else
	{
		direction = CalculationHelper::GetXAxisFromVTKMatrix(m_TibiaModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_TibiaTray->GetTrayMatrix());
		center = CalculationHelper::GetTranslationFromMatrix4x4(m_TibiaTray->GetTrayMatrix());
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	transform->Update();
	transform->GetMatrix(TImage2NewPros);
	//PrintDataHelper::CoutMatrix("SetTiltAngle", TImage2NewPros);
	return TImage2NewPros;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::AngleCalculationHelper::SetVaAngle(Va va, double angle, KneeModel kneeModel)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> TImage2NewPros = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->PostMultiply();
	transform->Identity();
	Eigen::Vector3d direction;
	Eigen::Vector3d center;
	if (kneeModel == KneeModel::Femur)
	{
		direction = CalculationHelper::GetYAxisFromVTKMatrix(m_FemurModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_FemurImplant->GetImplantMatrix());
		Eigen::Vector3d center = CalculationHelper::GetTranslationFromMatrix4x4(m_FemurImplant->GetImplantMatrix());
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	else
	{
		direction = CalculationHelper::GetYAxisFromVTKMatrix(m_TibiaModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_TibiaTray->GetTrayMatrix());
		center = CalculationHelper::GetTranslationFromMatrix4x4(m_TibiaTray->GetTrayMatrix());
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	transform->Update();
	transform->GetMatrix(TImage2NewPros);
	//PrintDataHelper::CoutMatrix("SetVaAngle", TImage2NewPros);
	return TImage2NewPros;
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::AngleCalculationHelper::SetProsRotationAngle(ProsRotation prosRotation, double angle, KneeModel kneeModel)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> TImage2NewPros = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->PostMultiply();
	transform->Identity();
	Eigen::Vector3d direction;
	Eigen::Vector3d center;
	if (kneeModel == KneeModel::Femur)
	{
		direction = CalculationHelper::GetZAxisFromVTKMatrix(m_FemurModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_FemurImplant->GetImplantMatrix());
		Eigen::Vector3d center = CalculationHelper::GetTranslationFromMatrix4x4(m_FemurImplant->GetImplantMatrix());
		//PrintDataHelper::CoutArray(center, "SetProsRotationAngle center");
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	else
	{
		direction = CalculationHelper::GetZAxisFromVTKMatrix(m_TibiaModel->GetBoneModelCoordinate());

		matrix->DeepCopy(m_TibiaTray->GetTrayMatrix());
		center = CalculationHelper::GetTranslationFromMatrix4x4(m_TibiaTray->GetTrayMatrix());
		transform->SetMatrix(matrix);
		transform->Translate(-center[0], -center[1], -center[2]);
		transform->RotateWXYZ(angle, direction.data());
		transform->Translate(center.data());
	}
	transform->Update();
	transform->GetMatrix(TImage2NewPros);
	//PrintDataHelper::CoutMatrix("SetRotationAngle", TImage2NewPros);
	return TImage2NewPros;
}

double lancetAlgorithm::AngleCalculationHelper::CalculateProud(KneeModel kneeModel)
{
	double result = std::numeric_limits<double>::max();
	Eigen::Vector3d closestPoint;
	Eigen::Vector3d proudPoint;
	Eigen::Vector3d proudDirection;
	mitk::Surface* surface;
	vtkSmartPointer<vtkPolyData> bonePolyData = vtkSmartPointer<vtkPolyData>::New();
	if (kneeModel == KneeModel::Femur)
	{
		proudPoint = m_FemurImplant->GetProudPoint();
		proudDirection = m_FemurImplant->GetProudDirection();
		surface = dynamic_cast<mitk::Surface*>(m_dataStorage->GetNamedNode(PKAData::m_FemurSurfaceNodeName.toStdString())->GetData());
		bonePolyData->DeepCopy(surface->GetVtkPolyData());
	}
	else
	{
		proudPoint = m_TibiaTray->GetProudPoint(); 
		proudDirection = m_TibiaTray->GetProudDirection();
		surface = dynamic_cast<mitk::Surface*>(m_dataStorage->GetNamedNode(PKAData::m_TibiaSurfaceNodeName.toStdString())->GetData());
		bonePolyData->DeepCopy(surface->GetVtkPolyData());
	}
	//PrintDataHelper::CoutArray(proudPoint, "proudPoint: ");
	//PrintDataHelper::CoutArray(proudDirection, "proudDirection: ");
	mitk::DataNode* node = m_dataStorage->GetNamedNode(m_ProudLine);
	if (node)
	{
		m_dataStorage->Remove(node);
	}
	// 计算射线的终点
	double point1[3], point2[3];
	point1[0] = proudPoint[0] - proudDirection[0] * 500;
	point1[1] = proudPoint[1] - proudDirection[1] * 500;
	point1[2] = proudPoint[2] - proudDirection[2] * 500;
	point2[0] = proudPoint[0] + proudDirection[0] * 500; // 确保线足够长
	point2[1] = proudPoint[1] + proudDirection[1] * 500;
	point2[2] = proudPoint[2] + proudDirection[2] * 500;
	//PrintDataHelper::CoutArray(point1, 3, "point1:");
	//PrintDataHelper::CoutArray(point2, 3, "point2:");
	double color[3] = { 1,0,0 };
	vtkSmartPointer<vtkPolyData> linePolyData = vtkSmartPointer<vtkPolyData>::New();
	linePolyData->DeepCopy(PKARenderHelper::DisplayDirection(m_dataStorage, proudPoint,
		proudDirection, m_ProudLine.c_str(), color, 100));

	vtkSmartPointer<vtkModifiedBSPTree> bspTree = vtkSmartPointer<vtkModifiedBSPTree>::New();
	bspTree->SetDataSet(bonePolyData);
	bspTree->Update();
	bspTree->BuildLocator();

	// 存储交点的数组
	vtkSmartPointer<vtkPoints> intersectionPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();

	// 进行交点查询
	int numIntersections = bspTree->IntersectWithLine(point1, point2, 0.001, intersectionPoints, cellIds);/*(point1, point2, 0.001, intersectionPoints, cellIds, cell);*/

	// 输出结果
	std::cout << "Number of intersections: " << numIntersections << std::endl;

	for (vtkIdType i = 0; i < intersectionPoints->GetNumberOfPoints(); i++)
	{
		double intersection[3];
		intersectionPoints->GetPoint(i, intersection);
		PrintDataHelper::CoutArray(intersection, 3, "vtkModifiedBSPTree Intersection: ");
		double distance = CalculationHelper::CalculateTwoPointsDistance(proudPoint, Eigen::Vector3d(intersection));
		if (result > distance)
		{
			result = distance;
			std::cout << "distance" << distance << std::endl;
			closestPoint = Eigen::Vector3d(intersection);
		}
	}

	Eigen::Vector3d directionVec = proudPoint - closestPoint;
	//PrintDataHelper::CoutArray(closestPoint, "closestPoint");
	//PrintDataHelper::CoutArray(proudPoint, "proudPoint");
	double dotProduct = directionVec.dot(proudDirection);

	return dotProduct > 0 ? result : -result;
}

lancetAlgorithm::AngleCalculationHelper::~AngleCalculationHelper()
{
	if (m_FemurModel != nullptr)
	{
		delete m_FemurModel;
		m_FemurModel = nullptr;
	}

	if (m_FemurImplant != nullptr)
	{
		delete m_FemurImplant;
		m_FemurImplant = nullptr;
	}

	if (m_dataStorage != nullptr)
	{
		m_dataStorage->Delete();
		m_dataStorage = nullptr;
	}
}

void lancetAlgorithm::AngleCalculationHelper::CalculateIntrPlanning()
{
}

std::pair<double, double> lancetAlgorithm::AngleCalculationHelper::CalculateLimbFlexionAndVarus()
{
	if (m_FemurModel == nullptr)
	{
		std::cout << "femur null ptr";
		return { 0,0 };
	}
	if (m_TibiaModel == nullptr)
	{
		std::cout << "tibia null ptr";
		return  { 0,0 };
	}

	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurLocal2FemurImage->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
	TFemurLocal2FemurImage->Invert();
	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2TibiaLocal = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkSmartPointer<vtkTransform> femurLocal2TibiaLocalTransform = vtkSmartPointer<vtkTransform>::New();
	femurLocal2TibiaLocalTransform->PreMultiply();           
	femurLocal2TibiaLocalTransform->SetMatrix(TFemurLocal2FemurImage); //TFemurLocal2FemurImage
	femurLocal2TibiaLocalTransform->Concatenate(this->CalculateTFemur2Tibia()); //TFemur2Tibia
	femurLocal2TibiaLocalTransform->Concatenate(this->m_TibiaModel->GetBoneModelCoordinate()); //TTibiaImage2TibiaLocal
	femurLocal2TibiaLocalTransform->GetMatrix(TFemurLocal2TibiaLocal);

	Eigen::Vector3d tibiaMechanicalAxisInTibia(0, 0, 1);
	Eigen::Vector3d tibiaMechanicalAxisInFemur = CalculationHelper::TransformByMatrix(tibiaMechanicalAxisInTibia, TFemurLocal2TibiaLocal);

	Eigen::Vector3d femurSagittalPlaneNormal;

	femurSagittalPlaneNormal = m_FemurModel->GetBoneSagittalPlaneNormal();

	Eigen::Vector3d origin(0, 0, 0);
	
	//投影在YZ面上 
	Eigen::Vector2d tibiaMechanicalYZ = Eigen::Vector2d(tibiaMechanicalAxisInFemur.y(), tibiaMechanicalAxisInFemur.z());
	// 计算投影向量与Z轴的夹角
	double ququ = std::atan2(tibiaMechanicalYZ.x(), tibiaMechanicalYZ.y());
	//std::cout << "CalculateTilt Angle:" << ququ << std::endl;

	// 将弧度转换为角度
	ququ = ququ * 180.0 / PI;
	//下肢屈曲
	//std::cout << "xia zhi qu qu : " << ququ;

	//下肢内外翻
	//投影在XZ面上 
	Eigen::Vector2d tibiaMechanicalXZ = Eigen::Vector2d(tibiaMechanicalAxisInFemur.x(), tibiaMechanicalAxisInFemur.y());
	// 计算投影向量与Z轴的夹角
	double va = std::atan2(tibiaMechanicalXZ.x(), tibiaMechanicalXZ.y());

	// 将角度转换为度数（可选）
	va = va * 180.0 / PI;
	//Va va = (tibiaMechanicalXZ.x() > 0) ? Va::Valgus : Va::Varus;
	//std::cout << "xia zhi nei wai fan: " << va;
	return { ququ, va };
}


std::pair<Tilt, double> lancetAlgorithm::AngleCalculationHelper::CalculateLimbFlexion()
{
	if (m_FemurModel == nullptr)
	{
		std::cout << "femur null ptr";
		return {Tilt::BackTilt, 0.0};
	}
	if (m_TibiaModel == nullptr)
	{
		std::cout << "tibia null ptr";
		return { Tilt::BackTilt, 0.0 };
	}

	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurLocal2FemurImage->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
	//PrintDataHelper::CoutMatrix("TFemurLocal2FemurImage", TFemurLocal2FemurImage);
	TFemurLocal2FemurImage->Invert();
	//PrintDataHelper::CoutMatrix("TFemurLocal2FemurImage", TFemurLocal2FemurImage);
	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2TibiaLocal = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkSmartPointer<vtkMatrix4x4> TFemur2Tibia = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> TTibiaBone2TibiaLocal = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemur2Tibia->DeepCopy(this->CalculateTFemur2Tibia());
	TTibiaBone2TibiaLocal->DeepCopy(this->m_TibiaModel->GetBoneModelCoordinate());
	TFemurLocal2TibiaLocal->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TFemurLocal2FemurImage, TFemur2Tibia, TTibiaBone2TibiaLocal));
	//PrintDataHelper::CoutMatrix("TFemurLocal2TibiaLocal", TFemurLocal2TibiaLocal);
	Eigen::Vector3d tibiaMechanicalAxisInTibia(0, 0, 1);
	Eigen::Vector3d tibiaMechanicalAxisInFemur = CalculationHelper::TransformByMatrix(tibiaMechanicalAxisInTibia, TFemurLocal2TibiaLocal);

	Eigen::Vector3d femurSagittalPlaneNormal = m_FemurModel->GetBoneSagittalPlaneNormal();
	//PrintDataHelper::CoutArray(femurSagittalPlaneNormal, "CalculateLimbFlexion: ");
	//投影在YZ面上 
	Eigen::Vector2d tibiaMechanicalYZ = Eigen::Vector2d(tibiaMechanicalAxisInFemur.y(), tibiaMechanicalAxisInFemur.z());
	// 计算投影向量与Z轴的夹角
	double rad = std::atan2(tibiaMechanicalYZ.x(), tibiaMechanicalYZ.y());

	// 将弧度转换为角度
	double angle = rad * 180.0 / PI;
	//std::cout << "angle: " << angle << std::endl;
	//下肢屈曲
	Tilt tilt = tibiaMechanicalYZ.x() > 0 ? Tilt::FrontTilt : Tilt::BackTilt;
	return{ tilt, std::abs( angle) };
}

std::pair<Va, double> lancetAlgorithm::AngleCalculationHelper::CalculateLimbVarus()
{
	if (m_FemurModel == nullptr)
	{
		std::cout << "femur null ptr";
		return { Va::Varus ,0.0 };
	}
	if (m_TibiaModel == nullptr)
	{
		std::cout << "tibia null ptr";
		return { Va::Varus ,0.0 };
	}

	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2FemurImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurLocal2FemurImage->DeepCopy(m_FemurModel->GetBoneModelCoordinate());
	TFemurLocal2FemurImage->Invert();
	vtkSmartPointer<vtkMatrix4x4> TFemurLocal2TibiaLocal = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> femurLocal2TibiaLocalTransform = vtkSmartPointer<vtkTransform>::New();
	femurLocal2TibiaLocalTransform->PreMultiply();
	femurLocal2TibiaLocalTransform->SetMatrix(TFemurLocal2FemurImage); //TFemurLocal2FemurImage
	femurLocal2TibiaLocalTransform->Concatenate(this->CalculateTFemur2Tibia()); //TFemur2Tibia
	femurLocal2TibiaLocalTransform->Concatenate(this->m_TibiaModel->GetBoneModelCoordinate()); //TTibiaImage2TibiaLocal
	femurLocal2TibiaLocalTransform->GetMatrix(TFemurLocal2TibiaLocal);

	Eigen::Vector3d tibiaMechanicalAxisInTibia(1, 0, 0);
	Eigen::Vector3d tibiaMechanicalAxisInFemur = CalculationHelper::TransformByMatrix(tibiaMechanicalAxisInTibia, TFemurLocal2TibiaLocal);

	Eigen::Vector3d femurSagittalPlaneNormal;

	femurSagittalPlaneNormal = m_FemurModel->GetBoneSagittalPlaneNormal();

	//下肢内外翻
	//投影在XY面上 
	Eigen::Vector2d tibiaMechanicalXZ = Eigen::Vector2d(tibiaMechanicalAxisInFemur.x(), tibiaMechanicalAxisInFemur.y());
	// 计算投影向量与Z轴的夹角
	double rad = std::atan2(tibiaMechanicalXZ.x(), tibiaMechanicalXZ.y());

	// 将角度转换为度数（可选）
	double angle = rad * 180.0 / PI;
	Va va = (tibiaMechanicalXZ.y() > 0) ? Va::Valgus : Va::Varus;
	//std::cout << "xia zhi nei wai fan: " << angle;
	return { va, std::abs(angle) };
}

vtkSmartPointer<vtkMatrix4x4> lancetAlgorithm::AngleCalculationHelper::CalculateTFemur2Tibia()
{
	vtkSmartPointer<vtkMatrix4x4> TFemur2Tibia = vtkSmartPointer<vtkMatrix4x4>::New();

	vtkSmartPointer<vtkMatrix4x4> TFemur2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemur2FemurRF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
	TFemur2FemurRF->Invert();

	//std::cout << "CalculateTFemur2Tibia" << std::endl;
	vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
	TFemurRF2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2TibiaRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2TibiaRF->DeepCopy(PKAData::m_TCamera2TibiaRF);

	vtkSmartPointer<vtkMatrix4x4> TTibiaRF2Tibia = vtkSmartPointer<vtkMatrix4x4>::New();
	TTibiaRF2Tibia->DeepCopy(PKAData::m_TTibiaRF2TibiaImage);

	//最后得到Femur 到Tibia的转换关系
	TFemur2Tibia->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TFemur2FemurRF, TFemurRF2Camera, TCamera2TibiaRF, TTibiaRF2Tibia));
	return TFemur2Tibia;
}

/// <summary>
/// 显示被假体切割后的模型
/// </summary>
void lancetAlgorithm::AngleCalculationHelper::ClipFemur()
{

}

void lancetAlgorithm::AngleCalculationHelper::ClipTibia()
{
}

//将假体都转换到股骨图像坐标系下
double lancetAlgorithm::AngleCalculationHelper::CalculateCutPlaneDistance()
{
	if (!m_FemurImplant || !m_TibiaTray)
		return 0.0;
	vtkSmartPointer<vtkMatrix4x4> TFemurImage2TibiaImage = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurImage2TibiaImage->DeepCopy(this->CalculateTFemur2Tibia());
	Eigen::Vector3d tibiaTrayProximalCut = m_TibiaTray->GetProximal();
	Eigen::Vector3d tibiaTrayProximalCutNormal = m_TibiaTray->GetProximalNormal();
	double result;
	Eigen::Vector3d tibiaTrayProximalCutInFemurImage = CalculationHelper::TransformByMatrix(tibiaTrayProximalCut, TFemurImage2TibiaImage);
	Eigen::Vector3d tibiaTrayProximalCutNormalInFemurImage = CalculationHelper::TransformByMatrix(tibiaTrayProximalCutNormal, TFemurImage2TibiaImage);
	Eigen::Vector3d tibiaTrayProximalVectorInFemurImage = CalculationHelper::CalculateDirection(tibiaTrayProximalCutInFemurImage, tibiaTrayProximalCutNormalInFemurImage);
	if (PKAData::m_LimbTiltAngle >= 85 &&PKAData::m_LimbTiltAngle <= 95)
	{
		//获得股骨图像中后端点的位置
		Eigen::Vector3d femurPosteriorCut = m_FemurImplant->GetPosteriorCut();
		Eigen::Vector3d femurPosteriorCutDirection = CalculationHelper::CalculateDirection(femurPosteriorCut, m_FemurImplant->GetPosteriorCutNormal());

		result = -CalculationHelper::DistanceFromPointToPlane(tibiaTrayProximalCutInFemurImage, femurPosteriorCutDirection, femurPosteriorCut);
		return result;
	}
	else if(PKAData::m_LimbTiltAngle>=175 && PKAData::m_LimbTiltAngle <= 185)
	{
		//计算伸直 
		Eigen::Vector3d femurDistalCut = m_FemurImplant->GetDistalCut();
		Eigen::Vector3d femurDistalCutDirection = m_FemurImplant->GetMechanicalAxis();
		
		result = -CalculationHelper::DistanceFromPointToPlane(tibiaTrayProximalCutInFemurImage, femurDistalCutDirection, femurDistalCut);
		std::cout << "175<TiltAngle<185" << result << std::endl;
		return result;
	}
	return 0.0;
}

bool lancetAlgorithm::AngleCalculationHelper::CheckVariable()
{
	if (m_FemurModel == nullptr)
	{
		std::cout << "femur null ptr";
		return true;
	}
	if (m_FemurImplant == nullptr)
	{
		std::cout << "femur implant null ptr";
		return true;
	}
	if (m_TibiaModel == nullptr)
	{
		std::cout << "tibia null ptr";
		return true;
	}
	if (m_TibiaTray == nullptr)
	{
		std::cout << "tibia implant null ptr";
		return true;
	}
	return true;
}
