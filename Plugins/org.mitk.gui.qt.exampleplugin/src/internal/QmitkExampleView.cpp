/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QmitkExampleView.h"

#include <berryIWorkbenchWindow.h>
#include <ExampleImageFilter.h>
#include <ExampleImageInteractor.h>
#include <mitkNodePredicateDataType.h>
#include <usModuleRegistry.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"

namespace
{
  // Helper function to create a fully set up instance of our
  // ExampleImageInteractor, based on the state machine specified in Paint.xml
  // as well as its configuration in PaintConfig.xml. Both files are compiled
  // into MitkExampleModule as resources.
  static ExampleImageInteractor::Pointer CreateExampleImageInteractor()
  {
    auto exampleModule = us::ModuleRegistry::GetModule("MitkExampleModule");

    if (nullptr != exampleModule)
    {
      auto interactor = ExampleImageInteractor::New();
      interactor->LoadStateMachine("Paint.xml", exampleModule);
      interactor->SetEventConfig("PaintConfig.xml", exampleModule);
      return interactor;
    }

    return nullptr;
  }
}

// Don't forget to initialize the VIEW_ID.
const std::string QmitkExampleView::VIEW_ID = "org.mitk.views.exampleview";

void QmitkExampleView::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);

  

  // Wire up the UI widgets with our functionality.
  
  connect(m_Controls.pushButton_test, SIGNAL(clicked()), this, SLOT(ShowDistalCut()));
}

void QmitkExampleView::PlotCoord(vtkMatrix4x4* coord, mitk::DataStorage* ds, std::string name)
{
	mitk::DataNode::Pointer coordnode = mitk::DataNode::New();
	coordnode->SetName(name);

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

	// Y-Axis (start with y, cause cylinder is oriented in y by vtk default...)
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

	// y symbol
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

	// X-axis
	vtkSmartPointer<vtkTransform> XTransform = vtkSmartPointer<vtkTransform>::New();
	XTransform->RotateZ(-90);
	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	TrafoFilter->SetTransform(XTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	// x symbol
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

	// Z-axis
	vtkSmartPointer<vtkTransform> ZTransform = vtkSmartPointer<vtkTransform>::New();
	ZTransform->RotateX(90);
	TrafoFilter->SetTransform(ZTransform);
	TrafoFilter->SetInputData(axis);
	TrafoFilter->Update();

	// z symbol
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

	// Center
	vtkSphere->SetRadius(0.5f);
	vtkSphere->SetCenter(0.0, 0.0, 0.0);
	vtkSphere->Update();

	appendPolyData->AddInputData(vtkSphere->GetOutput());
	appendPolyData->AddInputData(surface);
	appendPolyData->Update();
	surface->DeepCopy(appendPolyData->GetOutput());

	// Scale
	vtkSmartPointer<vtkTransform> ScaleTransform = vtkSmartPointer<vtkTransform>::New();
	ScaleTransform->Scale(5., 5., 5.);

	TrafoFilter->SetTransform(ScaleTransform);
	TrafoFilter->SetInputData(surface);
	TrafoFilter->Update();

	mySphere->SetVtkPolyData(TrafoFilter->GetOutput());

	coordnode->SetData(mySphere);

	double ref[3]{ 0, 0, 0 };
	mitk::Point3D refp{ ref };
	auto* doOp = new mitk::ApplyTransformMatrixOperation(mitk::OpAPPLYTRANSFORMMATRIX, coord, refp);
	// execute the Operation
	// here no undo is stored, because the movement-steps aren't interesting.
	// only the start and the end is interisting to store for undo.
	coordnode->GetData()->GetGeometry()->ExecuteOperation(doOp);
	delete doOp;

	ds->Add(coordnode);
}

void QmitkExampleView::ShowDistalCut()
{
	//获取切面法线

	auto NormalPointSet = GetDataStorage()->GetNamedObject<mitk::PointSet>("DistalCut");
	//获取远端切面法线
	auto NormalAntPointSet = GetDataStorage()->GetNamedObject<mitk::PointSet>("AnteriorChamferCut");
	auto _n0 = NormalPointSet->GetPoint(0);
	auto _n1 = NormalPointSet->GetPoint(1);

	auto _ref_n0 = NormalAntPointSet->GetPoint(0);

	Eigen::Vector3d n0{ _n0.GetDataPointer() };
	Eigen::Vector3d n1{ _n1.GetDataPointer() };
	Eigen::Vector3d ref_n0{ _ref_n0.GetDataPointer() };

	Eigen::Vector3d normal = (n1 - n0).normalized();
	// if (flip)
	// {
	//   normal = -normal;
	// }

	//借助前端面法线得到假体的前后方向，建立坐标系
	Eigen::Vector3d o{ n0 };
	Eigen::Vector3d x{ normal };
	Eigen::Vector3d tmp_y{ (ref_n0 - n0).normalized() };
	Eigen::Vector3d z = x.cross(tmp_y);
	Eigen::Vector3d y = z.cross(x);

	//向锯片y方向（刀头后撤）平移60mm 作为初始定位点，
	Eigen::Vector3d o_init = o + y * 60;

	Eigen::Matrix4d T;
	T.setIdentity();
	T.block(0, 0, 3, 1) = x;
	T.block(0, 1, 3, 1) = y;
	T.block(0, 2, 3, 1) = z;
	T.block(0, 3, 3, 1) = o_init;
	T.transposeInPlace();

	vtkNew<vtkMatrix4x4> mat;
	mat->DeepCopy(T.data());

	PlotCoord(mat, GetDataStorage(), "DistalCut_init");

	DrawCoord(mat, GetDataStorage(), "DistalCut_init_2");

	// vtkMatrix4x4::DeepCopy(MainStatic::G_NDI_Robot_PosteriorCut,T.data());
}

void QmitkExampleView::DrawVector(mitk::Point3D p_start,
	mitk::Vector3D vector,
	double length,
	mitk::DataStorage* ds,
	std::string name)
{
	mitk::Point3D p_end;
	p_end = p_start + vector * length;

	mitk::PointSet::Pointer pset = mitk::PointSet::New();
	pset->InsertPoint(p_start);
	pset->InsertPoint(p_end);

	mitk::DataNode::Pointer dn = mitk::DataNode::New();
	dn->SetData(pset);
	dn->SetName(name);
	dn->SetBoolProperty("show contour", true);
	dn->SetFloatProperty("contoursize", 1);
	dn->SetFloatProperty("pointsize", 3);

	ds->Add(dn);
}

void QmitkExampleView::DrawCoord(vtkMatrix4x4* coord, mitk::DataStorage* ds, std::string name)
{
	double x[3]{ coord->GetElement(0, 0), coord->GetElement(1, 0), coord->GetElement(2, 0) };
	double y[3]{ coord->GetElement(0, 1), coord->GetElement(1, 1), coord->GetElement(2, 1) };
	double z[3]{ coord->GetElement(0, 2), coord->GetElement(1, 2), coord->GetElement(2, 2) };
	double o[3]{ coord->GetElement(0, 3), coord->GetElement(1, 3), coord->GetElement(2, 3) };
	mitk::Point3D p_start{ o };

	mitk::Vector3D v_x{ x };
	mitk::Vector3D v_y{ y };
	mitk::Vector3D v_z{ z };
	// x
	DrawVector(p_start, v_x, 10, ds, name + "_x");
	// y
	DrawVector(p_start, v_y, 10, ds, name + "_y");
	// z
	DrawVector(p_start, v_z, 10, ds, name + "_z");
}

void QmitkExampleView::SetFocus()
{
  m_Controls.pushButton_test->setFocus();
}
