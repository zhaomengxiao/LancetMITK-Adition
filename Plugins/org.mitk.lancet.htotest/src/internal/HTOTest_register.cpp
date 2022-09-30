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
#include "HTOTest.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>
#include "mitkBoundingShapeCropper.h"
#include "mitkPointSet.h"
#include "surfaceregistraion.h"

#include "mitkSurface.h"
#include "mitkSurfaceToImageFilter.h"
#include "vtkOBBTree.h"


bool HTOTest::RegisterFemur()
{
	auto modelPointSetNode = GetDataStorage()->GetNamedNode("femur register source");
	auto bonePointSetNode = GetDataStorage()->GetNamedNode("femur register target");
	auto femurSurfaceNode = GetDataStorage()->GetNamedNode("femurSurface");
	auto femurSurfaceRegisterdNode = GetDataStorage()->GetNamedNode("femurSurface_registered");

	if(femurSurfaceRegisterdNode != nullptr)
	{
		GetDataStorage()->Remove(femurSurfaceRegisterdNode);
	}

	if(femurSurfaceNode == nullptr)
	{
		m_Controls.textBrowser->append("femurSurface is not ready");
		return false;
	}

	if(modelPointSetNode == nullptr || bonePointSetNode == nullptr)
	{
		m_Controls.textBrowser->append("femur register source points or target points are not ready");
		return false;
	}

	auto modelPointSet = dynamic_cast<mitk::PointSet*>(modelPointSetNode->GetData());
	auto bonePointSet = dynamic_cast<mitk::PointSet*>(bonePointSetNode->GetData());
	auto femurSurface = dynamic_cast<mitk::Surface*>(femurSurfaceNode->GetData());

	if(modelPointSet->GetSize() == 0 || bonePointSet->GetSize() == 0)
	{
		m_Controls.textBrowser->append("femur register source points or target points are not ready");
		return false;
	}

	if (modelPointSet->GetSize() != bonePointSet->GetSize())
	{
		m_Controls.textBrowser->append("femur register source points and target points do not match");
		return false;
	}

	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksTarget(bonePointSet);
	landmarkRegistrator->SetLandmarksSrc(modelPointSet);
	landmarkRegistrator->ComputeLandMarkResult();

	m_Controls.lineEdit_femurRegisterMaxError->setText(QString::number(landmarkRegistrator->GetmaxLandmarkError()));
	m_Controls.lineEdit_femurRegisterAvgError->setText(QString::number(landmarkRegistrator->GetavgLandmarkError()));


	// Apply the landmark registration result
	auto landmarkMatrix = landmarkRegistrator->GetResult();
	auto initialMatrix = femurSurface->GetGeometry()->GetVtkMatrix();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Identity();
	tmpTransform->SetMatrix(initialMatrix);
	tmpTransform->SetMatrix(landmarkMatrix);
	tmpTransform->Update();

	auto femurPolyData = femurSurface->GetVtkPolyData();

	vtkNew<vtkPolyData> femurPolyData_registered;
	femurPolyData_registered->DeepCopy(femurPolyData);
	
	vtkNew<vtkTransformFilter> tmpFilter;
	tmpFilter->SetInputData(femurPolyData_registered);
	tmpFilter->SetTransform(tmpTransform);
	tmpFilter->Update();


	// Add this registered surface to a dataNode
	auto tmpNode = mitk::DataNode::New();
	auto tmpSurface = mitk::Surface::New();
	tmpSurface->SetVtkPolyData(tmpFilter->GetPolyDataOutput());
	tmpNode->SetData(tmpSurface);
	tmpNode->SetName("femurSurface_registered");

	return true;

}





