
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include <vtkConnectivityFilter.h>
#include <vtkImplicitPolyDataDistance.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"


void DentalWidget::ExtractPlan(vtkSmartPointer<vtkPolyData> implant_polydata, vtkSmartPointer<vtkPolyData> teeth_polydata, double start[3], double end[3])
{
	int numImplantCells = implant_polydata->GetNumberOfCells();
	double implantCenter[3]{ 0,0,0 };
	for (vtkIdType i{ 0 }; i < numImplantCells; i++)
	{
		for (vtkIdType j{ 0 }; j < 3; j++)
		{
			implantCenter[0] += implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[0];
			implantCenter[1] += implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[1];
			implantCenter[2] += implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[2];
		}
	}

	implantCenter[0] = implantCenter[0] / (numImplantCells * 3);
	implantCenter[1] = implantCenter[1] / (numImplantCells * 3);
	implantCenter[2] = implantCenter[2] / (numImplantCells * 3);



	// Get the maximal distance between the center point and the cylinder surface
	double maxDistance{ 0 };
	for (vtkIdType i{ 0 }; i < numImplantCells; i++)
	{
		for (vtkIdType j{ 0 }; j < 3; j++)
		{
			double tmpDistance = sqrt(
				pow(implantCenter[0] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[0], 2) +
				pow(implantCenter[1] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[1], 2) +
				pow(implantCenter[2] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[2], 2)
			);

			if (maxDistance <= tmpDistance)
			{
				maxDistance = tmpDistance;
			}
		}
	}

	//m_Controls.textBrowser->append(QString::number(maxDistance));

	// Find all the points on the cylinder top and bottom
	auto topBottomPoints = mitk::PointSet::New();

	for (vtkIdType i{ 0 }; i < numImplantCells; i++)
	{
		for (vtkIdType j{ 0 }; j < 3; j++)
		{
			double tmpDistance = sqrt(
				pow(implantCenter[0] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[0], 2) +
				pow(implantCenter[1] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[1], 2) +
				pow(implantCenter[2] - implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[2], 2)
			);

			if (fabs(maxDistance - tmpDistance) < 0.2)
			{
				mitk::Point3D p;
				p[0] = implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[0];
				p[1] = implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[1];
				p[2] = implant_polydata->GetCell(i)->GetPoints()->GetPoint(j)[2];

				topBottomPoints->InsertPoint(p);
			}
		}
	}

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(topBottomPoints);
	tmpNode->SetName("TopBottomPoints");
	//GetDataStorage()->Add(tmpNode);


	// Divide topBottomPoints into 2 groups and find their centers
	int numOfSearch = topBottomPoints->GetSize() / 2 - 1;
	double tmpCenter[3]{ topBottomPoints->GetPoint(0)[0],topBottomPoints->GetPoint(0)[1],topBottomPoints->GetPoint(0)[2] };

	auto currentPoint = topBottomPoints->GetPoint(0);
	auto previousPoint = topBottomPoints->GetPoint(0);
	auto checkPoint = topBottomPoints->GetPoint(0);

	
	for (int i{ 0 }; i < numOfSearch; i++)
	{
		mitk::Point3D nextPoint;
		double minDistance = 20;
		for (int j{ 0 }; j < topBottomPoints->GetSize(); j++)
		{
			auto p = topBottomPoints->GetPoint(j);
			if (sqrt(pow(p[0] - previousPoint[0], 2) + pow(p[1] - previousPoint[1], 2) + pow(p[2] - previousPoint[2], 2)) > 0.05)
			{
				double d = sqrt(pow(p[0] - currentPoint[0], 2) + pow(p[1] - currentPoint[1], 2) + pow(p[2] - currentPoint[2], 2));
				if (d < minDistance && d > 0)
				{
					minDistance = d;
					nextPoint[0] = p[0];
					nextPoint[1] = p[1];
					nextPoint[2] = p[2];

				}
			}
		}


		tmpCenter[0] += nextPoint[0];
		tmpCenter[1] += nextPoint[1];
		tmpCenter[2] += nextPoint[2];

		previousPoint[0] = currentPoint[0];
		previousPoint[1] = currentPoint[1];
		previousPoint[2] = currentPoint[2];

		currentPoint[0] = nextPoint[0];
		currentPoint[1] = nextPoint[1];
		currentPoint[2] = nextPoint[2];
		
	}

	tmpCenter[0] = tmpCenter[0] / (numOfSearch + 1);
	tmpCenter[1] = tmpCenter[1] / (numOfSearch + 1);
	tmpCenter[2] = tmpCenter[2] / (numOfSearch + 1);

	// tmpCenter[0] = tmpCenter[0] / n;
	// tmpCenter[1] = tmpCenter[1] / n;
	// tmpCenter[2] = tmpCenter[2] / n;

	double oppositeCenter[3]
	{
		2 * implantCenter[0] - tmpCenter[0],
		2 * implantCenter[1] - tmpCenter[1],
		2 * implantCenter[2] - tmpCenter[2]
	};

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(teeth_polydata);
	double d0 = implicitPolyDataDistance->EvaluateFunction(tmpCenter);
	double d1 = implicitPolyDataDistance->EvaluateFunction(oppositeCenter);
	mitk::Point3D startPoint;
	mitk::Point3D endPoint;

	if (fabs(d0) >= fabs(d1))
	{
		startPoint[0] = oppositeCenter[0];
		startPoint[1] = oppositeCenter[1];
		startPoint[2] = oppositeCenter[2];
		
		endPoint[0] = tmpCenter[0];
		endPoint[1] = tmpCenter[1];
		endPoint[2] = tmpCenter[2];
	}
	else
	{
		endPoint[0] = oppositeCenter[0];
		endPoint[1] = oppositeCenter[1];
		endPoint[2] = oppositeCenter[2];

		startPoint[0] = tmpCenter[0];
		startPoint[1] = tmpCenter[1];
		startPoint[2] = tmpCenter[2];
	}

	start[0] = startPoint[0];
	start[1] = startPoint[1];
	start[2] = startPoint[2];

	end[0] = endPoint[0];
	end[1] = endPoint[1];
	end[2] = endPoint[2];

	// auto startPointSet = mitk::PointSet::New();
	// startPointSet->InsertPoint(startPoint);
	// auto testNode2 = mitk::DataNode::New();
	// testNode2->SetData(startPointSet);
	// testNode2->SetName("plan_startPoint");
	// GetDataStorage()->Add(testNode2);
	//
	// auto endPointSet = mitk::PointSet::New();
	// endPointSet->InsertPoint(endPoint);
	// auto testNode3 = mitk::DataNode::New();
	// testNode3->SetData(endPointSet);
	// testNode3->SetName("plan_endPoint");
	// GetDataStorage()->Add(testNode3);
}

void DentalWidget::ExtractAllPlans()
{
	auto inputData = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("ios")->GetData());
	vtkNew<vtkTransformFilter> transFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(inputData->GetGeometry()->GetVtkMatrix());

	transFilter->SetInputData(inputData->GetVtkPolyData());
	
	transFilter->SetTransform(tmpTransform);
	transFilter->Update();

	// Divide the model into the teeth and implant
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(transFilter->GetOutput());

	vtkConnectivityFilter->SetExtractionModeToAllRegions();
	vtkConnectivityFilter->Update();
	int numModelSubcomponent = vtkConnectivityFilter->GetNumberOfExtractedRegions(); // should be 2

	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

	vtkConnectivityFilter->InitializeSpecifiedRegionList();
	vtkConnectivityFilter->AddSpecifiedRegion(0);
	vtkConnectivityFilter->Update();
	
	vtkNew<vtkPolyData> teeth_polydata;
	teeth_polydata->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());
	int teeth_id = 0;

	for(int i{0}; i < numModelSubcomponent; i++)
	{
		vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(i);
		vtkConnectivityFilter->Update();

		vtkNew<vtkPolyData> tmpSurface;
		tmpSurface->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());

		if(tmpSurface->GetNumberOfCells() > teeth_polydata->GetNumberOfCells())
		{
			teeth_id = i;
			teeth_polydata->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());
		}
	}

	// auto testNode = mitk::DataNode::New();
	// testNode->SetName("Teeth");
	// auto testSurface = mitk::Surface::New();
	// testSurface->SetVtkPolyData(teeth_polydata);
	// testNode->SetData(testSurface);
	// GetDataStorage()->Add(testNode);

	int counter{ 1 };
	for(int i{0}; i < numModelSubcomponent; i++)
	{
		if (i != teeth_id)
		{
			vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

			vtkConnectivityFilter->InitializeSpecifiedRegionList();
			vtkConnectivityFilter->AddSpecifiedRegion(i);
			vtkConnectivityFilter->Update();

			vtkNew<vtkPolyData> tmpImplantSurface;
			tmpImplantSurface->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());

			double tmpStart[3]{ 0 };
			double tmpEnd[3]{ 0 };
			ExtractPlan(tmpImplantSurface, teeth_polydata, tmpStart, tmpEnd);

			mitk::Point3D start;
			start[0] = tmpStart[0];
			start[1] = tmpStart[1];
			start[2] = tmpStart[2];

			mitk::Point3D end;
			end[0] = tmpEnd[0];
			end[1] = tmpEnd[1];
			end[2] = tmpEnd[2];


			if (GetDataStorage()->GetNamedNode("Implant_" + QString::number(counter).toLatin1()) != nullptr)
			{
				GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Implant_" + QString::number(counter).toLatin1()));
			}

			auto implantPointSet = mitk::PointSet::New();
			implantPointSet->InsertPoint(start);
			implantPointSet->InsertPoint(end);
			auto tmpNode = mitk::DataNode::New();
			tmpNode->SetName("Implant_" + QString::number(counter).toLatin1());
			tmpNode->SetData(implantPointSet);
			GetDataStorage()->Add(tmpNode);
			counter += 1;
		}
	}

}


void DentalWidget::ExtractAllPlans_model()
{
	auto inputData = dynamic_cast<mitk::Surface*>(GetDataStorage()->GetNamedNode("model")->GetData());
	vtkNew<vtkTransformFilter> transFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(inputData->GetGeometry()->GetVtkMatrix());

	transFilter->SetInputData(inputData->GetVtkPolyData());

	transFilter->SetTransform(tmpTransform);
	transFilter->Update();

	// Divide the model into the teeth and implant
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(transFilter->GetOutput());

	vtkConnectivityFilter->SetExtractionModeToAllRegions();
	vtkConnectivityFilter->Update();
	int numModelSubcomponent = vtkConnectivityFilter->GetNumberOfExtractedRegions(); // should be 2

	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

	vtkConnectivityFilter->InitializeSpecifiedRegionList();
	vtkConnectivityFilter->AddSpecifiedRegion(0);
	vtkConnectivityFilter->Update();

	vtkNew<vtkPolyData> teeth_polydata;
	teeth_polydata->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());
	int teeth_id = 0;

	for (int i{ 0 }; i < numModelSubcomponent; i++)
	{
		vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(i);
		vtkConnectivityFilter->Update();

		vtkNew<vtkPolyData> tmpSurface;
		tmpSurface->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());

		if (tmpSurface->GetNumberOfCells() > teeth_polydata->GetNumberOfCells())
		{
			teeth_id = i;
			teeth_polydata->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());
		}
	}

	// auto testNode = mitk::DataNode::New();
	// testNode->SetName("Teeth");
	// auto testSurface = mitk::Surface::New();
	// testSurface->SetVtkPolyData(teeth_polydata);
	// testNode->SetData(testSurface);
	// GetDataStorage()->Add(testNode);

	int counter{ 1 };
	for (int i{ 0 }; i < numModelSubcomponent; i++)
	{
		if (i != teeth_id)
		{
			vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();

			vtkConnectivityFilter->InitializeSpecifiedRegionList();
			vtkConnectivityFilter->AddSpecifiedRegion(i);
			vtkConnectivityFilter->Update();

			vtkNew<vtkPolyData> tmpImplantSurface;
			tmpImplantSurface->DeepCopy(vtkConnectivityFilter->GetPolyDataOutput());

			double tmpStart[3]{ 0 };
			double tmpEnd[3]{ 0 };
			ExtractPlan(tmpImplantSurface, teeth_polydata, tmpStart, tmpEnd);

			mitk::Point3D start;
			start[0] = tmpStart[0];
			start[1] = tmpStart[1];
			start[2] = tmpStart[2];

			mitk::Point3D end;
			end[0] = tmpEnd[0];
			end[1] = tmpEnd[1];
			end[2] = tmpEnd[2];


			if (GetDataStorage()->GetNamedNode("Implant_" + QString::number(counter).toLatin1()) != nullptr)
			{
				GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Implant_" + QString::number(counter).toLatin1()));
			}

			auto implantPointSet = mitk::PointSet::New();
			implantPointSet->InsertPoint(start);
			implantPointSet->InsertPoint(end);
			auto tmpNode = mitk::DataNode::New();
			tmpNode->SetName("Implant_" + QString::number(counter).toLatin1());
			tmpNode->SetData(implantPointSet);
			GetDataStorage()->Add(tmpNode);
			counter += 1;
		}
	}

}
