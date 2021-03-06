/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>

#include <lancetNavigationObject.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

std::string lancet::NavigationObject::GetName()
{
  if (this->m_DataNode.IsNull()) { return ""; }
  else { return m_DataNode->GetName(); }
}

mitk::Surface::Pointer lancet::NavigationObject::GetObjectSurface()
{
  if (this->m_DataNode.IsNull()) { return nullptr; }
  else if (this->m_DataNode->GetData() == nullptr) { return nullptr; }
  else { return dynamic_cast<mitk::Surface*>(m_DataNode->GetData()); }
}

void lancet::NavigationObject::SetDefaultDataNode()
{
  if (m_DataNode.IsNull())
    m_DataNode = mitk::DataNode::New();

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

  this->GetDataNode()->SetData(mySphere);
}

lancet::NavigationObject::NavigationObject()
  :m_Landmarks(mitk::PointSet::New()),
   m_Landmarks_probe(mitk::PointSet::New()),
   m_IcpPoints(mitk::PointSet::New()),
   m_IcpPoints_probe(mitk::PointSet::New()),
   m_T_Object2ReferenceFrame(vtkMatrix4x4::New())
{
  SetDefaultDataNode();
}

lancet::NavigationObject::NavigationObject(const NavigationObject& other)
{
}

lancet::NavigationObject::~NavigationObject()
{
}

itk::LightObject::Pointer lancet::NavigationObject::InternalClone() const
{
  return DataObject::InternalClone();
}
