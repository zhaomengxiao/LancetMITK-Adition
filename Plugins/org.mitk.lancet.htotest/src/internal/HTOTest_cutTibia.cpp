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
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkSurface.h"
#include "vtkOBBTree.h"

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

	if(cellNum_1 >= cellNum_0)
	{
		largerSubPart->DeepCopy(tibiaPart_1);
		smallerSubPart->DeepCopy(tibiaPart_0);
	}else
	{
		largerSubPart->DeepCopy(tibiaPart_0);
		smallerSubPart->DeepCopy(tibiaPart_1);
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

	auto planeCenter = mitkCutSurface->GetGeometry()->GetCenter();

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

	// Obtain the normal of the mitkSurface
	double p0[3]; double p1[3]; double p2[3];

	tmpVtkSurface->GetCell(0)->GetPoints()->GetPoint(0, p0);
	tmpVtkSurface->GetCell(0)->GetPoints()->GetPoint(1, p1);
	tmpVtkSurface->GetCell(0)->GetPoints()->GetPoint(2, p2);

	Eigen::Vector3d a(*p0,*(p0+1),*(p0+2));
	Eigen::Vector3d b(*p1, *(p1 + 1), *(p1 + 2));
	Eigen::Vector3d c(*p2, *(p2 + 1), *(p2 + 2));

	Eigen::Vector3d tmpVector0 = b - a;
	Eigen::Vector3d tmpVector1 = c - a;

	Eigen::Vector3d normalVector = tmpVector0.cross(tmpVector1);

	double surfaceNormal[3]{ normalVector[0],normalVector[1] ,normalVector[2] };
	double cutPlaneCenter[3]{ planeCenter[0],planeCenter[1],planeCenter[2] };

	vtkNew<vtkPolyData> proximalTibiaSurface;
	vtkNew<vtkPolyData> distalTibiaSurface;

	CutPolyDataWithPlane(tibiaVtkSurface, distalTibiaSurface, proximalTibiaSurface, cutPlaneCenter, surfaceNormal);

	auto mitkProximalSurface = mitk::Surface::New();
	auto mitkDistalSurface = mitk::Surface::New();
	mitkProximalSurface->SetVtkPolyData(proximalTibiaSurface);
	mitkDistalSurface->SetVtkPolyData(distalTibiaSurface);
	auto tmpNode0 = mitk::DataNode::New();
	auto tmpNode1 = mitk::DataNode::New();
	tmpNode0->SetData(mitkProximalSurface);
	tmpNode0->SetName("proximal tibia");
	tmpNode1->SetData(mitkDistalSurface);
	tmpNode1->SetName("distal tibia");

	GetDataStorage()->Add(tmpNode0);
	GetDataStorage()->Add(tmpNode1);

	return true;
}




