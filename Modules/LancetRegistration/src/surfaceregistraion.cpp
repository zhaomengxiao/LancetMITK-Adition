/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "surfaceregistraion.h"

#include "vtkLandmarkTransform.h"
#include "vtkIterativeClosestPointTransform.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

void mitk::SurfaceRegistration::AddLandMark(mitk::Point3D point)
{
  if (m_LandmarksTarget!=nullptr)
  {
      m_LandmarksTarget->InsertPoint(point);
  }
}

void mitk::SurfaceRegistration::AddIcpPoints(mitk::Point3D point)
{
  if (m_IcpPoints)
  {
      m_IcpPoints->InsertPoint(point);
  }
}

void mitk::SurfaceRegistration::ClearLandMarks()
{
    m_LandmarksTarget->Clear();
}

void mitk::SurfaceRegistration::ClearIcpPoints()
{
    m_IcpPoints->Clear();
}

bool mitk::SurfaceRegistration::ComputeLandMarkResult()
{
	Clear();
	if (m_LandmarksTarget == nullptr || m_LandmarksSrc == nullptr)
	{
		MITK_ERROR << "SurfaceRegistration Error: landmark null";
		return false;
	}
	if (m_LandmarksTarget->GetSize() >= 3 && m_LandmarksTarget->GetSize() == m_LandmarksSrc->GetSize())
	{

		vtkSmartPointer<vtkPoints> pSource = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkPoints> pTarget = vtkSmartPointer<vtkPoints>::New();

		for (int i = 0; i < m_LandmarksSrc->GetSize(); i++)
		{
			pSource->InsertNextPoint(m_LandmarksSrc->GetPoint(i).GetDataPointer());
			pTarget->InsertNextPoint(m_LandmarksTarget->GetPoint(i).GetDataPointer());
		}

		vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();
		landmarkTransform->SetSourceLandmarks(pSource);
		landmarkTransform->SetTargetLandmarks(pTarget);
		landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY);
		landmarkTransform->Update();

		m_MatrixLandMark->DeepCopy(landmarkTransform->GetMatrix());
		return true;
	}
	MITK_ERROR << "SurfaceRegistration Error: landmark size wrong";
	return false;
}

bool mitk::SurfaceRegistration::ComputeIcpResult()
{
	if (m_SurfaceSrc == nullptr || m_IcpPoints == nullptr)
	{
		MITK_ERROR << "SurfaceRegistration Error: icp or surface null";
		return false;
	}
	auto icpPoints = vtkSmartPointer<vtkPoints>::New();
	auto icpPoints_transed = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < m_IcpPoints->GetSize(); i++)
	{	
		icpPoints->InsertNextPoint(m_IcpPoints->GetPoint(i).GetDataPointer());
	}

  //The new transformation is computed under the result of the preceding transformation,
  //but we don't move the source surface,so we move target icp points inversely.
	auto pTransform = vtkSmartPointer<vtkTransform>::New();
	pTransform->Identity();
	pTransform->Concatenate(GetResult());
	pTransform->Update();
	pTransform->Inverse();
	pTransform->TransformPoints(icpPoints, icpPoints_transed);
	
	auto pSource = vtkSmartPointer<vtkPolyData>::New();
	pSource->SetPoints(icpPoints_transed);

	vtkSmartPointer<vtkIterativeClosestPointTransform> pIcp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	pIcp->SetSource(pSource);
	pIcp->SetTarget(m_SurfaceSrc->GetVtkPolyData());
	pIcp->GetLandmarkTransform()->SetModeToRigidBody();
	pIcp->SetMaximumNumberOfIterations(1000);
	pIcp->SetCheckMeanDistance(true);
	pIcp->SetMaximumMeanDistance(0.0001);
	pIcp->Update();

  auto matrixIcp = vtkMatrix4x4::New();
	matrixIcp->DeepCopy(pIcp->GetMatrix());
	matrixIcp->Invert();

	m_MatrixList.push_back(matrixIcp);

	return true;
}

bool mitk::SurfaceRegistration::Undo()
{
  if (!m_MatrixList.empty())
  {
	  m_MatrixList.pop_back();
	  GetResult();
	  return true;
  }
  return false;
}

void mitk::SurfaceRegistration::Clear()
{
 
  
	m_MatrixList.clear();
	if (m_MatrixLandMark == nullptr)
	{
		MITK_INFO << "m_MatrixLandMark == nullptr";
		m_MatrixLandMark = vtkMatrix4x4::New();
	}
	if (m_ResultMatrix == nullptr)
	{
		MITK_INFO << "m_ResultMatrix == nullptr";
		m_ResultMatrix = vtkMatrix4x4::New();
	}
	m_MatrixLandMark->Identity();
	m_ResultMatrix->Identity();
	m_MatrixList.push_back(m_MatrixLandMark);
}

vtkMatrix4x4 * mitk::SurfaceRegistration::GetResult()
{
	auto transform = vtkSmartPointer<vtkTransform>::New();
  for (auto matrix : m_MatrixList)
  {
	  transform->Concatenate(matrix);
  }
	transform->Update();
	m_ResultMatrix->DeepCopy(transform->GetMatrix());
	return m_ResultMatrix;
}

mitk::SurfaceRegistration::SurfaceRegistration()
  : m_MatrixLandMark(vtkMatrix4x4::New()),
    m_ResultMatrix(vtkMatrix4x4::New())
{
  //LandMark registration result always the first and only one element in MatrixList
	m_MatrixList.push_back(m_MatrixLandMark);
}

mitk::SurfaceRegistration::~SurfaceRegistration()
{
	m_MatrixList.clear();
	m_MatrixLandMark->Delete();
}
