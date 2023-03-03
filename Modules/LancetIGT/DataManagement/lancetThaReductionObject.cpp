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

	CalOriginalSupineHipLengths();
	CalOriginalSupineCombinedOffsets();
}

void lancet::ThaReductionObject::CalOriginalSupineHipLengths()
{
	// hip length is the distance from the lesser trochanter to the ASIS line when the femur is mechanically aligned
	// the pelvisTilt is the supine tilt

	for(int hipSide{0}; hipSide < 2; hipSide++)
	{
		mitk::Point3D lesserTrochanter_femurFrame;
		vtkNew<vtkMatrix4x4> femurMatrix;
		if(hipSide == 0)
		{
			lesserTrochanter_femurFrame = m_FemurObject_R->Getpset_lesserTrochanter()->GetPoint(0);
			m_FemurObject_R->Getpset_lesserTrochanter()->GetGeometry()->WorldToIndex(lesserTrochanter_femurFrame, lesserTrochanter_femurFrame);
			femurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_R);
		}else
		{
			lesserTrochanter_femurFrame = m_FemurObject_L->Getpset_lesserTrochanter()->GetPoint(0);
			m_FemurObject_L->Getpset_lesserTrochanter()->GetGeometry()->WorldToIndex(lesserTrochanter_femurFrame, lesserTrochanter_femurFrame);
			femurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_L);
		}

		auto asis_R_pelvisFrame = m_PelvisObject->Getpset_ASIS()->GetPoint(0);
		m_PelvisObject->Getpset_ASIS()->GetGeometry()->WorldToIndex(asis_R_pelvisFrame, asis_R_pelvisFrame);

		auto asis_L_pelvisFrame = m_PelvisObject->Getpset_ASIS()->GetPoint(1);
		m_PelvisObject->Getpset_ASIS()->GetGeometry()->WorldToIndex(asis_L_pelvisFrame, asis_L_pelvisFrame);

		auto tmpPset = mitk::PointSet::New();

		// lesserTrochanter in worldFrame
		mitk::Point3D lesserTrochanter_worldFrame;
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurMatrix);
		tmpPset->GetGeometry()->IndexToWorld(lesserTrochanter_femurFrame,lesserTrochanter_worldFrame);

		// ASIS points in worldFrame when the pelvis is in supine position
		mitk::Point3D asis_R_worldFrame;
		mitk::Point3D asis_L_worldFrame;

		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);
		tmpPset->GetGeometry()->IndexToWorld(asis_L_pelvisFrame, asis_L_worldFrame);
		tmpPset->GetGeometry()->IndexToWorld(asis_R_pelvisFrame, asis_R_worldFrame);

		// calculate distance asis_R_worldFrame-asis_L_worldFrame line and lesserTrochanter_worldFrame
		Eigen::Vector3d asis_vector;
		asis_vector[0] = asis_L_worldFrame[0] - asis_R_worldFrame[0];
		asis_vector[1] = asis_L_worldFrame[1] - asis_R_worldFrame[1];
		asis_vector[2] = asis_L_worldFrame[2] - asis_R_worldFrame[2];

		Eigen::Vector3d asis_R_to_lesserTrochanter;
		asis_R_to_lesserTrochanter[0] = lesserTrochanter_worldFrame[0] - asis_R_worldFrame[0];
		asis_R_to_lesserTrochanter[1] = lesserTrochanter_worldFrame[1] - asis_R_worldFrame[1];
		asis_R_to_lesserTrochanter[2] = lesserTrochanter_worldFrame[2] - asis_R_worldFrame[2];

		double projection = asis_vector.dot(asis_R_to_lesserTrochanter)/asis_vector.norm();

		double hipLength = sqrt(pow(asis_R_to_lesserTrochanter.norm(), 2) - pow(projection, 2));

		if (hipSide == 0)
		{
			m_HipLength_supine_R = hipLength;
		}
		else
		{
			m_HipLength_supine_L = hipLength;
		}

	}

}

void lancet::ThaReductionObject::CalOriginalSupineCombinedOffsets()
{
	// CombinedOffset is the distance from the canal axis (canal alignment) to the sagittal plane passing through the midline point
	// the pelvis tilt does not influence the combinedOffset

	for (int hipSide{ 0 }; hipSide < 2; hipSide++)
	{
		mitk::Point3D proximalCanal_femurFrame;
		vtkNew<vtkMatrix4x4> femurMatrix;

		if( hipSide == 0 ) // right side
		{
			proximalCanal_femurFrame = m_FemurObject_R->Getpset_femurCanal()->GetPoint(0);
			m_FemurObject_R->Getpset_femurCanal()->GetGeometry()->WorldToIndex(proximalCanal_femurFrame, proximalCanal_femurFrame);
			femurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_R);
		}else
		{
			proximalCanal_femurFrame = m_FemurObject_L->Getpset_femurCanal()->GetPoint(0);
			m_FemurObject_L->Getpset_femurCanal()->GetGeometry()->WorldToIndex(proximalCanal_femurFrame, proximalCanal_femurFrame);
			femurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_L);
		}

		// calculate proximalCanal_worldFrame
		mitk::Point3D proximalCanal_worldFrame;

		auto tmpPset = mitk::PointSet::New();
		tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurMatrix);
		tmpPset->GetGeometry()->IndexToWorld(proximalCanal_femurFrame, proximalCanal_worldFrame);

		if(hipSide == 0)
		{
			m_CombinedOffset_supine_R = abs(proximalCanal_worldFrame[0]);
		}else
		{
			m_CombinedOffset_supine_L = abs(proximalCanal_worldFrame[0]);
		}

	}

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
	// calculate m_SupinePelvicTilt_pelvisMatrix using m_pelvicTilt_supine
	double pelvicTilt_degree = m_PelvisObject->GetpelvicTilt_supine();

	vtkNew<vtkTransform> pelvisTransform;
	pelvisTransform->Identity();
	pelvisTransform->RotateX(pelvicTilt_degree);
	pelvisTransform->Update();

	m_SupinePelvicTilt_pelvisMatrix->DeepCopy(pelvisTransform->GetMatrix());

	// calculate m_SupinePelvicTilt_canal_matrix_R and m_SupinePelvicTilt_canal_matrix_L
	auto pelvisCOR_R = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R, pelvisCOR_R);

	auto pelvisCOR_L = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_L, pelvisCOR_L);


	auto toolPset = mitk::PointSet::New();
	toolPset->InsertPoint(pelvisCOR_R);
	toolPset->InsertPoint(pelvisCOR_L);
	toolPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);
	auto target_R = toolPset->GetPoint(0);
	auto target_L = toolPset->GetPoint(1);

	// right femur, only translation is required
	auto sourcePoint_R = m_FemurObject_R->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_R->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint_R, sourcePoint_R);

	double rightTranslation[3];
	rightTranslation[0] = target_R[0] - sourcePoint_R[0];
	rightTranslation[1] = target_R[1] - sourcePoint_R[1];
	rightTranslation[2] = target_R[2] - sourcePoint_R[2];

	m_SupinePelvicTilt_canal_matrix_R->SetElement(0, 3, rightTranslation[0]);
	m_SupinePelvicTilt_canal_matrix_R->SetElement(1, 3, rightTranslation[1]);
	m_SupinePelvicTilt_canal_matrix_R->SetElement(2, 3, rightTranslation[2]);

	// left femur, only translation is required
	auto sourcePoint_L = m_FemurObject_L->Getpset_femurCOR()->GetPoint(0);
	m_FemurObject_L->Getpset_femurCOR()->GetGeometry()->WorldToIndex(sourcePoint_L, sourcePoint_L);

	double leftTranslation[3];
	leftTranslation[0] = target_L[0] - sourcePoint_L[0];
	leftTranslation[1] = target_L[1] - sourcePoint_L[1];
	leftTranslation[2] = target_L[2] - sourcePoint_L[2];

	m_SupinePelvicTilt_canal_matrix_L->SetElement(0, 3, leftTranslation[0]);
	m_SupinePelvicTilt_canal_matrix_L->SetElement(1, 3, leftTranslation[1]);
	m_SupinePelvicTilt_canal_matrix_L->SetElement(2, 3, leftTranslation[2]);

}

void lancet::ThaReductionObject::GetOriginalSupineTiltCanalMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix,
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix,
	vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	// Check if calculation has been performed; if not, perform the calculation
	if (m_NoPelvicTilt_canal_matrix_L->IsIdentity())
	{
		CalOriginalReductionMatrices();
	}

	pelvisMatrix->DeepCopy(m_SupinePelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_R);
	leftFemurMatrix->DeepCopy(m_SupinePelvicTilt_canal_matrix_L);
}

void lancet::ThaReductionObject::CalOriginalSupineTiltMechanicMatrices()
{
	// m_SupinePelvicTilt_pelvisMatrix has been calculated in CalOriginalSupineTiltCanalMatrices()
	// no need to calculate again

	auto pelvisCOR_R = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R, pelvisCOR_R);

	auto pelvisCOR_L = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);
	m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_L, pelvisCOR_L);

	auto toolPset = mitk::PointSet::New();
	toolPset->InsertPoint(pelvisCOR_R);
	toolPset->InsertPoint(pelvisCOR_L);
	toolPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_SupinePelvicTilt_pelvisMatrix);
	auto target_R = toolPset->GetPoint(0);
	auto target_L = toolPset->GetPoint(1);

	double offset_R[3]
	{
		target_R[0] - pelvisCOR_R[0],
		target_R[1] - pelvisCOR_R[1],
		target_R[2] - pelvisCOR_R[2]
	};
	double offset_L[3]
	{
		target_L[0] - pelvisCOR_L[0],
		target_L[1] - pelvisCOR_L[1],
		target_L[2] - pelvisCOR_L[2]
	};

	m_SupinePelvicTilt_mechanic_matrix_R->DeepCopy(m_NoPelvicTilt_mechanic_matrix_R);
	m_SupinePelvicTilt_mechanic_matrix_R->SetElement(0, 3, m_SupinePelvicTilt_mechanic_matrix_R->GetElement(0, 3) + offset_R[0]);
	m_SupinePelvicTilt_mechanic_matrix_R->SetElement(1, 3, m_SupinePelvicTilt_mechanic_matrix_R->GetElement(1, 3) + offset_R[1]);
	m_SupinePelvicTilt_mechanic_matrix_R->SetElement(2, 3, m_SupinePelvicTilt_mechanic_matrix_R->GetElement(2, 3) + offset_R[2]);

	m_SupinePelvicTilt_mechanic_matrix_L->DeepCopy(m_NoPelvicTilt_mechanic_matrix_L);
	m_SupinePelvicTilt_mechanic_matrix_L->SetElement(0, 3, m_SupinePelvicTilt_mechanic_matrix_L->GetElement(0, 3) + offset_L[0]);
	m_SupinePelvicTilt_mechanic_matrix_L->SetElement(1, 3, m_SupinePelvicTilt_mechanic_matrix_L->GetElement(1, 3) + offset_L[1]);
	m_SupinePelvicTilt_mechanic_matrix_L->SetElement(2, 3, m_SupinePelvicTilt_mechanic_matrix_L->GetElement(2, 3) + offset_L[2]);

}

void lancet::ThaReductionObject::GetOriginalSupineTiltMechanicMatrices(
	vtkSmartPointer<vtkMatrix4x4> pelvisMatrix,
	vtkSmartPointer<vtkMatrix4x4> rightFemurMatrix,
	vtkSmartPointer<vtkMatrix4x4> leftFemurMatrix)
{
	// Check if calculation has been performed; if not, perform the calculation
	if (m_NoPelvicTilt_canal_matrix_L->IsIdentity())
	{
		CalOriginalReductionMatrices();
	}

	pelvisMatrix->DeepCopy(m_SupinePelvicTilt_pelvisMatrix);
	rightFemurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_R);
	leftFemurMatrix->DeepCopy(m_SupinePelvicTilt_mechanic_matrix_L);
}