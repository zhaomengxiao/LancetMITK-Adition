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
#include "vtkImplicitPolyDataDistance.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTransformFilter.h>


mitk::SurfaceRegistration::SurfaceRegistration()
  : m_MatrixLandMark(vtkMatrix4x4::New()), m_ResultMatrix(vtkMatrix4x4::New())
{
  // LandMark registration result is always the first element in MatrixList
  m_MatrixList.push_back(m_MatrixLandMark);
}

mitk::SurfaceRegistration::~SurfaceRegistration()
{
  m_MatrixList.clear();
  m_MatrixLandMark->Delete();
}

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
	MITK_INFO << "Proceeding landmark registration";
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
		landmarkTransform->SetMode(VTK_LANDMARK_RIGIDBODY); // rigid: 6, similarity: 7, affine: 12

		landmarkTransform->Update();
		m_MatrixLandMark->DeepCopy(landmarkTransform->GetMatrix());


		// Compute the landmark registration final metric
		int pointNum = m_LandmarksSrc->GetSize();
		double maxLandmarkError{ 0 };
		double sumLandmarkError{ 0 };
		for (int i = 0; i < pointNum; i++)
		{
			auto sourcePoint = m_LandmarksSrc->GetPoint(i);
			auto targetPoint = m_LandmarksTarget->GetPoint(i);
			auto sourcePointMatrix = vtkMatrix4x4::New();
			sourcePointMatrix->Identity();
			sourcePointMatrix->SetElement(0, 3, sourcePoint[0]);
			sourcePointMatrix->SetElement(1, 3, sourcePoint[1]);
			sourcePointMatrix->SetElement(2, 3, sourcePoint[2]);

			auto tmpTransform = vtkTransform::New();
			tmpTransform->Identity();
			tmpTransform->PostMultiply();
			tmpTransform->SetMatrix(sourcePointMatrix);
			tmpTransform->Concatenate(m_MatrixLandMark);

			auto transformedSourcePointMatrix = tmpTransform->GetMatrix();


			double transformedSourcePointArray[3]
			{
				transformedSourcePointMatrix->GetElement(0,3),
				transformedSourcePointMatrix->GetElement(1,3),
				transformedSourcePointMatrix->GetElement(2,3)
			};

			double currentError = sqrt(pow(targetPoint[0] - transformedSourcePointArray[0], 2) +
				pow(targetPoint[1] - transformedSourcePointArray[1], 2) +
				pow(targetPoint[2] - transformedSourcePointArray[2], 2));

			sumLandmarkError = sumLandmarkError + currentError;
			if (currentError > maxLandmarkError)
			{
				maxLandmarkError = currentError;
			}
		}

		m_maxLandmarkError = maxLandmarkError;
		m_avgLandmarkError = sumLandmarkError / pointNum;

		return true;
	}

	MITK_ERROR << "SurfaceRegistration Error: landmark size wrong";
	return false;
}

bool mitk::SurfaceRegistration::ComputeIcpResult()
{
	MITK_INFO << "Proceeding ICP registration";
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
  //Reason: SetTarget should contain more points than SetSource 
	auto pTransform = vtkSmartPointer<vtkTransform>::New();
	pTransform->Identity();
	pTransform->Concatenate(GetResult());
	pTransform->Update();
	pTransform->Inverse();
	pTransform->TransformPoints(icpPoints, icpPoints_transed);
	
	auto pSource = vtkSmartPointer<vtkPolyData>::New();
	pSource->SetPoints(icpPoints_transed);

  // In case m_SurfaceSrc does not have an identity geometry matrix
  vtkTransform *tmpTrans = vtkTransform::New();
  tmpTrans->Identity();
  tmpTrans->PostMultiply();
  tmpTrans->SetMatrix(m_SurfaceSrc->GetGeometry()->GetVtkMatrix());
  vtkNew<vtkTransformFilter> transformFilter;
  transformFilter->SetInputData(m_SurfaceSrc->GetVtkPolyData());
  transformFilter->SetTransform(tmpTrans);
  transformFilter->Update();

	vtkSmartPointer<vtkIterativeClosestPointTransform> pIcp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	pIcp->SetSource(pSource);
	// pIcp->SetTarget(m_SurfaceSrc->GetVtkPolyData()); // the PolyData here must have an identity geometry matrix !
  pIcp->SetTarget(transformFilter->GetPolyDataOutput());

	pIcp->GetLandmarkTransform()->SetModeToRigidBody();
	pIcp->SetMaximumNumberOfIterations(1000);
	pIcp->SetCheckMeanDistance(true);
	pIcp->SetMaximumMeanDistance(0.0001);
	pIcp->Update();
	

  auto matrixIcp = vtkMatrix4x4::New();
	matrixIcp->DeepCopy(pIcp->GetMatrix());
	matrixIcp->Invert();

	m_MatrixList.push_back(matrixIcp);


	//------------- Calculate the ICP registration metric--------------------

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	

	vtkTransform* tmpTrans1 = vtkTransform::New();
	tmpTrans1->Identity();
	tmpTrans1->PostMultiply();
	tmpTrans1->Concatenate(m_SurfaceSrc->GetGeometry()->GetVtkMatrix());
	//tmpTrans1->SetMatrix(GetResult()->in);
	auto tmpMatrix = vtkMatrix4x4::New();
	//vtkMatrix4x4::Invert(GetResult(), tmpMatrix);
	//tmpTrans1->Concatenate(tmpMatrix);
	tmpTrans1->Concatenate(GetResult());

	vtkNew<vtkTransformFilter> transformFilter1;
	transformFilter1->SetInputData(m_SurfaceSrc->GetVtkPolyData());
	transformFilter1->SetTransform(tmpTrans1);
	transformFilter1->Update();

	int pointNum = m_IcpPoints->GetSize();
	double maxIcpError{ 0 };
	double sumIcpError = 0;
	for (int i = 0; i < pointNum; i++)
	{
		auto currentPoint = m_IcpPoints->GetPoint(i);
		double tmpPoint[3]{ currentPoint[0],currentPoint[1],currentPoint[2] };

		implicitPolyDataDistance->SetInput(transformFilter1->GetPolyDataOutput());

		double currentError = implicitPolyDataDistance->EvaluateFunction(tmpPoint);

		MITK_INFO << "ICP point error: " << currentError;
		sumIcpError = sumIcpError + fabs(currentError);
		if (fabs(currentError )> fabs(maxIcpError))
		{
			maxIcpError = fabs(currentError);
		}
	}

	m_maxIcpError = maxIcpError;
	m_avgIcpError = sumIcpError / pointNum;



	return true;
}

bool mitk::SurfaceRegistration::ComputeSurfaceIcpResult()
{
	if (m_SurfaceSrc == nullptr || m_SurfaceTarget == nullptr)
	{
		MITK_ERROR << "SurfaceRegistration Error: icp src or target surface null";
		return false;
	}
	

	// In case m_SurfaceSrc does not have an identity geometry matrix
	vtkTransform* tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->Concatenate(m_SurfaceSrc->GetGeometry()->GetVtkMatrix());
	vtkNew<vtkTransformFilter> transformFilter;
	transformFilter->SetInputData(m_SurfaceSrc->GetVtkPolyData());
	transformFilter->SetTransform(tmpTrans);
	transformFilter->Update();

	// In case m_SurfaceTarget does not have an identity geometry matrix
	vtkTransform* tmpTrans1 = vtkTransform::New();
	tmpTrans1->Identity();
	tmpTrans1->PostMultiply();
	tmpTrans1->SetMatrix(m_SurfaceTarget->GetGeometry()->GetVtkMatrix());
	vtkNew<vtkTransformFilter> transformFilter1;
	transformFilter1->SetInputData(m_SurfaceTarget->GetVtkPolyData());
	transformFilter1->SetTransform(tmpTrans1);
	transformFilter1->Update();

	vtkSmartPointer<vtkIterativeClosestPointTransform> pIcp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();

	pIcp->GetLandmarkTransform()->SetModeToRigidBody();
	pIcp->SetMaximumNumberOfIterations(1000);
	pIcp->SetCheckMeanDistance(true);
	pIcp->SetMaximumMeanDistance(0.0001);
	auto matrixIcp = vtkMatrix4x4::New();

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	if(m_SurfaceTarget->GetVtkPolyData()->GetNumberOfPoints() >= m_SurfaceSrc->GetVtkPolyData()->GetNumberOfPoints())
	{
		pIcp->SetTarget(transformFilter1->GetOutput());
		pIcp->SetSource(transformFilter->GetOutput());

		pIcp->Update();

		matrixIcp->DeepCopy(pIcp->GetMatrix());

		//////////// Error calculation: too time consuming
		
		// auto pointNum = transformFilter->GetOutput()->GetNumberOfPoints();
		// double maxIcpError{ 0 };
		// double sumIcpError = 0;
		// for (vtkIdType i = 0; i < pointNum; i++)
		// {
		// 	
		// 	double tmpPoint[3];
		//
		// 	tmpTrans->Concatenate(matrixIcp);
		// 	tmpTrans->Update();
		// 	transformFilter->SetTransform(tmpTrans);
		// 	transformFilter->SetInputData(m_SurfaceSrc->GetVtkPolyData());
		// 	transformFilter->SetTransform(tmpTrans);
		// 	transformFilter->Update();
		// 	transformFilter->GetOutput()->GetPoint(i,tmpPoint);
		//
		// 	implicitPolyDataDistance->SetInput(transformFilter1->GetPolyDataOutput());
		//
		// 	double currentError = implicitPolyDataDistance->EvaluateFunction(tmpPoint);
		//
		// 	sumIcpError = sumIcpError + fabs(currentError);
		// 	if (fabs(currentError) > fabs(maxIcpError))
		// 	{
		// 		maxIcpError = fabs(currentError);
		// 	}
		// }
		//
		// m_maxIcpError = maxIcpError;
		// m_avgIcpError = sumIcpError / pointNum;
		////////////
		
	}else
	{
		pIcp->SetSource(transformFilter1->GetOutput());
		pIcp->SetTarget(transformFilter->GetOutput());

		pIcp->Update();
		matrixIcp->DeepCopy(pIcp->GetMatrix());
		matrixIcp->Invert();

		//////////// Error calculation: too time consuming

		// auto pointNum = transformFilter1->GetOutput()->GetNumberOfPoints();
		// double maxIcpError{ 0 };
		// double sumIcpError = 0;
		// for (vtkIdType i = 0; i < pointNum; i++)
		// {
		//
		// 	double tmpPoint[3];
		//
		// 	tmpTrans1->Concatenate(matrixIcp);
		// 	tmpTrans1->Update();
		// 	transformFilter1->SetTransform(tmpTrans);
		// 	transformFilter1->SetInputData(m_SurfaceTarget->GetVtkPolyData());
		// 	transformFilter1->SetTransform(tmpTrans1);
		// 	transformFilter1->Update();
		// 	transformFilter1->GetOutput()->GetPoint(i, tmpPoint);
		//
		// 	implicitPolyDataDistance->SetInput(transformFilter->GetPolyDataOutput());
		//
		// 	double currentError = implicitPolyDataDistance->EvaluateFunction(tmpPoint);
		//
		// 	sumIcpError = sumIcpError + fabs(currentError);
		// 	if (fabs(currentError) > fabs(maxIcpError))
		// 	{
		// 		maxIcpError = fabs(currentError);
		// 	}
		// }
		//
		// m_maxIcpError = maxIcpError;
		// m_avgIcpError = sumIcpError / pointNum;
		// ////////////
	}

	

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
	m_MatrixLandMark->Identity();
  m_MatrixList.push_back(m_MatrixLandMark);

	m_ResultMatrix->Identity();
	
}

vtkMatrix4x4 * mitk::SurfaceRegistration::GetResult()
{
	auto transform = vtkSmartPointer<vtkTransform>::New(); // Use the default mode "premultiply"
  for (auto matrix : m_MatrixList)
  {
	  transform->Concatenate(matrix);
  }
	transform->Update();
	m_ResultMatrix->DeepCopy(transform->GetMatrix());
	return m_ResultMatrix;
}


