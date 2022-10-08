/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetApplySurfaceRegistratioinStaticImageFilter.h"

#include "mitkMatrixConvert.h"

lancet::ApplySurfaceRegistratioinStaticImageFilter::ApplySurfaceRegistratioinStaticImageFilter() : mitk::NavigationDataToNavigationDataFilter()
{
  m_NavigationDataOfRF = mitk::NavigationData::New();
  m_RegistrationMatrix = mitk::AffineTransform3D::New();
  //m_navigationImage = lancet::NavigationObject::New();
}


lancet::ApplySurfaceRegistratioinStaticImageFilter::~ApplySurfaceRegistratioinStaticImageFilter()
{
  m_NavigationDataOfRF = nullptr;
  m_RegistrationMatrix = nullptr;
  // m_navigationImage = nullptr;
}

void lancet::ApplySurfaceRegistratioinStaticImageFilter::GenerateData()
{
	// Assign the input directly to the output
	// Change the image(surface) geometry matrix separately
	
	// Change the image(surface) geometry matrix 
	if (m_NavigationDataOfRF.IsNull())
	{
		itkExceptionMacro("Invalid parameter: m_NavigationDataOfRF NULL.");
		return;
	}
	if (m_RegistrationMatrix.IsNull())
	{
		itkExceptionMacro("Invalid parameter: m_RegistrationMatrix NULL.");
		return;
	}
	// if (m_navigationImage.IsNull())
	// {
	// 	itkExceptionMacro("Invalid parameter: m_navigationImage NULL.");
	// 	return;
	// }

	//auto surfaceToRfMatrix = mitk::AffineTransform3D::New();
	auto rfToSurfaceMatrix = mitk::AffineTransform3D::New();

	//mitk::TransferVtkMatrixToItkTransform(m_RegistrationMatrix, surfaceToRfMatrix.GetPointer());
	//m_RegistrationMatrix->GetInverse(rfToSurfaceMatrix);

	vtkNew<vtkMatrix4x4> tmpMatrix;
	mitk::TransferItkTransformToVtkMatrix(m_RegistrationMatrix.GetPointer(), tmpMatrix);
	tmpMatrix->Invert();

	mitk::TransferVtkMatrixToItkTransform(tmpMatrix, rfToSurfaceMatrix.GetPointer());

	auto ndiToSurface = mitk::AffineTransform3D::New();
	ndiToSurface->Compose(rfToSurfaceMatrix);
	ndiToSurface->Compose(m_NavigationDataOfRF->GetAffineTransform3D());

	vtkNew<vtkMatrix4x4> vtkMatrixSurfaceToNdi;
	mitk::TransferItkTransformToVtkMatrix(ndiToSurface.GetPointer(), vtkMatrixSurfaceToNdi);
	vtkMatrixSurfaceToNdi->Invert();
	auto affineSurfaceToNdi = mitk::AffineTransform3D::New();
	mitk::TransferVtkMatrixToItkTransform(vtkMatrixSurfaceToNdi, affineSurfaceToNdi.GetPointer());

	// dynamic_cast<mitk::Surface*>(m_navigationImage->GetDataNode()->GetData())->GetGeometry()->SetIndexToWorldTransform(ndiToSurface);

	// Assign the input directly to the output
	DataObjectPointerArraySizeType numberOfInputs = this->GetNumberOfInputs();

	if (numberOfInputs == 0)
		return;

	this->CreateOutputsForAllInputs();

	// generate output
	for (unsigned int i = 0; i < numberOfInputs; ++i)
	{
		const mitk::NavigationData* input = this->GetInput(i);
		assert(input);

		mitk::NavigationData* output = this->GetOutput(i);
		assert(output);

		if (input->IsDataValid() == false)
		{
			output->SetDataValid(false);
			continue;
		}

		mitk::AffineTransform3D::Pointer inputTransform = input->GetAffineTransform3D();
		inputTransform->Compose(affineSurfaceToNdi);

		mitk::NavigationData::Pointer res = mitk::NavigationData::New(inputTransform);

		//copy information to output
		output->Graft(input); // copy all information from input to output
		output->SetPosition(res->GetPosition());
		output->SetOrientation(res->GetOrientation());
		//output->SetDataValid(input->IsDataValid());
	}
}

