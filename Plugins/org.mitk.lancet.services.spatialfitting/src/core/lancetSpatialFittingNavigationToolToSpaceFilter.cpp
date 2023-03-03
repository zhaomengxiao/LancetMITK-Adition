#include "lancetSpatialFittingNavigationToolToSpaceFilter.h"
BEGIN_SPATIAL_FITTING_NAMESPACE

NavigationToolToSpaceFilter::NavigationToolToSpaceFilter():mitk::NavigationDataToNavigationDataFilter()
{
	m_RegistrationMatrix = mitk::AffineTransform3D::New();
}

NavigationToolToSpaceFilter::~NavigationToolToSpaceFilter()
{
	m_RegistrationMatrix = nullptr;
}


void NavigationToolToSpaceFilter::GenerateData()
{
	MITK_DEBUG << __FUNCTION__ << ": log";
	if (m_RegistrationMatrix.IsNull())
	{
		itkExceptionMacro("Invalid parameter: m_RegistrationMatrix NULL.")
	}

	DataObjectPointerArraySizeType numberOfInputs = this->GetNumberOfInputs();

	if (numberOfInputs == 0)
	{
		return;
	}
		

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
		//transform nd from move device to reference device coords
		mitk::AffineTransform3D::Pointer inputTransform = input->GetAffineTransform3D();

		inputTransform->Compose(m_RegistrationMatrix);

		mitk::NavigationData::Pointer res = mitk::NavigationData::New(inputTransform);

		//copy information to output
		output->Graft(input); // copy all information from input to output
		output->SetPosition(res->GetPosition());
		output->SetOrientation(res->GetOrientation());
		//output->SetDataValid(input->IsDataValid());
	}

}
END_SPATIAL_FITTING_NAMESPACE

