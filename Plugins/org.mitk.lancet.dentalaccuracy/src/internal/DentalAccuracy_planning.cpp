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
#include <vtkKdTree.h>
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


void DentalAccuracy::ProjectImplantOntoPanorama()
{
	auto panorama_node = GetDataStorage()->GetNamedNode("Panorama");
	auto probeSurface_node = GetDataStorage()->GetNamedNode("Probe surface");
	auto implant_node = GetDataStorage()->GetNamedNode("implant");

	if(panorama_node == nullptr || probeSurface_node == nullptr)
	{
		m_Controls.textBrowser->append("Panorama hasn't been generated!");
		return;
	}

	if(implant_node == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing!");
		return;
	}

	// Get the implant tip point
	auto implantMatrix = implant_node->GetData()->GetGeometry()->GetVtkMatrix();
	auto maxBound_init = implant_node->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implant_node->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	mitk::Point3D implantTail_init;
	implantTail_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	implantTail_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	implantTail_init[2] = minBound_init.GetElement(2);

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->InsertPoint(implantTail_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantTipPoint = tmpPset->GetPoint(0);
	auto implantTailPoint = tmpPset->GetPoint(1);

	double tip[3]{ implantTipPoint[0], implantTipPoint[1] ,implantTipPoint[2] };
	double tail[3]{ implantTailPoint[0], implantTailPoint[1] ,implantTailPoint[2] };

	auto polydata = dynamic_cast<mitk::Surface*>(probeSurface_node->GetData())->GetVtkPolyData();

	// Build a kd-tree
	vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
	// kdTree->SetDataSet(polydata);
	// kdTree->BuildLocator();
	kdTree->BuildLocatorFromPoints(polydata->GetPoints());

	// Find the 1 closest points to tip
	// vtkIdType k = 1;
	vtkNew<vtkIdList> result_tip;
	vtkNew<vtkIdList> result_tail;

	kdTree->FindClosestNPoints(1, tip, result_tip);
	kdTree->FindClosestNPoints(1, tail, result_tail);

	vtkIdType point_ind_tip = result_tip->GetId(0);
	vtkIdType point_ind_tail = result_tail->GetId(0);

	double p_tip[3];
	polydata->GetPoints()->GetPoint(point_ind_tip, p_tip);

	double p_tail[3];
	polydata->GetPoints()->GetPoint(point_ind_tail, p_tail);
	
	auto projectPset = mitk::PointSet::New();
	mitk::Point3D p1;
	mitk::Point3D p2;

	for(int i{0}; i < 3; i++)
	{
		p1[i] = p_tip[i];
		p2[i] = p_tail[i];
	}

	projectPset->InsertPoint(p1);
	projectPset->InsertPoint(p2);

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(projectPset);
	tmpNode->SetName("implant_project");

	if(GetDataStorage()->GetNamedNode("implant_project") != nullptr)
	{
		GetDataStorage()->GetNamedNode("implant_project")->SetData(projectPset);
	}else
	{
		GetDataStorage()->Add(tmpNode);
	}

	// Test project the points onto the panorama (with known size: 251x620x60 )
	auto panoramaDims = dynamic_cast<mitk::Image*>(panorama_node->GetData())->GetDimensions();
	auto panoramaSpacings = dynamic_cast<mitk::Image*>(panorama_node->GetData())->GetVtkImageData()->GetSpacing();

	auto projectedPset = mitk::PointSet::New();

	int y_tip = static_cast<double>(point_ind_tip / panoramaDims[0]) * panoramaSpacings[1];
	int x_tip = static_cast<double>(point_ind_tip % panoramaDims[0]) * panoramaSpacings[0];

	mitk::Point3D tip_projection;
	tip_projection[0] = x_tip;
	tip_projection[1] = y_tip;
	tip_projection[2] = panoramaDims[2] * panoramaSpacings[2] / 2;

	int y_tail = static_cast<double>(point_ind_tail / panoramaDims[0])* panoramaSpacings[1];
	int x_tail = static_cast<double>(point_ind_tail % panoramaDims[0])* panoramaSpacings[0];

	mitk::Point3D tail_projection;
	tail_projection[0] = x_tail;
	tail_projection[1] = y_tail;
	tail_projection[2] = tip_projection[2];

	projectedPset->InsertPoint(tip_projection);
	projectedPset->InsertPoint(tail_projection);

	// Apply the -90 rotation along +z-axis because the panorama is rotated after generation
	auto rotateTrans = vtkTransform::New();
	rotateTrans->PostMultiply();
	rotateTrans->Identity();
	rotateTrans->RotateZ(-90);
	rotateTrans->Update();

	projectedPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(rotateTrans->GetMatrix());

	// Align the "implant_panorama" along the newly generated projectedPset
	// Copy the "implant" node data, the vtkPolyData is at the origin
	auto tmpPoly = vtkPolyData::New();
	tmpPoly->DeepCopy(dynamic_cast<mitk::Surface*>(implant_node->GetData())->GetVtkPolyData());
	auto implantProjection_surface = mitk::Surface::New();
	implantProjection_surface->SetVtkPolyData(tmpPoly);

	// Calculate the required rotation
	Eigen::Vector3d srcAxis{0,0,1};
	Eigen::Vector3d dstAxis{ projectedPset->GetPoint(0)[0] - projectedPset->GetPoint(1)[0],
		projectedPset->GetPoint(0)[1] - projectedPset->GetPoint(1)[1],
		projectedPset->GetPoint(0)[2] - projectedPset->GetPoint(1)[2]
	};
	dstAxis.normalize();

	Eigen::Vector3d rotAxis = srcAxis.cross(dstAxis);
	
	auto trans_implant = vtkTransform::New();
	trans_implant->PostMultiply();
	trans_implant->Identity();
	trans_implant->Translate(-boundCenter_init[0], -boundCenter_init[1], -boundCenter_init[2]);
	trans_implant->RotateWXYZ(90, rotAxis[0],rotAxis[1],rotAxis[2]);
	trans_implant->Translate(projectedPset->GetPoint(0)[0], projectedPset->GetPoint(0)[1], projectedPset->GetPoint(0)[2]);
	trans_implant->Update();

	implantProjection_surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans_implant->GetMatrix());


	if (GetDataStorage()->GetNamedNode("implantOnPanorama") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("implantOnPanorama"));
	}

	auto testNode = mitk::DataNode::New();
	testNode->SetName("implantOnPanorama");
	testNode->SetData(implantProjection_surface);
	testNode->SetColor(0, 1, 0);
	
	GetDataStorage()->Add(testNode);


}
