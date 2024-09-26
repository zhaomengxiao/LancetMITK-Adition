#include "ModelRegistration.h"
using namespace lancetAlgorithm;

void ModelRegistration::SetICPInput(mitk::PointSet* inputSource, mitk::PointSet* inputTarget, mitk::Surface* inputSurface)
{
	source = inputSource;
	target = inputTarget;
	surface = inputSurface;
	combineICPMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	screenPoints = vtkSmartPointer<vtkPoints>::New();
	targetScreenPoints = vtkSmartPointer<vtkPoints>::New();
	combineICPMatrix->Identity();
}

vtkSmartPointer<vtkMatrix4x4> ModelRegistration::GetICPMatrix()
{
	return combineICPMatrix;
}

vtkSmartPointer<vtkPoints> ModelRegistration::GetScreenPoint()
{
	return screenPoints;
}

void lancetAlgorithm::ModelRegistration::ComputeFemur(Eigen::Vector3d pointInFemurRF, Eigen::Vector3d targetPoint)
{
	UpdateFemurLandmarkMatrixGroup(pointInFemurRF, targetPoint);
	UpdateCamera2SurfaceICP();
	RemoveBadCamera2SurfacePoint();
	UpdateRemoveBadCamera2SurfaceICP();
}

void lancetAlgorithm::ModelRegistration::ComputeTibia(Eigen::Vector3d MedialMalleolusInTibiaRF, Eigen::Vector3d LateralMalleolusInTibiaRF, Eigen::Vector3d MedialMalleolus, Eigen::Vector3d LateralMalleolus)
{
	UpdateTibiaLandmarkMatrixGroup(MedialMalleolusInTibiaRF, LateralMalleolusInTibiaRF, MedialMalleolus, LateralMalleolus);
	UpdateCamera2SurfaceICP();
	RemoveBadCamera2SurfacePoint();
	UpdateRemoveBadCamera2SurfaceICP();
}

void ModelRegistration::Compute()
{
	UpdateLandmarkMatrixGroup();
	UpdateCamera2SurfaceICP();
	RemoveBadCamera2SurfacePoint();
	UpdateRemoveBadCamera2SurfaceICP();
}

double ModelRegistration::GetRegistrationRMS()
{
	return m_RegistrationRMS;
}

/// <summary>
/// 股骨验证点和胫骨验证点
/// </summary>
/// <param name="probePointInCamera"></param>
/// <returns></returns>
double lancetAlgorithm::ModelRegistration::VerifyBoneVerifyPoint(KneeModel kneeModel, Eigen::Vector3d probePointInCamera)
{
	double distance = 0.0;
	if (kneeModel == KneeModel::Femur)
	{
		vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
		TFemurRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
		TFemurRF2Camera->Invert();
		Eigen::Vector3d probePointInFemurRF = CalculationHelper::TransformByMatrix(probePointInCamera, TFemurRF2Camera);
		distance = CalculationHelper::CalculateTwoPointsDistance(PKAData::m_FemurVerifyPointInFemurRF, probePointInFemurRF);
	}
	else
	{
		vtkSmartPointer<vtkMatrix4x4> TTibiaRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
		TTibiaRF2Camera->DeepCopy(PKAData::m_TCamera2TibiaRF);
		TTibiaRF2Camera->Invert();
		Eigen::Vector3d probePointInTibiaRF = CalculationHelper::TransformByMatrix(probePointInCamera, TTibiaRF2Camera);
		distance = CalculationHelper::CalculateTwoPointsDistance(PKAData::m_TibiaVerifyPointInTibiaRF, probePointInTibiaRF);
	}

	return distance;
}

void lancetAlgorithm::ModelRegistration::UpdateFemurLandmarkMatrixGroup(Eigen::Vector3d pointInFemurRF, Eigen::Vector3d targetPoint)
{
	vtkSmartPointer<vtkPoints> pSource =
		vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> pTarget =
		vtkSmartPointer<vtkPoints>::New();
	pSource->InsertNextPoint(pointInFemurRF.data());
	pTarget->InsertNextPoint(targetPoint.data());
	for (int i = 0; i < target->GetSize(); i++)
	{
		if (i % 5 == 0)
		{
			auto pointVega = source->GetPoint(i);
			pSource->InsertNextPoint(pointVega[0], pointVega[1], pointVega[2]);
			auto pointSTL = target->GetPoint(i);
			pTarget->InsertNextPoint(pointSTL[0], pointSTL[1], pointSTL[2]);
		}
	}

	//求ICP的变化矩阵
	vtkSmartPointer<vtkLandmarkTransform> landmarkTransform =
		vtkSmartPointer<vtkLandmarkTransform>::New();
	std::cout << "pSource: " << pSource->GetNumberOfPoints() << std::endl;
	std::cout << "pTarget: " << pTarget->GetNumberOfPoints() << std::endl;
	landmarkTransform->SetSourceLandmarks(pSource);
	landmarkTransform->SetTargetLandmarks(pTarget);
	landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY);
	landmarkTransform->Update();
	camera2SurfaceLandmarkMatrix->DeepCopy(landmarkTransform->GetMatrix());
	PrintDataHelper::CoutMatrix("ModelRegistration::UpdateLandmarkMatrixGroup: ", camera2SurfaceLandmarkMatrix);
	vtkSmartPointer<vtkPoints> pTransformPoints =
		vtkSmartPointer<vtkPoints>::New();

	double cache[3];
	vtkSmartPointer<vtkTransform> pTransform =
		vtkSmartPointer<vtkTransform>::New();
	pTransform->SetMatrix(camera2SurfaceLandmarkMatrix);
	pTransform->TransformPoints(pSource, pTransformPoints);
	double LandMarkRMS = CalculateRMS(pTarget, pTransformPoints);
	std::cout << "LandMarkRMS " << LandMarkRMS << std::endl;
}

void lancetAlgorithm::ModelRegistration::UpdateTibiaLandmarkMatrixGroup(Eigen::Vector3d MedialMalleolusInTibiaRF,
	Eigen::Vector3d LateralMalleolusInTibiaRF, Eigen::Vector3d MedialMalleolus, Eigen::Vector3d LateralMalleolus)
{
	vtkSmartPointer<vtkPoints> pSource =
		vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> pTarget =
		vtkSmartPointer<vtkPoints>::New();

	pSource->InsertNextPoint(MedialMalleolusInTibiaRF.data());
	pSource->InsertNextPoint(LateralMalleolusInTibiaRF.data());
	PrintDataHelper::CoutArray(MedialMalleolus, "UpdateTibiaLandmarkMatrixGroup MedialMalleolus");
	PrintDataHelper::CoutArray(LateralMalleolus, "UpdateTibiaLandmarkMatrixGroup LateralMalleolus");
	pTarget->InsertNextPoint(MedialMalleolus.data());
	pTarget->InsertNextPoint(LateralMalleolus.data());
	for (int i = 0; i < target->GetSize(); i++)
	{
		if (i % 5 == 0)
		{
			auto pointVega = source->GetPoint(i);
			pSource->InsertNextPoint(pointVega[0], pointVega[1], pointVega[2]);
			auto pointSTL = target->GetPoint(i);
			pTarget->InsertNextPoint(pointSTL[0], pointSTL[1], pointSTL[2]);
		}
	}

	//求ICP的变化矩阵
	vtkSmartPointer<vtkLandmarkTransform> landmarkTransform =
		vtkSmartPointer<vtkLandmarkTransform>::New();
	std::cout << "pSource: " << pSource->GetNumberOfPoints() << std::endl;
	std::cout << "pTarget: " << pTarget->GetNumberOfPoints() << std::endl;
	landmarkTransform->SetSourceLandmarks(pSource);
	landmarkTransform->SetTargetLandmarks(pTarget);
	landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY);
	landmarkTransform->Update();
	camera2SurfaceLandmarkMatrix->DeepCopy(landmarkTransform->GetMatrix());
	PrintDataHelper::CoutMatrix("ModelRegistration::UpdateLandmarkMatrixGroup: ", camera2SurfaceLandmarkMatrix);
	vtkSmartPointer<vtkPoints> pTransformPoints =
		vtkSmartPointer<vtkPoints>::New();

	double cache[3];
	vtkSmartPointer<vtkTransform> pTransform =
		vtkSmartPointer<vtkTransform>::New();
	pTransform->SetMatrix(camera2SurfaceLandmarkMatrix);
	pTransform->TransformPoints(pSource, pTransformPoints);
	double LandMarkRMS = CalculateRMS(pTarget, pTransformPoints);
	std::cout << "LandMarkRMS " << LandMarkRMS << std::endl;
}

void ModelRegistration::UpdateLandmarkMatrixGroup()
{
	vtkSmartPointer<vtkPoints> pSource =
		vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> pTarget =
		vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < target->GetSize(); i++)
	{
		if (i % 5 == 0)
		{
			auto pointVega = source->GetPoint(i);
			pSource->InsertNextPoint(pointVega[0], pointVega[1], pointVega[2]);
			auto pointSTL = target->GetPoint(i);
			pTarget->InsertNextPoint(pointSTL[0], pointSTL[1], pointSTL[2]);
		}
	}

	//求ICP的变化矩阵
	vtkSmartPointer<vtkLandmarkTransform> landmarkTransform =
		vtkSmartPointer<vtkLandmarkTransform>::New();
	std::cout << "pSource: " << pSource->GetNumberOfPoints() << std::endl;
	std::cout << "pTarget: " << pTarget->GetNumberOfPoints() << std::endl;
	landmarkTransform->SetSourceLandmarks(pSource);
	landmarkTransform->SetTargetLandmarks(pTarget);
	landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY);
	landmarkTransform->Update();
	camera2SurfaceLandmarkMatrix->DeepCopy(landmarkTransform->GetMatrix());
	PrintDataHelper::CoutMatrix("ModelRegistration::UpdateLandmarkMatrixGroup: ", camera2SurfaceLandmarkMatrix);
	vtkSmartPointer<vtkPoints> pTransformPoints =
		vtkSmartPointer<vtkPoints>::New();
	
	double cache[3];
	vtkSmartPointer<vtkTransform> pTransform =
		vtkSmartPointer<vtkTransform>::New();
	pTransform->SetMatrix(camera2SurfaceLandmarkMatrix);
	for (int i = 0; i < target->GetSize(); i++)
	{
		if (i % 5 == 0)
		{
			pTransform->TransformPoint(source->GetPoint(i).GetDataPointer(), cache);
			pTransformPoints->InsertNextPoint(cache[0], cache[1], cache[2]);
		}
	}
	double LandMarkRMS = CalculateRMS(pTarget, pTransformPoints);
	std::cout << "LandMarkRMS " << LandMarkRMS << std::endl;
}

void ModelRegistration::UpdateCamera2SurfaceICP()
{
	vtkSmartPointer<vtkTransform> pTransform =
		vtkSmartPointer<vtkTransform>::New();
	pTransform->PostMultiply();
	pTransform->Identity();
	pTransform->Concatenate(camera2SurfaceLandmarkMatrix);

	vtkSmartPointer<vtkPolyData> pSource =
		vtkSmartPointer<vtkPolyData>::New();
	pSource->SetPoints(vtkSmartPointer<vtkPoints>::New());

	for (int i = 0; i < source->GetSize(); i++)
	{
		double cache[3];
		mitk::PointSet::PointType point3D;
		point3D = source->GetPoint(i);
		pTransform->TransformPoint(point3D.GetDataPointer(), cache);
		pSource->GetPoints()->InsertNextPoint(cache[0], cache[1], cache[2]);
	}

	vtkSmartPointer<vtkIterativeClosestPointTransform> pIcp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	pIcp->SetSource(pSource);
	pIcp->SetTarget(surface->GetVtkPolyData());
	pIcp->GetLandmarkTransform()->SetModeToRigidBody();
	pIcp->SetMaximumNumberOfIterations(5000);
	pIcp->SetCheckMeanDistance(1);
	pIcp->SetMaximumMeanDistance(0.01);
	pIcp->Update();
	camera2SurfaceICPMatrix->DeepCopy(pIcp->GetMatrix());
}

void ModelRegistration::RemoveBadCamera2SurfacePoint()
{
	vtkSmartPointer<vtkTransform> pTransformVerify =
		vtkSmartPointer<vtkTransform>::New();
	pTransformVerify->PostMultiply();
	pTransformVerify->Identity();
	pTransformVerify->Concatenate(camera2SurfaceLandmarkMatrix);  //新增
	pTransformVerify->Concatenate(camera2SurfaceICPMatrix);

	vtkSmartPointer<vtkKdTree> pKdTree =
		vtkSmartPointer<vtkKdTree>::New();
	pKdTree->BuildLocatorFromPoints(surface->GetVtkPolyData()->GetPoints());
	for (int i = 0; i < source->GetSize(); i++)
	{
		mitk::PointSet::PointType point = source->GetPoint(i);
		mitk::PointSet::PointType targetpoint = target->GetPoint(i);
		double cache[3];
		pTransformVerify->TransformPoint(point.GetDataPointer(), cache);
		//mitk::PointSet::PointType pointTransform(cache);
		double distance;
		pKdTree->FindClosestPoint(cache, distance);
		if (distance < 10)
		{
			screenPoints->InsertNextPoint(point[0], point[1], point[2]);
			targetScreenPoints->InsertNextPoint(targetpoint[0], targetpoint[1], targetpoint[2]);
		}
	}
}

void ModelRegistration::UpdateRemoveBadCamera2SurfaceICP()
{
	vtkSmartPointer<vtkTransform> pTransform =
		vtkSmartPointer<vtkTransform>::New();
	pTransform->PostMultiply();
	pTransform->Identity();
	pTransform->Concatenate(camera2SurfaceLandmarkMatrix);

	vtkSmartPointer<vtkPolyData> pSource =
		vtkSmartPointer<vtkPolyData>::New();
	pSource->SetPoints(vtkSmartPointer<vtkPoints>::New());

	for (int i = 0; i < screenPoints->GetNumberOfPoints(); i++)
	{
		double cache[3];
		mitk::PointSet::PointType point3D;
		point3D = screenPoints->GetPoint(i);
		pTransform->TransformPoint(point3D.GetDataPointer(), cache);
		pSource->GetPoints()->InsertNextPoint(cache[0], cache[1], cache[2]);
	}

	vtkSmartPointer<vtkIterativeClosestPointTransform> pIcp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	pIcp->SetSource(pSource);
	pIcp->SetTarget(surface->GetVtkPolyData());
	pIcp->GetLandmarkTransform()->SetModeToRigidBody();
	pIcp->SetMaximumNumberOfIterations(5000);
	pIcp->SetCheckMeanDistance(1);
	pIcp->SetMaximumMeanDistance(0.01);
	pIcp->Update();
	camera2SurfaceICPMatrix->DeepCopy(pIcp->GetMatrix());

	PrintDataHelper::CoutMatrix("ModelRegistration::UpdateRemoveBadCamera2SurfaceICP camera2SurfaceICPMatrix: ", camera2SurfaceICPMatrix);
	vtkSmartPointer<vtkTransform> transformCombine =
		vtkSmartPointer<vtkTransform>::New();
	transformCombine->PostMultiply();
	transformCombine->SetMatrix(camera2SurfaceLandmarkMatrix);
	transformCombine->Concatenate(camera2SurfaceICPMatrix);
	transformCombine->Update();
	transformCombine->GetMatrix(combineICPMatrix);
	PrintDataHelper::CoutMatrix("ModelRegistration::UpdateRemoveBadCamera2SurfaceICP combineICPMatrix: ", combineICPMatrix);
	auto screenTransPoints = CalculationHelper::TransformVTKPoints(screenPoints, combineICPMatrix);

	m_RegistrationRMS = CalculateRMS(screenTransPoints, targetScreenPoints);
}

double ModelRegistration::CalculateRMS(mitk::PointSet::Pointer source, mitk::PointSet::Pointer target)
{
	if (source->GetSize() != target->GetSize())
	{
		std::cout << "source->GetSize() != target->GetSize() " << std::endl;
		return (std::numeric_limits<double>::max)();
	}
	double dist = 0.0;
	int n = source->GetSize();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			double tmp = source->GetPoint(i)[j] - target->GetPoint(i)[j];
			dist += tmp * tmp;
		}
	}
	dist /= n;
	return sqrt(dist);
}

double ModelRegistration::CalculateRMS(vtkPoints* source, vtkPoints* target)
{
	if (source->GetNumberOfPoints() != target->GetNumberOfPoints())
	{
		std::cout << "source->GetNumberOfPoints() != target->GetNumberOfPoints() " << std::endl;
		return (std::numeric_limits<double>::max)();
	}
	double dist = 0.0;
	int n = source->GetNumberOfPoints();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			double tmp = source->GetPoint(i)[j] - target->GetPoint(i)[j];
			dist += tmp * tmp;
		}
	}
	dist /= n;
	return sqrt(dist);
}
