/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetApplySurfaceRegistratioinFilter.h"

#include "mitkMatrixConvert.h"

lancet::ApplySurfaceRegistratioinFilter::ApplySurfaceRegistratioinFilter() : mitk::NavigationDataToNavigationDataFilter()
{
  m_NavigationDataOfRF = mitk::NavigationData::New();
  m_RegistrationMatrix = mitk::AffineTransform3D::New();
  m_navigationImage = lancet::NavigationObject::New();
}


lancet::ApplySurfaceRegistratioinFilter::~ApplySurfaceRegistratioinFilter()
{
  m_NavigationDataOfRF = nullptr;
  m_RegistrationMatrix = nullptr;
  m_navigationImage = nullptr;
}

void lancet::ApplySurfaceRegistratioinFilter::GenerateData()
{
	// Assign the input directly to the output
	// Change the image(surface) geometry matrix separately
	
	// Change the image(surface) geometry matrix separately
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
	m_RegistrationMatrix->GetInverse(rfToSurfaceMatrix);

	auto ndiToSurface = mitk::AffineTransform3D::New();
	ndiToSurface->Compose(rfToSurfaceMatrix);
	ndiToSurface->Compose(m_NavigationDataOfRF->GetAffineTransform3D());

	dynamic_cast<mitk::Surface*>(m_navigationImage->GetDataNode()->GetData())->GetGeometry()->SetIndexToWorldTransform(ndiToSurface);

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

		//copy information to output
		output->Graft(input); // copy all information from input to output
		output->SetPosition(input->GetPosition());
		output->SetOrientation(input->GetOrientation());
		//output->SetDataValid(input->IsDataValid());
	}
}

