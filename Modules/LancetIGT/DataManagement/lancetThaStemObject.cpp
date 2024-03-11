
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetThaStemObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "surfaceregistraion.h"

#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"


lancet::ThaStemObject::ThaStemObject():
m_vtkMatrix_groupGeometry(vtkMatrix4x4::New()),
m_Surface_stem(mitk::Surface::New()),
m_Surface_stemFrame(mitk::Surface::New()),
m_Pset_headCenter(mitk::PointSet::New()),
m_Surface_head(mitk::Surface::New()),
m_Node_Surface_stem(mitk::DataNode::New()),
m_Node_Surface_head(mitk::DataNode::New()),
m_Node_Pset_headCenter(mitk::DataNode::New()),
m_Node_Surface_stemFrame(mitk::DataNode::New()),
m_Node_Pset_StemCutPlane(mitk::DataNode::New()),
m_Pset_StemCutPlane(mitk::PointSet::New()),
m_Node_Surface_StemCutPlane(mitk::DataNode::New()),
m_Surface_StemCutPlane(mitk::Surface::New()),
m_vtkMatrix_CutPlaneMatrix(vtkMatrix4x4::New())
{
	CreateInternalFrame();
}

lancet::ThaStemObject::~ThaStemObject()
{
}

void lancet::ThaStemObject::CreateInternalFrame()
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

	SetSurface_stemFrame(mySphere);
}


void lancet::ThaStemObject::SetGroupGeometry(vtkSmartPointer<vtkMatrix4x4> newMatrix)
{
	m_Surface_stem->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Surface_stemFrame->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_headCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	/*MITK_INFO << "HEAD CENTER:" << m_Pset_headCenter->GetPoint(0)[0] << ","
		<< m_Pset_headCenter->GetPoint(0)[1] << ","
		<< m_Pset_headCenter->GetPoint(0)[2];
	MITK_INFO << "================================";*/
	/*
	// The head surface should be first moved to the headCenter and apply the newMatrix
	mitk::Point3D headCenter_worldFrame = m_Pset_headCenter->GetPoint(0);
	mitk::Point3D headCenter_stemFrame;
	m_Pset_headCenter->GetGeometry()->WorldToIndex(headCenter_worldFrame, headCenter_stemFrame);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Translate(headCenter_stemFrame[0], headCenter_stemFrame[1], headCenter_stemFrame[2]);
	tmpTransform->Concatenate(newMatrix);
	tmpTransform->Update();
	
	m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());
	*/
	m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_Pset_StemCutPlane->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	//vtkNew<vtkTransform> CutPlaneMatrix;
	//CutPlaneMatrix->PostMultiply();
	//CutPlaneMatrix->SetMatrix(newMatrix);
	//CutPlaneMatrix->Concatenate(m_vtkMatrix_CutPlaneMatrix);
	//CutPlaneMatrix->Update();
	//m_Surface_StemCutPlane->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(CutPlaneMatrix->GetMatrix());
	m_Surface_StemCutPlane->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(newMatrix);

	m_vtkMatrix_groupGeometry->DeepCopy(newMatrix);
}

void lancet::ThaStemObject::SetStemSurface(mitk::Surface::Pointer stemSurface)
{
	m_Surface_stem = stemSurface;
	m_Surface_stem->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

void lancet::ThaStemObject::SetHeadCenter(mitk::PointSet::Pointer headCenterPset_stemFrame_new)
{
	/*
	mitk::Point3D headCenter_stemFrame_new = headCenterPset_stemFrame_new->GetPoint(0);
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Translate(headCenter_stemFrame_new[0], headCenter_stemFrame_new[1], headCenter_stemFrame_new[2]);
	tmpTransform->Concatenate(GetvtkMatrix_groupGeometry());
	tmpTransform->Update();

	m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());
	*/
	m_Pset_headCenter = headCenterPset_stemFrame_new;
	m_Pset_headCenter->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

void lancet::ThaStemObject::SetHeadSurface(mitk::Surface::Pointer headSurface)
{
	/*
	// The imported head surface should be moved to the head center
	vtkNew<vtkMatrix4x4> identityMatrix;
	headSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

	mitk::Point3D headCenter_worldFrame;
	if(m_Pset_headCenter->GetSize()>0)
	{
		headCenter_worldFrame = m_Pset_headCenter->GetPoint(0);
	}else
	{
		headCenter_worldFrame[0] = 0;
		headCenter_worldFrame[1] = 0;
		headCenter_worldFrame[2] = 0;
	}

	mitk::Point3D headCenter_stemFrame;
	m_Pset_headCenter->GetGeometry()->WorldToIndex(headCenter_worldFrame, headCenter_stemFrame);

	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->PostMultiply();
	tmpTransform->Translate(headCenter_stemFrame[0], headCenter_stemFrame[1], headCenter_stemFrame[2]);
	tmpTransform->Concatenate(GetvtkMatrix_groupGeometry());
	tmpTransform->Update();
	*/
	m_Surface_head = headSurface;

	//m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());
	m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}



bool lancet::ThaStemObject::CheckDataAvailability()
{
	if(m_Surface_stem->GetVtkPolyData()==nullptr)
	{
		MITK_ERROR << "m_Surface_stem is missing";
		return false;
	}

	if(m_Surface_stemFrame ->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_Surface_stemFrame is missing";
		return false;
	}

	if(m_Pset_headCenter->GetSize() == 0)
	{
		MITK_ERROR << "m_Pset_headCenter is missing";
		return false;
	}

	if(m_Surface_head->GetVtkPolyData() == nullptr)
	{
		MITK_ERROR << "m_Surface_head is missing";
		return false;
	}

	return true;
}

void lancet::ThaStemObject::SetStemNeckAngle(double angle)
{
	m_StemNeckAngle = angle;
}

double lancet::ThaStemObject::GetStemNeckAngle()
{
	return m_StemNeckAngle;
}

bool lancet::ThaStemObject::AlignStemObjectWithWorldFrame()
{
	// Since the imported stem models are already aligned
	// there is no need for extra alignment
	
	if(CheckDataAvailability()==false)
	{
		return false;
	}
	GenerateStemCutPlaneSurface();
	/*
	// The imported head surface should be moved to the head center
	auto headCenter = m_Pset_headCenter->GetPoint(0);
	
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->Translate(headCenter[0], headCenter[1], headCenter[2]);
	tmpTransform->Update();	
	
	m_Surface_head->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());
	*/
	//TO DO:
	//auto stemAssemblySurfaceCenter = m_Pset_stemLine->GetPoint(0);
	//auto headAssemblySurfaceCenter = m_Pset_headAxis->GetPoint(1);
	//auto stemAxisEnd = m_Pset_stemLine->GetPoint(1);
	//auto FemoralheadCenter = m_Pset_headAxis->GetPoint(0);
	//auto stemNormal1 = m_Pset_stemNormal->GetPoint(0);
	//auto stemNormal2 = m_Pset_stemNormal->GetPoint(1);
	//
	//vtkNew<vtkTransform> stemTransform;
	//
	//stemTransform->Translate(-stemAssemblySurfaceCenter[0], -stemAssemblySurfaceCenter[1], -stemAssemblySurfaceCenter[2]);

	//double stemAxis[3] = { stemAxisEnd[0] - stemAssemblySurfaceCenter[0],
	//stemAxisEnd[1] - stemAssemblySurfaceCenter[1],
	//stemAxisEnd[2] - stemAssemblySurfaceCenter[2] };

	//double headAxis[3] = { headAssemblySurfaceCenter[0] - FemoralheadCenter[0],
	//headAssemblySurfaceCenter[1] - FemoralheadCenter[1],
	//headAssemblySurfaceCenter[2] - FemoralheadCenter[2] };

	//double RotAngle = 0;
	//double stemAxisNorm = sqrt(stemAxis[0] * stemAxis[0] + stemAxis[1] * stemAxis[1] + stemAxis[2] * stemAxis[2]);
	//double headAxisNorm = sqrt(headAxis[0] * headAxis[0] + headAxis[1] * headAxis[1] + headAxis[2] * headAxis[2]);
	//if (stemAxisNorm != 0 || headAxisNorm != 0)
	//{
	//	RotAngle = std::acos((stemAxis[0] * headAxis[0] + stemAxis[1] * headAxis[1] + stemAxis[2] * headAxis[2]) /
	//		(stemAxisNorm * headAxisNorm)) / vtkMath::Pi() * 180;
	//}
	//double RotAxis[3]; 
	//vtkMath::Cross(stemAxis, headAxis, RotAxis);
	//stemTransform->RotateWXYZ(RotAngle, RotAxis);

	//double tmpStemAssemblySurfaceCenter[3] = { stemAssemblySurfaceCenter[0], stemAssemblySurfaceCenter[1], stemAssemblySurfaceCenter[2] };
	//stemTransform->TransformPoint(tmpStemAssemblySurfaceCenter, tmpStemAssemblySurfaceCenter);

	//stemTransform->Translate(headAssemblySurfaceCenter[0] - tmpStemAssemblySurfaceCenter[0],
	//	headAssemblySurfaceCenter[1] - tmpStemAssemblySurfaceCenter[1],
	//	headAssemblySurfaceCenter[2] - tmpStemAssemblySurfaceCenter[2]);

	//double TransStemAssemblySurfaceCenter[3], TransStemAxisEnd[3], TransStemNormal1[3], TransStemNormal2[3];
	//stemTransform->TransformPoint(tmpStemAssemblySurfaceCenter, TransStemAssemblySurfaceCenter);
	//stemTransform->TransformPoint(tmpStemAxisEnd, TransStemAxisEnd);
	//stemTransform->TransformPoint(tmpStemNormal1, TransStemNormal1);
	//stemTransform->TransformPoint(tmpStemNormal2, TransStemNormal2);

	//m_Surface_stem->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(stemTransform->GetMatrix());
	//m_Pset_stemLine->InsertPoint(0, TransStemAssemblySurfaceCenter);
	//m_Pset_stemLine->InsertPoint(1, TransStemAxisEnd);
	//m_Pset_stemNormal->InsertPoint(0, TransStemNormal1);
	//m_Pset_stemNormal->InsertPoint(1, TransStemNormal2);

	return true;
}

void lancet::ThaStemObject::SetNode_Surface_stemFrame(mitk::DataNode::Pointer node)
{
	m_Node_Surface_stemFrame->SetVisibility(0);

	m_Node_Surface_stemFrame = node;
	m_Surface_stemFrame = dynamic_cast<mitk::Surface*>(node->GetData());
}

void lancet::ThaStemObject::SetNode_Pset_StemCutPlane(mitk::DataNode::Pointer node)
{
	m_Node_Pset_StemCutPlane->SetVisibility(0);

	m_Node_Pset_StemCutPlane = node;
	SetPsetStemCutPlane(dynamic_cast<mitk::PointSet*>(node->GetData()));
}

void lancet::ThaStemObject::SetPsetStemCutPlane(mitk::PointSet::Pointer point)
{
	m_Pset_StemCutPlane = point;
	m_Pset_StemCutPlane->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
}

void lancet::ThaStemObject::GenerateStemCutPlaneSurface()
{
	mitk::Point3D origin;
	//auto origin = m_Pset_StemCutPlane->GetPoint(0);
	m_Pset_StemCutPlane->GetGeometry()->WorldToIndex(m_Pset_StemCutPlane->GetPoint(0), origin);
	mitk::Point3D end;
	//auto end = m_Pset_StemCutPlane->GetPoint(1);
	m_Pset_StemCutPlane->GetGeometry()->WorldToIndex(m_Pset_StemCutPlane->GetPoint(1), end);
	double normal[3] = { end[0] - origin[0], end[1] - origin[1], end[2] - origin[2] };
	vtkSmartPointer<vtkPlaneSource> planeSourceNew = vtkSmartPointer<vtkPlaneSource>::New();
	planeSourceNew->SetOrigin(origin[0] - 30, origin[1] - 25, origin[2]);
	planeSourceNew->SetPoint1(origin[0] + 30, origin[1] - 25, origin[2]);
	planeSourceNew->SetPoint2(origin[0] - 30, origin[1] + 25, origin[2]);
	planeSourceNew->SetNormal(normal);
	planeSourceNew->Update();

	m_Surface_StemCutPlane->SetVtkPolyData(planeSourceNew->GetOutput());

	m_Node_Surface_StemCutPlane->SetData(m_Surface_StemCutPlane);
}

//void lancet::ThaStemObject::SetNode_Pset_stemLine(mitk::DataNode::Pointer node)
//{
//	m_Node_Pset_stemLine->SetVisibility(0);
//	m_Node_Pset_stemLine = node;
//	SetPset_stemLine(dynamic_cast<mitk::PointSet*>(node->GetData()));
//}
//
//void lancet::ThaStemObject::SetNode_Pset_stemNormal(mitk::DataNode::Pointer node)
//{
//	m_Node_Pset_stemNormal->SetVisibility(0);
//	m_Node_Pset_stemNormal = node;
//	SetPset_stemNormal(dynamic_cast<mitk::PointSet*>(node->GetData()));
//}
//
//void lancet::ThaStemObject::SetNode_Pset_headAxis(mitk::DataNode::Pointer node)
//{
//	m_Node_Pset_headAxis->SetVisibility(0);
//	m_Node_Pset_headAxis = node;
//	SetPset_headAxis(dynamic_cast<mitk::PointSet*>(node->GetData()));
//}
//
//void lancet::ThaStemObject::SetPset_stemLine(mitk::PointSet::Pointer point)
//{
//	m_Pset_stemLine = point;
//	m_Pset_stemLine->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
//}
//
//void lancet::ThaStemObject::SetPset_stemNormal(mitk::PointSet::Pointer point)
//{
//	m_Pset_stemNormal = point;
//	m_Pset_stemNormal->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
//}
//
//void lancet::ThaStemObject::SetPset_headAxis(mitk::PointSet::Pointer point)
//{
//	m_Pset_headAxis = point;
//	m_Pset_headAxis->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(GetvtkMatrix_groupGeometry());
//}

void lancet::ThaStemObject::SetNode_Surface_stem(mitk::DataNode::Pointer node)
{
	m_Node_Surface_stem->SetVisibility(0);

	m_Node_Surface_stem = node;
	SetStemSurface(dynamic_cast<mitk::Surface*>(node->GetData()));
}

void lancet::ThaStemObject::SetNode_Pset_headCenter(mitk::DataNode::Pointer node)
{
	m_Node_Pset_headCenter->SetVisibility(0);

	m_Node_Pset_headCenter = node;
	SetHeadCenter(dynamic_cast<mitk::PointSet*>(node->GetData()));
}

void lancet::ThaStemObject::SetNode_Surface_head(mitk::DataNode::Pointer node)
{
	m_Node_Surface_head->SetVisibility(0);

	m_Node_Surface_head = node;
	SetHeadSurface(dynamic_cast<mitk::Surface*>(node->GetData()));
}



