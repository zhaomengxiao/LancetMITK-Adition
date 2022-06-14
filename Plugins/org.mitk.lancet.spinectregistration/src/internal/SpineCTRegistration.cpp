/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "SpineCTRegistration.h"

// Qt
#include "leastsquaresfit.h"
#include <QMessageBox>
#include <QPushButton>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"
#include "vtkConnectivityFilter.h"
#include <QPushButton>
#include <mitkImage.h>
#include <vtkSphereSource.h>
#include <ep\include\vtk-9.1\vtkTransformFilter.h>


void SpineCTRegistration::ReconstructSpineSurface()
{
  auto inputCtImage = dynamic_cast<mitk::Image *>(m_CtImageDataNode->GetData());

  // The isosurface of all steelballs as into a single polydata
  double threshold = m_Controls.lineEdit_BoneSurfaceThreshold->text().toDouble();
  auto mitkSteelBallSurfaces = mitk::Surface::New();
  mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

  imageToSurfaceFilter->SetInput(inputCtImage);
  imageToSurfaceFilter->SetThreshold(threshold);
  mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();

  // draw extracted  steel ball surfaces
  auto nodeSteelballSurfaces = mitk::DataNode::New();
  nodeSteelballSurfaces->SetName("Bone surface");
  // add new node
  nodeSteelballSurfaces->SetData(mitkSteelBallSurfaces);
  GetDataStorage()->Add(nodeSteelballSurfaces);
}

void SpineCTRegistration::DEMOregistration()
{
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_CtImageDataNode->GetData());

	auto pointset_extractedballs = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData());

	m_G_renderWindow_patientCT_matrix = inputCtImage->GetGeometry()->GetVtkMatrix()->GetData();


	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	MITK_INFO << "Proceedinng Landmark registration";
	if (m_DEMO_Pointset_extractedballs != nullptr && m_DEMO_Pointset_stlballs != nullptr)
	{

		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData()); // source pointSet needs an extra offset: G_renderWindow_patientCT_matrix
		
		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_stlballs->GetData());
		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
		landmarkRegistrator->SetLandmarksTarget(targetPointset);
		landmarkRegistrator->ComputeLandMarkResult();


	}

	std::ostringstream os;
	landmarkRegistrator->GetResult()->Print(os);

	m_Controls.textBrowser_Registration->append("-------------Start landmark registration----------");
	m_Controls.textBrowser_Registration->append(QString::fromStdString(os.str()));
	m_Controls.textBrowser_Registration->append("-------------End landmark registration----------");

// m_G_patientCT_CTdrf_matrix = landmarkRegistrator->GetResult()->GetData();

	auto patientCT_renderWindow_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(inputCtImage->GetGeometry()->GetVtkMatrix(), patientCT_renderWindow_matrix);
	vtkTransform* tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(landmarkRegistrator->GetResult());
	tmpTrans->Concatenate(patientCT_renderWindow_matrix);
	tmpTrans->Update();

	m_G_patientCT_CTdrf_matrix = tmpTrans->GetMatrix()->GetData();

	vtkSmartPointer<vtkMatrix4x4> CTdrf_patientCT_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkMatrix4x4::Invert(tmpTrans->GetMatrix(), CTdrf_patientCT_matrix);
	inputCtImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(CTdrf_patientCT_matrix);


}

// An alternative and viable way 
// void SpineCTRegistration::DEMOregistration()
// {
// 	auto inputCtImage = dynamic_cast<mitk::Image*>(m_CtImageDataNode->GetData());
//
// 	auto pointset_extractedballs = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData());
//
// 	m_G_renderWindow_patientCT_matrix = inputCtImage->GetGeometry()->GetVtkMatrix()->GetData();
//
//
// 	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
//
// 	MITK_INFO << "Proceedinng Landmark registration";
// 	if (m_LandmarkSrcPointsetDataNode != nullptr && m_LandmarkTargetPointsetDataNode != nullptr)
// 	{
//
// 		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_extractedballs->GetData()); // source pointSet needs an extra offset: G_renderWindow_patientCT_matrix
//
// 		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_DEMO_Pointset_stlballs->GetData());
// 		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
// 		landmarkRegistrator->SetLandmarksTarget(targetPointset);
// 		landmarkRegistrator->ComputeLandMarkResult();
//
//
// 	}
//
// 	std::ostringstream os;
// 	landmarkRegistrator->GetResult()->Print(os);
//
// 	m_Controls.textBrowser_Registration->append("-------------Start landmark registration----------");
// 	m_Controls.textBrowser_Registration->append(QString::fromStdString(os.str()));
// 	m_Controls.textBrowser_Registration->append("-------------End landmark registration----------");
//
// 	// m_G_patientCT_CTdrf_matrix = landmarkRegistrator->GetResult()->GetData();
//
// 	// auto patientCT_renderWindow_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
// 	// vtkMatrix4x4::Invert(inputCtImage->GetGeometry()->GetVtkMatrix(), patientCT_renderWindow_matrix);
// 	vtkTransform* tmpTrans = vtkTransform::New();
// 	tmpTrans->Identity();
// 	tmpTrans->PostMultiply();
// 	tmpTrans->SetMatrix(inputCtImage->GetGeometry()->GetVtkMatrix());
// 	tmpTrans->Concatenate(landmarkRegistrator->GetResult());
// 	tmpTrans->Update();
//
// 	// m_G_patientCT_CTdrf_matrix = tmpTrans->GetMatrix()->GetData();
// 	//
// 	// vtkSmartPointer<vtkMatrix4x4> CTdrf_patientCT_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
// 	// vtkMatrix4x4::Invert(tmpTrans->GetMatrix(), CTdrf_patientCT_matrix);
// 	inputCtImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());
//
//
//
// }


double* GetMatrixAnyToolDrfToRenderingWindow(mitk::PointSet* standardSteelballCenters /*from hardware design*/, mitk::Image* inputCtImage /*should contain steelballs*/, double voxelThreshold /*steelball voxel: 3000*/,
	double facetNumUpperBound /*1440*/, double facetNumLowerBound /*500*/, double arrayNdiCtDrf[16] /*NDI data*/,
	double arrayNdiTrackingDrf[16] /*NDI data*/)
{
	// --------------------------Extract steelball centers from the CT image----------------------------
	// The isosurface of all steelballs into a single polydata

	auto mitkSteelBallSurfaces = mitk::Surface::New();
	mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

	imageToSurfaceFilter->SetInput(inputCtImage);
	imageToSurfaceFilter->SetThreshold(voxelThreshold);
	mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();

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

		auto vtkSingleSteelBallSurface = vtkConnectivityFilter->GetPolyDataOutput();

		auto numberOfCells =
			vtkSingleSteelBallSurface->GetNumberOfCells(); // the total number of cells of a single mesh surface; each cell
														   // stores one facet of the mesh surface



		if (numberOfCells < facetNumUpperBound && numberOfCells > facetNumLowerBound)
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



	auto mitkSortedSingleSteelballCenterPointset = mitk::PointSet::New();

	

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		mitkSortedSingleSteelballCenterPointset->InsertPoint(mitkSingleSteelballCenterPointset->GetPoint(distanceRanks[i]));

		int m = i;
	}

	int k = mitkSortedSingleSteelballCenterPointset->GetSize();


	//-----------------------------renderingWindow to CtDrf-----------------------------------
	
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	if (standardSteelballCenters != nullptr)
	{

		landmarkRegistrator->SetLandmarksSrc(standardSteelballCenters);
		landmarkRegistrator->SetLandmarksTarget(mitkSortedSingleSteelballCenterPointset);
		landmarkRegistrator->ComputeLandMarkResult();

	}


	vtkMatrix4x4* matrixRenderingWindowToCtDrf = landmarkRegistrator->GetResult();

	// Calculate the registration metric
	int pointNum = standardSteelballCenters->GetSize();
	double maxLandmarkError{ 0 };
	double sumLandmarkError{ 0 };
	for(int i = 0; i < pointNum; i++)
	{
		auto sourcePoint = mitkSortedSingleSteelballCenterPointset->GetPoint(i);
		auto targetPoint = standardSteelballCenters->GetPoint(i);
		auto sourcePointMatrix = vtkMatrix4x4::New();
		sourcePointMatrix->Identity();
		sourcePointMatrix->SetElement(0, 3, sourcePoint[0]);
		sourcePointMatrix->SetElement(1, 3, sourcePoint[1]);
		sourcePointMatrix->SetElement(2, 3, sourcePoint[2]);

		auto tmpTransform = vtkTransform::New();
		tmpTransform->Identity();
		tmpTransform->PostMultiply();
		tmpTransform->SetMatrix(sourcePointMatrix);
		tmpTransform->Concatenate(landmarkRegistrator->GetResult());

		auto transformedSourcePointMatrix = tmpTransform->GetMatrix();

		
		double transformedSourcePointArray[3]
		{
			transformedSourcePointMatrix->GetElement(0,3),
			transformedSourcePointMatrix->GetElement(1,3),
			transformedSourcePointMatrix->GetElement(2,3)
		};

		double currentError = sqrt(pow(targetPoint[0]-transformedSourcePointArray[0],2)+
		pow(targetPoint[1]-transformedSourcePointArray[1],2)+
		pow(targetPoint[2]-transformedSourcePointArray[2],2));

		sumLandmarkError = sumLandmarkError + currentError;
		if(currentError > maxLandmarkError)
		{
			maxLandmarkError = currentError;
		}
	}

	MITK_INFO << maxLandmarkError;
	MITK_INFO << sumLandmarkError / pointNum;


	//------------------------------CtDrf to trackingDrf-------------------------
	auto toolMatrix = vtkMatrix4x4::New();
	auto ReferenceMatrix = vtkMatrix4x4::New();
	toolMatrix->DeepCopy(arrayNdiTrackingDrf);
	ReferenceMatrix->DeepCopy(arrayNdiCtDrf);
	



	ReferenceMatrix->Invert();
	

	auto tmpTransform = vtkTransform::New();
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->Concatenate(toolMatrix);
	tmpTransform->Concatenate(ReferenceMatrix);

	auto matrixCtDrfToTrackingDrf = tmpTransform->GetMatrix();



	//-----------------Assemble and output----------------
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->Identity();
	tmpTrans->Concatenate(matrixCtDrfToTrackingDrf);
	tmpTrans->Concatenate(matrixRenderingWindowToCtDrf);



	double* a = tmpTrans->GetMatrix()->GetData();

	auto test = tmpTrans->GetMatrix();

	return tmpTrans->GetMatrix()->GetData();


}