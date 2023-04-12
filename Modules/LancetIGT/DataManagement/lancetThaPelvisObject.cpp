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

#include "mitkMatrixConvert.h"
#include "surfaceregistraion.h"

lancet::ThaPelvisObject::ThaPelvisObject()
	:m_vtkMatrix_groupGeometry(vtkMatrix4x4::New()),
    m_image_pelvis(mitk::Image::New()),
	m_surface_pelvis(mitk::Surface::New()),
	m_surface_pelvisFrame(mitk::Surface::New()),
	m_pset_ASIS(mitk::PointSet::New()),
	m_pset_pelvisCOR(mitk::PointSet::New()),
	m_pset_midline(mitk::PointSet::New()),
m_Node_image_pelvis(mitk::DataNode::New()),
m_Node_surface_pelvis(mitk::DataNode::New()),
m_Node_surface_pelvisFrame(mitk::DataNode::New()),
m_Node_pset_ASIS(mitk::DataNode::New()),
m_Node_pset_pelvisCOR(mitk::DataNode::New()),
m_Node_pset_midline(mitk::DataNode::New()),
m_Surface_clippedPelvis(mitk::Surface::New()),
m_Pset_icpPoints(mitk::PointSet::New()),
m_Pset_superiorLandmark(mitk::PointSet::New()),
m_Pset_anteriorLandmark(mitk::PointSet::New()),
m_Pset_posteriorLandmark(mitk::PointSet::New()),
m_Pset_verificationPoints(mitk::PointSet::New()),
m_Node_Surface_clippedPelvis(mitk::DataNode::New()),
m_Node_Pset_icpPoints(mitk::DataNode::New()),
m_Node_Pset_superiorLandmark(mitk::DataNode::New()),
m_Node_Pset_anteriorLandmark(mitk::DataNode::New()),
m_Node_Pset_posteriorLandmark(mitk::DataNode::New()),
m_Node_Pset_verificationPoints(mitk::DataNode::New())
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

	m_Pset_anteriorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_icpPoints->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_posteriorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_superiorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_verificationPoints->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Surface_clippedPelvis->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	// previous m_vtkMatrix_groupGeometry and newMatrix should both be used to handle mitk::Image
	auto preAffineTrans = m_image_pelvis->GetGeometry()->GetIndexToWorldTransform();
	vtkNew<vtkMatrix4x4> preVtkMatrix;
	mitk::TransferItkTransformToVtkMatrix(preAffineTrans, preVtkMatrix);

	vtkNew<vtkTransform> tmpTrans;
	vtkNew<vtkMatrix4x4> tmpMatrix;
	tmpMatrix->DeepCopy(m_vtkMatrix_groupGeometry);
	tmpMatrix->Invert();
	tmpTrans->PostMultiply();
	tmpTrans->Concatenate(preVtkMatrix);
	tmpTrans->Concatenate(tmpMatrix);
	tmpTrans->Concatenate(newMatrix);
	tmpTrans->Update();
	
	m_image_pelvis->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

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
	if (m_image_pelvis->GetVtkImageData() == nullptr)
	{
		MITK_ERROR << "m_image_pelvis is missing";
		return false;
	}

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

	vtkNew<vtkPolyData> tmpSurface_pelvis;
	tmpSurface_pelvis->DeepCopy(tmpFilter->GetOutput());

	m_surface_pelvis->SetVtkPolyData(tmpSurface_pelvis);

	tmpFilter->SetInputData(m_Surface_clippedPelvis->GetVtkPolyData());
	tmpFilter->Update();
	m_Surface_clippedPelvis->SetVtkPolyData(tmpFilter->GetOutput());

	// Move the pelvis image
	auto preAffineTrans = m_image_pelvis->GetGeometry()->GetIndexToWorldTransform();
	vtkNew<vtkMatrix4x4> tmpMatrix;
	mitk::TransferItkTransformToVtkMatrix(preAffineTrans, tmpMatrix);
	vtkNew<vtkTransform> tmpTrans;
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(tmpMatrix);
	tmpTrans->Concatenate(pelvicToWorldMatrix);
	tmpTrans->Update();
	m_image_pelvis->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Rewrite the inside points of m_pset_ASIS, m_pset_pelvisCOR and m_pset_midline
	m_pset_ASIS->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_pset_pelvisCOR->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_pset_midline->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);

	m_Pset_anteriorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_Pset_icpPoints->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_Pset_posteriorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_Pset_superiorLandmark->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);
	m_Pset_verificationPoints->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(pelvicToWorldMatrix);

	RewritePointSetWithGeometryMatrix(m_pset_ASIS);
	RewritePointSetWithGeometryMatrix(m_pset_pelvisCOR);
	RewritePointSetWithGeometryMatrix(m_pset_midline);
	RewritePointSetWithGeometryMatrix(m_Pset_anteriorLandmark);
	RewritePointSetWithGeometryMatrix(m_Pset_icpPoints);
	RewritePointSetWithGeometryMatrix(m_Pset_posteriorLandmark);
	RewritePointSetWithGeometryMatrix(m_Pset_superiorLandmark);
	RewritePointSetWithGeometryMatrix(m_Pset_verificationPoints);

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


void lancet::ThaPelvisObject::SetNode_image_pelvis(mitk::DataNode::Pointer node)
{
	m_Node_image_pelvis = node;
	m_image_pelvis = dynamic_cast<mitk::Image*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_surface_pelvis(mitk::DataNode::Pointer node)
{
	m_Node_surface_pelvis = node;
	m_surface_pelvis = dynamic_cast<mitk::Surface*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_surface_pelvisFrame(mitk::DataNode::Pointer node)
{
	m_Node_surface_pelvisFrame = node;
	m_surface_pelvisFrame = dynamic_cast<mitk::Surface*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_pset_ASIS(mitk::DataNode::Pointer node)
{
	m_Node_pset_ASIS = node;
	m_pset_ASIS = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_pset_pelvisCOR(mitk::DataNode::Pointer node)
{
	m_Node_pset_pelvisCOR = node;
	m_pset_pelvisCOR = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_pset_midline(mitk::DataNode::Pointer node)
{
	m_Node_pset_midline = node;
	m_pset_midline = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Pset_anteriorLandmark(mitk::DataNode::Pointer node)
{
	m_Node_Pset_anteriorLandmark = node;
	m_Pset_anteriorLandmark = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Pset_icpPoints(mitk::DataNode::Pointer node)
{
	m_Node_Pset_icpPoints = node;
	m_Pset_icpPoints = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Pset_posteriorLandmark(mitk::DataNode::Pointer node)
{
	m_Node_Pset_posteriorLandmark = node;
	m_Pset_posteriorLandmark = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Pset_superiorLandmark(mitk::DataNode::Pointer node)
{
	m_Node_Pset_superiorLandmark = node;
	m_Pset_superiorLandmark = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Pset_verificationPoints(mitk::DataNode::Pointer node)
{
	m_Node_Pset_verificationPoints = node;
	m_Pset_verificationPoints = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaPelvisObject::SetNode_Surface_clippedPelvis(mitk::DataNode::Pointer node)
{
	m_Node_Surface_clippedPelvis = node;
	m_Surface_clippedPelvis = dynamic_cast<mitk::Surface*>(node->GetData());
}










