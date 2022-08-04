
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
#include "leastsquaresfit.h"

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
	// int numOfSearch = topBottomPoints->GetSize() / 2 - 1;
	int numOfSearch = topBottomPoints->GetSize();
	double tmpCenter[3]{ topBottomPoints->GetPoint(0)[0],topBottomPoints->GetPoint(0)[1],topBottomPoints->GetPoint(0)[2] };

	auto currentPoint = topBottomPoints->GetPoint(0);
	auto previousPoint = topBottomPoints->GetPoint(0);
	auto checkPoint = topBottomPoints->GetPoint(0);

	auto testPointset = mitk::PointSet::New();

	int count{ 1 };
	for (int i{ 0 }; i < numOfSearch; i++)
	{
		mitk::Point3D nextPoint;
		double minDistance = 20;
		for (int j{ 0 }; j < topBottomPoints->GetSize(); j++)
		{
			auto p = topBottomPoints->GetPoint(j);
			double d = sqrt(pow(p[0] - currentPoint[0], 2) + pow(p[1] - currentPoint[1], 2) + pow(p[2] - currentPoint[2], 2));
			double d1 = sqrt(pow(previousPoint[0] - currentPoint[0], 2) + pow(previousPoint[1] - currentPoint[1], 2) + pow(previousPoint[2] - currentPoint[2], 2));
			double d2 = sqrt(pow(previousPoint[0] - p[0], 2) + pow(previousPoint[1] - p[1], 2) + pow(previousPoint[2] - p[2], 2));

			if (d2 > d1)
			{
				if (d < minDistance)
				{
					minDistance = d;
					nextPoint[0] = p[0];
					nextPoint[1] = p[1];
					nextPoint[2] = p[2];
									   
				}
			}
		}
		if (count > 15 && 
			sqrt(pow(previousPoint[0] - nextPoint[0], 2) + pow(previousPoint[1] - nextPoint[1], 2) + pow(previousPoint[2] - nextPoint[2], 2)) < 0.05)
		{
			break;
		}

		testPointset->InsertPoint(nextPoint);

		tmpCenter[0] += nextPoint[0];
		tmpCenter[1] += nextPoint[1];
		tmpCenter[2] += nextPoint[2];

		previousPoint[0] = currentPoint[0];
		previousPoint[1] = currentPoint[1];
		previousPoint[2] = currentPoint[2];

		currentPoint[0] = nextPoint[0];
		currentPoint[1] = nextPoint[1];
		currentPoint[2] = nextPoint[2];

		count += 1;
	}
	//
	// tmpCenter[0] = tmpCenter[0] / (numOfSearch + 1);
	// tmpCenter[1] = tmpCenter[1] / (numOfSearch + 1);
	// tmpCenter[2] = tmpCenter[2] / (numOfSearch + 1);

	auto testNode = mitk::DataNode::New();
	testNode->SetName("test Pts");
	testNode->SetData(testPointset);
	//GetDataStorage()->Add(testNode);
	
	// fit circle 1 using testPointset
	std::vector<double> inp_pointset(
		3*(testPointset->GetSize()));
	std::array<double, 3> outp_center;
	double outp_radius;
	std::array<double, 3> outp_normal;
	for(int i{0}; i < (testPointset->GetSize()); i++)
	{
		inp_pointset[3*i] = testPointset->GetPoint(i)[0];
		inp_pointset[3*i + 1] = testPointset->GetPoint(i)[1];
		inp_pointset[3 * i + 2] = testPointset->GetPoint(i)[2];

	}


	lancetAlgorithm::fit_circle_3d(inp_pointset, outp_center, outp_radius, outp_normal);


	tmpCenter[0] = outp_center[0];//tmpCenter[0] / count;
	tmpCenter[1] = outp_center[1];//tmpCenter[1] / count;
	tmpCenter[2] = outp_center[2];//tmpCenter[2] / count;

	auto oppositePointset = mitk::PointSet::New();
	double oppositeCenter[3]{ 0,0,0 };
	int oppositeCount{ 0 };
	for (int j{ 0 }; j < topBottomPoints->GetSize(); j++)
	{
		auto p = topBottomPoints->GetPoint(j);
		double d = sqrt(pow(p[0] - tmpCenter[0], 2) + pow(p[1] - tmpCenter[1], 2) + pow(p[2] - tmpCenter[2], 2));
		if(d > maxDistance)
		{
			oppositePointset->InsertPoint(p);
			oppositeCenter[0] += p[0];
			oppositeCenter[1] += p[1];
			oppositeCenter[2] += p[2];
			oppositeCount += 1;
		}
	}


	// fit circle 1 using testPointset
	std::vector<double> inp_pointset1(
		3 * (oppositePointset->GetSize()));
	std::array<double, 3> outp_center1;
	double outp_radius1;
	std::array<double, 3> outp_normal1;
	for (int i{ 0 }; i < (oppositePointset->GetSize()); i++)
	{
		inp_pointset1[3 * i] = oppositePointset->GetPoint(i)[0];
		inp_pointset1[3 * i + 1] = oppositePointset->GetPoint(i)[1];
		inp_pointset1[3 * i + 2] = oppositePointset->GetPoint(i)[2];

	}


	lancetAlgorithm::fit_circle_3d(inp_pointset1, outp_center1, outp_radius1, outp_normal1);


	oppositeCenter[0] = outp_center1[0];// / oppositeCount;
	oppositeCenter[1] = outp_center1[1];// / oppositeCount;
	oppositeCenter[2] = outp_center1[2];// / oppositeCount;

	auto testOppositeNode = mitk::DataNode::New();
	testOppositeNode->SetData(oppositePointset);
	testOppositeNode->SetName("opposites");
	//GetDataStorage()->Add(testOppositeNode);

	// double oppositeCenter[3]
	// {
	// 	2 * implantCenter[0] - tmpCenter[0],
	// 	2 * implantCenter[1] - tmpCenter[1],
	// 	2 * implantCenter[2] - tmpCenter[2]
	// };

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
