
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaFemurStemCouple.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaFemurStemCouple::ThaFemurStemCouple():
m_FemurObject(lancet::ThaFemurObject::New()),
m_StemObject(lancet::ThaStemObject::New()),
m_vtkMatrix_femurFrameToStemFrame(vtkMatrix4x4::New()),
m_vtkMatrix_coupleGeometry(vtkMatrix4x4::New())
{
	
}

lancet::ThaFemurStemCouple::~ThaFemurStemCouple()
{
	
}

void lancet::ThaFemurStemCouple::SetCoupleGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	m_FemurObject->SetGroupGeometry(newMatrix);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(m_vtkMatrix_femurFrameToStemFrame);
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Update();

	auto stemMatrix = tmpTransform->GetMatrix();

	m_StemObject->SetGroupGeometry(stemMatrix);

	m_vtkMatrix_coupleGeometry->DeepCopy(newMatrix);
}

void lancet::ThaFemurStemCouple::SetFemurFrameToStemFrameMatrix(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	auto femurMatrix = m_FemurObject->GetvtkMatrix_groupGeometry();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Concatenate(m_vtkMatrix_coupleGeometry);
	tmpTransform->Update();

	auto stemMatrix = tmpTransform->GetMatrix();

	m_StemObject->SetGroupGeometry(stemMatrix);

	m_vtkMatrix_femurFrameToStemFrame->DeepCopy(newMatrix);

}


void lancet::ThaFemurStemCouple::InitializeFemurFrameToStemFrameMatrix()
{
	// Check if m_PelvisObject and m_CupObject are both ready
	if (m_FemurObject->Getsurface_femur()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_FemurObject is empty when InitializeFemurFrameToStemFrameMatrix()";
	}

	if (m_StemObject->GetSurface_stem()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_StemObject is empty when InitializeFemurFrameToStemFrameMatrix()";
	}

	int operationSide = m_StemObject->GetOperationSide(); // right side (0) ; left side (1) 

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();

	// if the operationSide is right, the stem should rotate 180 along the Z axis
	if(operationSide == 0)
	{
		tmpTransform->RotateZ(180);
	}
	/*
	// the stem is then translated upward so that the stemCOR and the femurCOR is aligned in z-dimension
	auto femurCOR = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	auto stemCOR = m_StemObject->GetPset_headCenter()->GetPoint(0);

	tmpTransform->Translate(0, 0, femurCOR[2] - stemCOR[2]);

	tmpTransform->Update();

	m_vtkMatrix_femurFrameToStemFrame->DeepCopy(tmpTransform->GetMatrix());

	SetCoupleGeometry(m_FemurObject->GetvtkMatrix_groupGeometry());
	*/
	//TO DO:

	auto femurCor = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	auto femurProximal = m_FemurObject->Getpset_neckCenter()->GetPoint(0);
	
	//auto stemAssemblyCenter = m_StemObject->GetPset_stemLine()->GetPoint(0);
	//auto stemLineEnd = m_StemObject->GetPset_stemLine()->GetPoint(1);
	auto headCenter = m_StemObject->GetPset_headCenter()->GetPoint(0);

	double femurNeckAxis[3] = { femurCor[0] - femurProximal[0],femurCor[1] - femurProximal[1],femurCor[2] - femurProximal[2] };
	double stemLine[3] = { 0,0,1 };

	double RotAngle = 0;
	double stemAxisNorm = sqrt(femurNeckAxis[0] * femurNeckAxis[0] + femurNeckAxis[1] * femurNeckAxis[1] + femurNeckAxis[2] * femurNeckAxis[2]);
	double headAxisNorm = sqrt(stemLine[0] * stemLine[0] + stemLine[1] * stemLine[1] + stemLine[2] * stemLine[2]);
	if (stemAxisNorm != 0 || headAxisNorm != 0)
	{
		RotAngle = std::acos((femurNeckAxis[0] * stemLine[0] + femurNeckAxis[1] * stemLine[1] + femurNeckAxis[2] * stemLine[2]) /
			(stemAxisNorm * headAxisNorm)) / vtkMath::Pi() * 180;
	}
	double RotAxis[3] = {0,1,0};
	//vtkMath::Cross(stemLine, femurNeckAxis, RotAxis);
	
	tmpTransform->RotateWXYZ(-45, RotAxis);
	tmpTransform->Update();

	double tmpheadCenter[3] = { headCenter[0], headCenter[1], headCenter[2] };
	tmpTransform->TransformPoint(tmpheadCenter, tmpheadCenter);

	vtkNew<vtkTransform> TargetTransform;
	TargetTransform->PostMultiply();
	TargetTransform->SetMatrix(tmpTransform->GetMatrix());
	TargetTransform->Translate(femurCor[0] - tmpheadCenter[0], femurCor[1] - tmpheadCenter[1], femurCor[2] - tmpheadCenter[2]);
	TargetTransform->Update();

	m_vtkMatrix_femurFrameToStemFrame->DeepCopy(TargetTransform->GetMatrix());
	//m_vtkMatrix_femurFrameToStemFrame
	SetCoupleGeometry(m_FemurObject->GetvtkMatrix_groupGeometry());
}

mitk::Point3D lancet::ThaFemurStemCouple::GetStemHeadCenterInFemurFrame()
{
	mitk::Point3D stemHeadCenterInFemurFrame;

	auto stemHeadCenterInStemFrame = m_StemObject->GetPset_headCenter()->GetPoint(0);
	m_StemObject->GetPset_headCenter()->GetGeometry()->WorldToIndex(stemHeadCenterInStemFrame, stemHeadCenterInStemFrame);

	auto tmpPset = mitk::PointSet::New();
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_vtkMatrix_femurFrameToStemFrame);
	tmpPset->GetGeometry()->IndexToWorld(stemHeadCenterInStemFrame, stemHeadCenterInFemurFrame);

	return stemHeadCenterInFemurFrame;
}

vtkSmartPointer<vtkMatrix4x4> lancet::ThaFemurStemCouple::GetCoupleFrameToCanalFrame()
{
	// coupleFrame is the same as the femur frame
	// the coupleCanalFrame should take the stem COR into consideration

	vtkNew<vtkMatrix4x4> coupleFrameToCanalFrameMatrix;
	coupleFrameToCanalFrameMatrix->Identity();
	
	auto distalCanalPointInFemurFrame = m_FemurObject->Getpset_femurCanal()->GetPoint(1);
	m_FemurObject->Getpset_femurCanal()->GetGeometry()->WorldToIndex(distalCanalPointInFemurFrame, distalCanalPointInFemurFrame);

	auto femurCORinFemurFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject->Getpset_femurCanal()->GetGeometry()->WorldToIndex(femurCORinFemurFrame, femurCORinFemurFrame);


	auto stemCORinFemurFrame = GetStemHeadCenterInFemurFrame();

	// z axis is distal femurCanal--> proximal femurCanal
	Eigen::Vector3d z_vector;
	z_vector[0] = 0 - distalCanalPointInFemurFrame[0];
	z_vector[1] = 0 - distalCanalPointInFemurFrame[1];
	z_vector[2] = 0 - distalCanalPointInFemurFrame[2];
	z_vector.normalize();

	// for right femur: y axis is the cross product of z axis and (proximal femurCanal--> stemCOR)
	// for left femur: y axis is the cross product of  (proximal femurCanal--> stemCOR) and z axis
	Eigen::Vector3d tmpVector;
	tmpVector[0] = stemCORinFemurFrame[0];
	tmpVector[1] = stemCORinFemurFrame[1];
	tmpVector[2] = stemCORinFemurFrame[2];
	tmpVector.normalize();

	Eigen::Vector3d tmpVector_femur;
	tmpVector_femur[0] = femurCORinFemurFrame[0];
	tmpVector_femur[1] = femurCORinFemurFrame[1];
	tmpVector_femur[2] = femurCORinFemurFrame[2];

	Eigen::Vector3d y_vector;
	Eigen::Vector3d y_vector_femur;
	if (m_FemurObject->GetfemurSide() == 0)
	{
		y_vector = z_vector.cross(tmpVector);
		y_vector_femur = z_vector.cross(tmpVector_femur);
	}
	else
	{
		y_vector = tmpVector.cross(z_vector);
		y_vector_femur = tmpVector_femur.cross(z_vector);
	}

	// In case the StemCOR moves to the opposite side of the original femurCOR
	// the direction of y_vector should be corrected according the y vector of the femurObject frame (y_vector_femur)

	y_vector = y_vector.dot(y_vector_femur) * y_vector;

	y_vector.normalize();

	Eigen::Vector3d x_vector;
	x_vector = y_vector.cross(z_vector);

	for (int i{ 0 }; i < 3; i++)
	{
		coupleFrameToCanalFrameMatrix->SetElement(i, 0, x_vector[i]);
		coupleFrameToCanalFrameMatrix->SetElement(i, 1, y_vector[i]);
		coupleFrameToCanalFrameMatrix->SetElement(i, 2, z_vector[i]);
		coupleFrameToCanalFrameMatrix->SetElement(i, 3, 0);
	}

	return coupleFrameToCanalFrameMatrix;

}

// TODO: this function might be problematic
vtkSmartPointer<vtkMatrix4x4> lancet::ThaFemurStemCouple::GetCoupleFrameToMechanicFrame()
{
	vtkNew<vtkMatrix4x4> coupleFrameToMechanicFrameMatrix;
	coupleFrameToMechanicFrameMatrix->Identity();

	// coupleFrame is the same as the femur frame
	// the coupleMechanicFrame should take the stemCOR into consideration

	// mechanic frame:
	// origin: proximal femurCanal
	// z vector: midpoint of the epicondyles --> femurCOR
	// toolVector: femurCOR's perpendicular foot on the canal axis --> femurCOR
	// y vector: z vector X toolVector (right femur) \ toolVector X z vector (left femur)

	auto stemCORinFemurFrame = GetStemHeadCenterInFemurFrame();

	auto femurCORinFemurFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject->Getpset_femurCanal()->GetGeometry()->WorldToIndex(femurCORinFemurFrame, femurCORinFemurFrame);

	auto distalCanalPointInFemurFrame = m_FemurObject->Getpset_femurCanal()->GetPoint(1);
	m_FemurObject->Getpset_femurCanal()->GetGeometry()->WorldToIndex(distalCanalPointInFemurFrame, distalCanalPointInFemurFrame);

	auto medialEpiInFemurFrame = m_FemurObject->Getpset_epicondyles()->GetPoint(0);
	m_FemurObject->Getpset_epicondyles()->GetGeometry()->WorldToIndex(medialEpiInFemurFrame, medialEpiInFemurFrame);

	auto lateralEpiInFemurFrame = m_FemurObject->Getpset_epicondyles()->GetPoint(1);
	m_FemurObject->Getpset_epicondyles()->GetGeometry()->WorldToIndex(lateralEpiInFemurFrame, lateralEpiInFemurFrame);

	double midEpi[3];
	midEpi[0] = (medialEpiInFemurFrame[0] + lateralEpiInFemurFrame[0]) / 2;
	midEpi[1] = (medialEpiInFemurFrame[1] + lateralEpiInFemurFrame[1]) / 2;
	midEpi[2] = (medialEpiInFemurFrame[2] + lateralEpiInFemurFrame[2]) / 2;

	Eigen::Vector3d z_vector;
	z_vector[0] = stemCORinFemurFrame[0] - midEpi[0];
	z_vector[1] = stemCORinFemurFrame[1] - midEpi[1];
	z_vector[2] = stemCORinFemurFrame[2] - midEpi[2];
	z_vector.normalize();

	Eigen::Vector3d toolVector;
	toolVector[0] = stemCORinFemurFrame[0] ;
	toolVector[1] = stemCORinFemurFrame[1] ;
	toolVector[2] = stemCORinFemurFrame[2] ;

	Eigen::Vector3d toolVector_femur;
	toolVector_femur[0] = femurCORinFemurFrame[0];
	toolVector_femur[1] = femurCORinFemurFrame[1];
	toolVector_femur[2] = femurCORinFemurFrame[2];

	Eigen::Vector3d toolVector2;
	toolVector2[0] = -distalCanalPointInFemurFrame[0];
	toolVector2[1] = -distalCanalPointInFemurFrame[1];
	toolVector2[2] = -distalCanalPointInFemurFrame[2];
	toolVector2.normalize();

	Eigen::Vector3d toolVector3;
	toolVector3 = toolVector - (toolVector.dot(toolVector2)) * toolVector2;

	Eigen::Vector3d toolVector3_femur;
	toolVector3_femur = toolVector_femur - (toolVector_femur.dot(toolVector2))*toolVector2;

	Eigen::Vector3d y_vector;
	Eigen::Vector3d y_vector_femur;
	if (m_FemurObject->GetfemurSide() == 0)
	{
		y_vector = z_vector.cross(toolVector3);
		y_vector_femur = z_vector.cross(toolVector3_femur);
	}
	else
	{
		y_vector = toolVector3.cross(z_vector);
		y_vector_femur = toolVector3_femur.cross(z_vector);
	}

	// In case the StemCOR moves to the opposite side of the original femurCOR
	// the direction of y_vector should be corrected according the y vector of the femurObject mechanical frame (y_vector_femur)

	y_vector = y_vector.dot(y_vector_femur) * y_vector;

	y_vector.normalize();

	Eigen::Vector3d x_vector;
	x_vector = y_vector.cross(z_vector);

	for (int i{ 0 }; i < 3; i++)
	{
		coupleFrameToMechanicFrameMatrix->SetElement(i, 0, x_vector[i]);
		coupleFrameToMechanicFrameMatrix->SetElement(i, 1, y_vector[i]);
		coupleFrameToMechanicFrameMatrix->SetElement(i, 2, z_vector[i]);
	}

	return coupleFrameToMechanicFrameMatrix;
}

double lancet::ThaFemurStemCouple::CalStemVersion()
{
	return 0;
}

void lancet::ThaFemurStemCouple::ChangeHead(mitk::Surface::Pointer head,mitk::PointSet::Pointer point)
{
	//calculate final transformlation
	auto headCenter = m_StemObject->GetPset_headCenter()->GetPoint(0);
	MITK_INFO << "HEAD CENTER:" << headCenter[0] << ","
		<< headCenter[1] << ","
		<< headCenter[2];
	MITK_INFO << "================================";

	auto newHeadCenter = point->GetPoint(0);
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(m_vtkMatrix_femurFrameToStemFrame);
	tmpTransform->Update();

	mitk::Point3D tmpNewHeadCenter;
	//double tmpNewHeadCenter[3] = { newHeadCenter[0],newHeadCenter[1],newHeadCenter[2] };
	m_StemObject->GetPset_headCenter()->GetGeometry()->IndexToWorld(newHeadCenter, tmpNewHeadCenter);
	//tmpTransform->TransformPoint(tmpNewHeadCenter, tmpNewHeadCenter);

	vtkNew<vtkTransform> TargetTransform;
	TargetTransform->PostMultiply();
	TargetTransform->SetMatrix(m_vtkMatrix_femurFrameToStemFrame);
	TargetTransform->Translate(headCenter[0] - tmpNewHeadCenter[0], headCenter[1] - tmpNewHeadCenter[1], headCenter[2] - tmpNewHeadCenter[2]);
	TargetTransform->Update();

	m_vtkMatrix_femurFrameToStemFrame->DeepCopy(TargetTransform->GetMatrix());
	//m_vtkMatrix_femurFrameToStemFrame
	SetCoupleGeometry(m_FemurObject->GetvtkMatrix_groupGeometry());

	//assume that new surface are always align with the world coordinate
	//1¡¢change m_StemObject head
	m_StemObject->SetHeadSurface(head);
	
	//2¡¢change m_StemObject head center
	m_StemObject->SetHeadCenter(point);
	//3¡¢change FemurToStemMatrix
	m_vtkMatrix_femurFrameToStemFrame->DeepCopy(TargetTransform->GetMatrix());
	SetCoupleGeometry(m_FemurObject->GetvtkMatrix_groupGeometry());


}

void lancet::ThaFemurStemCouple::ChangeStem(mitk::Surface::Pointer stem)
{
	m_StemObject->SetStemSurface(stem);
}



