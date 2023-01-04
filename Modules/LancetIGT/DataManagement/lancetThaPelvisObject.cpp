/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaPelvisObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaPelvisObject::ThaPelvisObject()
	:m_vtkMatrix_groupGeometry(vtkMatrix4x4::New()),
	m_surface_pelvis(mitk::Surface::New()),
	m_surface_pelvisFrame(mitk::Surface::New()),
	m_pset_ASIS(mitk::PointSet::New()),
	m_pset_pelvisCOR(mitk::PointSet::New()),
	m_pset_midline(mitk::PointSet::New())
{
	CreateInternalFrame();
}

lancet::ThaPelvisObject::~ThaPelvisObject()
{
}

void lancet::ThaPelvisObject::CreateInternalFrame()
{
	mitk::Surface::Pointer mySphere = mitk::Surface::New();

	double axisLength = 5.;

	vtkSmartPointer<vtkSphereSource> vtkSphere = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkConeSource> vtkCone = vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkCylinderSource> vtkCylinder = vtkSmartPointer<vtkCylinderSource>::New();
	vtkSmartPointer<vtkPolyData> axis = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkLineSource> vtkLine = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine2 = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> vtkLine3 = vtkSmartPointer<vtkLineSource>::New();

	vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();

	//Y-Axis (start with y, cause cylinder is oriented in y by vtk default...)
	vtkCone->SetDirection(0, 1, 0);
	vtkCone->SetHeight(1.0);
	vtkCone->SetRadius(0.4f);
	vtkCone->SetResolution(16);
	vtkCone->SetCenter(0.0, axisLength, 0.0);
	vtkCone->Update();

	vtkCylinder->SetRadius(0.05);
	vtkCylinder->SetHeight(axisLength);
	vtkCylinder->SetCenter(0.0, 0.5 * axisLength, 0.0);
	vtkCylinder->Update();

	appendPolyData->AddInputData(vtkCone->GetOutput());
	appendPolyData->AddInputData(vtkCylinder->GetOutput());
	appendPolyData->Update();
	axis->DeepCopy(appendPolyData->GetOutput());

	//y symbol
	vtkLine->SetPoint1(-0.5, axisLength + 2., 0.0);
	vtkLine->SetPoint2(0.0, axisLength + 1.5, 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(0.5, axisLength + 2., 0.0);
	vtkLine2->SetPoint2(-0.5, axisLength + 1., 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(axis);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//X-axis
	vtkSmartPointer<vtkTransform> XTransform = vtkSmartPointer<vtkTransform>::New();
	XTransform->RotateZ(-90);
	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	TrafoFilter->SetTransform(XTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	//x symbol
	vtkLine->SetPoint1(axisLength + 2., -0.5, 0.0);
	vtkLine->SetPoint2(axisLength + 1., 0.5, 0.0);
	vtkLine->Update();

	vtkLine2->SetPoint1(axisLength + 2., 0.5, 0.0);
	vtkLine2->SetPoint2(axisLength + 1., -0.5, 0.0);
	vtkLine2->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Z-axis
	vtkSmartPointer<vtkTransform> ZTransform = vtkSmartPointer<vtkTransform>::New();
	ZTransform->RotateX(90);
	TrafoFilter->SetTransform(ZTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	//z symbol
	vtkLine->SetPoint1(-0.5, 0.0, axisLength + 2.);
	vtkLine->SetPoint2(0.5, 0.0, axisLength + 2.);
	vtkLine->Update();

	vtkLine2->SetPoint1(-0.5, 0.0, axisLength + 2.);
	vtkLine2->SetPoint2(0.5, 0.0, axisLength + 1.);
	vtkLine2->Update();

	vtkLine3->SetPoint1(0.5, 0.0, axisLength + 1.);
	vtkLine3->SetPoint2(-0.5, 0.0, axisLength + 1.);
	vtkLine3->Update();

	appendPolyData->AddInputData(vtkLine->GetOutput());
	appendPolyData->AddInputData(vtkLine2->GetOutput());
	appendPolyData->AddInputData(vtkLine3->GetOutput());
	appendPolyData->AddInputData(TrafoFilter->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Center
	vtkSphere->SetRadius(0.5f);
	vtkSphere->SetCenter(0.0, 0.0, 0.0);
	vtkSphere->Update();

	appendPolyData->AddInputData(vtkSphere->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	//Scale
	vtkSmartPointer<vtkTransform> ScaleTransform = vtkSmartPointer<vtkTransform>::New();
	ScaleTransform->Scale(20., 20., 20.);

	TrafoFilter->SetTransform(ScaleTransform);
	TrafoFilter->SetInputData(surface);
	TrafoFilter->Update();

	mySphere->SetVtkPolyData(TrafoFilter->GetOutput());

	Setsurface_pelvisFrame(mySphere);
}

vtkSmartPointer<vtkMatrix4x4> lancet::ThaPelvisObject::CalculateWorldToPelvisTransform()
{
	vtkNew<vtkMatrix4x4> worldToPelvisMatrix;
	worldToPelvisMatrix->Identity();

	// Check if m_pset_ASIS and m_pset_midline are ready
	if (m_pset_ASIS->IsEmpty() || m_pset_midline->IsEmpty())
	{
		MITK_ERROR << "m_pset_ASIS or m_pset_midline is empty when CalculateWorldtoPelvisTransform()";
		return worldToPelvisMatrix;
	}

	auto origin = m_pset_midline->GetPoint(0);
	auto asis_r = m_pset_ASIS->GetPoint(0);
	auto asis_l = m_pset_ASIS->GetPoint(1);

	Eigen::Vector3d x_vector;
	x_vector[0] = asis_l[0] - asis_r[0];
	x_vector[1] = asis_l[1] - asis_r[1];
	x_vector[2] = asis_l[2] - asis_r[2];
	x_vector.normalize();

	Eigen::Vector3d originToRightAsis;
	originToRightAsis[0] = asis_r[0] - origin[0];
	originToRightAsis[1] = asis_r[1] - origin[1];
	originToRightAsis[2] = asis_r[2] - origin[2];

	Eigen::Vector3d originToLeftAsis;
	originToLeftAsis[0] = asis_l[0] - origin[0];
	originToLeftAsis[1] = asis_l[1] - origin[1];
	originToLeftAsis[2] = asis_l[2] - origin[2];

	Eigen::Vector3d y_vector;
	y_vector = originToRightAsis.cross(originToLeftAsis);
	y_vector.normalize();

	Eigen::Vector3d z_vector;
	z_vector = x_vector.cross(y_vector);
	z_vector.normalize();

	for (int i{0}; i < 3; i++)
	{
		worldToPelvisMatrix->SetElement(i, 0, x_vector[i]);
		worldToPelvisMatrix->SetElement(i, 1, y_vector[i]);
		worldToPelvisMatrix->SetElement(i, 2, z_vector[i]);
		worldToPelvisMatrix->SetElement(i, 3, origin[i]);
	}

	return worldToPelvisMatrix;
}


void lancet::ThaPelvisObject::SetGroupGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	m_surface_pelvis->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_surface_pelvisFrame->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_pset_ASIS->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_pset_pelvisCOR->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_pset_midline->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	// m_vtkMatrix_groupGeometry and newMatrix should both be used to handle mitk::Image

	m_vtkMatrix_groupGeometry->DeepCopy(newMatrix);
}


void lancet::ThaPelvisObject::CalculateSupinePelvicTilt()
{
	// if the patient direction meets the requirements, the supine pelvis tilt is angle between pelvicFrame's
	// y axis and worldFrame's y axis

	auto tmpMatrix = CalculateWorldToPelvisTransform();

	Eigen::Vector3d y_pelvicFrame;
	y_pelvicFrame[0] = tmpMatrix->GetElement(0, 1);
	y_pelvicFrame[1] = tmpMatrix->GetElement(1, 1);
	y_pelvicFrame[2] = tmpMatrix->GetElement(2, 1);

	Eigen::Vector3d y_worldFrame;
	y_worldFrame[0] = 0;
	y_worldFrame[1] = 1;
	y_worldFrame[2] = 0;

	double tmpDotProduct = y_pelvicFrame.dot(y_worldFrame);
	Eigen::Vector3d tmpCrossProduct = y_pelvicFrame.cross(y_worldFrame);
	double angle = (180 / 3.14159) * acos(tmpDotProduct);

	if(tmpCrossProduct[0] < 0)
	{
		SetpelvicTilt_supine(angle);
	}else
	{
		SetpelvicTilt_supine(-angle);
	}
	
}


bool lancet::ThaPelvisObject::CheckDataAvailability()
{
	if (m_surface_pelvis->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_surface_pelvis is missing";
		return false;
	}

	if (m_surface_pelvisFrame->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_surface_pelvisFrame is missing";
		return false;
	}

	if (m_pset_ASIS->GetSize() == 0)
	{
		MITK_ERROR << "m_pset_ASIS is not ready";
		return false;
	}

	if (m_pset_pelvisCOR->GetSize() == 0)
	{
		MITK_ERROR << "m_pset_pelvisCOR is not ready";
		return false;
	}

	if (m_pset_midline->GetSize() == 0)
	{
		MITK_ERROR << "m_pset_midline is not ready";
		return false;
	}

	return true;
}

bool lancet::ThaPelvisObject::AlignPelvicObjectWithWorldFrame()
{
	if(CheckDataAvailability() == false)
	{
		return false;
	}

	CalculateSupinePelvicTilt();

	auto worldToPelvicTransform = CalculateWorldToPelvisTransform();

	// Rewrite the vtkPolyData of m_surface_pelvis and m_surface_pelvisFrame
	vtkNew<vtkMatrix4x4> pelvicToWorldMatrix;
	pelvicToWorldMatrix->DeepCopy(worldToPelvicTransform);
	pelvicToWorldMatrix->Invert();

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->SetMatrix(pelvicToWorldMatrix);

	vtkNew<vtkTransformPolyDataFilter> tmpFilter;
	tmpFilter->SetTransform(tmpTransform);
	tmpFilter->SetInputData(m_surface_pelvis->GetVtkPolyData());
	tmpFilter->Update();

	m_surface_pelvis->SetVtkPolyData(tmpFilter->GetOutput());
	
	// Rewrite the inside points of m_pset_ASIS, m_pset_pelvisCOR and m_pset_midline
	m_pset_ASIS->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_pset_pelvisCOR->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_pset_midline->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);

	RewritePointSetWithGeometryMatrix(m_pset_ASIS);
	RewritePointSetWithGeometryMatrix(m_pset_pelvisCOR);
	RewritePointSetWithGeometryMatrix(m_pset_midline);
	
	return true;

}


void lancet::ThaPelvisObject::RewritePointSetWithGeometryMatrix( mitk::PointSet::Pointer inputPointSet)
{
	int size = inputPointSet->GetSize();

	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < size; i++)
	{
		// inputPointSet->SetPoint(i, GetPointWithGeometryMatrix(inputPointSet, i));
		tmpPset->InsertPoint(inputPointSet->GetPoint(i));
	}

	vtkNew<vtkMatrix4x4> tmpMatrix;
	tmpMatrix->Identity();

	inputPointSet->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	for (int i{ 0 }; i < size; i++)
	{
		inputPointSet->SetPoint(i, tmpPset->GetPoint(i));
	}

}

mitk::PointSet::Pointer lancet::ThaPelvisObject::GetPointSetWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet)
{
	int size = inputPointSet->GetSize();

	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < size; i++)
	{
		tmpPset->InsertPoint(inputPointSet->GetPoint(i));
	}

	return tmpPset;
}

