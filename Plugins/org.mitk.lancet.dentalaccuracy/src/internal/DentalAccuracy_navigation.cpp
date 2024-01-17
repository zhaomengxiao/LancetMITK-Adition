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
#include "DentalAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCardinalSpline.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkRendererCollection.h>
#include <vtkSplineFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "leastsquaresfit.h"
#include "mitkGizmo.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkPointSet.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"



void DentalAccuracy::on_pushButton_imageRegisNew_clicked()
{
	auto cmm_Ball_node = GetDataStorage()->GetNamedNode("steelball_cmm"); // steelballs from CMM

	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image"); // steelballs in image

	auto cmm_ProbeDitch_node = GetDataStorage()->GetNamedNode("probePoints_cmm"); // probe ditch points from CMM

	if (cmm_Ball_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_cmm is missing");
		return;
	}

	if (extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("'steelball_image' is missing");
		return;
	}

	if (cmm_ProbeDitch_node == nullptr)
	{
		m_Controls.textBrowser->append("probePoints_cmm is missing");
		return;
	}

	if(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_cmm")->GetData())->GetSize() !=
		dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_image")->GetData())->GetSize()
		)
	{
		m_Controls.textBrowser->append("steelball_image extraction is not complete!");
		return;
	}

	// Step 1: Calculate the transform from steelballs_cmm to steelballs_image
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_cmm")->GetData()));
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_image")->GetData()));

	landmarkRegistrator->ComputeLandMarkResult();

	auto tmpMatrix = landmarkRegistrator->GetResult();

	// Step 2: Apply tmpMatrix to 'probePoints_cmm' to get 'probePoints_image'
	auto probePoints_cmm = GetDataStorage()->GetNamedObject<mitk::PointSet>("probePoints_cmm");
	probePoints_cmm->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	auto probePoints_image = mitk::PointSet::New();

	for (int i{ 0 }; i < probePoints_cmm->GetSize(); i++)
	{
		probePoints_image->InsertPoint(probePoints_cmm->GetPoint(i));
	}

	// Step 3: Check if enough probe ditch points have been collected
	if (m_probeDitchPset_rf == nullptr)
	{
		m_Controls.textBrowser->append("No probe ditch point has been captured");
		return;
	}

	if (m_probeDitchPset_rf->GetSize() < 4)
	{
		m_Controls.textBrowser->append("At least 5 probe ditch points should be captured");
		return;
	}

	// Step 4: Calculate m_T_patientRFtoImage
	

	int collectedPointNum = m_probeDitchPset_rf->GetSize();
	int totalPointNum = probePoints_image->GetSize();

	auto sorted_probeDitchPset_rf = mitk::PointSet::New();
	SortPointSetByDistance(m_probeDitchPset_rf, sorted_probeDitchPset_rf);

	// Generate the tmp landmark_src
	auto tmp_landmark_src = mitk::PointSet::New();
	auto sorted_landmark_src = mitk::PointSet::New();
	double maxError{ 1000 };
	double avgError{ 1000 };

	std::vector<std::vector<int>> combinations = GenerateAllCombinations(totalPointNum-1, collectedPointNum);

	for (const auto& combination : combinations) {

		tmp_landmark_src->Clear();

		for (int value : combination) {
			tmp_landmark_src->InsertPoint(probePoints_image->GetPoint(value));
		}

		SortPointSetByDistance(tmp_landmark_src, sorted_landmark_src);

		auto tmpLandmarkRegistrator = mitk::SurfaceRegistration::New();
		tmpLandmarkRegistrator->SetLandmarksSrc(sorted_landmark_src);
		tmpLandmarkRegistrator->SetLandmarksTarget(sorted_probeDitchPset_rf);

		m_Controls.textBrowser->append("sorted_landmark_src Pnum: " + QString::number(sorted_landmark_src->GetSize()));

		m_Controls.textBrowser->append("sorted_probeDitchPset_rf Pnum: " + QString::number(sorted_probeDitchPset_rf->GetSize()));


		tmpLandmarkRegistrator->ComputeLandMarkResult();
		double tmpMaxError = tmpLandmarkRegistrator->GetmaxLandmarkError();
		double tmpAvgError = tmpLandmarkRegistrator->GetavgLandmarkError();

		m_Controls.lineEdit_maxError->setText(QString::number(tmpMaxError));
		m_Controls.lineEdit_avgError->setText(QString::number(tmpAvgError));

		if (tmpMaxError < 2.5)
		{
			maxError = tmpMaxError;
			avgError = tmpAvgError;

			memcpy_s(m_T_patientRFtoImage, sizeof(double) * 16, tmpLandmarkRegistrator->GetResult()->GetData(), sizeof(double) * 16);

			m_Stat_patientRFtoImage = true;

			break;
		}
	}

	if(maxError < 2.5 && avgError < 2.5)
	{
		m_Controls.textBrowser->append("Image registration succeeded");
		

	}else
	{
		m_Controls.textBrowser->append("Image registration failed, please collect more points");
	}

	// old pipeline style
	// // The surface node should have no offset, i.e., should have an identity matrix!
	// auto surfaceNode = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");
	//
	// if (surfaceNode == nullptr)
	// {
	// 	m_Controls.textBrowser->append("Reconstructed CBCT surface is missing!");
	// 	return;
	// }
	//
	// m_NavigatedImage = lancet::NavigationObject::New();
	//
	// auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();
	//
	// if (matrix->IsIdentity() == false)
	// {
	// 	vtkNew<vtkMatrix4x4> identityMatrix;
	// 	identityMatrix->Identity();
	// 	dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	//
	// 	m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	// }
	//
	// m_NavigatedImage->SetDataNode(surfaceNode);
	//
	// m_NavigatedImage->SetLandmarks(probePoints_image);
	//
	// m_NavigatedImage->SetReferencFrameName(surfaceNode->GetName());
	//
	// m_Controls.textBrowser->append("--- NavigatedImage has been set up ---");
	//
	// m_NavigatedImage->SetLandmarks_probe(m_probeDitchPset_rf);
	//
	// /// Apply image registration
	// m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	// m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);
	//
	// m_NavigatedImage->UpdateObjectToRfMatrix();
	// m_Controls.textBrowser->append("Avg landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_avgError()));
	// m_Controls.textBrowser->append("Max landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_maxError()));
	//
	// m_ImageRegistrationMatrix->DeepCopy(m_NavigatedImage->GetT_Object2ReferenceFrame());
	//
	// auto tmpMatrix_1 = mitk::AffineTransform3D::New();
	//
	// mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix, tmpMatrix_1.GetPointer());
	//
	// m_VegaToolStorage->GetToolByName("patientRF")->SetToolRegistrationMatrix(tmpMatrix_1);
	//
	// m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("patientRF")->GetToolRegistrationMatrix());
	//
	// m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("patientRF"));
	//
	// m_VegaVisualizeTimer->stop();
	// m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	// m_VegaVisualizeTimer->start();


}

void DentalAccuracy::GenerateCombinations(int m, int n, int index, std::vector<int>& currentCombination, std::vector<std::vector<int>>& result)
{
	if (currentCombination.size() == n) {
		result.push_back(currentCombination);
		return;
	}
	for (int i = index; i <= m; ++i) {
		currentCombination.push_back(i);
		GenerateCombinations(m, n, i + 1, currentCombination, result);
		currentCombination.pop_back();
	}
}

std::vector<std::vector<int>> DentalAccuracy::GenerateAllCombinations(int m, int n)
{
	std::vector<std::vector<int>> result;
	std::vector<int> currentCombination;
	GenerateCombinations(m, n, 0, currentCombination, result);
	return result;
}



// Function to calculate the Euclidean distance between two points
double DentalAccuracy::CalculateDistance(const mitk::Point3D& point1, const mitk::Point3D& point2)
{
	return std::sqrt(std::pow(point1[0] - point2[0], 2) +
		std::pow(point1[1] - point2[1], 2) +
		std::pow(point1[2] - point2[2], 2));
}

// Function to calculate the mass center of a mitk::PointSet
mitk::Point3D DentalAccuracy::CalculateMassCenter(const mitk::PointSet::Pointer& pointSet)
{
	mitk::Point3D massCenter;

	// Calculate the sum of coordinates
	for (int i{ 0 }; i < pointSet->GetSize() ; i++)
	{
		massCenter[0] += pointSet->GetPoint(i)[0];
		massCenter[1] += pointSet->GetPoint(i)[1];
		massCenter[2] += pointSet->GetPoint(i)[2];
	}

	// Divide by the number of points to get the average (mass center)
	int numberOfPoints = pointSet->GetSize();
	massCenter[0] /= numberOfPoints;
	massCenter[1] /= numberOfPoints;
	massCenter[2] /= numberOfPoints;

	return massCenter;
}

// Comparator function for sorting points based on distance from mass center
bool DentalAccuracy::ComparePointsByDistance(const mitk::Point3D& massCenter, const mitk::Point3D& point1, const mitk::Point3D& point2)
{
	return CalculateDistance(massCenter, point1) < CalculateDistance(massCenter, point2);
}

void DentalAccuracy::SortPointSetByDistance(mitk::PointSet::Pointer inputPointSet, mitk::PointSet::Pointer outputPointSet)
{
	// Calculate the mass center
	mitk::Point3D massCenter = CalculateMassCenter(inputPointSet);

	// Get the points container
	// mitk::PointSet::PointsContainer::Pointer pointsContainer = inputPointSet->GetPoints();

	// Convert the points container to a vector for sorting
	std::vector<mitk::Point3D> pointsVector;
	for (int i{0}; i < inputPointSet->GetSize(); i++)
	{
		auto point = inputPointSet->GetPoint(i);
		pointsVector.push_back(point);
	}

	// Sort the points based on their distance from the mass center
	std::sort(pointsVector.begin(), pointsVector.end(),
		[&](const mitk::Point3D& point1, const mitk::Point3D& point2)
		{
			return ComparePointsByDistance(massCenter, point1, point2);
		});

	// Clear existing points in the point set
	outputPointSet->Clear();

	// Add sorted points back to the point set
	for (const auto& sortedPoint : pointsVector)
	{
		outputPointSet->InsertPoint(sortedPoint);
	}
}


void DentalAccuracy::on_pushButton_resetImageRegis_clicked()
{
	m_Controls.label_15->setText("0");
	m_probeDitchPset_rf = mitk::PointSet::New();

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->Identity();

	memcpy_s(m_T_patientRFtoImage, sizeof(double) * 16, tmpMatrix->GetData(), sizeof(double) * 16);

	m_Stat_patientRFtoImage = false;

	disconnect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DentalAccuracy::UpdateDrillVisual);

	m_Controls.lineEdit_maxError->setText("NaN");
	m_Controls.lineEdit_avgError->setText("NaN");

	// m_ImageRegistrationMatrix->Identity();
}


void DentalAccuracy::on_pushButton_collectDitch_clicked()
{
	if (GetDataStorage()->GetNamedNode("probePoints_cmm") == nullptr)
	{
		m_Controls.textBrowser->append("probePoints_cmm is missing.");
		return;
	}

	m_probeDitchPset_cmm = GetDataStorage()->GetNamedObject<mitk::PointSet>("probePoints_cmm");

	if (m_probeDitchPset_rf == nullptr)
	{
		m_probeDitchPset_rf = mitk::PointSet::New();
	}

	if (m_probeDitchPset_rf->GetSize() == m_probeDitchPset_cmm->GetSize())
	{
		m_Controls.textBrowser->append("Enough points have been captured");
		return;
	}

	m_Controls.label_16->setText("/" + QString::number(m_probeDitchPset_cmm->GetSize()));

	if(m_Stat_handpieceRFtoDrill == false)
	{
		m_Controls.textBrowser->append("Handpiece has not been calibrated !");
		return;
	}

	if(m_Stat_cameraToHandpieceRF == false || m_Stat_cameraToPatientRF == false)
	{
		m_Controls.textBrowser->append("RF is not visible !");
		return;
	}

	auto T_handpieceRFtoDrill = vtkMatrix4x4::New();
	T_handpieceRFtoDrill->DeepCopy(m_T_handpieceRFtoDrill);

	auto T_patientRFtoCamera = vtkMatrix4x4::New();
	T_patientRFtoCamera->DeepCopy(m_T_cameraToPatientRF);
	T_patientRFtoCamera->Invert();

	auto T_cameraToHandpieceRF = vtkMatrix4x4::New();
	T_cameraToHandpieceRF->DeepCopy(m_T_cameraToHandpieceRF);

	auto trans_patientRFtoDrill = vtkTransform::New();
	trans_patientRFtoDrill->Identity();
	trans_patientRFtoDrill->PostMultiply();
	trans_patientRFtoDrill->SetMatrix(T_handpieceRFtoDrill);
	trans_patientRFtoDrill->Concatenate(T_cameraToHandpieceRF);
	trans_patientRFtoDrill->Concatenate(T_patientRFtoCamera);
	trans_patientRFtoDrill->Update();

	auto patientRFtoDrillMatrix = trans_patientRFtoDrill->GetMatrix();

	mitk::Point3D tipPointUnderPatientRF;
	tipPointUnderPatientRF[0] = patientRFtoDrillMatrix->GetElement(0, 3);
	tipPointUnderPatientRF[1] = patientRFtoDrillMatrix->GetElement(1, 3);
	tipPointUnderPatientRF[2] = patientRFtoDrillMatrix->GetElement(2, 3);

	double minDistance{ 100 };
	for(int i{0}; i < m_probeDitchPset_rf->GetSize() ; i++)
	{
		mitk::Point3D testPoint = m_probeDitchPset_rf->GetPoint(i);
		double tmpDistance = sqrt(pow(testPoint[0]-tipPointUnderPatientRF[0],2)+
			pow(testPoint[1] - tipPointUnderPatientRF[1], 2)+
			pow(testPoint[2] - tipPointUnderPatientRF[2], 2));

		if(tmpDistance < minDistance)
		{
			minDistance = tmpDistance;
		}
	}

	if(minDistance > 2.5)
	{
		m_probeDitchPset_rf->InsertPoint(tipPointUnderPatientRF);

		m_Controls.label_15->setText(QString::number(m_probeDitchPset_rf->GetSize()));
	}else
	{
		m_Controls.textBrowser->append("Don't capture the same point");
	}
	

	// old pipeline style: Calculate T_drillDRFtoCalibratorDRF
	// auto patientRFindex = m_VegaToolStorage->GetToolIndexByName("patientRF");
	// auto drillDRFindex = m_VegaToolStorage->GetToolIndexByName("drillRF");
	// if (patientRFindex == -1 || drillDRFindex == -1)
	// {
	// 	m_Controls.textBrowser->append("There is no 'patientRF' or 'drillDRF' in the toolStorage!");
	// 	return;
	// }
	//
	// mitk::NavigationData::Pointer nd_ndiToPatientRF = m_VegaSource->GetOutput(patientRFindex);
	// mitk::NavigationData::Pointer nd_ndiTodrillDRF = m_VegaSource->GetOutput(drillDRFindex);
	//
	// if (nd_ndiToPatientRF->IsDataValid() == 0 || nd_ndiTodrillDRF->IsDataValid() == 0)
	// {
	// 	m_Controls.textBrowser->append("'patientRF' or 'drillDRF' is missing");
	// 	return;
	// }
	//
	// mitk::NavigationData::Pointer nd_patientRFtoDrillRF = GetNavigationDataInRef(nd_ndiTodrillDRF, nd_ndiToPatientRF);
	//
	// vtkMatrix4x4* T_patientRFtoDrillDRF = vtkMatrix4x4::New();
	// mitk::TransferItkTransformToVtkMatrix(nd_patientRFtoDrillRF->GetAffineTransform3D().GetPointer(), T_patientRFtoDrillDRF);
	//
	// auto drillRFtoTipMatrix = vtkMatrix4x4::New();
	// mitk::TransferItkTransformToVtkMatrix(m_VegaToolStorage->GetToolByName("drillRF")->GetToolRegistrationMatrix().GetPointer(), drillRFtoTipMatrix);
	//
	// if (drillRFtoTipMatrix->IsIdentity())
	// {
	// 	m_Controls.textBrowser->append("The drill has not been calibrated yet.");
	// 	return;
	// }
	//
	// auto tmpTrans = vtkTransform::New();
	// tmpTrans->Identity();
	// tmpTrans->PostMultiply();
	// tmpTrans->SetMatrix(drillRFtoTipMatrix);
	// tmpTrans->Concatenate(T_patientRFtoDrillDRF);
	// tmpTrans->Update();
	//
	// auto patientRFtoDrillTipMatrix = tmpTrans->GetMatrix();
	//
	// mitk::Point3D tipPointUnderPatientRF;
	// tipPointUnderPatientRF[0] = patientRFtoDrillTipMatrix->GetElement(0, 3);
	// tipPointUnderPatientRF[1] = patientRFtoDrillTipMatrix->GetElement(1, 3);
	// tipPointUnderPatientRF[2] = patientRFtoDrillTipMatrix->GetElement(2, 3);
	//
	// m_probeDitchPset_rf->InsertPoint(tipPointUnderPatientRF);
	//
	// m_Controls.textBrowser->append("Captured point " + QString::number(m_probeDitchPset_rf->GetSize()));
	//
	// m_Controls.label_15->setText(QString::number(m_probeDitchPset_rf->GetSize()));

}

void DentalAccuracy::on_pushButton_connectVega_clicked()
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());


	MITK_INFO << "Vega tracking";
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();

	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DentalAccuracy::OnVegaVisualizeTimer);

	ShowToolStatus_Vega();

	m_VegaVisualizeTimer->start(200); //Every 200ms the method OnTimer() is called. -> 5fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	m_ImageRegistrationMatrix->Identity();

}


void DentalAccuracy::OnVegaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.

	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		// auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
		// mitk::RenderingManager::GetInstance()->InitializeViews(geo);
		this->RequestRenderWindowUpdate();
	}

	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();

	// Update the global variables which hold the camera data
	auto calibratorRFindex = m_VegaToolStorage->GetToolIndexByName("calibratorRF");
	auto handpieceRFindex = m_VegaToolStorage->GetToolIndexByName("handpieceRF");
	auto patientRFindex = m_VegaToolStorage->GetToolIndexByName("patientRF");

	if (calibratorRFindex == -1 || handpieceRFindex == -1 || patientRFindex == -1)
	{
		return;
	}

	mitk::NavigationData::Pointer nd_cameraToCalibratorRF = m_VegaSource->GetOutput(calibratorRFindex);
	mitk::NavigationData::Pointer nd_cameraToHandpieceRF = m_VegaSource->GetOutput(handpieceRFindex);
	mitk::NavigationData::Pointer nd_cameraToPatientRF = m_VegaSource->GetOutput(patientRFindex);

	m_Stat_cameraToCalibratorRF = nd_cameraToCalibratorRF->IsDataValid();
	m_Stat_cameraToHandpieceRF = nd_cameraToHandpieceRF->IsDataValid();
	m_Stat_cameraToPatientRF = nd_cameraToPatientRF->IsDataValid();

	if (m_Stat_cameraToCalibratorRF)
	{
		auto tmpMatrix = vtkMatrix4x4::New();
		mitk::TransferItkTransformToVtkMatrix(nd_cameraToCalibratorRF->GetAffineTransform3D().GetPointer(), tmpMatrix);
		memcpy_s(m_T_cameraToCalibratorRF, sizeof(double) * 16, tmpMatrix->GetData(), sizeof(double) * 16);
	}

	if (m_Stat_cameraToHandpieceRF)
	{
		auto tmpMatrix = vtkMatrix4x4::New();
		mitk::TransferItkTransformToVtkMatrix(nd_cameraToHandpieceRF->GetAffineTransform3D().GetPointer(), tmpMatrix);
		memcpy_s(m_T_cameraToHandpieceRF, sizeof(double) * 16, tmpMatrix->GetData(), sizeof(double) * 16);
	}

	if (m_Stat_cameraToPatientRF)
	{
		auto tmpMatrix = vtkMatrix4x4::New();
		mitk::TransferItkTransformToVtkMatrix(nd_cameraToPatientRF->GetAffineTransform3D().GetPointer(), tmpMatrix);
		memcpy_s(m_T_cameraToPatientRF, sizeof(double) * 16, tmpMatrix->GetData(), sizeof(double) * 16);
	}

}


void DentalAccuracy::on_pushButton_calibrateDrill_clicked()
{
	// This function is aimed to calculate m_T_handpieceRFtoDrill

	// Step 1: calculate T_calibratorRFtoDrill from the
	// PointSet probe_head_tail_mandible or probe_head_tail_maxilla in the dataStorage
	if(GetDataStorage()->GetNamedNode("probe_head_tail_mandible") == nullptr ||
		GetDataStorage()->GetNamedNode("probe_head_tail_maxilla") == nullptr)
	{
		m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is missing!");
		return;
	}

	auto probe_head_tail_mandible = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_mandible")->GetData());
	auto probe_head_tail_maxilla = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("probe_head_tail_maxilla")->GetData());

	if(probe_head_tail_mandible->GetSize() != 2 || probe_head_tail_maxilla->GetSize() != 2)
	{
		m_Controls.textBrowser->append("probe_head_tail_mandible or probe_head_tail_maxilla is problematic!");
		return;
	}

	auto probe_head_tail = mitk::PointSet::New();

	if(m_Controls.radioButton_maxilla->isChecked())
	{
		probe_head_tail = probe_head_tail_maxilla;
	}else
	{
		probe_head_tail = probe_head_tail_mandible;
	}

	auto probe_head = probe_head_tail->GetPoint(0);
	auto probe_tail = probe_head_tail->GetPoint(1);

	Eigen::Vector3d z_probeInCalibratorRF;
	z_probeInCalibratorRF[0] = probe_tail[0] - probe_head[0];
	z_probeInCalibratorRF[1] = probe_tail[1] - probe_head[1];
	z_probeInCalibratorRF[2] = probe_tail[2] - probe_head[2];
	z_probeInCalibratorRF.normalize();

	Eigen::Vector3d z_std{0,0,1};

	Eigen::Vector3d rotAxis = z_std.cross(z_probeInCalibratorRF);

	rotAxis.normalize();

	if(rotAxis.norm() < 0.00001) // in case the rotAxis becomes a zero vector
	{
		rotAxis[0] = 1;
		rotAxis[1] = 0;
		rotAxis[2] = 0;
	}

	double rotAngle = 180 * acos(z_std.dot(z_probeInCalibratorRF)) / 3.141592654;

	auto trans_calibratorRFtoDrill = vtkTransform::New();
	trans_calibratorRFtoDrill->Identity();
	trans_calibratorRFtoDrill->PostMultiply();
	trans_calibratorRFtoDrill->RotateWXYZ(rotAngle, rotAxis[0], rotAxis[1], rotAxis[2]);
	trans_calibratorRFtoDrill->Update();

	auto T_calibratorRFtoDrill = trans_calibratorRFtoDrill->GetMatrix();
	T_calibratorRFtoDrill->SetElement(0, 3, probe_head[0]);
	T_calibratorRFtoDrill->SetElement(1, 3, probe_head[1]);
	T_calibratorRFtoDrill->SetElement(2, 3, probe_head[2]);

	for (int i{ 0 }; i < 16; i++)
	{
		m_T_calibratorRFtoDrill[i] = T_calibratorRFtoDrill->GetData()[i];
	}
	
	m_Stat_calibratorRFtoDrill = true;

	// Step 2: Obtain the camera data and assemble the matrix:
	// T_handpieceRFtoDrill = (T_cameraTohandpieceRF)^-1 * T_cameraToCalibratorRF * T_calibratorRFtoDrill
	if (m_Stat_cameraToCalibratorRF == false || m_Stat_cameraToHandpieceRF == false)
	{
		m_Controls.textBrowser->append("calibratorRF or handpieceRF is invisible");
		return;
	}
	   

	if (m_Stat_calibratorRFtoDrill == false)
	{
		m_Controls.textBrowser->append("m_T_calibratorRFtoDrill from hardware design is not ready");
		return;
	}

	auto T_handpieceRFtoCamera = vtkMatrix4x4::New();
	auto T_cameraToCalibratorRF = vtkMatrix4x4::New();
	auto T_calibratorRFToDrill = vtkMatrix4x4::New();

	T_handpieceRFtoCamera->DeepCopy(m_T_cameraToHandpieceRF);
	T_handpieceRFtoCamera->Invert();

	T_cameraToCalibratorRF->DeepCopy(m_T_cameraToCalibratorRF);

	T_calibratorRFToDrill->DeepCopy(m_T_calibratorRFtoDrill);

	auto trans_handpieceRFtoDrill = vtkTransform::New();
	trans_handpieceRFtoDrill->Identity();
	trans_handpieceRFtoDrill->PostMultiply();
	trans_handpieceRFtoDrill->SetMatrix(T_calibratorRFToDrill);
	trans_handpieceRFtoDrill->Concatenate(T_cameraToCalibratorRF);
	trans_handpieceRFtoDrill->Concatenate(T_handpieceRFtoCamera);
	trans_handpieceRFtoDrill->Update();

	// Todo: the matrix below should be averaged for a time span before being stored into m_T_handpieceRFtoDrill
	auto T_handpieceRFtoDrill = trans_handpieceRFtoDrill->GetMatrix();

	memcpy_s(m_T_handpieceRFtoDrill, sizeof(double) * 16, T_handpieceRFtoDrill->GetData(), sizeof(double) * 16);

	m_Stat_handpieceRFtoDrill = true;

	m_Controls.textBrowser->append("Handpiece calibration succeeded!");

	// Calculate T_drillDRFtoCalibratorDRF (realization with pipeline)
	// auto calibratorDRFindex = m_VegaToolStorage->GetToolIndexByName("calibrator");
	// auto drillDRFindex = m_VegaToolStorage->GetToolIndexByName("drillRF");
	// if (calibratorDRFindex == -1 || drillDRFindex == -1)
	// {
	// 	m_Controls.textBrowser->append("There is no 'calibrator' or 'drillDRF' in the toolStorage!");
	// }
	//
	// mitk::NavigationData::Pointer nd_ndiTocalibratorDRF = m_VegaSource->GetOutput(calibratorDRFindex);
	// mitk::NavigationData::Pointer nd_ndiTodrillDRF = m_VegaSource->GetOutput(drillDRFindex);
	//
	// if (nd_ndiTocalibratorDRF->IsDataValid() == 0 || nd_ndiTodrillDRF->IsDataValid() == 0)
	// {
	// 	m_Controls.textBrowser->append("calibrator or drillRF is missing");
	// 	return;
	// }
	//
	// mitk::NavigationData::Pointer nd_drillDRFtoCalibratorDRF = GetNavigationDataInRef(nd_ndiTocalibratorDRF, nd_ndiTodrillDRF);
	//
	// vtkMatrix4x4* T_drillDRFtoCalibratorDRF = vtkMatrix4x4::New();
	// mitk::TransferItkTransformToVtkMatrix(nd_drillDRFtoCalibratorDRF->GetAffineTransform3D().GetPointer(), T_drillDRFtoCalibratorDRF);
	//
	// m_Controls.textBrowser->append("Drill calibration matrix");
	// for (int i{ 0 }; i < 4; i++)
	// {
	// 	for (int j{ 0 }; j < 4; j++)
	// 	{
	// 		m_Controls.textBrowser->append(QString::number(T_drillDRFtoCalibratorDRF->GetElement(i, j)));
	// 	}
	// }
	//
	// m_VegaToolStorage->GetToolByName("drillRF")->SetToolRegistrationMatrix(nd_drillDRFtoCalibratorDRF->GetAffineTransform3D());


}

void DentalAccuracy::on_pushButton_startNavi_clicked()
{
	disconnect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DentalAccuracy::UpdateDrillVisual);

	if (m_Stat_handpieceRFtoDrill == false)
	{
		m_Controls.textBrowser->append("Handpiece calibration is not ready!");
		return;
	}

	if (m_Stat_patientRFtoImage == false)
	{
		m_Controls.textBrowser->append("Image registration is not ready!");
		return;
	}

	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DentalAccuracy::UpdateDrillVisual);

}


void DentalAccuracy::UpdateDrillVisual()
{
	if(GetDataStorage()->GetNamedNode("drillSurface") == nullptr)
	{
		m_Controls.textBrowser->append("drillSurface is missing");
		return;
	}

	if(m_Stat_cameraToHandpieceRF == false || m_Stat_cameraToPatientRF == false)
	{
		// m_Controls.textBrowser->append("RF is not visible!");
		return;
	}

	auto T_cameraToHandpieceRF = vtkMatrix4x4::New();
	T_cameraToHandpieceRF->DeepCopy(m_T_cameraToHandpieceRF);

	auto T_handpieceRFtoDrill = vtkMatrix4x4::New();
	T_handpieceRFtoDrill->DeepCopy(m_T_handpieceRFtoDrill);

	auto T_patientRFtoCamera = vtkMatrix4x4::New();
	T_patientRFtoCamera->DeepCopy(m_T_cameraToPatientRF);
	T_patientRFtoCamera->Invert();

	auto T_imageToPatientRF = vtkMatrix4x4::New();
	T_imageToPatientRF->DeepCopy(m_T_patientRFtoImage);
	T_imageToPatientRF->Invert();

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(T_handpieceRFtoDrill);
	tmpTrans->Concatenate(T_cameraToHandpieceRF);
	tmpTrans->Concatenate(T_patientRFtoCamera);
	tmpTrans->Concatenate(T_imageToPatientRF);
	tmpTrans->Update();

	auto T_imageToDrill = tmpTrans->GetMatrix();

	GetDataStorage()->GetNamedNode("drillSurface")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(T_imageToDrill);

	// m_Controls.textBrowser->append("drillSurface updated!");
}



void DentalAccuracy::on_pushButton_imageRegis_clicked()
{
	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image");

	auto stdBall_node = GetDataStorage()->GetNamedNode("steelball_rf");

	if (extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_image is missing");
		return;
	}

	if (stdBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_rf is missing");
		return;
	}

	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	auto sourcePointset = dynamic_cast<mitk::PointSet*>(extractedBall_node->GetData());
	auto targetPointset = dynamic_cast<mitk::PointSet*>(stdBall_node->GetData());
	landmarkRegistrator->SetLandmarksSrc(sourcePointset);
	landmarkRegistrator->SetLandmarksTarget(targetPointset);
	landmarkRegistrator->ComputeLandMarkResult();

	auto T_patientRFtoImage = landmarkRegistrator->GetResult();

	memcpy_s(m_T_patientRFtoImage, sizeof(double) * 16, T_patientRFtoImage->GetData(), sizeof(double) * 16);

	m_Stat_patientRFtoImage = true;

	m_Controls.textBrowser->append("Image registration succeeded!");

	// Realization with pipeline
	// if (m_ImageRegistrationMatrix->IsIdentity() == false)
	// {
	// 	m_Controls.textBrowser->append("Image registration has been done.");
	// 	return;
	// }
	//
	// auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image");
	//
	// auto stdBall_node = GetDataStorage()->GetNamedNode("steelball_rf");
	//
	// if (extractedBall_node == nullptr)
	// {
	// 	m_Controls.textBrowser->append("steelball_image is missing");
	// 	return;
	// }
	//
	// if (stdBall_node == nullptr)
	// {
	// 	m_Controls.textBrowser->append("steelball_rf is missing");
	// 	return;
	// }
	//
	// auto extractedBall_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_image");
	// auto stdball_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_rf");
	// int extracted_num = extractedBall_pset->GetSize();
	//
	// if (extracted_num < stdball_pset->GetSize())
	// {
	// 	m_Controls.textBrowser->append("steelball_image extraction incomplete");
	// 	return;
	// }
	//
	//
	// // The surface node should have no offset, i.e., should have an identity matrix!
	// auto surfaceNode = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");
	//
	// if (surfaceNode == nullptr)
	// {
	// 	m_Controls.textBrowser->append("Reconstructed CBCT surface is missing!");
	// 	return;
	// }
	//
	// m_NavigatedImage = lancet::NavigationObject::New();
	//
	// auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();
	//
	// if (matrix->IsIdentity() == false)
	// {
	// 	vtkNew<vtkMatrix4x4> identityMatrix;
	// 	identityMatrix->Identity();
	// 	dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);
	//
	// 	m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	// }
	//
	// m_NavigatedImage->SetDataNode(surfaceNode);
	//
	// m_NavigatedImage->SetLandmarks(extractedBall_pset);
	//
	// m_NavigatedImage->SetReferencFrameName(surfaceNode->GetName());
	//
	// m_Controls.textBrowser->append("--- NavigatedImage has been set up ---");
	//
	// m_NavigatedImage->SetLandmarks_probe(stdball_pset);
	//
	// /// Apply image registration
	// m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	// m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);
	//
	// m_NavigatedImage->UpdateObjectToRfMatrix();
	// m_Controls.textBrowser->append("Avg landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_avgError()));
	// m_Controls.textBrowser->append("Max landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_maxError()));
	//
	// m_ImageRegistrationMatrix->DeepCopy(m_NavigatedImage->GetT_Object2ReferenceFrame());
	//
	// auto tmpMatrix = mitk::AffineTransform3D::New();
	//
	// mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix, tmpMatrix.GetPointer());
	//
	// m_VegaToolStorage->GetToolByName("patientRF")->SetToolRegistrationMatrix(tmpMatrix);
	//
	// m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("patientRF")->GetToolRegistrationMatrix());
	//
	// m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("patientRF"));
	//
	// m_VegaVisualizeTimer->stop();
	// m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	// m_VegaVisualizeTimer->start();

}

