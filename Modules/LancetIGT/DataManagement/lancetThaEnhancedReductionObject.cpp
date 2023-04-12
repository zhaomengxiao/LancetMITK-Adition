#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaEnhancedReductionObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaEnhancedReductionObject::ThaEnhancedReductionObject():
	m_PelvisCupCouple(lancet::ThaPelvisCupCouple::New()),
	m_FemurStemCouple(lancet::ThaFemurStemCouple::New()),
	m_FemurObject(lancet::ThaFemurObject::New()),
	m_NoPelvicTilt_pelvisMatrix(vtkMatrix4x4::New()),
	m_NoPelvicTilt_canal_matrix_R(vtkMatrix4x4::New()),
	m_NoPelvicTilt_canal_matrix_L(vtkMatrix4x4::New()),
	m_NoPelvicTilt_mechanic_matrix_R(vtkMatrix4x4::New()),
	m_NoPelvicTilt_mechanic_matrix_L(vtkMatrix4x4::New()),
	m_SupinePelvicTilt_pelvisMatrix(vtkMatrix4x4::New()),
	m_SupinePelvicTilt_canal_matrix_R(vtkMatrix4x4::New()),
	m_SupinePelvicTilt_canal_matrix_L(vtkMatrix4x4::New()),
	m_SupinePelvicTilt_mechanic_matrix_R(vtkMatrix4x4::New()),
	m_SupinePelvicTilt_mechanic_matrix_L(vtkMatrix4x4::New())
{
	
}

lancet::ThaEnhancedReductionObject::~ThaEnhancedReductionObject()
{
	
}

void lancet::ThaEnhancedReductionObject::GetNoTiltCanalMatrices(vtkSmartPointer<vtkMatrix4x4> pelvisMatrix, vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix, vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	CalReductionMatrices();
	pelvisMatrix->DeepCopy(m_NoPelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_NoPelvicTilt_canal_matrix_R);
	leftFemurMatrix->DeepCopy(m_NoPelvicTilt_canal_matrix_L);

}

void lancet::ThaEnhancedReductionObject::GetNoTiltMechanicMatrices(vtkSmartPointer<vtkMatrix4x4> pelvisMatrix, vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix, vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	CalReductionMatrices();
	pelvisMatrix->DeepCopy(m_NoPelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_NoPelvicTilt_mechanic_matrix_R);
	leftFemurMatrix->DeepCopy(m_NoPelvicTilt_mechanic_matrix_L);
}

void lancet::ThaEnhancedReductionObject::GetSupineTiltCanalMatrices(vtkSmartPointer<vtkMatrix4x4> pelvisMatrix, vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix, vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	CalReductionMatrices();
	pelvisMatrix->DeepCopy(m_SupinePelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_R);
	leftFemurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_L);
}

void lancet::ThaEnhancedReductionObject::GetSupineTiltMechanicMatrices(vtkSmartPointer<vtkMatrix4x4> pelvisMatrix, vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix, vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	CalReductionMatrices();
	pelvisMatrix->DeepCopy(m_SupinePelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_R);
	leftFemurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_L);

}




void lancet::ThaEnhancedReductionObject::CalNoTiltCanalMatrices()
{
	// Since there is no pelvic tilt, m_NoPelvicTilt_pelvisMatrix remains as an identity matrix 
	m_NoPelvicTilt_pelvisMatrix->Identity();

	// For the femurObject, only translation is needed
	// For the femurStemCouple, both rotation and translation should be considered

	// Right femur: m_FemurStemCouple
	// Left femur: m_FemurObject
	if(m_FemurObject->GetfemurSide() == 1)
	{
		// Right femur: m_FemurStemCouple
		auto stemCOR_FemurFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();
		
		auto targetPoint = m_PelvisCupCouple->GetCupCenterInPelvisFrame();

		auto canalFrameToFemurFrameMatrix = m_FemurStemCouple->GetCoupleFrameToCanalFrame();
		canalFrameToFemurFrameMatrix->Invert();

		mitk::Point3D stemCOR_tmp;
		auto tmpPset = mitk::PointSet::New();
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(canalFrameToFemurFrameMatrix);
		tmpPset->GetGeometry()->IndexToWorld(stemCOR_FemurFrame, stemCOR_tmp);

		m_NoPelvicTilt_canal_matrix_R->DeepCopy(canalFrameToFemurFrameMatrix);
		m_NoPelvicTilt_canal_matrix_R->SetElement(0, 3, targetPoint[0] - stemCOR_tmp[0]);
		m_NoPelvicTilt_canal_matrix_R->SetElement(1, 3, targetPoint[1] - stemCOR_tmp[1]);
		m_NoPelvicTilt_canal_matrix_R->SetElement(2, 3, targetPoint[2] - stemCOR_tmp[2]);

		// Left femur: m_FemurObject
		auto sourcePoint_ = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
		m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint_, sourcePoint_);

		auto targetPoint_ = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(1);
		m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint_, targetPoint_);

		double leftTranslation[3];
		leftTranslation[0] = targetPoint_[0] - sourcePoint_[0];
		leftTranslation[1] = targetPoint_[1] - sourcePoint_[1];
		leftTranslation[2] = targetPoint_[2] - sourcePoint_[2];

		m_NoPelvicTilt_canal_matrix_L->SetElement(0, 3, leftTranslation[0]);
		m_NoPelvicTilt_canal_matrix_L->SetElement(1, 3, leftTranslation[1]);
		m_NoPelvicTilt_canal_matrix_L->SetElement(2, 3, leftTranslation[2]);
	}

	// Right femur: m_FemurObject
	// Left femur: m_FemurStemCouple
	if(m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur: m_FemurObject
		auto sourcePoint = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
		m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint, sourcePoint);

		auto targetPoint = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(0);
		m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint, targetPoint);

		double rightTranslation[3];
		rightTranslation[0] = targetPoint[0] - sourcePoint[0];
		rightTranslation[1] = targetPoint[1] - sourcePoint[1];
		rightTranslation[2] = targetPoint[2] - sourcePoint[2];

		m_NoPelvicTilt_canal_matrix_R->SetElement(0, 3, rightTranslation[0]);
		m_NoPelvicTilt_canal_matrix_R->SetElement(1, 3, rightTranslation[1]);
		m_NoPelvicTilt_canal_matrix_R->SetElement(2, 3, rightTranslation[2]);

		// Left femur: m_FemurStemCouple
		auto stemCOR_FemurFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();

		auto targetPoint_ = m_PelvisCupCouple->GetCupCenterInPelvisFrame();

		auto canalFrameToFemurFrameMatrix = m_FemurStemCouple->GetCoupleFrameToCanalFrame();
		canalFrameToFemurFrameMatrix->Invert();

		mitk::Point3D stemCOR_tmp;
		auto tmpPset = mitk::PointSet::New();
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(canalFrameToFemurFrameMatrix);
		tmpPset->GetGeometry()->IndexToWorld(stemCOR_FemurFrame, stemCOR_tmp);

		m_NoPelvicTilt_canal_matrix_L->DeepCopy(canalFrameToFemurFrameMatrix);
		m_NoPelvicTilt_canal_matrix_L->SetElement(0, 3, targetPoint_[0] - stemCOR_tmp[0]);
		m_NoPelvicTilt_canal_matrix_L->SetElement(1, 3, targetPoint_[1] - stemCOR_tmp[1]);
		m_NoPelvicTilt_canal_matrix_L->SetElement(2, 3, targetPoint_[2] - stemCOR_tmp[2]);
	}

}


void lancet::ThaEnhancedReductionObject::CalNoTiltMechanicMatrices()
{
	// The pelvisCupCouple has noTilt, so m_NoPelvicTilt_pelvisMatrix remains as an identity matrix
	m_NoPelvicTilt_pelvisMatrix->Identity();

	// Allocate pelvisCOR_pelvisFrame and cupCOR_pelvisFrame
	auto pelvisCOR_R_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(0);
	auto pelvisCOR_L_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R_pelvisFrame, pelvisCOR_R_pelvisFrame);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_L_pelvisFrame, pelvisCOR_L_pelvisFrame);

	auto cupCOR_pelvisFrame = m_PelvisCupCouple->GetCupCenterInPelvisFrame();

	vtkNew<vtkMatrix4x4> mechanicFrameToCoupleFrame;
	mechanicFrameToCoupleFrame->DeepCopy( m_FemurStemCouple->GetCoupleFrameToMechanicFrame()); // m_FemurStemCouple mechanicFrame-->coupleFrame
	mechanicFrameToCoupleFrame->Invert(); 

	vtkNew<vtkMatrix4x4> mechanicFrameToCanalFrame;
	mechanicFrameToCanalFrame->DeepCopy(m_FemurObject->GetvtkMatrix_canalFrameToMechanicFrame()) ; // m_FemurObject mechanicFrame-->canalFrame 
	mechanicFrameToCanalFrame->Invert();

	auto stemCOR_coupleFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();
	mitk::Point3D stemCOR_tmp;
	auto tmpPset_0 = mitk::PointSet::New();
	tmpPset_0->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(mechanicFrameToCoupleFrame);
	tmpPset_0->GetGeometry()->IndexToWorld(stemCOR_coupleFrame, stemCOR_tmp);

	auto femurCOR_canalFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	mitk::Point3D femurCOR_tmp;
	m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_canalFrame, femurCOR_canalFrame);
	auto tmpPset_1 = mitk::PointSet::New();
	tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(mechanicFrameToCanalFrame);
	tmpPset_1->GetGeometry()->IndexToWorld(femurCOR_canalFrame,femurCOR_tmp);

	// Right femur: m_FemurStemCouple
	// Left femur: m_FemurObject
	if(m_FemurObject->GetfemurSide() == 1)
	{
		// Right femur: m_FemurStemCouple
		m_NoPelvicTilt_mechanic_matrix_R->DeepCopy(mechanicFrameToCoupleFrame);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(0, 3, cupCOR_pelvisFrame[0] - stemCOR_tmp[0]);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(1, 3, cupCOR_pelvisFrame[1] - stemCOR_tmp[1]);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(2, 3, cupCOR_pelvisFrame[2] - stemCOR_tmp[2]);
		
		// Left femur: m_FemurObject
		m_NoPelvicTilt_mechanic_matrix_L->DeepCopy(mechanicFrameToCanalFrame);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(0, 3, pelvisCOR_L_pelvisFrame[0] - femurCOR_tmp[0]);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(1, 3, pelvisCOR_L_pelvisFrame[1] - femurCOR_tmp[1]);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(2, 3, pelvisCOR_L_pelvisFrame[2] - femurCOR_tmp[2]);
	}

	// Right femur: m_FemurObject
	// Left femur: m_FemurStemCouple
	if (m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur: m_FemurObject
		m_NoPelvicTilt_mechanic_matrix_R->DeepCopy(mechanicFrameToCanalFrame);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(0, 3, pelvisCOR_R_pelvisFrame[0] - femurCOR_tmp[0]);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(1, 3, pelvisCOR_R_pelvisFrame[1] - femurCOR_tmp[1]);
		m_NoPelvicTilt_mechanic_matrix_R->SetElement(2, 3, pelvisCOR_R_pelvisFrame[2] - femurCOR_tmp[2]);

		// Left femur: m_FemurStemCouple
		m_NoPelvicTilt_mechanic_matrix_L->DeepCopy(mechanicFrameToCoupleFrame);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(0, 3, cupCOR_pelvisFrame[0] - stemCOR_tmp[0]);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(1, 3, cupCOR_pelvisFrame[1] - stemCOR_tmp[1]);
		m_NoPelvicTilt_mechanic_matrix_L->SetElement(2, 3, cupCOR_pelvisFrame[2] - stemCOR_tmp[2]);
	}

}

void lancet::ThaEnhancedReductionObject::CalSupineCanalMatrices()
{
	// The pelvis should be rotated in x direction
	double pelvicTilt_degree = m_PelvisCupCouple->GetPelvisObject()->GetpelvicTilt_supine();

	vtkNew<vtkTransform> pelvisTransform;
	pelvisTransform->Identity();
	pelvisTransform->RotateX(pelvicTilt_degree);
	pelvisTransform->Update();

	m_SupinePelvicTilt_pelvisMatrix->DeepCopy(pelvisTransform->GetMatrix());

	auto pelvisCOR_R_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R_pelvisFrame, pelvisCOR_R_pelvisFrame);
	
	auto pelvisCOR_L_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_L_pelvisFrame, pelvisCOR_L_pelvisFrame);

	auto cupCOR_pelvisFrame = m_PelvisCupCouple->GetCupCenterInPelvisFrame();

	mitk::Point3D pelvisCOR_R_target;
	mitk::Point3D pelvisCOR_L_target;
	mitk::Point3D cupCOR_target;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);

	tmpPset->GetGeometry()->IndexToWorld(pelvisCOR_R_pelvisFrame, pelvisCOR_R_target);
	tmpPset->GetGeometry()->IndexToWorld(pelvisCOR_L_pelvisFrame, pelvisCOR_L_target);
	tmpPset->GetGeometry()->IndexToWorld(cupCOR_pelvisFrame, cupCOR_target);

	// Right femur: m_FemurStemCouple
	// Left femur: m_FemurObject
	if (m_FemurObject->GetfemurSide() == 1)
	{
		// Right femur: m_FemurStemCouple, both rotation and translation should be considered
		auto stemCOR_FemurFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();
		
		auto canalFrameToFemurFrameMatrix = m_FemurStemCouple->GetCoupleFrameToCanalFrame();
		canalFrameToFemurFrameMatrix->Invert();

		mitk::Point3D stemCOR_tmp;
		auto tmpPset = mitk::PointSet::New();
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(canalFrameToFemurFrameMatrix);
		tmpPset->GetGeometry()->IndexToWorld(stemCOR_FemurFrame, stemCOR_tmp);

		m_SupinePelvicTilt_canal_matrix_R->DeepCopy(canalFrameToFemurFrameMatrix);
		m_SupinePelvicTilt_canal_matrix_R->SetElement(0, 3, cupCOR_target[0] - stemCOR_tmp[0]);
		m_SupinePelvicTilt_canal_matrix_R->SetElement(1, 3, cupCOR_target[1] - stemCOR_tmp[1]);
		m_SupinePelvicTilt_canal_matrix_R->SetElement(2, 3, cupCOR_target[2] - stemCOR_tmp[2]);

		// Left femur: m_FemurObject, only translation is required
		auto femurCOR_femurFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
		m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_femurFrame, femurCOR_femurFrame);
		
		double leftTranslation[3];
		leftTranslation[0] = pelvisCOR_L_target[0] - femurCOR_femurFrame[0];
		leftTranslation[1] = pelvisCOR_L_target[1] - femurCOR_femurFrame[1];
		leftTranslation[2] = pelvisCOR_L_target[2] - femurCOR_femurFrame[2];

		m_SupinePelvicTilt_canal_matrix_L->Identity();
		m_SupinePelvicTilt_canal_matrix_L->SetElement(0, 3, leftTranslation[0]);
		m_SupinePelvicTilt_canal_matrix_L->SetElement(1, 3, leftTranslation[1]);
		m_SupinePelvicTilt_canal_matrix_L->SetElement(2, 3, leftTranslation[2]);


	}

	// Right femur: m_FemurObject
	// Left femur: m_FemurStemCouple
	if (m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur : m_FemurObject, only translation is required
		auto femurCOR_femurFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
		m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_femurFrame, femurCOR_femurFrame);

		double rightTranslation[3];
		rightTranslation[0] = pelvisCOR_R_target[0] - femurCOR_femurFrame[0];
		rightTranslation[1] = pelvisCOR_R_target[1] - femurCOR_femurFrame[1];
		rightTranslation[2] = pelvisCOR_R_target[2] - femurCOR_femurFrame[2];

		m_SupinePelvicTilt_canal_matrix_R->Identity();
		m_SupinePelvicTilt_canal_matrix_R->SetElement(0, 3, rightTranslation[0]);
		m_SupinePelvicTilt_canal_matrix_R->SetElement(1, 3, rightTranslation[1]);
		m_SupinePelvicTilt_canal_matrix_R->SetElement(2, 3, rightTranslation[2]);

		// Left femur: m_FemurStemCouple, both translation and rotation is required
		auto canalFrameToFemurFrameMatrix = m_FemurStemCouple->GetCoupleFrameToCanalFrame();
		canalFrameToFemurFrameMatrix->Invert();

		auto stemCOR_FemurFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();

		mitk::Point3D stemCOR_tmp;
		auto tmpPset = mitk::PointSet::New();
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(canalFrameToFemurFrameMatrix);
		tmpPset->GetGeometry()->IndexToWorld(stemCOR_FemurFrame, stemCOR_tmp);

		m_SupinePelvicTilt_canal_matrix_L->Identity();
		m_SupinePelvicTilt_canal_matrix_L->SetElement(0, 3, cupCOR_target[0] - stemCOR_tmp[0]);
		m_SupinePelvicTilt_canal_matrix_L->SetElement(1, 3, cupCOR_target[1] - stemCOR_tmp[1]);
		m_SupinePelvicTilt_canal_matrix_L->SetElement(2, 3, cupCOR_target[2] - stemCOR_tmp[2]);


	}
}

void lancet::ThaEnhancedReductionObject::CalSupineMechanicMatrices()
{
	// m_PelvisCupCouple should be rotated in x direction by the supineTilt angle
	double pelvicTilt_degree = m_PelvisCupCouple->GetPelvisObject()->GetpelvicTilt_supine();

	vtkNew<vtkTransform> pelvisTransform;
	pelvisTransform->Identity();
	pelvisTransform->RotateX(pelvicTilt_degree);
	pelvisTransform->Update();

	m_SupinePelvicTilt_pelvisMatrix->DeepCopy(pelvisTransform->GetMatrix());

	auto pelvisCOR_R_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R_pelvisFrame, pelvisCOR_R_pelvisFrame);

	auto pelvisCOR_L_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_L_pelvisFrame, pelvisCOR_L_pelvisFrame);

	auto cupCOR_pelvisFrame = m_PelvisCupCouple->GetCupCenterInPelvisFrame();

	mitk::Point3D pelvisCOR_R_target;
	mitk::Point3D pelvisCOR_L_target;
	mitk::Point3D cupCOR_target;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);

	tmpPset->GetGeometry()->IndexToWorld(pelvisCOR_R_pelvisFrame, pelvisCOR_R_target);
	tmpPset->GetGeometry()->IndexToWorld(pelvisCOR_L_pelvisFrame, pelvisCOR_L_target);
	tmpPset->GetGeometry()->IndexToWorld(cupCOR_pelvisFrame, cupCOR_target);

	auto mechanicFrameToCoupleFrame = m_FemurStemCouple->GetCoupleFrameToMechanicFrame(); // m_FemurStemCouple mechanicFrame-->coupleFrame
	mechanicFrameToCoupleFrame->Invert();

	vtkNew<vtkMatrix4x4> mechanicFrameToCanalFrame;
	mechanicFrameToCanalFrame->DeepCopy(m_FemurObject->GetvtkMatrix_canalFrameToMechanicFrame()); // m_FemurObject mechanicFrame-->canalFrame 
	mechanicFrameToCanalFrame->Invert();

	auto stemCOR_coupleFrame = m_FemurStemCouple->GetStemHeadCenterInFemurFrame();
	mitk::Point3D stemCOR_tmp;
	auto tmpPset_0 = mitk::PointSet::New();
	tmpPset_0->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(mechanicFrameToCoupleFrame);
	tmpPset_0->GetGeometry()->IndexToWorld(stemCOR_coupleFrame, stemCOR_tmp);

	auto femurCOR_canalFrame = m_FemurObject->Getpset_femurCOR()->GetPoint(0);
	mitk::Point3D femurCOR_tmp;
	m_FemurObject->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_canalFrame, femurCOR_canalFrame);
	auto tmpPset_1 = mitk::PointSet::New();
	tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(mechanicFrameToCanalFrame);
	tmpPset_1->GetGeometry()->IndexToWorld(femurCOR_canalFrame, femurCOR_tmp);

	// Right femur: m_FemurStemCouple
	// Left femur: m_FemurObject
	if (m_FemurObject->GetfemurSide() == 1)
	{
		// Right femur: m_FemurStemCouple
		m_SupinePelvicTilt_mechanic_matrix_R->DeepCopy(mechanicFrameToCoupleFrame);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(0, 3, cupCOR_target[0] - stemCOR_tmp[0]);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(1, 3, cupCOR_target[1] - stemCOR_tmp[1]);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(2, 3, cupCOR_target[2] - stemCOR_tmp[2]);

		// Left femur: m_FemurObject
		m_SupinePelvicTilt_mechanic_matrix_L->DeepCopy(mechanicFrameToCanalFrame);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(0, 3, pelvisCOR_L_target[0] - femurCOR_tmp[0]);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(1, 3, pelvisCOR_L_target[1] - femurCOR_tmp[1]);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(2, 3, pelvisCOR_L_target[2] - femurCOR_tmp[2]);
	}

	// Right femur: m_FemurObject
	// Left femur: m_FemurStemCouple
	if (m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur: m_FemurObject
		m_SupinePelvicTilt_mechanic_matrix_R->DeepCopy(mechanicFrameToCanalFrame);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(0, 3, pelvisCOR_R_target[0] - femurCOR_tmp[0]);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(1, 3, pelvisCOR_R_target[1] - femurCOR_tmp[1]);
		m_SupinePelvicTilt_mechanic_matrix_R->SetElement(2, 3, pelvisCOR_R_target[2] - femurCOR_tmp[2]);

		// Left femur: m_FemurStemCouple
		m_SupinePelvicTilt_mechanic_matrix_L->DeepCopy(mechanicFrameToCoupleFrame);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(0, 3, cupCOR_target[0] - stemCOR_tmp[0]);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(1, 3, cupCOR_target[1] - stemCOR_tmp[1]);
		m_SupinePelvicTilt_mechanic_matrix_L->SetElement(2, 3, cupCOR_target[2] - stemCOR_tmp[2]);
	}

}


void lancet::ThaEnhancedReductionObject::CalReductionMatrices()
{
	// Check data availability
	if(m_PelvisCupCouple->GetPelvisObject()->Getsurface_pelvis()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_PelvisCupCouple is missing for enhancedReduction";
		return;
	}

	if(m_FemurStemCouple->GetStemObject()->GetSurface_stem()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_FemurStemCouple is missing for enhancedReduction";
		return;
	}

	if(m_FemurObject->Getsurface_femur()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_FemurStemCouple is missing for enhancedReduction";
		return;
	}

	CalNoTiltCanalMatrices();
	CalNoTiltMechanicMatrices();
	CalSupineCanalMatrices();
	CalSupineMechanicMatrices();

	CalSupineHipLengths();
	CalSupineCombinedOffsets();
}

void lancet::ThaEnhancedReductionObject::CalSupineHipLengths()
{
	// hip length is the distance from the lesser trochanter to the ASIS line when the femur is mechanically aligned
	// the pelvisTilt is the supine tilt

	auto asis_R_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_ASIS()->GetPoint(0);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_ASIS()->GetGeometry()->WorldToIndex(asis_R_pelvisFrame, asis_R_pelvisFrame);

	auto asis_L_pelvisFrame = m_PelvisCupCouple->GetPelvisObject()->Getpset_ASIS()->GetPoint(1);
	m_PelvisCupCouple->GetPelvisObject()->Getpset_ASIS()->GetGeometry()->WorldToIndex(asis_L_pelvisFrame, asis_L_pelvisFrame);

	mitk::Point3D asis_R_worldFrame;
	mitk::Point3D asis_L_worldFrame;
	auto tmpPset = mitk::PointSet::New();
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);
	tmpPset->GetGeometry()->IndexToWorld(asis_R_pelvisFrame, asis_R_worldFrame);
	tmpPset->GetGeometry()->IndexToWorld(asis_L_pelvisFrame, asis_L_worldFrame);

	auto lesTro_couple_coupleFrame = m_FemurStemCouple->GetFemurObject()->Getpset_lesserTrochanter()->GetPoint(0);
	m_FemurStemCouple->GetFemurObject()->Getpset_lesserTrochanter()->GetGeometry()->WorldToIndex(lesTro_couple_coupleFrame, lesTro_couple_coupleFrame);

	auto lesTro_object_femurFrame = m_FemurObject->Getpset_lesserTrochanter()->GetPoint(0);
	m_FemurObject->Getpset_lesserTrochanter()->GetGeometry()->WorldToIndex(lesTro_object_femurFrame, lesTro_object_femurFrame);

	mitk::Point3D lesTro_R_worldFrame;
	mitk::Point3D lesTro_L_worldFrame;

	// Right femur: m_FemurStemCouple
	// left femur: m_femurObject
	if(m_FemurObject ->GetfemurSide() == 1)
	{
		// Right femur : m_FemurStemCouple
		auto tmpPset_1 = mitk::PointSet::New();
		tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_mechanic_matrix_R);
		tmpPset_1->GetGeometry()->IndexToWorld(lesTro_couple_coupleFrame, lesTro_R_worldFrame);

		// left femur: m_femurObject
		auto tmpPset_2 = mitk::PointSet::New();
		tmpPset_2->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_mechanic_matrix_L);
		tmpPset_2->GetGeometry()->IndexToWorld(lesTro_object_femurFrame, lesTro_L_worldFrame);

	}

	// Right femur: m_femurObject
	// left femur: m_FemurStemCouple
	if (m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur: m_femurObject
		auto tmpPset_1 = mitk::PointSet::New();
		tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_mechanic_matrix_R);
		tmpPset_1->GetGeometry()->IndexToWorld(lesTro_object_femurFrame, lesTro_R_worldFrame);

		// left femur: m_FemurStemCouple
		auto tmpPset_2 = mitk::PointSet::New();
		tmpPset_2->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_mechanic_matrix_L);
		tmpPset_2->GetGeometry()->IndexToWorld(lesTro_couple_coupleFrame, lesTro_L_worldFrame);
	}

	m_HipLength_supine_R = GetPointLineDistance(lesTro_R_worldFrame, asis_R_worldFrame, asis_L_worldFrame);
	m_HipLength_supine_L = GetPointLineDistance(lesTro_L_worldFrame, asis_R_worldFrame, asis_L_worldFrame);

}

void lancet::ThaEnhancedReductionObject::CalSupineCombinedOffsets()
{
	// CombinedOffset is the distance from the canal axis (canal alignment) to the sagittal plane passing through the midline point
	// the pelvis tilt does not influence the combinedOffset

	auto proximalCanal_couple_coupleFrame = m_FemurStemCouple->GetFemurObject()->Getpset_femurCanal()->GetPoint(0);
	m_FemurStemCouple->GetFemurObject()->Getpset_femurCanal()->GetGeometry()->WorldToIndex(proximalCanal_couple_coupleFrame, proximalCanal_couple_coupleFrame);

	auto proximalCanal_object_femurFrame = m_FemurObject->Getpset_femurCanal()->GetPoint(0);
	m_FemurObject->Getpset_femurCanal()->GetGeometry()->WorldToIndex(proximalCanal_object_femurFrame, proximalCanal_object_femurFrame);

	mitk::Point3D proximalCanal_R_worldFrame;
	mitk::Point3D proximalCanal_L_worldFrame;

	// Right femur: m_FemurStemCouple
	// left femur: m_femurObject
	if (m_FemurObject->GetfemurSide() == 1)
	{
		// Right femur: m_FemurStemCouple
		auto tmpPset_0 = mitk::PointSet::New();
		tmpPset_0->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_canal_matrix_R);
		tmpPset_0->GetGeometry()->IndexToWorld(proximalCanal_couple_coupleFrame, proximalCanal_R_worldFrame);

		// Left femur: m_femurObject
		auto tmpPset_1 = mitk::PointSet::New();
		tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_canal_matrix_L);
		tmpPset_1->GetGeometry()->IndexToWorld(proximalCanal_object_femurFrame, proximalCanal_L_worldFrame);

	}

	// Right femur: m_femurObject
	// left femur: m_FemurStemCouple
	if (m_FemurObject->GetfemurSide() == 0)
	{
		// Right femur: m_femurObject
		auto tmpPset_0 = mitk::PointSet::New();
		tmpPset_0->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_canal_matrix_R);
		tmpPset_0->GetGeometry()->IndexToWorld(proximalCanal_object_femurFrame, proximalCanal_R_worldFrame);

	    // left femur: m_FemurStemCouple
		auto tmpPset_1 = mitk::PointSet::New();
		tmpPset_1->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_canal_matrix_L);
		tmpPset_1->GetGeometry()->IndexToWorld(proximalCanal_couple_coupleFrame, proximalCanal_L_worldFrame);
	}

	m_CombinedOffset_supine_R = abs(proximalCanal_R_worldFrame[0]);
	m_CombinedOffset_supine_L = abs(proximalCanal_L_worldFrame[0]);


}


double lancet::ThaEnhancedReductionObject::GetPointLineDistance(mitk::Point3D point, mitk::Point3D linePoint_0, mitk::Point3D linePoint_1)
{
	Eigen::Vector3d lineVector;
	lineVector[0] = linePoint_0[0] - linePoint_1[0];
	lineVector[1] = linePoint_0[1] - linePoint_1[1];
	lineVector[2] = linePoint_0[2] - linePoint_1[2];

	Eigen::Vector3d tmpVector;
	tmpVector[0] = linePoint_0[0] - point[0];
	tmpVector[1] = linePoint_0[1] - point[1];
	tmpVector[2] = linePoint_0[2] - point[2];

	double projection = lineVector.dot(tmpVector) / lineVector.norm();

	double distance = sqrt(pow(tmpVector.norm(), 2) - pow(projection, 2));

	return distance;
}
