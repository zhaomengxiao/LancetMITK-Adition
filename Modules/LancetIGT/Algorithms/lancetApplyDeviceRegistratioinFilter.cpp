/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetApplyDeviceRegistratioinFilter.h"

lancet::ApplyDeviceRegistratioinFilter::ApplyDeviceRegistratioinFilter() : mitk::NavigationDataToNavigationDataFilter()
{
  m_NavigationDataOfRF = mitk::NavigationData::New();
  m_RegistrationMatrix = mitk::AffineTransform3D::New();
}


lancet::ApplyDeviceRegistratioinFilter::~ApplyDeviceRegistratioinFilter()
{
  m_NavigationDataOfRF = nullptr;
  m_RegistrationMatrix = nullptr;
}

void lancet::ApplyDeviceRegistratioinFilter::GenerateData()
{
  // only update data if m_Transform was set
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
  DataObjectPointerArraySizeType numberOfInputs = this->GetNumberOfInputs();

  if (numberOfInputs == 0)
    return;

  this->CreateOutputsForAllInputs();

  // generate output
  for (unsigned int i = 0; i < numberOfInputs; ++i)
  {
    const mitk::NavigationData *input = this->GetInput(i);
    assert(input);

    mitk::NavigationData *output = this->GetOutput(i);
    assert(output);

    if (input->IsDataValid() == false)
    {
      output->SetDataValid(false);
      continue;
    }
    //transform nd from move device to reference device coords
    mitk::AffineTransform3D::Pointer inputTransform = input->GetAffineTransform3D();

    inputTransform->Compose(m_RegistrationMatrix);
    inputTransform->Compose(m_NavigationDataOfRF->GetAffineTransform3D());

    mitk::NavigationData::Pointer res = mitk::NavigationData::New(inputTransform);

    //copy information to output
    output->Graft(input); // copy all information from input to output
    output->SetPosition(res->GetPosition());
    output->SetOrientation(res->GetOrientation());
    //output->SetDataValid(input->IsDataValid());
  }
}

