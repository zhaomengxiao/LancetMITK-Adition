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
#include "SpineDemo.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QTimer>

#include "mitkMatrixConvert.h"
#include "surfaceregistraion.h"


void SpineDemo::on_pushButton_detectBalls_3d_clicked()
{
	// Step 1: calculate the transform matrix from the steelballRF to patientRF
	auto patientRFindex = m_VegaToolStorage->GetToolIndexByName("patientRF");
	auto steelballRFindex = m_VegaToolStorage->GetToolIndexByName("steelballRF");
	if (patientRFindex == -1 || steelballRFindex == -1)
	{
		m_Controls.textBrowser->append("There is no 'patientRF' or 'steelballRF' in the toolStorage!");
		return;
	}

	mitk::NavigationData::Pointer nd_ndiToPatientRF = m_VegaSource->GetOutput(patientRFindex);
	mitk::NavigationData::Pointer nd_ndiToSteelballRF = m_VegaSource->GetOutput(steelballRFindex);

	if (nd_ndiToPatientRF->IsDataValid() == 0 || nd_ndiToSteelballRF->IsDataValid() == 0)
	{
		m_Controls.textBrowser->append("'patientRF' or 'steelballRF' is not visible");
		return;
	}

	vtkMatrix4x4* T_ndiToPatientRF = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_ndiToPatientRF->GetAffineTransform3D().GetPointer(), T_ndiToPatientRF);

	vtkMatrix4x4* T_ndiToSteelballRF = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_ndiToSteelballRF->GetAffineTransform3D().GetPointer(), T_ndiToSteelballRF);

	vtkMatrix4x4* T_steelballRFtoNdi = vtkMatrix4x4::New();
	T_steelballRFtoNdi->DeepCopy(T_ndiToSteelballRF);
	T_steelballRFtoNdi->Invert();

	auto trans_steelballRFtoPatientRF = vtkTransform::New();
	trans_steelballRFtoPatientRF->PostMultiply();
	trans_steelballRFtoPatientRF->Identity();
	trans_steelballRFtoPatientRF->SetMatrix(T_ndiToPatientRF);
	trans_steelballRFtoPatientRF->Concatenate(T_steelballRFtoNdi);
	trans_steelballRFtoPatientRF->Update();

	// mitk::NavigationData::Pointer nd_steeballRFtoPatientRF = GetNavigationDataInRef(nd_ndiToPatientRF, nd_ndiToSteelballRF);

	vtkMatrix4x4* T_steelballRFtoPatientRF = trans_steelballRFtoPatientRF->GetMatrix();
	// mitk::TransferItkTransformToVtkMatrix(nd_steeballRFtoPatientRF->GetAffineTransform3D().GetPointer(), T_steelballRFtoPatientRF);

	// Step 2: Extract the steelball center pointset in the CT image
	// TODO: Realize the steelball center searching function
	// Use the manually picked Pset temporarily
	if(GetDataStorage()->GetNamedNode("ballCenters_image") == nullptr)
	{
		m_Controls.textBrowser->append("Auto-extraction failed, please pick the steelballs manually.");
		return;
	}

	auto ballCenters_image = GetDataStorage()->GetNamedObject<mitk::PointSet>("ballCenters_image");

	// Step 3: calculate the transform matrix from the image frame to patientRF
	// TODO: ballCenters_ballRF is from design value and should be stored in .json 
	double designValues[21]
	{
		21.71,	21, -37.37,
        43.52,	16.21, -37.37,
        61.76,	21.23, -37.37,
        60.1,	42.17, -37.37,
        22.54,	69.12, -37.37,
        39.18,	58.03, -37.37,
        59.35,	65.16, -37.37
	};

	auto ballCenters_ballRF = mitk::PointSet::New();
	for(int i{0}; i < 7; i++)
	{
		double tmpDouble[3]{ designValues[3 * (i)],designValues[3 * (i)+1],designValues[3 * (i)+2] };
		mitk::Point3D tmpPoint{ tmpDouble };
		ballCenters_ballRF->InsertPoint(tmpPoint);
	}

	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(ballCenters_ballRF);
	landmarkRegistrator->SetLandmarksTarget(ballCenters_image);
	landmarkRegistrator->ComputeLandMarkResult();
	double maxError = landmarkRegistrator->GetmaxLandmarkError();
	double avgError = landmarkRegistrator->GetavgLandmarkError();

	m_Controls.textBrowser->append("Max image registration error: " + QString::number(maxError));
	m_Controls.textBrowser->append("Average image registration error: " + QString::number(avgError));

	auto T_imageFrameToSteelballRF = landmarkRegistrator->GetResult();
	
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->Identity();
	tmpTrans->SetMatrix(T_steelballRFtoPatientRF);
	tmpTrans->Concatenate(T_imageFrameToSteelballRF);
	tmpTrans->Update();
	
	auto T_imageFrameToPatientRF = tmpTrans->GetMatrix();
	
	
	// Step 4: Establish the pipeline for testing purpose
	m_NavigatedImage = lancet::NavigationObject::New();

	m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);

	m_NavigatedImage->SetDataNode(GetDataStorage()->GetNamedNode("boneSurface"));
	m_NavigatedImage->SetT_Object2ReferenceFrame(T_imageFrameToPatientRF);
	m_ImageRegistrationMatrix->DeepCopy(T_imageFrameToPatientRF);
	
	auto tmpMatrix = mitk::AffineTransform3D::New();
	
	mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix, tmpMatrix.GetPointer());
	
	m_VegaToolStorage->GetToolByName("patientRF")->SetToolRegistrationMatrix(tmpMatrix);
	
	m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("patientRF")->GetToolRegistrationMatrix());
	
	m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("patientRF"));
	
	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();


	MITK_INFO << "Image registration result: " << tmpMatrix << endl;
}


void SpineDemo::on_pushButton_initCheckPoint_3d_clicked()
{
	auto patientRFindex = m_VegaToolStorage->GetToolIndexByName("patientRF");
	auto probeIndex = m_VegaToolStorage->GetToolIndexByName("probe");
	if (patientRFindex == -1 || probeIndex == -1)
	{
		m_Controls.textBrowser->append("There is no 'patientRF' or 'probe' in the toolStorage!");
		return;
	}

	mitk::NavigationData::Pointer nd_ndiToPatientRF = m_VegaSource->GetOutput(patientRFindex);
	mitk::NavigationData::Pointer nd_ndiToProbe = m_VegaSource->GetOutput(probeIndex);

	if (nd_ndiToPatientRF->IsDataValid() == 0 || nd_ndiToProbe->IsDataValid() == 0)
	{
		m_Controls.textBrowser->append("'patientRF' or 'probe' is not visible");
		return;
	}

	mitk::NavigationData::Pointer nd_patientRFtoProbe = GetNavigationDataInRef(nd_ndiToProbe, nd_ndiToPatientRF);

	vtkMatrix4x4* T_patientRFtoProbe = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_patientRFtoProbe->GetAffineTransform3D().GetPointer(), T_patientRFtoProbe);

	m_InitPatientCheckpoint[0] = T_patientRFtoProbe->GetElement(0, 3);
	m_InitPatientCheckpoint[1] = T_patientRFtoProbe->GetElement(1, 3);
	m_InitPatientCheckpoint[2] = T_patientRFtoProbe->GetElement(2, 3);

	m_Controls.textBrowser->append("Initial patient checkpoint: " + QString::number(m_InitPatientCheckpoint[0]) + "/"+
		QString::number(m_InitPatientCheckpoint[1]) + "/"+
		QString::number(m_InitPatientCheckpoint[2]) + "/");
}


mitk::NavigationData::Pointer SpineDemo::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}



