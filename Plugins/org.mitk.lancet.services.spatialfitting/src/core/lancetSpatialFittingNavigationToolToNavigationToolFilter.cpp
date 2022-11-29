#include "lancetSpatialFittingNavigationToolToNavigationToolFilter.h"

BEGIN_SPATIAL_FITTING_NAMESPACE

// struct NavigationToolToNavigationToolFilter::NavigationToolToNavigationToolFilterPrivateImp
// {
// };

// NavigationToolToNavigationToolFilter::NavigationToolToNavigationToolFilter()
// 	: imp(std::make_shared<NavigationToolToNavigationToolFilterPrivateImp>())
// {
// }

NavigationToolToNavigationToolFilter::NavigationToolToNavigationToolFilter() : mitk::NavigationDataToNavigationDataFilter()
{
	m_UCSNavigationData = mitk::NavigationData::New();
}

NavigationToolToNavigationToolFilter::~NavigationToolToNavigationToolFilter()
{
	m_UCSNavigationData = nullptr;
}


void NavigationToolToNavigationToolFilter::GenerateData()
{
	MITK_DEBUG << __FUNCTION__ << ": log";

	if (m_UCSNavigationData.IsNull())
	{
		itkExceptionMacro("Invalid parameter: m_UCSNavigationData NULL.");
		return;
	}

	auto inverseUCSNavigationData = m_UCSNavigationData->GetInverse();

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
		//transform nd from move device to reference device coords
		mitk::AffineTransform3D::Pointer inputTransform = input->GetAffineTransform3D();

		inputTransform->Compose(inverseUCSNavigationData->GetAffineTransform3D());
		
		mitk::NavigationData::Pointer res = mitk::NavigationData::New(inputTransform);

		//copy information to output
		output->Graft(input); // copy all information from input to output
		output->SetPosition(res->GetPosition());
		output->SetOrientation(res->GetOrientation());
		//output->SetDataValid(input->IsDataValid());
	}
}

END_SPATIAL_FITTING_NAMESPACE
