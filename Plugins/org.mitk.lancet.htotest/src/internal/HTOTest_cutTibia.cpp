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
#include <vtkFeatureEdges.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkStripper.h>
#include <vtkTriangleFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>
#include "mitkBoundingShapeCropper.h"
#include <mitkRemeshing.h>
#include <vtkPolyDataPlaneClipper.h>
#include <vtkFillHolesFilter.h>

#include "mitkSurface.h"
#include "mitkSurfaceToImageFilter.h"
#include "mitkVtkInterpolationProperty.h"
#include "vtkOBBTree.h"
#include "vtkDelaunay2D.h"

bool HTOTest::CreateOneCutPlane()
{
	// m_Controls.textBrowser->append("Creating cut plane(s)");
	auto dataNode_cutPlane = GetDataStorage()->GetNamedNode("tibia cut plane");
	auto dataNode_tibiaSurface = GetDataStorage()->GetNamedNode("tibiaSurface");

	if (dataNode_tibiaSurface == nullptr)
	{
		m_Controls.textBrowser->append("tibiaSurface is missing");
		return false;
	}

	auto tibiaSurface = dynamic_cast<mitk::Surface*>(dataNode_tibiaSurface->GetData());

	// Get the OBB of tibia surface

	auto initialTibiaPolyData = tibiaSurface->GetVtkPolyData();
	vtkNew<vtkTransform> tibiaTransform;
	tibiaTransform->SetMatrix(tibiaSurface->GetGeometry()->GetVtkMatrix());
	vtkNew<vtkTransformFilter> tmpFilter;
	tmpFilter->SetTransform(tibiaTransform);
	tmpFilter->SetInputData(initialTibiaPolyData);
	tmpFilter->Update();

	vtkNew<vtkPolyData> tibiaPolyData;
	tibiaPolyData->DeepCopy(tmpFilter->GetPolyDataOutput());


	vtkNew<vtkOBBTree> obbTree;
	obbTree->SetDataSet(tibiaPolyData);
	obbTree->SetMaxLevel(2);
	obbTree->BuildLocator();

	double corner[3] = { 0.0, 0.0, 0.0 };
	double max[3] = { 0.0, 0.0, 0.0 };
	double mid[3] = { 0.0, 0.0, 0.0 };
	double min[3] = { 0.0, 0.0, 0.0 };
	double size[3] = { 0.0, 0.0, 0.0 };

	obbTree->ComputeOBB(tibiaPolyData, corner, max, mid, min, size);

	vtkNew<vtkPolyData> obbPolydata;
	obbTree->GenerateRepresentation(0, obbPolydata);

	// auto tmpNode = mitk::DataNode::New();
	// auto tmpSurface = mitk::Surface::New();
	// tmpSurface->SetVtkPolyData(obbPolydata);
	// tmpNode->SetData(tmpSurface);
	// tmpNode->SetName("OBB");
	// GetDataStorage()->Add(tmpNode);

	auto cutPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();

	cutPlaneSource->SetOrigin(0, 0, 0);

	cutPlaneSource->SetPoint1(0, 70, 0);
	cutPlaneSource->SetPoint2(70, 0, 0);

	cutPlaneSource->SetNormal(max);
	
	// Determine the optimal plane location

	vtkNew<vtkPolyData> largerSubpart_0;
	vtkNew<vtkPolyData> smallerSubpart_0;
	double origin_0[3]
	{
		corner[0] + 0.5 * (1.7 * max[0] + mid[0] + min[0]),
		corner[1] + 0.5 * (1.7 * max[1] + mid[1] + min[1]),
		corner[2] + 0.5 * (1.7 * max[2] + mid[2] + min[2])
	};

	vtkNew<vtkPolyData> largerSubpart_1;
	vtkNew<vtkPolyData> smallerSubpart_1;
	double origin_1[3]
	{
		corner[0] + 0.5 * (0.3 * max[0] + mid[0] + min[0]),
		corner[1] + 0.5 * (0.3 * max[1] + mid[1] + min[1]),
		corner[2] + 0.5 * (0.3 * max[2] + mid[2] + min[2])
	};

	CutPolyDataWithPlane(tibiaPolyData, largerSubpart_0, smallerSubpart_0, origin_0, max);
	CutPolyDataWithPlane(tibiaPolyData, largerSubpart_1, smallerSubpart_1, origin_1, max);

	if(smallerSubpart_0->GetNumberOfCells() >= smallerSubpart_1->GetNumberOfCells())
	{
		cutPlaneSource->SetCenter(origin_0);
	}else
	{
		cutPlaneSource->SetCenter(origin_1);
	}


	// cutPlaneSource->SetCenter(corner[0]+0.5*(max[0]+mid[0]+min[0]),
	// 	corner[1] + 0.5 * (max[1] + mid[1] + min[1]),
	// 	corner[2] + 0.5 * (max[2] + mid[2] + min[2]));
	cutPlaneSource->Update();

	auto cutSurface = mitk::Surface::New();
	cutSurface->SetVtkPolyData(cutPlaneSource->GetOutput());

	auto planeNode = mitk::DataNode::New();
	planeNode->SetData(cutSurface);
	planeNode->SetName("tibia cut plane");
	GetDataStorage()->Add(planeNode);
	return true;


	
}


bool HTOTest::CreateCutPlane()
{
	if(m_Controls.radioButton_oneCut->isChecked())
	{
		if(CreateOneCutPlane())
		{
			auto cutSurfaceNode = GetDataStorage()->GetNamedNode("tibia cut plane");
			cutSurfaceNode->SetColor(0,1,0);
			cutSurfaceNode->SetOpacity(0.5);
			return true;
		}
	}

	if(m_Controls.radioButton_twoCuts->isChecked())
	{
		if (CreateOneCutPlane())
		{
			auto cutSurfaceNode = GetDataStorage()->GetNamedNode("tibia cut plane");
			cutSurfaceNode->SetColor(0, 1, 0);
			cutSurfaceNode->SetOpacity(0.5);
			cutSurfaceNode->SetName("1st cut plane");

			CreateOneCutPlane();
			auto cutSurfaceNode2 = GetDataStorage()->GetNamedNode("tibia cut plane");
			cutSurfaceNode2->SetColor(1, 0, 0);
			cutSurfaceNode2->SetOpacity(0.5);
			cutSurfaceNode2->SetName("2nd cut plane");

			return true;
		}

	}

	return false;
}


bool HTOTest::CutPolyDataWithPlane(vtkSmartPointer<vtkPolyData> dataToCut,
	vtkSmartPointer<vtkPolyData> largerSubPart,
	vtkSmartPointer<vtkPolyData> smallerSubPart,
	double planeOrigin[3], double planeNormal[3])
{
	vtkNew<vtkPlane> implicitPlane;
	implicitPlane->SetNormal(planeNormal);
	implicitPlane->SetOrigin(planeOrigin);

	// vtkNew<vtkPolyDataPlaneClipper> planeClipper;
	// planeClipper->SetPlane(implicitPlane);
	// planeClipper->SetInputData(dataToCut);
	// planeClipper->CappingOn();
	// planeClipper->Update();
	//
	// auto tibiaPart_0 = planeClipper->GetOutput();
	// auto tibiaPart_0_cap = planeClipper->GetCap();
	// int cellNum_0 = tibiaPart_0->GetNumberOfCells();
	//
	// vtkSmartPointer<vtkAppendPolyData> appendFilter =
	// 	vtkSmartPointer<vtkAppendPolyData>::New();
	// vtkSmartPointer<vtkCleanPolyData> cleanFilter =
	// 	vtkSmartPointer<vtkCleanPolyData>::New();
	//
	// appendFilter->AddInputData(tibiaPart_0);
	// appendFilter->AddInputData(tibiaPart_0_cap);
	// appendFilter->Update();
	//
	// cleanFilter->SetInputData(appendFilter->GetOutput());
	// cleanFilter->Update();
	//
	// vtkNew<vtkPolyData> tibia_0;
	// tibia_0->DeepCopy(cleanFilter->GetOutput());
	//
	// vtkNew<vtkPlane> implicitPlane1;
	// double planeNormal1[3]{ -planeNormal[0], -planeNormal[1],-planeNormal[2] };
	// implicitPlane1->SetNormal(planeNormal1);
	// implicitPlane1->SetOrigin(planeOrigin);
	//
	// vtkNew<vtkPolyDataPlaneClipper> planeClipper1;
	// planeClipper1->SetPlane(implicitPlane1);
	// planeClipper1->SetInputData(dataToCut);
	// planeClipper1->CappingOn();
	// planeClipper1->Update();
	//
	// auto tibiaPart_1 = planeClipper1->GetOutput();
	// auto tibiaPart_1_cap = planeClipper1->GetCap();
	// int cellNum_1 = tibiaPart_1->GetNumberOfCells();
	//
	// vtkSmartPointer<vtkAppendPolyData> appendFilter1 =
	// 	vtkSmartPointer<vtkAppendPolyData>::New();
	// vtkSmartPointer<vtkCleanPolyData> cleanFilter1 =
	// 	vtkSmartPointer<vtkCleanPolyData>::New();
	//
	// appendFilter1->AddInputData(tibiaPart_1);
	// appendFilter1->AddInputData(tibiaPart_1_cap);
	// appendFilter1->Update();
	//
	// cleanFilter1->SetInputData(appendFilter1->GetOutput());
	// cleanFilter1->Update();
	//
	// vtkNew<vtkPolyData> tibia_1;
	// tibia_1->DeepCopy(cleanFilter1->GetOutput());

	 vtkNew<vtkClipPolyData> clipper;
	 clipper->SetInputData(dataToCut);
	 clipper->GenerateClippedOutputOn();
	 clipper->SetClipFunction(implicitPlane);
	
	 clipper->Update();
	 vtkNew<vtkPolyData> tibiaPart_0;
	 tibiaPart_0->DeepCopy(clipper->GetClippedOutput());
	 int cellNum_0 = tibiaPart_0->GetNumberOfCells();
	
	 clipper->Update();
	 vtkNew<vtkPolyData> tibiaPart_1;
	 tibiaPart_1->DeepCopy(clipper->GetOutput());
	 int cellNum_1 = tibiaPart_1->GetNumberOfCells();


	// Create the capping 
	 // vtkNew<vtkFeatureEdges> boundaryEdges;
	 // boundaryEdges->SetInputData(tibiaPart_0);
	 // boundaryEdges->BoundaryEdgesOn();
	 // boundaryEdges->FeatureEdgesOff();
	 // boundaryEdges->NonManifoldEdgesOff();
	 // boundaryEdges->ManifoldEdgesOff();
	 //
	 // vtkNew<vtkStripper> boundaryStrips;
	 // boundaryStrips->SetInputConnection(boundaryEdges->GetOutputPort());
	 // boundaryStrips->Update();
	 //
	 // // Change the polylines into polygons
	 // vtkNew<vtkPolyData> boundaryPoly;
	 // boundaryPoly->SetPoints(boundaryStrips->GetOutput()->GetPoints());
	 // boundaryPoly->SetPolys(boundaryStrips->GetOutput()->GetLines());
	 //
	 //
	 // vtkNew<vtkPolyData> boundaryPoly_copy;
	 // boundaryPoly_copy->DeepCopy(boundaryPoly);
	 //
	 // // Append the capping
	 // vtkSmartPointer<vtkAppendPolyData> appendFilter =
	 // 	vtkSmartPointer<vtkAppendPolyData>::New();
	 // vtkSmartPointer<vtkCleanPolyData> cleanFilter =
	 // 	vtkSmartPointer<vtkCleanPolyData>::New();
	 //
	 // appendFilter->AddInputData(tibiaPart_0);
	 // appendFilter->AddInputData(boundaryPoly);
	 // appendFilter->Update();
	 //
	 // cleanFilter->SetInputData(appendFilter->GetOutput());
	 // cleanFilter->Update();
	 //
	 // vtkNew<vtkPolyData> tibia_0;
	 // tibia_0->DeepCopy(cleanFilter->GetOutput());
	 //
	 //
	 // vtkSmartPointer<vtkAppendPolyData> appendFilter2 =
	 // 	vtkSmartPointer<vtkAppendPolyData>::New();
	 // vtkSmartPointer<vtkCleanPolyData> cleanFilter2 =
	 // 	vtkSmartPointer<vtkCleanPolyData>::New();
	 //
	 // appendFilter2->AddInputData(tibiaPart_1);
	 // appendFilter2->AddInputData(boundaryPoly_copy);
	 // appendFilter2->Update();
	 //
	 // cleanFilter2->SetInputData(appendFilter2->GetOutput());
	 // cleanFilter2->Update();
	 //
	 // vtkNew<vtkPolyData> tibia_1;
	 // tibia_1->DeepCopy(cleanFilter2->GetOutput());


	// Fill holes
	vtkNew<vtkFillHolesFilter> holeFiller0;
	holeFiller0->SetInputData(tibiaPart_0);
	holeFiller0->SetHoleSize(500);
	holeFiller0->Update();
	vtkNew<vtkPolyData> tibia_filled_0;
	tibia_filled_0->DeepCopy(holeFiller0->GetOutput());
	
	
	vtkNew<vtkFillHolesFilter> holeFiller1;
	holeFiller1->SetInputData(tibiaPart_1);
	holeFiller1->SetHoleSize(500);
	holeFiller1->Update();
	vtkNew<vtkPolyData> tibia_filled_1;
	tibia_filled_1->DeepCopy(holeFiller1->GetOutput());
	
	if(cellNum_1 >= cellNum_0)
	{
		largerSubPart->DeepCopy(tibia_filled_0);
		smallerSubPart->DeepCopy(tibia_filled_1);
	}else
	{
		largerSubPart->DeepCopy(tibia_filled_0);
		smallerSubPart->DeepCopy(tibia_filled_1);
	}

	return true;
}


bool HTOTest::CutTibiaWithOnePlane()
{
	m_Controls.textBrowser->append("Cutting with one plane");
	auto cutPlaneNode = GetDataStorage()->GetNamedNode("tibia cut plane");
	auto tibiaSurfaceNode = GetDataStorage()->GetNamedNode("tibiaSurface");
	if(cutPlaneNode == nullptr)
	{
		m_Controls.textBrowser->append("'tibia cut plane' is not ready");
		return false;
	}
	if (tibiaSurfaceNode == nullptr)
	{
		m_Controls.textBrowser->append("'tibiaSurface' is not ready");
		return false;
	}

	auto mitkCutSurface = dynamic_cast<mitk::Surface*>(cutPlaneNode->GetData());

	auto tibiaMitkSurface = dynamic_cast<mitk::Surface*>(tibiaSurfaceNode->GetData());
	
	auto tmpVtkSurface_initial = mitkCutSurface->GetVtkPolyData();

	auto tibiaVtkSurface_initial = tibiaMitkSurface->GetVtkPolyData();

	// transform tmpVtkSurface
	vtkNew<vtkTransform> cutPlaneTransform;
	cutPlaneTransform->SetMatrix(mitkCutSurface->GetGeometry()->GetVtkMatrix());
	vtkNew<vtkTransformFilter> cutPlaneTransformFilter;
	cutPlaneTransformFilter->SetTransform(cutPlaneTransform);
	cutPlaneTransformFilter->SetInputData(tmpVtkSurface_initial);
	cutPlaneTransformFilter->Update();

	vtkNew<vtkPolyData> tmpVtkSurface;
	tmpVtkSurface->DeepCopy(cutPlaneTransformFilter->GetPolyDataOutput());

	// transform tibiaVtkSurface
	vtkNew<vtkTransform> tibiaTransform;
	tibiaTransform->SetMatrix(tibiaMitkSurface->GetGeometry()->GetVtkMatrix());
	vtkNew<vtkTransformFilter> tibiaTransformFilter;
	tibiaTransformFilter->SetTransform(tibiaTransform);
	tibiaTransformFilter->SetInputData(tibiaVtkSurface_initial);
	tibiaTransformFilter->Update();


	vtkNew<vtkPolyData> tibiaVtkSurface;
	tibiaVtkSurface->DeepCopy(tibiaTransformFilter->GetPolyDataOutput());
	
	double surfaceNormal[3];
	double cutPlaneCenter[3];

	GetPlaneProperty(tmpVtkSurface, surfaceNormal, cutPlaneCenter);
	

	vtkNew<vtkPolyData> proximalTibiaSurface;
	vtkNew<vtkPolyData> distalTibiaSurface;

	CutPolyDataWithPlane(tibiaVtkSurface, distalTibiaSurface, proximalTibiaSurface, cutPlaneCenter, surfaceNormal);

	// vtkSmartPointer<vtkCleanPolyData> proximalCleanFilter =
	// 	vtkSmartPointer<vtkCleanPolyData>::New();
	// vtkSmartPointer<vtkCleanPolyData> distalCleanFilter =
	// 	vtkSmartPointer<vtkCleanPolyData>::New();
	// proximalCleanFilter->SetInputData(proximalTibiaSurface);
	// proximalCleanFilter->Update();
	//
	// distalCleanFilter->SetInputData(distalTibiaSurface);
	// distalCleanFilter->Update();

	auto mitkProximalSurface = mitk::Surface::New();
	auto mitkDistalSurface = mitk::Surface::New();
	mitkProximalSurface->SetVtkPolyData(proximalTibiaSurface);
	mitkDistalSurface->SetVtkPolyData(distalTibiaSurface);

	//// for visualization purpose: mitkProximalSurface and mitkProximalSurface have rough surfaces
	auto proximal_remehsed = mitk::Remeshing::Decimate(mitkProximalSurface,1,true,true);
	auto distal_remehsed = mitk::Remeshing::Decimate(mitkDistalSurface, 1, true, true);

	vtkNew<vtkFillHolesFilter> holeFiller0;
	holeFiller0->SetInputData(proximal_remehsed->GetVtkPolyData());
	holeFiller0->SetHoleSize(500);
	holeFiller0->Update();
	vtkNew<vtkPolyData> proximalTibia;
	proximalTibia->DeepCopy(holeFiller0->GetOutput());
	auto proximalSurface = mitk::Surface::New();
	proximalSurface->SetVtkPolyData(proximalTibia);

	vtkNew<vtkFillHolesFilter> holeFiller1;
	holeFiller1->SetInputData(distal_remehsed->GetVtkPolyData());
	holeFiller1->SetHoleSize(500);
	holeFiller1->Update();
	vtkNew<vtkPolyData> distalTibia;
	distalTibia->DeepCopy(holeFiller1->GetOutput());
	auto distalSurface = mitk::Surface::New();
	distalSurface->SetVtkPolyData(distalTibia);


	auto tmpNode0 = mitk::DataNode::New();
	auto tmpNode1 = mitk::DataNode::New();
	tmpNode0->SetData(proximalSurface);
	tmpNode0->SetName("proximal tibiaSurface");

	mitk::VtkInterpolationProperty::Pointer interpolationProp0;
	tmpNode0->GetProperty(interpolationProp0, "material.interpolation");
	interpolationProp0->SetInterpolationToFlat();
	tmpNode0->SetProperty("material.interpolation", interpolationProp0);


	tmpNode1->SetData(distalSurface);
	tmpNode1->SetName("distal tibiaSurface");

	mitk::VtkInterpolationProperty::Pointer interpolationProp1;
	tmpNode1->GetProperty(interpolationProp1, "material.interpolation");
	interpolationProp1->SetInterpolationToFlat();
	tmpNode1->SetProperty("material.interpolation", interpolationProp1);

	GetDataStorage()->Add(tmpNode0);
	GetDataStorage()->Add(tmpNode1);

	return true;
}

bool HTOTest::CutTibiaWithTwoPlanes()
{
	auto cutplane_0 = GetDataStorage()->GetNamedNode("1st cut plane");
	auto cutplane_1 = GetDataStorage()->GetNamedNode("2nd cut plane");
	auto tibiaNode = GetDataStorage()->GetNamedNode("tibiaSurface");

	if(cutplane_0 == nullptr || cutplane_1 == nullptr)
	{
		m_Controls.textBrowser->append("'1st cut plane' or '2nd cut plane' is not ready");
		return false;
	}
	if (tibiaNode == nullptr)
	{
		m_Controls.textBrowser->append("'tibiaSurface' is not ready");
		return false;
	}

	auto mitkCutPlane_0 = dynamic_cast<mitk::Surface*>(cutplane_0->GetData());
	auto mitkCutPlane_1 = dynamic_cast<mitk::Surface*>(cutplane_1->GetData());
	auto mitkTibia = dynamic_cast<mitk::Surface*>(tibiaNode->GetData());

	vtkNew<vtkPolyData> vtkCutPlane_0;
	vtkNew<vtkPolyData> vtkCutPlane_1;
	vtkNew<vtkPolyData> vtkTibia;
	
	// Append the geometry offset matrices
	vtkNew<vtkTransform> cutPlaneTransform_0;
	cutPlaneTransform_0->SetMatrix(mitkCutPlane_0->GetGeometry()->GetVtkMatrix());

	vtkNew<vtkTransform> cutPlaneTransform_1;
	cutPlaneTransform_1->SetMatrix(mitkCutPlane_1->GetGeometry()->GetVtkMatrix());

	vtkNew<vtkTransform> tibiaTransform;
	tibiaTransform->SetMatrix(mitkTibia->GetGeometry()->GetVtkMatrix());

	vtkNew<vtkTransformFilter> cutPlaneTransformFilter_0;
	cutPlaneTransformFilter_0->SetTransform(cutPlaneTransform_0);
	cutPlaneTransformFilter_0->SetInputData(mitkCutPlane_0->GetVtkPolyData());
	cutPlaneTransformFilter_0->Update();
	vtkCutPlane_0->DeepCopy(cutPlaneTransformFilter_0->GetPolyDataOutput());

	vtkNew<vtkTransformFilter> cutPlaneTransformFilter_1;
	cutPlaneTransformFilter_1->SetTransform(cutPlaneTransform_1);
	cutPlaneTransformFilter_1->SetInputData(mitkCutPlane_1->GetVtkPolyData());
	cutPlaneTransformFilter_1->Update();
	vtkCutPlane_1->DeepCopy(cutPlaneTransformFilter_1->GetPolyDataOutput());

	vtkNew<vtkTransformFilter> tibiaTransformFilter;
	tibiaTransformFilter->SetTransform(tibiaTransform);
	tibiaTransformFilter->SetInputData(mitkTibia->GetVtkPolyData());
	tibiaTransformFilter->Update();
	vtkTibia->DeepCopy(tibiaTransformFilter->GetPolyDataOutput());

	double cutPlaneCenter_0[3];
	double cutPlaneNormal_0[3];
	double cutPlaneCenter_1[3];
	double cutPlaneNormal_1[3];

	GetPlaneProperty(vtkCutPlane_0, cutPlaneNormal_0, cutPlaneCenter_0);
	GetPlaneProperty(vtkCutPlane_1, cutPlaneNormal_1, cutPlaneCenter_1);

	vtkNew<vtkPolyData> largetPart;
	vtkNew<vtkPolyData> tmpMiddlePart;
	vtkNew<vtkPolyData> middlePart;
	vtkNew<vtkPolyData> smallPart;


	// Cut and merge
	CutPolyDataWithPlane(vtkTibia, largetPart, tmpMiddlePart, cutPlaneCenter_0, cutPlaneNormal_0);
	CutPolyDataWithPlane(tmpMiddlePart, middlePart, smallPart, cutPlaneCenter_1, cutPlaneNormal_1);

	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkCleanPolyData> cleanFilter =
		vtkSmartPointer<vtkCleanPolyData>::New();

	appendFilter->AddInputData(largetPart);
	appendFilter->AddInputData(smallPart);
	appendFilter->Update();

	cleanFilter->SetInputData(appendFilter->GetOutput());
	cleanFilter->Update();

	auto proximalSurface = mitk::Surface::New();
	auto distalSurface = mitk::Surface::New();

	proximalSurface->SetVtkPolyData(middlePart);
	distalSurface->SetVtkPolyData(cleanFilter->GetOutput());

	//// for visualization purpose: mitkProximalSurface and mitkProximalSurface have rough surfaces
	auto proximal_remehsed = mitk::Remeshing::Decimate(proximalSurface, 1, true, true);
	auto distal_remehsed = mitk::Remeshing::Decimate(distalSurface, 1, true, true);

	// // Fill holes
	// vtkNew<vtkFillHolesFilter> holeFiller0;
	// holeFiller0->SetInputData(proximal_remehsed->GetVtkPolyData());
	// holeFiller0->SetHoleSize(20);
	// holeFiller0->Update();
	// vtkNew<vtkPolyData> proximalTibia_remeshed_filled_vtk;
	// proximalTibia_remeshed_filled_vtk->DeepCopy(holeFiller0->GetOutput());
	// proximalSurface->SetVtkPolyData(proximalTibia_remeshed_filled_vtk);
	//
	// vtkNew<vtkFillHolesFilter> holeFiller1;
	// holeFiller1->SetInputData(distal_remehsed->GetVtkPolyData());
	// holeFiller1->SetHoleSize(20);
	// holeFiller1->Update();
	// vtkNew<vtkPolyData> distalTibia_remeshed_filled_vtk;
	// distalTibia_remeshed_filled_vtk->DeepCopy(holeFiller1->GetOutput());
	// distalSurface->SetVtkPolyData(distalTibia_remeshed_filled_vtk);

	// Save into nodes
	auto proximalNode = mitk::DataNode::New();
	auto distalNode = mitk::DataNode::New();

	proximalNode->SetName("proximal tibiaSurface");
	proximalNode->SetData(proximalSurface);
	//proximalNode->SetData(proximal_remehsed);
	distalNode->SetName("distal tibiaSurface");
	distalNode->SetData(distalSurface);
	//distalNode->SetData(distal_remehsed);


	GetDataStorage()->Add(distalNode);
	GetDataStorage()->Add(proximalNode);

	return true;
}


bool HTOTest::CutTibiaSurface()
{
	if (m_Controls.radioButton_oneCut->isChecked())
	{
		if(CutTibiaWithOnePlane())
		{
			return true;
		}
	}

	if (m_Controls.radioButton_twoCuts->isChecked())
	{
		if(CutTibiaWithTwoPlanes())
		{
			return true;
		}

	}

	return false;
}

bool HTOTest::CutTibiaImage()
{
	auto proximalNode = GetDataStorage()->GetNamedNode("proximal tibiaSurface");
	auto distalNode = GetDataStorage()->GetNamedNode("distal tibiaSurface");
	auto imageNode = GetDataStorage()->GetNamedNode("tibiaImage");

	if(proximalNode== nullptr || distalNode==nullptr)
	{
		m_Controls.textBrowser->append("'proximal tibiaSurface' or 'distal tibiaSurface' is missing");
		return false;
	}

	if(imageNode ==nullptr)
	{
		m_Controls.textBrowser->append("'tibiaImage' is missing");
		return false;
	}

	auto mitkProximalSurface = dynamic_cast<mitk::Surface*>(proximalNode->GetData());
	auto proximalBounds = mitkProximalSurface->GetGeometry()->GetBounds();
	auto proximalOrigin = mitkProximalSurface->GetGeometry()->GetOrigin();

	auto mitkDistalSurface = dynamic_cast<mitk::Surface*>(distalNode->GetData());
	auto distalBounds = mitkDistalSurface->GetGeometry()->GetBounds();
	auto distalOrigin = mitkDistalSurface->GetGeometry()->GetOrigin();

	auto image = dynamic_cast<mitk::Image*>(imageNode->GetData());

	// Apply the geometric offset matrices
	vtkNew<vtkPolyData> vtkDistal;
	vtkNew<vtkPolyData> vtkProximal;

	vtkNew<vtkTransform> distalTransform;
	distalTransform->SetMatrix(mitkDistalSurface->GetGeometry()->GetVtkMatrix());

	vtkNew<vtkTransform> proximalTransform;
	proximalTransform->SetMatrix(mitkProximalSurface->GetGeometry()->GetVtkMatrix());

	vtkNew<vtkTransformFilter> distalTransformFilter;
	distalTransformFilter->SetTransform(distalTransform);
	distalTransformFilter->SetInputData(mitkDistalSurface->GetVtkPolyData());
	distalTransformFilter->Update();
	vtkDistal->DeepCopy(distalTransformFilter->GetPolyDataOutput());

	vtkNew<vtkTransformFilter> proximalTransformFilter;
	proximalTransformFilter->SetTransform(proximalTransform);
	proximalTransformFilter->SetInputData(mitkProximalSurface->GetVtkPolyData());
	proximalTransformFilter->Update();
	vtkProximal->DeepCopy(proximalTransformFilter->GetPolyDataOutput());

	auto imageClone = image->Clone();

	auto proximalImage = mitk::Image::New();
	auto distalImage = mitk::Image::New();

	auto surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	//surfaceToImageFilter->SetMakeOutputBinary(false);
	surfaceToImageFilter->SetBackgroundValue(0);
	//surfaceToImageFilter->SetUShortBinaryPixelType(false);
	surfaceToImageFilter->SetImage(imageClone);
	surfaceToImageFilter->SetInput(mitkProximalSurface);
	

	surfaceToImageFilter->Update();
	proximalImage = surfaceToImageFilter->GetOutput()->Clone();

	// Cut the proximal image
	auto proximalBoundingBox = mitk::GeometryData::New();
	proximalBoundingBox->SetGeometry(mitkProximalSurface->GetGeometry());

	auto cutter = mitk::BoundingShapeCropper::New();
	cutter->SetGeometry(proximalBoundingBox);
	cutter->SetUseWholeInputRegion(false);
	cutter->SetInput(proximalImage);
	cutter->Update();

	surfaceToImageFilter->SetInput(mitkDistalSurface);
	surfaceToImageFilter->Update();
	distalImage = surfaceToImageFilter->GetOutput()->Clone();

	// Cut the distal image
	auto distalBoundingBox = mitk::GeometryData::New();
	distalBoundingBox->SetGeometry(mitkDistalSurface->GetGeometry());

	auto cutterDistal = mitk::BoundingShapeCropper::New();
	cutterDistal->SetGeometry(distalBoundingBox);
	cutterDistal->SetUseWholeInputRegion(false);
	cutterDistal->SetInput(distalImage);
	cutterDistal->Update();


	auto tmpNode0 = mitk::DataNode::New();
	tmpNode0->SetName("distal tibiaImage");
	tmpNode0->SetData(cutterDistal->GetOutput());
	GetDataStorage()->Add(tmpNode0,distalNode);

	auto tmpNode1 = mitk::DataNode::New();
	tmpNode1->SetName("proximal tibiaImage");
	tmpNode1->SetData(cutter->GetOutput());
	GetDataStorage()->Add(tmpNode1, proximalNode);

	return true;
}


bool HTOTest::CutTibia()
{
	if(CutTibiaSurface() && CutTibiaImage())
	{
		return true;
	}

	return false;

}


bool HTOTest::GetPlaneProperty(vtkSmartPointer<vtkPolyData> plane, double normal[3], double center[3])
{
	auto tmpCenter = plane->GetCenter();

	center[0] = *tmpCenter;
	center[1] = *(tmpCenter + 1);
	center[2] = *(tmpCenter + 2);

	// Obtain the normal of the mitkSurface
	double p0[3]; double p1[3]; double p2[3];

	plane->GetCell(0)->GetPoints()->GetPoint(0, p0);
	plane->GetCell(0)->GetPoints()->GetPoint(1, p1);
	plane->GetCell(0)->GetPoints()->GetPoint(2, p2);

	Eigen::Vector3d a(*p0, *(p0 + 1), *(p0 + 2));
	Eigen::Vector3d b(*p1, *(p1 + 1), *(p1 + 2));
	Eigen::Vector3d c(*p2, *(p2 + 1), *(p2 + 2));

	Eigen::Vector3d tmpVector0 = b - a;
	Eigen::Vector3d tmpVector1 = c - a;

	Eigen::Vector3d normalVector = tmpVector0.cross(tmpVector1);

	normal[0] = normalVector[0]; 
	normal[1] = normalVector[1];
	normal[2] = normalVector[2];

	return true;
}




