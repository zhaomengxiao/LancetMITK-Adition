
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

void DentalWidget::TestClipPolyData()
{
	auto inputPolyData = dynamic_cast<mitk::Surface*>
		(m_Controls.mitkNodeSelectWidget_testInputSurface->GetSelectedNode()->GetData())->GetVtkPolyData();

	auto inputPointSet = dynamic_cast<mitk::PointSet*>
		(m_Controls.mitkNodeSelectWidget_testClipPoints->GetSelectedNode()->GetData());

	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter =
		vtkSmartPointer<vtkCleanPolyData>::New();

	int inputPointNum = inputPointSet->GetSize();

	for (int i{0}; i < inputPointNum; i++)
	{
		double tmpPoint[3]
		{
			inputPointSet->GetPoint(i)[0],
			inputPointSet->GetPoint(i)[1],
			inputPointSet->GetPoint(i)[2],
		};

		vtkNew<vtkClipPolyData> clip;
		vtkNew<vtkPlanes> planes;
		// double planesArray[24]{0};
		double boxSize{ m_Controls.lineEdit_testBoxSize->text().toDouble() };

		// for(int j{0}; j < 24; j++)
		// {
		// 	planesArray[j] = 
		// }

		planes->SetBounds(
			tmpPoint[0] - boxSize / 2, 
			tmpPoint[0] + boxSize / 2,
			tmpPoint[1] - boxSize / 2, 
			tmpPoint[1] + boxSize / 2,
			tmpPoint[2] - boxSize / 2, 
			tmpPoint[2] + boxSize / 2
		);
		
		clip->SetInputData(inputPolyData);
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
	tmpNode->SetName("Clipped Data");
	GetDataStorage()->Add(tmpNode);
}




