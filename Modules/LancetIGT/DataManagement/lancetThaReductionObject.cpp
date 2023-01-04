#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaReductionObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaReductionObject::ThaReductionObject():
m_PelvisObject(lancet::ThaPelvisObject::New()),
m_FemurObject_R(lancet::ThaFemurObject::New()),
m_FemurObject_L(lancet::ThaFemurObject::New()),
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

lancet::ThaReductionObject::~ThaReductionObject()
{
	
}

bool lancet::ThaReductionObject::CheckBoneAvailablity()
{
	// CHeck if pelvisObject is available
	if(m_PelvisObject->Getpset_midline()->GetSize() == 0)
	{
		MITK_ERROR << "m_PelvisObject is missing for ThaReductionObject";
		return false;
	}

	// Check if left femurObject is available
	if(m_FemurObject_L->Getpset_femurCOR()->GetSize() == 0)
	{
		MITK_ERROR << "m_FemurObject_L is missing for ThaReductionObject";
		return false;
	}
	
	// Check if right femurObject is available
	if (m_FemurObject_R->Getpset_femurCOR()->GetSize() == 0)
	{
		MITK_ERROR << "m_FemurObject_R is missing for ThaReductionObject";
		return false;
	}

	return true;
}


void lancet::ThaReductionObject::CalOriginalReductionMatrices()
{
	if(CheckBoneAvailablity() == false)
	{
		return;
	}

	CalOriginalNoTiltCanalMatrices();
	CalOriginalNoTiltMechanicMatrices();
	CalOriginalSupineTiltCanalMatrices();
	CalOriginalSupineTiltMechanicMatrices();

}

void lancet::ThaReductionObject::CalOriginalNoTiltCanalMatrices()
{
	// pelvic matrix is identity matrix so there's no need for extra calculation
	// m_NoPelvicTilt_pelvisMatrix;

	// right femurObject matrix, only translation is required
	auto sourcePoint = m_FemurObject_R->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_R->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint, sourcePoint);

	auto targetPoint = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint, targetPoint);

	double rightTranslation[3];
	rightTranslation[0] = targetPoint[0] - sourcePoint[0];
	rightTranslation[1] = targetPoint[1] - sourcePoint[1];
	rightTranslation[2] = targetPoint[2] - sourcePoint[2];

	m_NoPelvicTilt_canal_matrix_R->SetElement(0, 3, rightTranslation[0]);
	m_NoPelvicTilt_canal_matrix_R->SetElement(1, 3, rightTranslation[1]);
	m_NoPelvicTilt_canal_matrix_R->SetElement(2, 3, rightTranslation[2]);

	// left femurObject matrix
	auto sourcePoint_ = m_FemurObject_L->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_L->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint_, sourcePoint_);

	auto targetPoint_ = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint_, targetPoint_);

	double leftTranslation[3];
	leftTranslation[0] = targetPoint_[0] - sourcePoint_[0];
	leftTranslation[1] = targetPoint_[1] - sourcePoint_[1];
	leftTranslation[2] = targetPoint_[2] - sourcePoint_[2];

	m_NoPelvicTilt_canal_matrix_L->SetElement(0, 3, leftTranslation[0]);
	m_NoPelvicTilt_canal_matrix_L->SetElement(1, 3, leftTranslation[1]);
	m_NoPelvicTilt_canal_matrix_L->SetElement(2, 3, leftTranslation[2]);
}

void lancet::ThaReductionObject::GetOriginalNoTiltCanalMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix, 
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix, 
	vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	// Check if calculation has been performed; if not, perform the calculation
	if(m_NoPelvicTilt_canal_matrix_L ->IsIdentity())
	{
		CalOriginalReductionMatrices();
	}

	pelvisMatrix->DeepCopy(m_NoPelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_NoPelvicTilt_canal_matrix_R);
	leftFemurMatrix->DeepCopy(m_NoPelvicTilt_canal_matrix_L);
}

void lancet::ThaReductionObject::CalOriginalNoTiltMechanicMatrices()
{
	// pelvic matrix is identity matrix so there's no need for extra calculation
	// m_NoPelvicTilt_pelvisMatrix;

	// right femur
	vtkNew<vtkMatrix4x4>mechanicToCanal_matrix_R;
	mechanicToCanal_matrix_R->DeepCopy(m_FemurObject_R->GetvtkMatrix_canalFrameToMechanicFrame());
	mechanicToCanal_matrix_R->Invert();
	auto femurCOR_R_old = m_FemurObject_R->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_R->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_R_old,femurCOR_R_old);

	vtkNew<vtkMatrix4x4> toolMatrix_R;
	toolMatrix_R->SetElement(0, 3, femurCOR_R_old[0]);
	toolMatrix_R->SetElement(1, 3, femurCOR_R_old[1]);
	toolMatrix_R->SetElement(2, 3, femurCOR_R_old[2]);

	vtkNew<vtkTransform> tmpTransform_R;
	tmpTransform_R->PostMultiply();
	tmpTransform_R->SetMatrix(toolMatrix_R);
	tmpTransform_R->Concatenate(mechanicToCanal_matrix_R);
	tmpTransform_R->Update();

	auto resultMatrix_R = tmpTransform_R->GetMatrix();

	double sourcePoint_R[3]
	{
		resultMatrix_R->GetElement(0,3),
		resultMatrix_R->GetElement(1,3),
		resultMatrix_R->GetElement(2,3)
	};

	auto targetPoint_R = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint_R, targetPoint_R);

	double translation_R[3];
	translation_R[0] = targetPoint_R[0] - sourcePoint_R[0];
	translation_R[1] = targetPoint_R[1] - sourcePoint_R[1];
	translation_R[2] = targetPoint_R[2] - sourcePoint_R[2];

	mechanicToCanal_matrix_R->SetElement(0, 3, translation_R[0]);
	mechanicToCanal_matrix_R->SetElement(1, 3, translation_R[1]);
	mechanicToCanal_matrix_R->SetElement(2, 3, translation_R[2]);

	m_NoPelvicTilt_mechanic_matrix_R->DeepCopy(mechanicToCanal_matrix_R);

	// left femur 
	vtkNew<vtkMatrix4x4>mechanicToCanal_matrix_L;
	mechanicToCanal_matrix_L->DeepCopy(m_FemurObject_L->GetvtkMatrix_canalFrameToMechanicFrame());
	mechanicToCanal_matrix_L->Invert();
	auto femurCOR_L_old = m_FemurObject_L->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_L->Getpset_femurCOR()->GetGeometry()->WorldToIndex(femurCOR_L_old, femurCOR_L_old);

	vtkNew<vtkMatrix4x4> toolMatrix_L;
	toolMatrix_L->SetElement(0, 3, femurCOR_L_old[0]);
	toolMatrix_L->SetElement(1, 3, femurCOR_L_old[1]);
	toolMatrix_L->SetElement(2, 3, femurCOR_L_old[2]);

	vtkNew<vtkTransform> tmpTransform_L;
	tmpTransform_L->PostMultiply();
	tmpTransform_L->SetMatrix(toolMatrix_L);
	tmpTransform_L->Concatenate(mechanicToCanal_matrix_L);
	tmpTransform_L->Update();

	auto resultMatrix_L = tmpTransform_L->GetMatrix();

	double sourcePoint_L[3]
	{
		resultMatrix_L->GetElement(0,3),
		resultMatrix_L->GetElement(1,3),
		resultMatrix_L->GetElement(2,3)
	};

	auto targetPoint_L = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(targetPoint_L, targetPoint_L);

	double translation_L[3];
	translation_L[0] = targetPoint_L[0] - sourcePoint_L[0];
	translation_L[1] = targetPoint_L[1] - sourcePoint_L[1];
	translation_L[2] = targetPoint_L[2] - sourcePoint_L[2];

	mechanicToCanal_matrix_L->SetElement(0, 3, translation_L[0]);
	mechanicToCanal_matrix_L->SetElement(1, 3, translation_L[1]);
	mechanicToCanal_matrix_L->SetElement(2, 3, translation_L[2]);

	m_NoPelvicTilt_mechanic_matrix_L->DeepCopy(mechanicToCanal_matrix_L);
}

void lancet::ThaReductionObject::GetOriginalNoTiltMechanicMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix,
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix,
	vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	// Check if calculation has been performed; if not, perform the calculation
	if (m_NoPelvicTilt_canal_matrix_L->IsIdentity())
	{
		CalOriginalReductionMatrices();
	}

	pelvisMatrix->DeepCopy(m_NoPelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_NoPelvicTilt_mechanic_matrix_R);
	leftFemurMatrix->DeepCopy(m_NoPelvicTilt_mechanic_matrix_L);
}

void lancet::ThaReductionObject::CalOriginalSupineTiltCanalMatrices()
{

}

void lancet::ThaReductionObject::GetOriginalSupineTiltCanalMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix,
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix,
	vtkSmartPointer<vtkMatrix4x4> leftFemurMat)
{

}

void lancet::ThaReductionObject::CalOriginalSupineTiltMechanicMatrices()
{

}

void lancet::ThaReductionObject::GetOriginalSupineTiltMechanicMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix,
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix,
	vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{

}