
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkPlanes.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"

void DentalWidget::ClipTeeth()
{
	auto inputPolyData = dynamic_cast<mitk::Surface*>
		(GetDataStorage()->GetNamedNode("ios")->GetData());

	auto inputPointSet = dynamic_cast<mitk::PointSet*>
		(GetDataStorage()->GetNamedNode("landmark_src")->GetData());

	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter =
		vtkSmartPointer<vtkCleanPolyData>::New();

	int inputPointNum = inputPointSet->GetSize();

	for (int i{ 0 }; i < inputPointNum; i++)
	{
		double tmpPoint[3]
		{
			inputPointSet->GetPoint(i)[0],
			inputPointSet->GetPoint(i)[1],
			inputPointSet->GetPoint(i)[2],
		};

		vtkNew<vtkClipPolyData> clip;
		vtkNew<vtkPlanes> planes;

		double boxSize{ 6 };

		planes->SetBounds(
			tmpPoint[0] - boxSize / 2,
			tmpPoint[0] + boxSize / 2,
			tmpPoint[1] - boxSize / 2,
			tmpPoint[1] + boxSize / 2,
			tmpPoint[2] - boxSize / 2,
			tmpPoint[2] + boxSize / 2
		);

		clip->SetInputData(inputPolyData->GetVtkPolyData());
		clip->SetClipFunction(planes);
		clip->InsideOutOn();
		clip->GenerateClippedOutputOn();
		clip->Update();

		appendFilter->AddInputData(clip->GetOutput());
		appendFilter->Update();
	}

	cleanFilter->SetInputData(appendFilter->GetOutput());
	cleanFilter->Update();


	auto tmpNode = mitk::DataNode::New();
	auto tmpData = mitk::Surface::New();
	tmpData->SetVtkPolyData(cleanFilter->GetOutput());
	tmpNode->SetData(tmpData);
	tmpNode->SetName("Clipped data");
	GetDataStorage()->Add(tmpNode);
}


