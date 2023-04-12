/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaFemurObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkMatrixConvert.h"
#include "surfaceregistraion.h"

lancet::ThaFemurObject::ThaFemurObject():
m_vtkMatrix_groupGeometry(vtkMatrix4x4::New()),
m_vtkMatrix_canalFrameToMechanicFrame(vtkMatrix4x4::New()),
m_image_femur(mitk::Image::New()),
m_surface_femur(mitk::Surface::New()),
m_surface_femurFrame(mitk::Surface::New()),
m_pset_neckCenter(mitk::PointSet::New()),
m_pset_lesserTrochanter(mitk::PointSet::New()),
m_pset_femurCOR(mitk::PointSet::New()),
m_pset_femurCanal(mitk::PointSet::New()),
m_pset_epicondyles(mitk::PointSet::New()),
m_Node_image_femur(mitk::DataNode::New()),
m_Node_surface_femur(mitk::DataNode::New()),
m_Node_surface_femurFrame(mitk::DataNode::New()),
m_Node_pset_neckCenter(mitk::DataNode::New()),
m_Node_pset_lesserTrochanter(mitk::DataNode::New()),
m_Node_pset_femurCOR(mitk::DataNode::New()),
m_Node_pset_femurCanal(mitk::DataNode::New()),
m_Node_pset_epicondyles(mitk::DataNode::New())
{
	CreateInternalFrame();
}

lancet::ThaFemurObject::~ThaFemurObject()
{
}

void lancet::ThaFemurObject::SetGroupGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	m_surface_femur->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_surface_femurFrame-> GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_pset_lesserTrochanter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_pset_femurCOR->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_pset_neckCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_pset_femurCanal->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);
	m_pset_epicondyles->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	// previous m_vtkMatrix_groupGeometry and newMatrix should both be used to handle mitk::Image
	auto affineTrans = m_image_femur->GetGeometry()->GetIndexToWorldTransform();
	vtkNew<vtkMatrix4x4> tmpMatrix;
	mitk::TransferItkTransformToVtkMatrix(affineTrans, tmpMatrix);

	vtkNew<vtkMatrix4x4> preMatrix;
	preMatrix->DeepCopy(m_vtkMatrix_groupGeometry);
	preMatrix->Invert();

	vtkNew<vtkTransform> tmpTrans;
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(tmpMatrix);
	tmpTrans->Concatenate(preMatrix);
	tmpTrans->Concatenate(newMatrix);
	tmpTrans->Update();

	m_image_femur->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	m_vtkMatrix_groupGeometry->DeepCopy(newMatrix);
}


void lancet::ThaFemurObject::CalculateFemurVersion()
{
	// The native femoral version is the angle between the neck axis and epicondylar axis when
	// these 2 axes are projected on a plane perpendicular to the femur canal
	// a positive value (anteversion) is returned when the neck axis angled anteriorly relative to the
	// epicondylar axis

	// Compose neck axis: neck center + femurCOR
	// Compose epicondylar axis, 0 is medial, 1 is lateral:
	// Compose femur canal axis, 0 is proximal, 1 is distal;
	Eigen::Vector3d neckAxis;
	Eigen::Vector3d epicondylarAxis;
	Eigen::Vector3d canalAxis;

	neckAxis[0] = m_pset_femurCOR->GetPoint(0)[0] - m_pset_neckCenter->GetPoint(0)[0];
	neckAxis[1] = m_pset_femurCOR->GetPoint(0)[1] - m_pset_neckCenter->GetPoint(0)[1];
	neckAxis[2] = m_pset_femurCOR->GetPoint(0)[2] - m_pset_neckCenter->GetPoint(0)[2];

	epicondylarAxis[0] = m_pset_epicondyles->GetPoint(0)[0] - m_pset_epicondyles->GetPoint(1)[0];
	epicondylarAxis[1] = m_pset_epicondyles->GetPoint(0)[1] - m_pset_epicondyles->GetPoint(1)[1];
	epicondylarAxis[2] = m_pset_epicondyles->GetPoint(0)[2] - m_pset_epicondyles->GetPoint(1)[2];

	canalAxis[0] = m_pset_femurCanal->GetPoint(0)[0] - m_pset_femurCanal->GetPoint(1)[0];
	canalAxis[1] = m_pset_femurCanal->GetPoint(0)[1] - m_pset_femurCanal->GetPoint(1)[1];
	canalAxis[2] = m_pset_femurCanal->GetPoint(0)[2] - m_pset_femurCanal->GetPoint(1)[2];

	canalAxis.normalize();

	// Project the neckAxis onto the canal axis
	Eigen::Vector3d neckAxis_ontoCanalAxis = neckAxis.dot(canalAxis) * canalAxis;

	// neckAxis projection onto the perpendicular plane 
	Eigen::Vector3d neckAxis_ontoPlane = neckAxis - neckAxis_ontoCanalAxis;

	// Project the epicondylarAxis onto the canal axis
	Eigen::Vector3d epicondylarAxis_ontoCanalAxis = epicondylarAxis.dot(canalAxis) * canalAxis;

	// epicondylarAxis projection onto the perpendicular plane
	Eigen::Vector3d epicondylarAxis_ontoPlane = epicondylarAxis - epicondylarAxis_ontoCanalAxis;

	double femoralVersion = (180 / 3.14159) * acos(epicondylarAxis_ontoPlane.dot(neckAxis_ontoPlane)
		/ (epicondylarAxis_ontoPlane.norm() * neckAxis_ontoPlane.norm()));

	// Determine anteversion or retroversion; if ante, assign femoralVersion as (+); if retro, assign as (-)
	double tmpValue = epicondylarAxis_ontoPlane.cross(neckAxis_ontoPlane).dot(canalAxis);
	if (m_femurSide == 1)
	{
		if (tmpValue < 0)
		{
			femoralVersion = -femoralVersion;
		}
	}
	else
	{
		if (tmpValue > 0)
		{
			femoralVersion = -femoralVersion;
		}
	}

	SetfemurVersion(femoralVersion);
}

void lancet::ThaFemurObject::CreateInternalFrame()
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

	Setsurface_femurFrame(mySphere);
}

bool lancet::ThaFemurObject::CheckDataAvailability()
{
	// if m_isOperationSide == 0, less data are required
	if(m_isOperationSide == 0)
	{
		if(m_image_femur->GetVtkImageData() == nullptr)
		{
			MITK_ERROR << "m_image_femur is missing";
			return false;
		}

		if( m_surface_femur ->GetVtkPolyData() == nullptr)
		{
			MITK_ERROR << "m_surface_femur is missing";
			return false;
		}

		if(m_surface_femurFrame->GetVtkPolyData() == nullptr)
		{
			MITK_ERROR << "m_surface_femurFrame is missing";
			return false;
		}

		if(m_pset_lesserTrochanter->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_lessreTrochanter is missing";
			return false;
		}

		if(m_pset_femurCOR ->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_femurCOR is missing";
			return false;
		}

		if(m_pset_neckCenter ->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_neckCenter is missing";
			return false;
		}

		if(m_pset_femurCanal ->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_femurCanal is missing";
			return false;
		}

		if(m_pset_epicondyles ->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_epicondyles is missing";
			return false;
		}
	}
	// if m_isOperationSide == 1, more data are needed
	else
	{
		if (m_surface_femur->GetVtkPolyData() == nullptr)
		{
			MITK_ERROR << "m_surface_femur is missing";
			return false;
		}

		if (m_surface_femurFrame->GetVtkPolyData() == nullptr)
		{
			MITK_ERROR << "m_surface_femurFrame is missing";
			return false;
		}

		if (m_pset_lesserTrochanter->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_lessreTrochanter is missing";
			return false;
		}

		if (m_pset_femurCOR->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_femurCOR is missing";
			return false;
		}

		if (m_pset_neckCenter->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_neckCenter is missing";
			return false;
		}

		if (m_pset_femurCanal->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_femurCanal is missing";
			return false;
		}

		if (m_pset_epicondyles->GetSize() == 0)
		{
			MITK_ERROR << "m_pset_epicondyles is missing";
			return false;
		}
	}
	
	return true;
}

bool lancet::ThaFemurObject::AlignFemurObjectWithWorldFrame()
{
	if(CheckDataAvailability() ==false)
	{
		return false;
	}

	CalculateFemurVersion();

	auto worldToFemurTransform = CalculateWorldToFemurTransform();
	vtkNew<vtkMatrix4x4> femurToWorldMatrix;
	femurToWorldMatrix->DeepCopy(worldToFemurTransform);
	femurToWorldMatrix->Invert();

	// Rewrite the vtkPolyData of m_surface_femur
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Identity();
	tmpTransform->SetMatrix(femurToWorldMatrix);

	vtkNew<vtkTransformPolyDataFilter> tmpFilter;
	tmpFilter->SetTransform(tmpTransform);
	tmpFilter->SetInputData(m_surface_femur->GetVtkPolyData());
	tmpFilter->Update();

	m_surface_femur->SetVtkPolyData(tmpFilter->GetOutput());

	// Move femur image
	auto affineTrans = m_image_femur->GetGeometry()->GetIndexToWorldTransform();
	vtkNew<vtkMatrix4x4> tmpMatrix;
	mitk::TransferItkTransformToVtkMatrix(affineTrans, tmpMatrix);

	vtkNew<vtkTransform> tmpTrans;
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(tmpMatrix);
	tmpTrans->Concatenate(femurToWorldMatrix);
	tmpTrans->Update();

	m_image_femur->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());


	// Rewrite the inside points of m_pset_lesserTrochanter, m_pset_femurCOR, m_pset_neckCenter, m_pset_femurCanal and m_pset_epicondyles
	m_pset_lesserTrochanter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurToWorldMatrix);
	m_pset_lesserTrochanter->Update();
	m_pset_femurCOR->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurToWorldMatrix);
	m_pset_neckCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurToWorldMatrix);
	m_pset_femurCanal->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurToWorldMatrix);
	m_pset_epicondyles->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(femurToWorldMatrix);

	RewritePointSetWithGeometryMatrix(m_pset_lesserTrochanter);
	RewritePointSetWithGeometryMatrix(m_pset_femurCOR);
	RewritePointSetWithGeometryMatrix(m_pset_neckCenter);
	RewritePointSetWithGeometryMatrix(m_pset_femurCanal);
	RewritePointSetWithGeometryMatrix(m_pset_epicondyles);

	CalculateCanalFrameToMechanicFrame();

	return true;
}

vtkSmartPointer<vtkMatrix4x4> lancet::ThaFemurObject::CalculateWorldToFemurTransform()
{
	vtkNew<vtkMatrix4x4> worldToFemurMatrix;
	worldToFemurMatrix->Identity();

	//check if m_pset_femurCanal and m_pset_femurCOR are available
	if(m_pset_femurCOR->IsEmpty() || m_pset_femurCanal->GetSize() != 2)
	{
		MITK_ERROR << "m_pset_femurCanal or m_pset_femurCOR are not ready for CalculateWorldToFemurTransform()";
		return worldToFemurMatrix;
	}

	auto origin = m_pset_femurCanal->GetPoint(0);
	auto distalFemurCanal = m_pset_femurCanal->GetPoint(1);
	auto femurCOR = m_pset_femurCOR->GetPoint(0);

	// z axis is distal femurCanal--> proximal femurCanal
	Eigen::Vector3d z_vector;
	z_vector[0] = origin[0] - distalFemurCanal[0];
	z_vector[1] = origin[1] - distalFemurCanal[1];
	z_vector[2] = origin[2] - distalFemurCanal[2];
	z_vector.normalize();

	// for right femur: y axis is the cross product of z axis and (proximal femurCanal--> femurCOR)
	// for left femur: y axis is the cross product of  (proximal femurCanal--> femurCOR) and z axis
	Eigen::Vector3d tmpVector;
	tmpVector[0] = femurCOR[0] - origin[0];
	tmpVector[1] = femurCOR[1] - origin[1];
	tmpVector[2] = femurCOR[2] - origin[2];
	tmpVector.normalize();

	Eigen::Vector3d y_vector;
	if(m_femurSide == 0)
	{
		y_vector = z_vector.cross(tmpVector);
	}else
	{
		y_vector = tmpVector.cross(z_vector);
	}
	y_vector.normalize();

	Eigen::Vector3d x_vector;
	x_vector = y_vector.cross(z_vector);

	for(int i{0}; i < 3; i++)
	{
		worldToFemurMatrix->SetElement(i, 0, x_vector[i]);
		worldToFemurMatrix->SetElement(i, 1, y_vector[i]);
		worldToFemurMatrix->SetElement(i, 2, z_vector[i]);
		worldToFemurMatrix->SetElement(i, 3, origin[i]);
	}

	return worldToFemurMatrix;
}

void lancet::ThaFemurObject::RewritePointSetWithGeometryMatrix(mitk::PointSet::Pointer inputPointSet)
{
	int size = inputPointSet->GetSize();
	
	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < size; i++)
	{
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

mitk::PointSet::Pointer lancet::ThaFemurObject::GetPointSetWithGeometryMatrix(const mitk::PointSet::Pointer inputPointSet)
{
	int size = inputPointSet->GetSize();

	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < size; i++)
	{
		tmpPset->InsertPoint(inputPointSet->GetPoint(i));
	}

	return tmpPset;
}

void lancet::ThaFemurObject::CalculateCanalFrameToMechanicFrame()
{
	// Provided that the femur is already aligned with the canal axis
	// for mechanic axis alignment, construct a temporary coordinate system:
	// origin: proximal femurCanal
	// z vector: midpoint of the epicondyles --> femurCOR
	// toolVector: femurCOR's perpendicular foot on the canal axis --> femurCOR
	// y vector: z vector X toolVector (right femur) / toolVector X z vector (left femur)
	auto femurCOR = Getpset_femurCOR()->GetPoint(0);
	auto proximalCanal = Getpset_femurCanal()->GetPoint(0);
	auto distalCanal = Getpset_femurCanal()->GetPoint(1);
	auto medialEpi = Getpset_epicondyles()->GetPoint(0);
	auto lateralEpi = Getpset_epicondyles()->GetPoint(1);
	double midEpi[3];
	midEpi[0] = (lateralEpi[0] + medialEpi[0]) / 2;
	midEpi[1] = (lateralEpi[1] + medialEpi[1]) / 2;
	midEpi[2] = (lateralEpi[2] + medialEpi[2]) / 2;

	Eigen::Vector3d z_vector;
	z_vector[0] = femurCOR[0] - midEpi[0];
	z_vector[1] = femurCOR[1] - midEpi[1];
	z_vector[2] = femurCOR[2] - midEpi[2];
	z_vector.normalize();

	Eigen::Vector3d toolVector;
	toolVector[0] = femurCOR[0] - proximalCanal[0];
	toolVector[1] = femurCOR[1] - proximalCanal[1];
	toolVector[2] = femurCOR[2] - proximalCanal[2];

	Eigen::Vector3d toolVector2;
	toolVector2[0] = proximalCanal[0] - distalCanal[0];
	toolVector2[1] = proximalCanal[1] - distalCanal[1];
	toolVector2[2] = proximalCanal[2] - distalCanal[2];
	toolVector2.normalize();

	Eigen::Vector3d toolVector3;
	toolVector3 = toolVector - (toolVector.dot(toolVector2)) * toolVector2;


	Eigen::Vector3d y_vector;
	if(m_femurSide == 0)
	{
		y_vector = z_vector.cross(toolVector3);
	}else
	{
		y_vector = toolVector3.cross(z_vector);
	}

	y_vector.normalize();

	Eigen::Vector3d x_vector;
	x_vector = y_vector.cross(z_vector);

	for(int i{0}; i < 3; i++)
	{
		m_vtkMatrix_canalFrameToMechanicFrame->SetElement(i, 0, x_vector[i]);
		m_vtkMatrix_canalFrameToMechanicFrame->SetElement(i, 1, y_vector[i]);
		m_vtkMatrix_canalFrameToMechanicFrame->SetElement(i, 2, z_vector[i]);
	}

}

void lancet::ThaFemurObject::SetNode_image_femur(mitk::DataNode::Pointer node)
{
	m_Node_image_femur = node;
	m_image_femur = dynamic_cast<mitk::Image*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_surface_femur(mitk::DataNode::Pointer node)
{
	m_Node_surface_femur = node;
	m_surface_femur = dynamic_cast<mitk::Surface*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_pset_neckCenter(mitk::DataNode::Pointer node)
{
	m_Node_pset_neckCenter = node;
	m_pset_neckCenter = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_pset_femurCOR(mitk::DataNode::Pointer node)
{
	m_Node_pset_femurCOR = node;
	m_pset_femurCOR = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_pset_epicondyles(mitk::DataNode::Pointer node)
{
	m_Node_pset_epicondyles = node;
	m_pset_epicondyles = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_pset_femurCanal(mitk::DataNode::Pointer node)
{
	m_Node_pset_femurCanal = node;
	m_pset_femurCanal = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_pset_lesserTrochanter(mitk::DataNode::Pointer node)
{
	m_Node_pset_lesserTrochanter = node;
	m_pset_lesserTrochanter = dynamic_cast<mitk::PointSet*>(node->GetData());
}

void lancet::ThaFemurObject::SetNode_surface_femurFrame(mitk::DataNode::Pointer node)
{
	m_Node_surface_femurFrame = node;
	m_surface_femurFrame = dynamic_cast<mitk::Surface*>(node->GetData());
}





