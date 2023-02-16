
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaPelvisCupCouple.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaPelvisCupCouple::ThaPelvisCupCouple():
m_PelvisObject(lancet::ThaPelvisObject::New()),
m_CupObject(lancet::ThaCupObject::New()),
m_vtkMatrix_pelvisFrameToCupFrame(vtkMatrix4x4::New()),
m_vtkMatrix_coupleGeometry(vtkMatrix4x4::New())
{
	
}

lancet::ThaPelvisCupCouple::~ThaPelvisCupCouple()
{
	
}

void lancet::ThaPelvisCupCouple::InitializePelvisFrameToCupFrameMatrix()
{
	// Check if m_PelvisObject and m_CupObject are both ready
	if(m_PelvisObject->Getsurface_pelvis()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_PelvisObject is empty when InitializePelvisFrameToCupFrameMatrix()";
	}

	if(m_CupObject->GetSurface_cupFrame()->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_CupObject is empty when InitializePelvisFrameToCupFrameMatrix()";
	}

	// Initial cup orientation has been set to 40 degrees of inclination and 20 degrees of anteversion in the Stand pose
	// Mako THA 4.0 Page 22
	// For simplification reasons, do not consider pelvic tilt
	// i.e. Initial cup orientation has been set to 40 degrees of inclination and 20 degrees of anteversion with the
	// pelvic plane aligned with the frontal plane.

	double pelvicTilt_stand = m_PelvisObject->GetpelvicTilt_stand();
	
	if (m_CupObject->GetOperationSide() == 0)
	{
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->PostMultiply();
		tmpTransform->RotateX(-20);
		tmpTransform->RotateY(40);

		auto pelvisCOR_R = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);

		tmpTransform->Translate(
			pelvisCOR_R[0],
			pelvisCOR_R[1],
			pelvisCOR_R[2]
		);

		m_vtkMatrix_pelvisFrameToCupFrame->DeepCopy(tmpTransform->GetMatrix());
	}

	if (m_CupObject->GetOperationSide() == 1)
	{
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->PostMultiply();
		tmpTransform->RotateZ(180);
		tmpTransform->RotateX(-20);
		tmpTransform->RotateY(-40);

		auto pelvisCOR_L = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);

		tmpTransform->Translate(
			pelvisCOR_L[0],
			pelvisCOR_L[1],
			pelvisCOR_L[2]
		);

		m_vtkMatrix_pelvisFrameToCupFrame->DeepCopy(tmpTransform->GetMatrix());
	}

	// Move the cupObject to the initial position
	auto pelvisObjectMatrix = m_PelvisObject->GetvtkMatrix_groupGeometry();
	SetCoupleGeometry(pelvisObjectMatrix);

	UpdateCupAngles();
	UpdateRelativeCupCOR();
}

void lancet::ThaPelvisCupCouple::SetCoupleGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	// Check whether the couple has been initialized
	if(m_vtkMatrix_pelvisFrameToCupFrame->IsIdentity())
	{
		MITK_INFO << "InitializePelvisFrameToCupFrameMatrix() before SetCoupleGeometry()";
		InitializePelvisFrameToCupFrameMatrix();
	}

	m_PelvisObject->SetGroupGeometry(newMatrix);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(m_vtkMatrix_pelvisFrameToCupFrame);
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Update();

	auto cupMatrix = tmpTransform->GetMatrix();

	m_CupObject->SetGroupGeometry(cupMatrix);

	m_vtkMatrix_coupleGeometry->DeepCopy(newMatrix);
}

void lancet::ThaPelvisCupCouple::UpdateCupAngles()
{
	vtkNew<vtkMatrix4x4> pelvisFrameToCupFrameMatrix;
	pelvisFrameToCupFrameMatrix->DeepCopy(m_vtkMatrix_pelvisFrameToCupFrame);

	Eigen::Vector3d minusZvector;
	minusZvector[0] = 0;
	minusZvector[1] = 0;
	minusZvector[2] = -1;

	// calculate worldFrameToCupFrameMatrix with supine/stand/sit pelvic tilt
	// and calculate the cup version and inclination with this matrix
	// pelvic tilt is in degree, anterior(+), posterior(-)

	//------ supine angles ----------
	vtkNew<vtkTransform> worldFrameToCupFrameTransform_supine;
	worldFrameToCupFrameTransform_supine->PostMultiply();
	worldFrameToCupFrameTransform_supine->SetMatrix(pelvisFrameToCupFrameMatrix);
	double pelvicTilt_supine{ m_PelvisObject->GetpelvicTilt_supine() };
	worldFrameToCupFrameTransform_supine->RotateX(pelvicTilt_supine);
	worldFrameToCupFrameTransform_supine->Update();

	auto worldFrameToCupFrameMatrix_supine = worldFrameToCupFrameTransform_supine->GetMatrix();

	// Cup axis in worldFrame: the minus z axis of cupFrame in worldFrame 
	Eigen::Vector3d cupAxisInWorldFrame_supine;
	cupAxisInWorldFrame_supine[0] = -worldFrameToCupFrameMatrix_supine->GetElement(0, 2);
	cupAxisInWorldFrame_supine[1] = -worldFrameToCupFrameMatrix_supine->GetElement(1, 2);
	cupAxisInWorldFrame_supine[2] = -worldFrameToCupFrameMatrix_supine->GetElement(2, 2);

	// if cupAxisInWorldFrame_supine[1] is minus, the cup is anteversed(+ degree)
	// if cupAxisInWorldFrame_supine[1] is positive, the cup is retroversed(- degree)
	m_CupVersion_supine = asin(-cupAxisInWorldFrame_supine[1]) * 180 / 3.1415926;

	Eigen::Vector3d cupAxisProjectInWorldFrame_supine;
	cupAxisProjectInWorldFrame_supine[0] = -worldFrameToCupFrameMatrix_supine->GetElement(0, 2);
	cupAxisProjectInWorldFrame_supine[1] = 0;
	cupAxisProjectInWorldFrame_supine[2] = -worldFrameToCupFrameMatrix_supine->GetElement(2, 2);

	m_CupInclination_supine = acos(cupAxisProjectInWorldFrame_supine.dot(minusZvector)/ cupAxisProjectInWorldFrame_supine.norm()) * 180 / 3.1415926;

	if(m_CupObject->GetOperationSide() == 0) // right operation side
	{
		if(cupAxisProjectInWorldFrame_supine[0] > 0)
		{
			m_CupInclination_supine = -m_CupInclination_supine;
		}
	}

	if (m_CupObject->GetOperationSide() != 0) // left operation side
	{
		if (cupAxisProjectInWorldFrame_supine[0] < 0)
		{
			m_CupInclination_supine = -m_CupInclination_supine;
		}
	}

	//------ stand angles -----------
	vtkNew<vtkTransform> worldFrameToCupFrameTransform_stand;
	worldFrameToCupFrameTransform_stand->PostMultiply();
	worldFrameToCupFrameTransform_stand->SetMatrix(pelvisFrameToCupFrameMatrix);
	double pelvicTilt_stand{ m_PelvisObject->GetpelvicTilt_stand() };
	worldFrameToCupFrameTransform_stand->RotateX(pelvicTilt_stand);
	worldFrameToCupFrameTransform_stand->Update();

	auto worldFrameToCupFrameMatrix_stand = worldFrameToCupFrameTransform_stand->GetMatrix();

	// Cup axis in worldFrame: the minus z axis of cupFrame in worldFrame 
	Eigen::Vector3d cupAxisInWorldFrame_stand;
	cupAxisInWorldFrame_stand[0] = -worldFrameToCupFrameMatrix_stand->GetElement(0, 2);
	cupAxisInWorldFrame_stand[1] = -worldFrameToCupFrameMatrix_stand->GetElement(1, 2);
	cupAxisInWorldFrame_stand[2] = -worldFrameToCupFrameMatrix_stand->GetElement(2, 2);

	// if cupAxisInWorldFrame_stand[1] is minus, the cup is anteversed(+ degree)
	// if cupAxisInWorldFrame_stand[1] is positive, the cup is retroversed(- degree)
	m_CupVersion_stand = asin(-cupAxisInWorldFrame_stand[1]) * 180 / 3.1415926;

	Eigen::Vector3d cupAxisProjectInWorldFrame_stand;
	cupAxisProjectInWorldFrame_stand[0] = -worldFrameToCupFrameMatrix_stand->GetElement(0, 2);
	cupAxisProjectInWorldFrame_stand[1] = 0;
	cupAxisProjectInWorldFrame_stand[2] = -worldFrameToCupFrameMatrix_stand->GetElement(2, 2);

	m_CupInclination_stand = acos(cupAxisProjectInWorldFrame_stand.dot(minusZvector) / cupAxisProjectInWorldFrame_stand.norm()) * 180 / 3.1415926;

	if (m_CupObject->GetOperationSide() == 0) // right operation side
	{
		if (cupAxisProjectInWorldFrame_stand[0] > 0)
		{
			m_CupInclination_stand = -m_CupInclination_stand;
		}
	}

	if (m_CupObject->GetOperationSide() != 0) // left operation side
	{
		if (cupAxisProjectInWorldFrame_stand[0] < 0)
		{
			m_CupInclination_stand = -m_CupInclination_stand;
		}
	}

	//------- sit angles ------------
	vtkNew<vtkTransform> worldFrameToCupFrameTransform_sit;
	worldFrameToCupFrameTransform_sit->PostMultiply();
	worldFrameToCupFrameTransform_sit->SetMatrix(pelvisFrameToCupFrameMatrix);
	double pelvicTilt_sit{ m_PelvisObject->GetpelvicTilt_sit() };
	worldFrameToCupFrameTransform_sit->RotateX(pelvicTilt_sit);
	worldFrameToCupFrameTransform_sit->Update();

	auto worldFrameToCupFrameMatrix_sit = worldFrameToCupFrameTransform_sit->GetMatrix();

	// Cup axis in worldFrame: the minus z axis of cupFrame in worldFrame 
	Eigen::Vector3d cupAxisInWorldFrame_sit;
	cupAxisInWorldFrame_sit[0] = -worldFrameToCupFrameMatrix_sit->GetElement(0, 2);
	cupAxisInWorldFrame_sit[1] = -worldFrameToCupFrameMatrix_sit->GetElement(1, 2);
	cupAxisInWorldFrame_sit[2] = -worldFrameToCupFrameMatrix_sit->GetElement(2, 2);

	// if cupAxisInWorldFrame_sit[1] is minus, the cup is anteversed(+ degree)
	// if cupAxisInWorldFrame_sit[1] is positive, the cup is retroversed(- degree)
	m_CupVersion_sit = asin(-cupAxisInWorldFrame_sit[1]) * 180 / 3.1415926;

	Eigen::Vector3d cupAxisProjectInWorldFrame_sit;
	cupAxisProjectInWorldFrame_sit[0] = -worldFrameToCupFrameMatrix_sit->GetElement(0, 2);
	cupAxisProjectInWorldFrame_sit[1] = 0;
	cupAxisProjectInWorldFrame_sit[2] = -worldFrameToCupFrameMatrix_sit->GetElement(2, 2);

	m_CupInclination_sit = acos(cupAxisProjectInWorldFrame_sit.dot(minusZvector) / cupAxisProjectInWorldFrame_sit.norm()) * 180 / 3.1415926;

	if (m_CupObject->GetOperationSide() == 0) // right operation side
	{
		if (cupAxisProjectInWorldFrame_sit[0] > 0)
		{
			m_CupInclination_sit = -m_CupInclination_sit;
		}
	}

	if (m_CupObject->GetOperationSide() != 0) // left operation side
	{
		if (cupAxisProjectInWorldFrame_sit[0] < 0)
		{
			m_CupInclination_sit = -m_CupInclination_sit;
		}
	}

	// no pelvic tilt
	vtkNew<vtkTransform> worldFrameToCupFrameTransform_noTilt;
	worldFrameToCupFrameTransform_noTilt->PostMultiply();
	worldFrameToCupFrameTransform_noTilt->SetMatrix(pelvisFrameToCupFrameMatrix);
	double pelvicTilt_noTilt{ 0 };
	worldFrameToCupFrameTransform_noTilt->RotateX(pelvicTilt_noTilt);
	worldFrameToCupFrameTransform_noTilt->Update();

	auto worldFrameToCupFrameMatrix_noTilt = worldFrameToCupFrameTransform_noTilt->GetMatrix();

	// Cup axis in worldFrame: the minus z axis of cupFrame in worldFrame 
	Eigen::Vector3d cupAxisInWorldFrame_noTilt;
	cupAxisInWorldFrame_noTilt[0] = -worldFrameToCupFrameMatrix_noTilt->GetElement(0, 2);
	cupAxisInWorldFrame_noTilt[1] = -worldFrameToCupFrameMatrix_noTilt->GetElement(1, 2);
	cupAxisInWorldFrame_noTilt[2] = -worldFrameToCupFrameMatrix_noTilt->GetElement(2, 2);

	// if cupAxisInWorldFrame_noTilt[1] is minus, the cup is anteversed(+ degree)
	// if cupAxisInWorldFrame_noTilt[1] is positive, the cup is retroversed(- degree)
	m_CupVersion_noTilt = asin(-cupAxisInWorldFrame_noTilt[1]) * 180 / 3.1415926;

	Eigen::Vector3d cupAxisProjectInWorldFrame_noTilt;
	cupAxisProjectInWorldFrame_noTilt[0] = -worldFrameToCupFrameMatrix_noTilt->GetElement(0, 2);
	cupAxisProjectInWorldFrame_noTilt[1] = 0;
	cupAxisProjectInWorldFrame_noTilt[2] = -worldFrameToCupFrameMatrix_noTilt->GetElement(2, 2);

	m_CupInclination_noTilt = acos(cupAxisProjectInWorldFrame_noTilt.dot(minusZvector) / cupAxisProjectInWorldFrame_noTilt.norm()) * 180 / 3.1415926;

	if (m_CupObject->GetOperationSide() == 0) // right operation side
	{
		if (cupAxisProjectInWorldFrame_noTilt[0] > 0)
		{
			m_CupInclination_noTilt = -m_CupInclination_noTilt;
		}
	}

	if (m_CupObject->GetOperationSide() != 0) // left operation side
	{
		if (cupAxisProjectInWorldFrame_noTilt[0] < 0)
		{
			m_CupInclination_noTilt = -m_CupInclination_noTilt;
		}
	}
}

void lancet::ThaPelvisCupCouple::UpdateRelativeCupCOR()
{
	// Calculate pelvisCOR in worldFrame
	auto pelvisCORs = m_PelvisObject->Getpset_pelvisCOR();

	auto pelvisCOR = pelvisCORs->GetPoint(m_CupObject->GetOperationSide());

	vtkNew<vtkMatrix4x4> tmpPelvisCORmatrix;
	tmpPelvisCORmatrix->Identity();
	tmpPelvisCORmatrix->SetElement(0, 3, pelvisCOR[0]);
	tmpPelvisCORmatrix->SetElement(1,3, pelvisCOR[1]);
	tmpPelvisCORmatrix->SetElement(2, 3, pelvisCOR[2]);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(tmpPelvisCORmatrix);
	double pelvicTilt_supine{ m_PelvisObject->GetpelvicTilt_supine() };
	tmpTransform->RotateX(pelvicTilt_supine);
	tmpTransform->Update();

	auto tmpMatrix = tmpTransform->GetMatrix();

	double pelvisCORinWorldFrame[3]
	{
		tmpMatrix->GetElement(0,3),
		tmpMatrix->GetElement(1,3),
		tmpMatrix->GetElement(2,3)
	};

	// Calculate cupCOR in worldFrame
	vtkNew<vtkTransform> worldFrameToCupFrameTransform;
	worldFrameToCupFrameTransform->Identity();
	worldFrameToCupFrameTransform->PostMultiply();
	worldFrameToCupFrameTransform->Concatenate(m_vtkMatrix_pelvisFrameToCupFrame);
	worldFrameToCupFrameTransform->RotateX(pelvicTilt_supine);
	worldFrameToCupFrameTransform->Update();

	auto worldFrameToCupFrameMatrix = worldFrameToCupFrameTransform->GetMatrix();

	double cupCORinWorldFrame[3]
	{
		worldFrameToCupFrameMatrix->GetElement(0,3),
		worldFrameToCupFrameMatrix->GetElement(1,3),
		worldFrameToCupFrameMatrix->GetElement(2,3)
	};

	// superior (+) / inferior (-)
	m_CupCOR_SI_supine = cupCORinWorldFrame[2] - pelvisCORinWorldFrame[2];
	m_CupCOR_SI_noTilt = m_vtkMatrix_pelvisFrameToCupFrame->GetElement(2, 3) - pelvisCOR[2];

	// medial (+) / lateral (-)
	m_CupCOR_ML_supine = abs(pelvisCORinWorldFrame[0]) - abs(cupCORinWorldFrame[0]);
	m_CupCOR_ML_noTilt = abs(pelvisCOR[0]) - abs(m_vtkMatrix_pelvisFrameToCupFrame->GetElement(0, 3));

	// anterior (+) / posterior (-)
	m_CupCOR_AP_supine = pelvisCORinWorldFrame[1] - cupCORinWorldFrame[1];
	m_CupCOR_AP_noTilt = pelvisCOR[1] - m_vtkMatrix_pelvisFrameToCupFrame->GetElement(1, 3);
}

void lancet::ThaPelvisCupCouple::SetPelvisFrameToCupFrameMatrix(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->SetMatrix(m_vtkMatrix_coupleGeometry);
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Update();

	m_CupObject->SetGroupGeometry(tmpTransform->GetMatrix());

	m_vtkMatrix_pelvisFrameToCupFrame->DeepCopy(newMatrix);

	UpdateCupAngles();
	UpdateRelativeCupCOR();
}

mitk::Point3D lancet::ThaPelvisCupCouple::GetCupCenterInPelvisFrame()
{
	mitk::Point3D cupCenterInPelvisFrame;

	cupCenterInPelvisFrame[0] = m_vtkMatrix_pelvisFrameToCupFrame->GetElement(0, 3);
	cupCenterInPelvisFrame[1] = m_vtkMatrix_pelvisFrameToCupFrame->GetElement(1, 3);
	cupCenterInPelvisFrame[2] = m_vtkMatrix_pelvisFrameToCupFrame->GetElement(2, 3);

	return cupCenterInPelvisFrame;
}





