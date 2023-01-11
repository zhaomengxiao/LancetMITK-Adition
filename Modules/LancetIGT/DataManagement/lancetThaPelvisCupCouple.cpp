
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
	

	double pelvicTilt_stand = m_PelvisObject->GetpelvicTilt_stand();

	// Operation side: right
	if( m_CupObject->GetOperationSide() == 0)
	{
		/*calculate the transform from cupFrame to pelvisFrame
		 * considering pelvicTilt_stand
		 * cup inclination: 40 degrees
		 * cup anteversion: 20 degrees
		 */

		vtkNew<vtkTransform> cupToPelvisTransform;
		cupToPelvisTransform->Identity();
		cupToPelvisTransform->PostMultiply();
		cupToPelvisTransform->RotateX(-20);
		cupToPelvisTransform->RotateY(40);
		cupToPelvisTransform->RotateX(-pelvicTilt_stand);
		cupToPelvisTransform->RotateZ(180);

		auto pelvisCOR_R = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(0);
		m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvisCOR_R, pelvisCOR_R);

		cupToPelvisTransform->Translate(
			-pelvisCOR_R[0],
			-pelvisCOR_R[1], 
			-pelvisCOR_R[2]);

		cupToPelvisTransform->Update();

		auto pelvisToCupMatrix = cupToPelvisTransform->GetMatrix();
		pelvisToCupMatrix->Invert();

		m_vtkMatrix_pelvisFrameToCupFrame->DeepCopy(pelvisToCupMatrix);
	}

	// operation side: left
	if(m_CupObject->GetOperationSide() == 1)
	{
		/*calculate the transform from cupFrame to pelvisFrame
		 * considering pelvicTilt_stand
		 * cup inclination: 40 degrees
		 * cup anteversion: 20 degrees
		 */

		vtkNew<vtkTransform> cupToPelvisTransform;
		cupToPelvisTransform->Identity();
		cupToPelvisTransform->PostMultiply();
		cupToPelvisTransform->RotateX(20);
		cupToPelvisTransform->RotateY(40);
		cupToPelvisTransform->RotateX(pelvicTilt_stand);

		auto pelvicCOR_L = m_PelvisObject->Getpset_pelvisCOR()->GetPoint(1);
		m_PelvisObject->Getpset_pelvisCOR()->GetGeometry()->WorldToIndex(pelvicCOR_L, pelvicCOR_L);

		cupToPelvisTransform->Translate(
			-pelvicCOR_L[0],
			-pelvicCOR_L[1],
			-pelvicCOR_L[2]
		);

		cupToPelvisTransform->Update();

		auto pelvisToCupMatrix = cupToPelvisTransform->GetMatrix();
		pelvisToCupMatrix->Invert();

		m_vtkMatrix_pelvisFrameToCupFrame->DeepCopy(pelvisToCupMatrix);

	}

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
}

// TODO: Jan.11, 2023
void lancet::ThaPelvisCupCouple::UpdateCupAngles()
{
	// operation side: right
	if(m_CupObject->GetOperationSide() == 0)
	{
		Eigen::Vector3d cupAxis;
		cupAxis[0] = - m_vtkMatrix_pelvisFrameToCupFrame->GetElement(0, 2);
		cupAxis[1] = - m_vtkMatrix_pelvisFrameToCupFrame->GetElement(1, 2);
		cupAxis[2] = - m_vtkMatrix_pelvisFrameToCupFrame->GetElement(2, 2);
		//------ supine angles ----------
		Eigen::Vector3d planeNormal_supine; // the normal of the supine frontal plane in pelvisFrame
		planeNormal_supine[0] = 0;
		planeNormal_supine[1] = -1;
		planeNormal_supine[2] = 0;

		Eigen::Vector3d siVector_supine; // the superior-inferior vector in pelvisFrame
		siVector_supine[0] = 0;
		siVector_supine[1] = 0;
		siVector_supine[2] = -1;



		//------ stand angles -----------

	    //------- sit angles ------------
	}


	// operation side: left
	if (m_CupObject->GetOperationSide() == 1)
	{
		//------ supine angles ----------

		//------ stand angles -----------

		//------- sit angles ------------
	}
	
}







