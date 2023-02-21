
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include <vtkConnectivityFilter.h>
#include <vtkImageIterator.h>

#include "leastsquaresfit.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"
#include "vtkImageCast.h"

void DentalWidget::CheckUseSmoothing()
{
	if (m_Controls.lineEdit_smoothIteration->isEnabled())
	{
		m_Controls.lineEdit_smoothIteration->setDisabled(true);
		m_Controls.lineEdit_smoothRelaxtion->setDisabled(true);
	}
	else
	{
		m_Controls.lineEdit_smoothIteration->setEnabled(true);
		m_Controls.lineEdit_smoothRelaxtion->setEnabled(true);
	}
}

bool DentalWidget::ReconstructSurface()
{
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	if (inputCtImage != nullptr)
	{
		// The isosurface of the CT data into a single polydata
		double threshold = m_Controls.lineEdit_surfaceLowerBound->text().toDouble();
		auto mitkRecontructedSurfaces = mitk::Surface::New();
		mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

		// Set smoothing parameters
		if (m_Controls.checkBox_useSmooth->isChecked())
		{
			int iteration = m_Controls.lineEdit_smoothIteration->text().toInt();
			double relaxtion = m_Controls.lineEdit_smoothRelaxtion->text().toDouble();
			imageToSurfaceFilter->SmoothOn();
			imageToSurfaceFilter->SetSmoothRelaxation(relaxtion);
			imageToSurfaceFilter->SetSmoothIteration(iteration);
		}


		imageToSurfaceFilter->SetInput(inputCtImage);
		imageToSurfaceFilter->SetThreshold(threshold);
		mitkRecontructedSurfaces = imageToSurfaceFilter->GetOutput();
		

		// draw extracted surface
		auto nodeSteelballSurfaces = mitk::DataNode::New();
		nodeSteelballSurfaces->SetName("Reconstructed CBCT surface");
		nodeSteelballSurfaces->SetColor(1, 0.5, 0);
		// add new node
		nodeSteelballSurfaces->SetData(mitkRecontructedSurfaces);
		GetDataStorage()->Add(nodeSteelballSurfaces);
		return true;
	}
	return false;
}

bool DentalWidget::AutoReconstructSurface()
{
	if (GetDataStorage()->GetNamedNode("CBCT") == nullptr)
	{
		return false;
	}

	auto inputCtImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("CBCT")->GetData());

	if (inputCtImage != nullptr)
	{
		// The isosurface of the CT data into a single polydata
		double threshold = 1500;
		auto mitkRecontructedSurfaces = mitk::Surface::New();
		mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

		// Set smoothing parameters
		// if (m_Controls.checkBox_useSmooth->isChecked())
		// {
		// 	int iteration = m_Controls.lineEdit_smoothIteration->text().toInt();
		// 	double relaxtion = m_Controls.lineEdit_smoothRelaxtion->text().toDouble();
		// 	imageToSurfaceFilter->SmoothOn();
		// 	imageToSurfaceFilter->SetSmoothRelaxation(relaxtion);
		// 	imageToSurfaceFilter->SetSmoothIteration(iteration);
		// }


		imageToSurfaceFilter->SetInput(inputCtImage);
		imageToSurfaceFilter->SetThreshold(threshold);
		mitkRecontructedSurfaces = imageToSurfaceFilter->GetOutput();


		// draw extracted surface
		auto nodeSteelballSurfaces = mitk::DataNode::New();
		nodeSteelballSurfaces->SetName("Reconstructed CBCT surface");
		nodeSteelballSurfaces->SetColor(1, 0.5, 0);
		// add new node
		nodeSteelballSurfaces->SetData(mitkRecontructedSurfaces);
		GetDataStorage()->Add(nodeSteelballSurfaces);
		return true;
	}
	return false;
}

bool DentalWidget::GetCoarseSteelballCenters(double steelballVoxel)
{
	if (GetDataStorage()->GetNamedNode("Steelball centers") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	}

	// INPUT 1: inputCtImage (MITK image)
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	// TODO: automatically obtain maximal voxel value of the input image
	// auto inputVtkImage = inputCtImage->GetVtkImageData();
	// int dims[3];
	// inputVtkImage->GetDimensions(dims);
	// //int tmpRegion[6]{ 0, dims[0],0,dims[1],0,dims[2] };
	//
	// //m_Controls.textBrowser->append(QString::number(dims[0]));
	// //m_Controls.textBrowser->append(QString::number(dims[1]));
	// //m_Controls.textBrowser->append(QString::number(dims[2]));
	//
	//
	// //int tmpRegion[6]{ 0,100,0,100,0,100 };
	//
	// int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };
	//
	// auto caster = vtkImageCast::New();
	// caster->SetInputData(inputVtkImage);
	// caster->SetOutputScalarTypeToDouble();
	// caster->Update();
	// double tmpMaxVoxel{ 0 };

	// for (int k = 0; k < dims[2]; k++) {
	// 	for (int j = 0; j < dims[1]; j++) {
	// 		for (int i = 0; i < dims[0]; i++) {
	// 			double* pixel =
	// 				(double*)(caster->GetOutput()->GetScalarPointer(i, j, k));
	// 			if(tmpMaxVoxel < double(*pixel))
	// 			{
	// 				tmpMaxVoxel = double(*pixel);
	// 			}
	// 		}
	// 	}
	// }

	// vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	// while (!iter.IsAtEnd())
	// {
	// 	double* inSI = iter.BeginSpan();
	// 	double* inSIEnd = iter.EndSpan();
	//
	// 	while (inSI != inSIEnd)
	// 	{
	// 		if (double(*inSI) > tmpMaxVoxel)
	// 		{
	// 			tmpMaxVoxel = double(*inSI);
	// 		}
	// 		++inSI;
	// 	}
	// 	iter.NextSpan();
	//
	// }

	//m_Controls.textBrowser->append(QString::number(tmpMaxVoxel));


	// The isosurface of all steelballs as into a single polydata

	  // INPUT 2: voxelThreshold (double)
	// double voxelThreshold = m_Controls.lineEdit_ballGrayValue->text().toDouble();
	// double voxelThreshold = 2 * tmpMaxVoxel / 5;
	// m_Controls.lineEdit_ballGrayValue->setText(QString::number(steelballVoxel));

	auto mitkSteelBallSurfaces = mitk::Surface::New();
	mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

	imageToSurfaceFilter->SetInput(inputCtImage);
	imageToSurfaceFilter->SetThreshold(steelballVoxel);
	mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();


	if(mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells() > 300000) // To ensure the speed
	{
		auto tmpPointset = mitk::PointSet::New();
		auto nodeSortedSteelballCenters = mitk::DataNode::New();
		nodeSortedSteelballCenters->SetName("Steelball centers");
		// add new node
		nodeSortedSteelballCenters->SetData(tmpPointset);
		GetDataStorage()->Add(nodeSortedSteelballCenters);
	
		return false;
	}

	// m_Controls.textBrowser->append("cells: "+QString::number(mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells()));

	// Separate steelball surface by examining their connectivity
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(mitkSteelBallSurfaces->GetVtkPolyData());

	vtkConnectivityFilter->SetExtractionModeToAllRegions();
	vtkConnectivityFilter->Update();
	int numberOfPotentialSteelBalls = vtkConnectivityFilter->GetNumberOfExtractedRegions();


	auto mitkSingleSteelballCenterPointset = mitk::PointSet::New(); // store each steelball's center
	double centerOfAllSteelballs[3]{ 0, 0, 0 };                       // the center of all steel balls

	// Iterate over all the potential steelball regions, extract the real ones by assessing their size (facets numbers)
	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();
	for (int m = 0; m < numberOfPotentialSteelBalls; m++)
	{
		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(m);
		vtkConnectivityFilter->Update();
		// m_Controls.textBrowser->append("Connectivity passed");

		auto vtkSingleSteelBallSurface = vtkConnectivityFilter->GetPolyDataOutput();

		auto numberOfCells =
			vtkSingleSteelBallSurface->GetNumberOfCells(); // the total number of cells of a single mesh surface; each cell
														   // stores one facet of the mesh surface

		  // INPUT 3 & 4: facetNumberUpperThreshold (int) & facetNumberUpperThreshold (int)
		int facetNumberUpperThreshold = m_Controls.lineEdit_ballMaxCell->text().toInt();
		int facetNumberLowerThreshold = m_Controls.lineEdit_ballMinCell->text().toInt();

		if (numberOfCells < facetNumberUpperThreshold && numberOfCells > facetNumberLowerThreshold)
		{
			std::vector<double> inp_x(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_y(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_z(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface

			for (int n = 0; n < numberOfCells; n++)
			{
				auto tmpPoint = vtkSingleSteelBallSurface->GetCell(n)->GetPoints()->GetPoint(0);

				inp_x[n] = tmpPoint[0];
				inp_y[n] = tmpPoint[1];
				inp_z[n] = tmpPoint[2];
			}

			// use inp_x, inp_y and inp_z to simulate a sphere
			double cx, cy, cz;
			double R;

			lancetAlgorithm::fit_sphere(inp_x, inp_y, inp_z, cx, cy, cz, R);

			mitk::Point3D mitkTmpCenterPoint3D;
			mitkTmpCenterPoint3D[0] = cx;
			mitkTmpCenterPoint3D[1] = cy;
			mitkTmpCenterPoint3D[2] = cz;
			mitkSingleSteelballCenterPointset->InsertPoint(mitkTmpCenterPoint3D);

			centerOfAllSteelballs[0] = centerOfAllSteelballs[0] + cx;
			centerOfAllSteelballs[1] = centerOfAllSteelballs[1] + cy;
			centerOfAllSteelballs[2] = centerOfAllSteelballs[2] + cz;

			// // Draw simulated spheres
			// auto vtkBallSource0 = vtkSmartPointer<vtkSphereSource>::New();
			// vtkBallSource0->SetCenter(cx, cy, cz);
			// vtkBallSource0->SetRadius(R);
			// vtkBallSource0->Update();
			//
			// auto tmpNode = mitk::DataNode::New();
			//
			// tmpNode->SetName("Single steelball sphere");
			// auto mitkSteelBallSurfacesNew1 = mitk::Surface::New();
			// mitkSteelBallSurfacesNew1->SetVtkPolyData(vtkBallSource0->GetOutput());
			// tmpNode->SetData(mitkSteelBallSurfacesNew1);
			// // GetDataStorage()->Add(tmpNode);
		}


	}

	int numberOfActualSteelballs = mitkSingleSteelballCenterPointset->GetSize();

	centerOfAllSteelballs[0] = centerOfAllSteelballs[0] / numberOfActualSteelballs;
	centerOfAllSteelballs[1] = centerOfAllSteelballs[1] / numberOfActualSteelballs;
	centerOfAllSteelballs[2] = centerOfAllSteelballs[2] / numberOfActualSteelballs;

	// Sort the centers of the separate steelballs according to their distances to the group center
	std::vector<double> distancesToPointSetCenter(numberOfActualSteelballs);
	std::vector<int> distanceRanks(numberOfActualSteelballs);

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		distancesToPointSetCenter[i] =
			sqrt(pow(centerOfAllSteelballs[0] - mitkSingleSteelballCenterPointset->GetPoint(i)[0], 2) +
				pow(centerOfAllSteelballs[1] - mitkSingleSteelballCenterPointset->GetPoint(i)[1], 2) +
				pow(centerOfAllSteelballs[2] - mitkSingleSteelballCenterPointset->GetPoint(i)[2], 2));

		distanceRanks[i] = i;
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance before sorting: " << distancesToPointSetCenter[i];
	}

	for (int i = 0; i < numberOfActualSteelballs - 2; i++)
	{
		for (int j = 0; j < numberOfActualSteelballs - 1 - i; j++)
		{
			double temp = 0;
			double temp2 = 0;
			if (distancesToPointSetCenter[j] > distancesToPointSetCenter[j + 1])
			{
				temp = distancesToPointSetCenter[j];
				distancesToPointSetCenter[j] = distancesToPointSetCenter[j + 1];
				distancesToPointSetCenter[j + 1] = temp;

				temp2 = distanceRanks[j];
				distanceRanks[j] = distanceRanks[j + 1];
				distanceRanks[j + 1] = temp2;
			}
		}
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance after sorting: " << distancesToPointSetCenter[i];
	}

	auto mitkSortedSingleSteelballCenterPointset = mitk::PointSet::New();
	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		mitkSortedSingleSteelballCenterPointset->InsertPoint(mitkSingleSteelballCenterPointset->GetPoint(distanceRanks[i]));
	}

	// // draw extracted  steel ball surfaces
	// auto nodeSteelballSurfaces = mitk::DataNode::New();
	// nodeSteelballSurfaces->SetName("Steelball surfaces");
	// // add new node
	// nodeSteelballSurfaces->SetData(mitkSteelBallSurfaces);
	// // GetDataStorage()->Add(nodeSteelballSurfaces);
	//
	// // add steel ball centers
	// auto nodeSteelballCenters = mitk::DataNode::New();
	// nodeSteelballCenters->SetName("Steelball centers");
	// // add new node
	// nodeSteelballCenters->SetData(mitkSingleSteelballCenterPointset);
	// // GetDataStorage()->Add(nodeSteelballCenters);


	// add sorted steel ball centers
	auto nodeSortedSteelballCenters = mitk::DataNode::New();
	nodeSortedSteelballCenters->SetName("Steelball centers");
	// add new node
	nodeSortedSteelballCenters->SetData(mitkSortedSingleSteelballCenterPointset);
	GetDataStorage()->Add(nodeSortedSteelballCenters);

	return true;

}

double DentalWidget::GetPointDistance(const mitk::Point3D p0, const mitk::Point3D p1)
{
	double distance = sqrt(
		pow(p0[0] - p1[0], 2) +
		pow(p0[1] - p1[1], 2) +
		pow(p0[2] - p1[2], 2)
	);

	return distance;
}

void DentalWidget::ScreenCoarseSteelballCenters()
{
	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	int inputPoinSetNum = inputPointSet->GetSize();
	auto steelballCenters = mitk::PointSet::New();

	int lengthOfFingerPrint{ 6 };
	int numOfTargetSteelballs{ 7 };


	for (int q{0}; q < numOfTargetSteelballs; q++)
	{
		double fingerPrint[6]
		{
			allBallFingerPrint[6 * q],
			allBallFingerPrint[6 * q + 1],
			allBallFingerPrint[6 * q + 2],
			allBallFingerPrint[6 * q + 3],
			allBallFingerPrint[6 * q + 4],
			allBallFingerPrint[6 * q + 5],
		};
		

		for (int i{ 0 }; i < inputPoinSetNum; i++)
		{
			int metric{ 0 };

			for (int j{ 0 }; j < lengthOfFingerPrint; j++)
			{
				bool metRequirement = false;
				double standardDistance = fingerPrint[j];

				for (int k{ 0 }; k < inputPoinSetNum; k++)
				{
					double tmpDistance = GetPointDistance(inputPointSet->GetPoint(i), inputPointSet->GetPoint(k));
					if (fabs(tmpDistance - standardDistance) < 0.4)
					{
						metRequirement = true;
						metric += 1;
						break;
					}
				}

				if (metRequirement == false)
				{
					break;
				}

			}

			//m_Controls.textBrowser->append("metric: " + QString::number(metric));
			if (metric == lengthOfFingerPrint)
			{

				// Add this point to the pointset
				steelballCenters->InsertPoint(inputPointSet->GetPoint(i));
				//break;
			}

		}

	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(steelballCenters);
	GetDataStorage()->Add(tmpNode);

}

void DentalWidget::ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7])
{
	foundIDs[0] = 0;
	foundIDs[1] = 0;
	foundIDs[2] = 0;
	foundIDs[3] = 0;
	foundIDs[4] = 0;
	foundIDs[5] = 0;
	foundIDs[6] = 0;

	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	int inputPoinSetNum = inputPointSet->GetSize();
	auto steelballCenters = mitk::PointSet::New();

	int lengthOfFingerPrint = stdNeighborNum;
	int numOfTargetSteelballs = stdNeighborNum + 1;


	for (int q{ 0 }; q < numOfTargetSteelballs; q++)
	{
		double fingerPrint[6]
		{
			allBallFingerPrint[6 * q],
			allBallFingerPrint[6 * q + 1],
			allBallFingerPrint[6 * q + 2],
			allBallFingerPrint[6 * q + 3],
			allBallFingerPrint[6 * q + 4],
			allBallFingerPrint[6 * q + 5],
		};


		for (int i{ 0 }; i < inputPoinSetNum; i++)
		{
			int metric{ 0 };

			for (int j{ 0 }; j < lengthOfFingerPrint; j++)
			{
				bool metSingleRequirement = false;
				double standardDistance = fingerPrint[j];

				for (int k{ 0 }; k < inputPoinSetNum; k++)
				{
					double tmpDistance = GetPointDistance(inputPointSet->GetPoint(i), inputPointSet->GetPoint(k));
					if (fabs(tmpDistance - standardDistance) < 0.4)
					{
						metSingleRequirement = true;
						metric += 1;
						break;
					}
				}

				// if (metSingleRequirement == false)
				// {
				// 	break;
				// }

			}

			//m_Controls.textBrowser->append("metric: " + QString::number(metric));
			if (metric >= requiredNeighborNum)
			{

				// Add this point to the pointset
				steelballCenters->InsertPoint(inputPointSet->GetPoint(i));
				//break;
				foundIDs[q] = 1;
			}

		}

	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(steelballCenters);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();

}

void DentalWidget::RemoveRedundantCenters()
{

	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto tmpPointSet = mitk::PointSet::New();

	for(int i{0}; i < inputPointSet->GetSize(); i++)
	{
		bool unique = true;
		for(int j{0}; j < tmpPointSet->GetSize(); j++)
		{
			if(GetPointDistance(tmpPointSet->GetPoint(j),inputPointSet->GetPoint(i)) < 0.05)
			{
				unique = false;
				break;
			}
		}

		if(unique == true)
		{
			tmpPointSet->InsertPoint(inputPointSet->GetPoint(i));
		}
	}


	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPointSet);
	GetDataStorage()->Add(tmpNode);

}

void DentalWidget::GetSteelballCenters_iosCBCT()
{
	// Initial preparation
	m_Controls.textBrowser->append("------- Started steelball searching -------");
	m_Controls.lineEdit_ballGrayValue->setText("---Pending---");

	if (GetDataStorage()->GetNamedNode("Steelball centers") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	}

	auto standartSteelballCenters = mitk::PointSet::New();
	int stdCenterNum{ 7 };

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	UpdateAllBallFingerPrint(standartSteelballCenters);

	// Get maximal voxel
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	// Get maximal voxel value 
	auto inputVtkImage = inputCtImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxVoxel{ 0 };

	/// Iteration like below is Too slow
	// for (int k = 0; k < dims[2]; k++) {
	// 	for (int j = 0; j < dims[1]; j++) {
	// 		for (int i = 0; i < dims[0]; i++) {
	// 			double* pixel =
	// 				(double*)(caster->GetOutput()->GetScalarPointer(i, j, k));
	// 			if(tmpMaxVoxel < double(*pixel))
	// 			{
	// 				tmpMaxVoxel = double(*pixel);
	// 			}
	// 		}
	// 	}
	// }

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxVoxel)
			{
				tmpMaxVoxel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}

	int searchIterations{ 5 };

	bool succeeded{ false };
	for(int i{0}; i < searchIterations; i++)
	{
		
		double tmpVoxelThreshold = (1 - double(i) / searchIterations) * tmpMaxVoxel;

		GetCoarseSteelballCenters(tmpVoxelThreshold);

		int oldNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

		ScreenCoarseSteelballCenters();
		int newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

		int limit{ 0 };
		while (newNumOfCenters != oldNumOfCenters)
		{
			oldNumOfCenters = newNumOfCenters;
			ScreenCoarseSteelballCenters();
			newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
			limit += 1;
			if(limit==searchIterations)
			{
				m_Controls.textBrowser->append("Maximal steelball center error: " + QString::number(newNumOfCenters));
				break;
			}
		}

		RemoveRedundantCenters();

		auto landmarkRegistrator = mitk::SurfaceRegistration::New();
		landmarkRegistrator->SetLandmarksSrc(standartSteelballCenters);
		landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData()));

		landmarkRegistrator->ComputeLandMarkResult();
		double maxError = landmarkRegistrator->GetmaxLandmarkError();
		double avgError = landmarkRegistrator->GetavgLandmarkError();

		if (maxError < 1.5 && avgError < 1 && dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == standartSteelballCenters->GetSize())
		{
			m_Controls.textBrowser->append("Maximal steelball center error: " + QString::number(maxError));
			m_Controls.textBrowser->append("Average steelball center error: " + QString::number(avgError));
			m_Controls.textBrowser->append("------- The correct centers have been found ! -------");
			m_Controls.lineEdit_ballGrayValue->setText(QString::number(tmpVoxelThreshold));
			succeeded = true;
			break;
		}
		
	}

	if(succeeded == false)
	{
		m_Controls.textBrowser->append("!!!!!! Warning: failed to find all the centers; the found centers are highly problematic !!!!!!");

	}


}

void DentalWidget::GetSteelballCenters_modelCBCT()
{
	UpdateStdCenters();

	// Initial preparation
	m_Controls.textBrowser->append("------- Started steelball searching -------");

	if (GetDataStorage()->GetNamedNode("Steelball centers") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	}

	if (GetDataStorage()->GetNamedNode("std centers (full)") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("std centers (full)"));
	}

	if (GetDataStorage()->GetNamedNode("std centers (partial)") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("std centers (partial)"));
	}

	auto standartSteelballCenters = mitk::PointSet::New();
	int stdCenterNum{ 7 };

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	if(m_Controls.checkBox_generateStandardBallCenters->isChecked())
	{
		auto stdSteelballNode = mitk::DataNode::New();
		stdSteelballNode->SetName("std centers (full)");
		stdSteelballNode->SetData(standartSteelballCenters);
		GetDataStorage()->Add(stdSteelballNode);
	}
	
	UpdateAllBallFingerPrint(standartSteelballCenters);

	// Get maximal and minimal voxel
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	auto inputVtkImage = inputCtImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxVoxel{ 0 };
	double tmpMinVoxel{ 0 };

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxVoxel)
			{
				tmpMaxVoxel = double(*inSI);
			}
			if (double(*inSI) < tmpMinVoxel)
			{
				tmpMinVoxel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}

	int searchIterations{ 5 }; // optimal voxel value

	double voxelThres{ tmpMaxVoxel };
	int foundCleanCenterNum{ 0 };
	int foundCenterNum{ 0 };
	int foundIDs[7]{ 0 };

	if(m_Controls.comboBox_ballThresMode->currentIndex()==1)
	{
		m_Controls.lineEdit_ballGrayValue->setText("---Pending---");

		// Search for the best voxel value threshold
		for (int i{ 0 }; i < searchIterations; i++)
		{
			double tmpVoxelThreshold = (1 - (8 / 8) * double(i) / searchIterations) * (tmpMaxVoxel - tmpMinVoxel) + tmpMinVoxel;

			GetCoarseSteelballCenters(tmpVoxelThreshold);

			foundCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
			// m_Controls.textBrowser->append(QString::number(foundCenterNum));
			m_Controls.lineEdit_ballGrayValue->setText(QString::number(tmpVoxelThreshold));
			if (foundCenterNum >= 40)
			{
				break;
			}

			IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);

			if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() > foundCleanCenterNum)
			{
				foundCleanCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
				voxelThres = tmpVoxelThreshold;
				if (foundCleanCenterNum == stdCenterNum)
				{
					//m_Controls.textBrowser->append("~~All steelballs have been found~~");
					break;

				}
			}

		}
	}else
	{
		voxelThres = m_Controls.lineEdit_ballGrayValue->text().toDouble();
	}



	GetCoarseSteelballCenters(voxelThres);
	m_Controls.lineEdit_ballGrayValue->setText(QString::number(voxelThres));
	IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);
	RearrangeSteelballs(6, foundIDs); // this function is redundant ??


	auto partialStdPointset = mitk::PointSet::New();
	for (int q{ 0 }; q < 7; q++){
		if(foundIDs[q] == 1)
		{
			partialStdPointset->InsertPoint(standartSteelballCenters->GetPoint(q));
		}
	}

	if(m_Controls.checkBox_generateStandardBallCenters->isChecked())
	{
		auto tmpNode = mitk::DataNode::New();
		tmpNode->SetName("std centers (partial)");
		tmpNode->SetData(partialStdPointset);
		GetDataStorage()->Add(tmpNode);
	}
	


	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(partialStdPointset);
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData()));
	
	landmarkRegistrator->ComputeLandMarkResult();
	double maxError = landmarkRegistrator->GetmaxLandmarkError();
	double avgError = landmarkRegistrator->GetavgLandmarkError();
	
	m_Controls.textBrowser->append("Maximum steelball error: " + QString::number(maxError));
	m_Controls.textBrowser->append("Average steelball error: " + QString::number(avgError));

	if(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == 7)
	{
		m_Controls.textBrowser->append("~~All steelballs have been found!~~");
	}else
	{
		m_Controls.textBrowser->append("!!!Warning: Only found " + QString::number(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize())
		+ " steelballs!!!!");
		m_Controls.textBrowser->append("Please compare 'Steelball centers', 'std centers (partial)' and 'std centers (full)' carefully!");
	}

	if (avgError > 1)
	{
		m_Controls.textBrowser->append("!!!Warning: The found centers are highly problematic!!!");
	}

	m_Controls.textBrowser->append("------- End of steelball searching -------");

	// auto tmpMatrix = landmarkRegistrator->GetResult();
	//
	// GetDataStorage()->GetNamedNode("std_modelWithPlan")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	auto parentNode = m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode();
	auto tmpPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	auto childNode = mitk::DataNode::New();
	childNode->SetName("Steelball centers2");
	childNode->SetData(tmpPointSet);
	GetDataStorage()->Add(childNode, parentNode);
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	childNode->SetName("Steelball centers");
}

void DentalWidget::UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters)
{
	int pointNum = stdSteelballCenters->GetSize();

	int currentIndex{ 0 };
	for(int i{0}; i < pointNum; i++)
	{

		int currentColumn{ 0 };
		for(int j{0}; j < pointNum; j++)
		{
			if(i != j)
			{
				allBallFingerPrint[currentIndex] = GetPointDistance(stdSteelballCenters->GetPoint(i), stdSteelballCenters->GetPoint(j));
				//m_Controls.textBrowser->append(QString::number(currentIndex)+": "+ QString::number(allBallFingerPrint[3 * i + currentColumn]));
				currentColumn += 1;
				currentIndex += 1;
			}
		}
	}
	
}

void DentalWidget::UpdateStdCenters()
{
	if(m_Controls.radioButton_model->isChecked())
	{
		for (int i{ 0 }; i < 21; i++)
		{
			stdCenters[i] = modelStdCenters[i];
		}

	}

	if (m_Controls.radioButton_ios->isChecked())
	{
		for (int i{ 0 }; i < 21; i++)
		{
			stdCenters[i] = iosStdCenters[i];
		}

	}
	
}



void DentalWidget::IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7])
{
	int oldNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	
	ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
	int newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	// m_Controls.textBrowser->append("old: " + QString::number(oldNumOfCenters));
	// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
	//
	int limit{ 0 };
	   
	while (newNumOfCenters != oldNumOfCenters)
	{
		// m_Controls.textBrowser->append("old: "+ QString::number(oldNumOfCenters));
		// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
		oldNumOfCenters = newNumOfCenters;
		// m_Controls.textBrowser->append(QString::number(limit));

		ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
		newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
		limit += 1;
		 if (limit == 20)
		 {
		 	m_Controls.textBrowser->append("--- Warning: Maximal screening iteration cycle has been reached ---");
		 	break;
		 }
	}

	RemoveRedundantCenters();
}


// void DentalWidget::RearrangeSteelballs(int stdNeighborNum, int foundIDs[7])
// {
// 	foundIDs[0] = 0;
// 	foundIDs[1] = 0;
// 	foundIDs[2] = 0;
// 	foundIDs[3] = 0;
// 	foundIDs[4] = 0;
// 	foundIDs[5] = 0;
// 	foundIDs[6] = 0;
//
// 	double distanceList[7]{ 0 };
//
// 	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
// 	int inputPoinSetNum = inputPointSet->GetSize();
// 	auto steelballCenters = mitk::PointSet::New();
//
// 	int lengthOfFingerPrint = stdNeighborNum;
// 	int numOfTargetSteelballs = stdNeighborNum + 1;
//
//
// 	for (int q{ 0 }; q < numOfTargetSteelballs; q++)
// 	{
// 		double fingerPrint[6]
// 		{
// 			allBallFingerPrint[6 * q],
// 			allBallFingerPrint[6 * q + 1],
// 			allBallFingerPrint[6 * q + 2],
// 			allBallFingerPrint[6 * q + 3],
// 			allBallFingerPrint[6 * q + 4],
// 			allBallFingerPrint[6 * q + 5],
// 		};
//
//
// 		for (int i{ 0 }; i < inputPoinSetNum; i++)
// 		{
// 			int metric{ 0 };
//
// 			double totalDistance{ 0 };
// 			for (int j{ 0 }; j < lengthOfFingerPrint; j++)
// 			{
// 				
// 				double standardDistance = fingerPrint[j];
//
//
// 				for (int k{ 0 }; k < inputPoinSetNum; k++)
// 				{
// 					double tmpDistance = GetPointDistance(inputPointSet->GetPoint(i), inputPointSet->GetPoint(k));
// 					if (fabs(tmpDistance - standardDistance) < 0.4)
// 					{
// 						metric += 1;
// 						totalDistance += tmpDistance;
// 						break;
// 					}
// 				}
//
// 				// if (metSingleRequirement == false)
// 				// {
// 				// 	break;
// 				// }
//
// 			}
//
// 			//m_Controls.textBrowser->append("metric: " + QString::number(metric));
// 			if (metric == (inputPoinSetNum-1))
// 			{
// 				if (foundIDs[q] == 0)
// 				{
// 					steelballCenters->InsertPoint(inputPointSet->GetPoint(i));
// 					distanceList[q] = totalDistance;
// 					foundIDs[q] = 1;
// 				}
//
// 				if (foundIDs[q] == 1)
// 				{
// 					if (totalDistance < distanceList[q])
// 					{
// 						distanceList[q] = totalDistance;
// 						int tmpSize = steelballCenters->GetSize();
// 						steelballCenters->SetPoint(tmpSize - 1, inputPointSet->GetPoint(i));
// 					}
// 				}
//
// 			}
//
// 		}
//
// 	}
//
// 	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
//
// 	auto tmpNode = mitk::DataNode::New();
// 	tmpNode->SetName("Steelball centers");
// 	tmpNode->SetData(steelballCenters);
// 	GetDataStorage()->Add(tmpNode);
// 	RemoveRedundantCenters();
// }


// void DentalWidget::RearrangeSteelballs(int stdNeighborNum, int foundIDs[7])
// {
// 	auto extractedPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
//
// 	auto standartSteelballCenters = mitk::PointSet::New();
// 	int stdCenterNum{ 7 };
// 	double stdCentroid[3]{ 0 };
//
// 	for (int i{ 0 }; i < stdCenterNum; i++)
// 	{
// 		if(foundIDs[i] == 1)
// 		{
// 			stdCentroid[0] += stdCenters[i * 3];
// 			stdCentroid[1] += stdCenters[i * 3 + 1];
// 			stdCentroid[2] += stdCenters[i * 3 + 2];
// 		}
//
// 		double tmpArray[3]
// 		{
// 			stdCenters[i * 3],
// 			stdCenters[i * 3 + 1],
// 			stdCenters[i * 3 + 2]
// 		};
// 		mitk::Point3D p(tmpArray);
// 		standartSteelballCenters->InsertPoint(p);
// 	}
//
// 	stdCentroid[0] = stdCentroid[0] / extractedPointSet->GetSize();
// 	stdCentroid[1] = stdCentroid[1] / extractedPointSet->GetSize();
// 	stdCentroid[2] = stdCentroid[2] / extractedPointSet->GetSize();
//
// 	double stdBallToCentroid[7]{0};
// 	for (int i{ 0 }; i < stdCenterNum; i++)
// 	{
// 		stdBallToCentroid[i] = sqrt(pow(stdCentroid[0]- standartSteelballCenters->GetPoint(i)[0],2)+
// 			pow(stdCentroid[1] - standartSteelballCenters->GetPoint(i)[1], 2) + 
// 			pow(stdCentroid[2] - standartSteelballCenters->GetPoint(i)[2], 2));
// 	}
//
// 	for (int i{ 0 }; i < stdCenterNum; i++)
// 	{
// 		if(foundIDs[i] == 0)
// 		{
// 			stdBallToCentroid[i] = 500;
// 		}
// 	}
// 	   	 
// 	// get the centroid of the extracted pointset
// 	double extractedCentroid[3]{0};
// 	for(int i{0}; i < extractedPointSet->GetSize(); i ++)
// 	{
// 		extractedCentroid[0] += extractedPointSet->GetPoint(i)[0];
// 		extractedCentroid[1] += extractedPointSet->GetPoint(i)[1];
// 		extractedCentroid[2] += extractedPointSet->GetPoint(i)[2];
// 	}
//
// 	extractedCentroid[0] = extractedCentroid[0] / (extractedPointSet->GetSize());
// 	extractedCentroid[1] = extractedCentroid[1] / (extractedPointSet->GetSize());
// 	extractedCentroid[2] = extractedCentroid[2] / (extractedPointSet->GetSize());
//
// 	auto rearrangedPset = mitk::PointSet::New();
//
// 	for (int i{ 0 }; i < 7; i++)
// 	{
// 		if(foundIDs[i] == 1)
// 		{
// 			double stdDistance = stdBallToCentroid[i];
// 			double minDistanceDifference{ 500 };
// 			int pointFlag{ 0 };
//
// 			for(int j{ 0 }; j < extractedPointSet->GetSize(); j++)
// 			{
// 				double extractedDistance =  sqrt(pow(extractedCentroid[0] - extractedPointSet->GetPoint(j)[0], 2) +
// 					pow(extractedCentroid[1] - extractedPointSet->GetPoint(j)[1], 2) +
// 					pow(extractedCentroid[2] - extractedPointSet->GetPoint(j)[2], 2));
//
// 				double tmpDistanceDifference = abs(extractedDistance - stdDistance);
//
// 				if(tmpDistanceDifference < minDistanceDifference)
// 				{
// 					minDistanceDifference = tmpDistanceDifference;
// 					pointFlag = j;
// 				}
// 			}
//
// 			rearrangedPset->InsertPoint(extractedPointSet->GetPoint(pointFlag));			
// 			
//
// 		}
// 	}
//
// 	
// 	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
//
// 	auto tmpNode = mitk::DataNode::New();
// 	tmpNode->SetName("Steelball centers");
// 	tmpNode->SetData(rearrangedPset);
// 	GetDataStorage()->Add(tmpNode);
// 	RemoveRedundantCenters();
//
// }

void DentalWidget::RearrangeSteelballs(int stdNeighborNum, int foundIDs[7])
{
	auto extractedPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto standartSteelballCenters = mitk::PointSet::New();
	int stdCenterNum{ 7 };
	

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	auto partialStdCenters = mitk::PointSet::New();
	for(int i{0}; i < stdCenterNum; i++)
	{
		if(foundIDs[i] == 1)
		{
			partialStdCenters->InsertPoint(standartSteelballCenters->GetPoint(i));
		}
	}

	int a[] = { 0,1,2,3,4,5,6 };
	std::sort(a, a + 7);

	double errorSum = 20;
	int b[7]{ 0 };
	while (std::next_permutation(a, a + 7))
	{
		auto newPset = mitk::PointSet::New();

		for (int i{0}; i < 7; i++)
		{
			if (a[i] < extractedPointSet->GetSize())
			{
				newPset->InsertPoint(extractedPointSet->GetPoint(i));
			}
		}

		auto landmarkRegistrator = mitk::SurfaceRegistration::New();
		landmarkRegistrator->SetLandmarksSrc(newPset);
		landmarkRegistrator->SetLandmarksTarget(partialStdCenters);

		landmarkRegistrator->ComputeLandMarkResult();
		double maxError = landmarkRegistrator->GetmaxLandmarkError();
		double avgError = landmarkRegistrator->GetavgLandmarkError();
		double tmpError = maxError + avgError;

		if(tmpError < errorSum)
		{
			errorSum = tmpError;
			b[0] = a[0];
			b[1] = a[1];
			b[2] = a[2];
			b[3] = a[3];
			b[4] = a[4];
			b[5] = a[5];
			b[6] = a[6];
		}

	}


	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < 7; i++)
	{
		if (b[i] < extractedPointSet->GetSize())
		{
			tmpPset->InsertPoint(extractedPointSet->GetPoint(i));
		}
	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPset);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();


}

