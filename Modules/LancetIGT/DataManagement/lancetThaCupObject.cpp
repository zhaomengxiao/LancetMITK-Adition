
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaCupObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

lancet::ThaCupObject::ThaCupObject() :
	m_vtkMatrix_groupGeometry(vtkMatrix4x4::New()),
	m_Surface_cup(mitk::Surface::New()),
	m_Surface_liner(mitk::Surface::New()),
	m_Surface_cupFrame(mitk::Surface::New()),
	m_Node_Surface_cup(mitk::DataNode::New()),
	m_Node_Surface_liner(mitk::DataNode::New()),
	m_Node_Surface_cupFrame(mitk::DataNode::New()),
	m_Node_Pset_cupCenter(mitk::DataNode::New()),
	m_Pset_cupCenter(mitk::PointSet::New())
{
	CreateInternalFrame();
}

lancet::ThaCupObject::~ThaCupObject()
{
}

void lancet::ThaCupObject::CreateInternalFrame()
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

	SetSurface_cupFrame(mySphere);
}


void lancet::ThaCupObject::SetGroupGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	m_Surface_cup->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Surface_liner->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Surface_cupFrame->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_cupCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_vtkMatrix_groupGeometry->DeepCopy(newMatrix);
}

void lancet::ThaCupObject::SetCupSurface(mitk::Surface::Pointer cupSurface)
{
	m_Surface_cup = cupSurface;
	m_Surface_cup->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

void lancet::ThaCupObject::SetLinerSurface(mitk::Surface::Pointer linerSurface)
{
	m_Surface_liner = linerSurface;
	m_Surface_liner->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

void lancet::ThaCupObject::SetPsetCupCenter(mitk::PointSet::Pointer cupCenterPset)
{
	m_Pset_cupCenter = cupCenterPset;
	m_Pset_cupCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

bool lancet::ThaCupObject::AlignCupObjectWithWorldFrame()
{
	// Since the imported cup and liner stl models are already aligned
	// there is no need for extra alignment
	// this function is remained to keep cupObject consistent with the pelvis and femur object

	if(CheckDataAvailability() == 0)
	{
		return false;
	}


	return true;
}


bool lancet::ThaCupObject::CheckDataAvailability()
{
	if(m_Surface_cup->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_Surface_cup is missing";
		return false;
	}

	if(m_Surface_liner->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_Surface_liner is missing";
		return false;
	}

	if(m_Surface_cupFrame->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_Surface_cupFrame is missing";
		return false;
	}

	return true;
}

void lancet::ThaCupObject::SetNode_Surface_cup(mitk::DataNode::Pointer node)
{
	m_Node_Surface_cup->SetVisibility(0);
	m_Node_Surface_cup = node;
	// m_Surface_cup = dynamic_cast<mitk::Surface*>(node->GetData());
	SetCupSurface(dynamic_cast<mitk::Surface*>(node->GetData()));
}

void lancet::ThaCupObject::SetNode_Surface_liner(mitk::DataNode::Pointer node)
{
	m_Node_Surface_liner->SetVisibility(0);
	m_Node_Surface_liner = node;
	// m_Surface_liner = dynamic_cast<mitk::Surface*>(node->GetData());
	SetLinerSurface(dynamic_cast<mitk::Surface*>(node->GetData()));
}

void lancet::ThaCupObject::SetNode_Surface_cupFrame(mitk::DataNode::Pointer node)
{
	m_Node_Surface_cupFrame->SetVisibility(0);
	m_Node_Surface_cupFrame = node;
	m_Surface_cupFrame = dynamic_cast<mitk::Surface*>(node->GetData());
}

void lancet::ThaCupObject::SetNode_Pset_cupCenter(mitk::DataNode::Pointer node)
{
	m_Node_Pset_cupCenter->SetVisibility(0);
	m_Node_Pset_cupCenter = node;
	// m_Surface_liner = dynamic_cast<mitk::Surface*>(node->GetData());
	SetPsetCupCenter(dynamic_cast<mitk::PointSet*>(node->GetData()));
}



