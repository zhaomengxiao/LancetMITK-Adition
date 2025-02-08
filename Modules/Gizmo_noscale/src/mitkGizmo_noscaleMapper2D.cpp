/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkGizmo_noscaleMapper2D.h"

#include "mitkGizmo_noscale.h"

// MITK includes
#include <mitkBaseRenderer.h>
#include <mitkCameraController.h>
#include <mitkLookupTableProperty.h>
#include <mitkVtkInterpolationProperty.h>
#include <mitkVtkRepresentationProperty.h>
#include <mitkVtkScalarModeProperty.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCharArray.h>
#include <vtkConeSource.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkTubeFilter.h>
#include <vtkVectorOperators.h>

mitk::Gizmo_noscaleMapper2D::LocalStorage::LocalStorage()
{
  m_VtkPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  m_Actor = vtkSmartPointer<vtkActor>::New();
  m_Actor->SetMapper(m_VtkPolyDataMapper);
}

const mitk::Gizmo_noscale *mitk::Gizmo_noscaleMapper2D::GetInput()
{
  return static_cast<const Gizmo_noscale *>(GetDataNode()->GetData());
}

void mitk::Gizmo_noscaleMapper2D::ResetMapper(mitk::BaseRenderer *renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  ls->m_Actor->VisibilityOff();
}

namespace
{
  //! Helper method: will assign given value to all points in given polydata object.
  void AssignScalarValueTo(vtkPolyData *polydata, char value)
  {
    vtkSmartPointer<vtkCharArray> pointData = vtkSmartPointer<vtkCharArray>::New();

    int numberOfPoints = polydata->GetNumberOfPoints();
    pointData->SetNumberOfComponents(1);
    pointData->SetNumberOfTuples(numberOfPoints);
    pointData->FillComponent(0, value);
    polydata->GetPointData()->SetScalars(pointData);
  }

  //! Helper method: will create a vtkPolyData representing a disk
  //! around center, inside the plane defined by viewRight and viewUp,
  //! and with the given radius.
  vtkSmartPointer<vtkPolyData> Create2DDisk(
    mitk::Vector3D axis, mitk::Vector3D viewRight,
                                            mitk::Vector3D viewUp,
                                            mitk::Point3D center,
                                            double radius)

  {
    // build the axis itself (as a tube around the line defining the axis)
    vtkSmartPointer<vtkPolyData> disk = vtkSmartPointer<vtkPolyData>::New();

    mitk::Vector3D ringPointer;
    unsigned int numberOfRingPoints = 36;
    vtkSmartPointer<vtkPoints> ringPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> ringSkeleton = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> ringPoly = vtkSmartPointer<vtkCellArray>::New();
    ringPoly->InsertNextCell(numberOfRingPoints + 1);
    for (unsigned int segment = 0; segment < numberOfRingPoints; ++segment)
    {
      double x = std::cos((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());
      double y = std::sin((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());

      ringPointer = viewRight * x + viewUp * y;

      ringPoints->InsertPoint(segment, (center + ringPointer * radius*0.2).GetDataPointer());
      ringPoly->InsertCellPoint(segment);
    }
    ringPoly->InsertCellPoint(0);
    // transform ring points (copied from vtkConeSource)
    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    //t->Translate(center.GetDataPointer());
    double vMag = vtkMath::Norm(axis.GetDataPointer());
    if (axis[0] == -1)
    {
      // flip x -> -x to avoid instability
     // t->RotateWXYZ(180, (axis[0] - vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
    //  t->RotateWXYZ(-180.0, 0, 1, 0);
     // t->Translate(0, -3, -4.5);
    }
    else if (axis[1] == 1)
    {
     // t->RotateWXYZ(180.0, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
     // t->Translate(-3, -4, -4);
    }
    else if (axis[2] == 1)
    {
    //  t->RotateWXYZ(180.0, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
     // t->Translate(-3, 4, -2);
    }

    double thisPoint[3];
    for (unsigned int i = 0; i < numberOfRingPoints; ++i)
    {
      ringPoints->GetPoint(i, thisPoint);
      t->TransformPoint(thisPoint, thisPoint);
      ringPoints->SetPoint(i, thisPoint);
    }
    disk->SetPoints(ringPoints);
    disk->SetPolys(ringPoly);
	
	ringSkeleton->SetPoints(ringPoints);
    ringSkeleton->SetLines(ringPoly);

    // Use vtkSplineFilter to smooth the input polydata
    vtkSmartPointer<vtkSplineFilter> splineFilter = vtkSmartPointer<vtkSplineFilter>::New();
    splineFilter->SetInputData(ringSkeleton);
    splineFilter->SetSubdivideToSpecified();
    splineFilter->SetNumberOfSubdivisions(100); // Increase this value for smoother results
    splineFilter->Update();
    vtkSmartPointer<vtkTubeFilter> ringSource;
    // Create the tube filter
    ringSource = vtkSmartPointer<vtkTubeFilter>::New();
    ringSource->SetInputConnection(splineFilter->GetOutputPort());
    ringSource->SetNumberOfSides(50); // Increase this value for a smoother tube
    ringSource->SetRadius(0.60);
    ringSource->Update();


    return disk;
  }

  vtkSmartPointer<vtkTubeFilter> Create2DRing(
    mitk::Vector3D axis, mitk::Vector3D viewRight, mitk::Vector3D viewUp, mitk::Point3D center, double radius)

  {
    // build the axis itself (as a tube around the line defining the axis)
    vtkSmartPointer<vtkPolyData> disk = vtkSmartPointer<vtkPolyData>::New();

    mitk::Vector3D ringPointer;
    unsigned int numberOfRingPoints = 36;
    vtkSmartPointer<vtkPoints> ringPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> ringSkeleton = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> ringPoly = vtkSmartPointer<vtkCellArray>::New();
    ringPoly->InsertNextCell(numberOfRingPoints + 1);
    for (unsigned int segment = 0; segment < numberOfRingPoints; ++segment)
    {
      double x = std::cos((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());
      double y = std::sin((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());

      ringPointer = viewRight * x + viewUp * y;

      ringPoints->InsertPoint(segment, (center + ringPointer * radius).GetDataPointer());
      ringPoly->InsertCellPoint(segment);
    }
    ringPoly->InsertCellPoint(0);
    // transform ring points (copied from vtkConeSource)
    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    // t->Translate(center.GetDataPointer());
    double vMag = vtkMath::Norm(axis.GetDataPointer());
    if (axis[0] == -1)
    {
      // flip x -> -x to avoid instability
      // t->RotateWXYZ(180, (axis[0] - vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
      //  t->RotateWXYZ(-180.0, 0, 1, 0);
      //t->Translate(0, -3, -4.5);
    }
    else if (axis[1] == 1)
    {
      // t->RotateWXYZ(180.0, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
      //t->Translate(-3, -4, -4);
    }
    else if (axis[2] == 1)
    {
      //  t->RotateWXYZ(180.0, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
     // t->Translate(-3, 4, -2);
    }

    double thisPoint[3];
    for (unsigned int i = 0; i < numberOfRingPoints; ++i)
    {
      ringPoints->GetPoint(i, thisPoint);
      t->TransformPoint(thisPoint, thisPoint);
      ringPoints->SetPoint(i, thisPoint);
    }
    disk->SetPoints(ringPoints);
    disk->SetPolys(ringPoly);

    ringSkeleton->SetPoints(ringPoints);
    ringSkeleton->SetLines(ringPoly);

    // Use vtkSplineFilter to smooth the input polydata
    vtkSmartPointer<vtkSplineFilter> splineFilter = vtkSmartPointer<vtkSplineFilter>::New();
    splineFilter->SetInputData(ringSkeleton);
    splineFilter->SetSubdivideToSpecified();
    splineFilter->SetNumberOfSubdivisions(100); // Increase this value for smoother results
    splineFilter->Update();
    vtkSmartPointer<vtkTubeFilter> ringSource;
    // Create the tube filter
    ringSource = vtkSmartPointer<vtkTubeFilter>::New();
    ringSource->SetInputConnection(splineFilter->GetOutputPort());
    ringSource->SetNumberOfSides(50); // Increase this value for a smoother tube
    ringSource->SetRadius(0.5);
    ringSource->Update();

    return ringSource;
  }

  vtkSmartPointer<vtkTubeFilter> Create2DScale(mitk::Vector3D axis, mitk::Point3D center)
  {
    vtkSmartPointer<vtkTubeFilter> ringSource; // used after if block, so declare it here
    const double shaftRadius = 0.04;
    // build the ring orthogonal to the axis (as another tube)
    vtkSmartPointer<vtkPolyData> ringSkeleton = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> ringPoints = vtkSmartPointer<vtkPoints>::New();
    ringPoints->SetDataTypeToDouble(); // just some decision (see cast below)
    unsigned int numberOfRingPoints = 50;
    vtkSmartPointer<vtkCellArray> ringLines = vtkSmartPointer<vtkCellArray>::New();
    ringLines->InsertNextCell(numberOfRingPoints + 1);
    mitk::Vector3D ringPointer;
    for (unsigned int segment = 0; segment < numberOfRingPoints; ++segment)
    {
      ringPointer[0] = 0;
     
      ringPointer[1] = ((double)(segment) / (double)numberOfRingPoints * 2.0);
     
      ringPointer[2] = ((double)(segment) / (double)numberOfRingPoints * 2.0);

      ringPoints->InsertPoint(segment, (ringPointer * 0.5).GetDataPointer());

      ringLines->InsertCellPoint(segment);
    }
    ringLines->InsertCellPoint(0);

    // transform ring points (copied from vtkConeSource)
    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    t->Translate(center.GetDataPointer());
    double vMag = vtkMath::Norm(axis.GetDataPointer());
    if (axis[0] == -1)
    {
      // flip x -> -x to avoid instability
      t->RotateWXYZ(180, (axis[0] - vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
      t->RotateWXYZ(-180, 1, 0, 0);
      t->Translate(-1, -1, -5);
    }
    else if (axis[1] == 1)
    {
      t->RotateWXYZ(180, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
      t->RotateWXYZ(-180, 1, 0, 0);
      t->Translate(-1, 1, -4.5);
    }
    else if (axis[2] == 1)
    {
      t->RotateWXYZ(180, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
      t->RotateWXYZ(-180, 0, 1, 1);
      t->Translate(-1, -2, -4.5);
    }

    double thisPoint[3];
    for (unsigned int i = 0; i < numberOfRingPoints; ++i)
    {
      ringPoints->GetPoint(i, thisPoint);
      t->TransformPoint(thisPoint, thisPoint);
      ringPoints->SetPoint(i, thisPoint);
    }

    ringSkeleton->SetPoints(ringPoints);
    ringSkeleton->SetLines(ringLines);

    // Use vtkSplineFilter to smooth the input polydata
    vtkSmartPointer<vtkSplineFilter> splineFilter = vtkSmartPointer<vtkSplineFilter>::New();
    splineFilter->SetInputData(ringSkeleton);
    splineFilter->SetSubdivideToSpecified();
    splineFilter->SetNumberOfSubdivisions(100); // Increase this value for smoother results
    splineFilter->Update();

    // Create the tube filter
    ringSource = vtkSmartPointer<vtkTubeFilter>::New();
    ringSource->SetInputConnection(splineFilter->GetOutputPort());
    ringSource->SetNumberOfSides(50); // Increase this value for a smoother tube
    ringSource->SetRadius(0.8);
    ringSource->Update();
    return ringSource;
  }

  //! Helper method: will create a vtkPolyData representing a 2D arrow
  //! that is oriented from arrowStart to arrowTip, orthogonal
  //! to camera direction. The arrow tip will contain scalar values
  //! of vertexValueScale, the arrow shaft will contain scalar values
  //! of vertexValueMove. Those values are used for picking during interaction.
  vtkSmartPointer<vtkPolyData> Create2DArrow(mitk::Vector3D cameraDirection,
                                             mitk::Point3D arrowStart,
                                             mitk::Point3D arrowTip,
                                             int vertexValueMove,
                                             int vertexValueScale)
  {
    mitk::Vector3D arrowDirection = arrowTip - arrowStart;
    mitk::Vector3D arrowOrthogonal = itk::CrossProduct(cameraDirection, arrowDirection);
    arrowOrthogonal.Normalize();

    double triangleFraction = 0.2;

    vtkSmartPointer<vtkPolyData> arrow = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    // shaft : points 0, 1
    points->InsertPoint(0, arrowStart.GetDataPointer());
    points->InsertPoint(1, (arrowStart + arrowDirection * (1.0 - triangleFraction)).GetDataPointer());

    // tip : points 2, 3, 4
    points->InsertPoint(2, arrowTip.GetDataPointer());
    points->InsertPoint(3,
                        (arrowStart + (1.0 - triangleFraction) * arrowDirection +
                         arrowOrthogonal * (0.5 * triangleFraction * arrowDirection.GetNorm()))
                          .GetDataPointer());
    points->InsertPoint(4,
                        (arrowStart + (1.0 - triangleFraction) * arrowDirection -
                         arrowOrthogonal * (0.5 * triangleFraction * arrowDirection.GetNorm()))
                          .GetDataPointer());
   
    arrow->SetPoints(points);

    // define line connection for shaft
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints[] = {0, 1};
    lines->InsertNextCell(2, shaftLinePoints);
    //arrow->SetLines(lines);

    // define polygon for triangle
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType tipLinePoints[] = {2, 3, 4};
    polys->InsertNextCell(3, tipLinePoints);
    arrow->SetPolys(polys);

    // assign scalar values
    vtkSmartPointer<vtkCharArray> pointData = vtkSmartPointer<vtkCharArray>::New();
    pointData->SetNumberOfComponents(1);
    pointData->SetNumberOfTuples(5);
    pointData->FillComponent(0, vertexValueScale);
    pointData->SetTuple1(0, vertexValueMove);
    pointData->SetTuple1(1, vertexValueMove);
    arrow->GetPointData()->SetScalars(pointData);
  
    return arrow;
  }
} // namespace

vtkPolyData *mitk::Gizmo_noscaleMapper2D::GetVtkPolyData(mitk::BaseRenderer *renderer)
{
  return m_LSH.GetLocalStorage(renderer)->m_VtkPolyDataMapper->GetInput();
}

void mitk::Gizmo_noscaleMapper2D::GenerateDataForRenderer(mitk::BaseRenderer *renderer)
{
  auto gizmo_noscale = GetInput();
  auto node = GetDataNode();

  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);

  // check if something important has changed and we need to re-render
  if ((ls->m_LastUpdateTime >= node->GetMTime()) && (ls->m_LastUpdateTime >= gizmo_noscale->GetPipelineMTime()) &&
      (ls->m_LastUpdateTime >= renderer->GetCameraController()->GetMTime()) &&
      (ls->m_LastUpdateTime >= renderer->GetCurrentWorldPlaneGeometryUpdateTime()) &&
      (ls->m_LastUpdateTime >= renderer->GetCurrentWorldPlaneGeometry()->GetMTime()) &&
      (ls->m_LastUpdateTime >= node->GetPropertyList()->GetMTime()) &&
      (ls->m_LastUpdateTime >= node->GetPropertyList(renderer)->GetMTime()))
  {
    return;
  }

  ls->m_LastUpdateTime.Modified();
  // 获取可移动、可旋转和可缩放属性

  // 初始化变量，设置默认值
  bool moveable = true;
  bool rotatable = true;
  bool scalable = true;

  // 尝试获取布尔属性，检查是否成功
  //if (!node->GetBoolProperty("Moveable", moveable))
  //{
  //  std::cout << "Moveable property not found, using default value: " << moveable << std::endl;
  //}

  //if (!node->GetBoolProperty("Rotatable", rotatable))
  //{
  //  std::cout << "Rotatable property not found, using default value: " << rotatable << std::endl;
  //}

  //if (!node->GetBoolProperty("Scalable", scalable))
  //{
  //  std::cout << "Scalable property not found, using default value: " << scalable << std::endl;
  //}
  node->GetBoolProperty("Moveable", moveable);
  node->GetBoolProperty("Rotatable", rotatable);
  node->GetBoolProperty("Scalable", scalable);
   
  //std::cout << "2d" << std::endl;
  //std::cout << "Moveable is " << moveable << std::endl;
  //std::cout << "Rotatable is " << rotatable << std::endl;
  //std::cout << "Scalable is " << scalable << std::endl;
  // some special handling around visibility: let two properties steer
  // visibility in 2D instead of many renderer specific "visible" properties
  bool visible2D = true;
  this->GetDataNode()->GetBoolProperty("show in 2D", visible2D);
  if (!visible2D && renderer->GetMapperID() == BaseRenderer::Standard2D)
  {
    ls->m_Actor->VisibilityOff();
    return;
  }
  else
  {
    ls->m_Actor->VisibilityOn();
  }

  auto camera = renderer->GetVtkRenderer()->GetActiveCamera();

  auto plane = renderer->GetCurrentWorldPlaneGeometry();

  Point3D gizmo_noscaleCenterView = plane->ProjectPointOntoPlane(gizmo_noscale->GetCenter());
  //std::cout << "gizmoCenterView:" << std::endl;
  //std::cout << gizmoCenterView[0] << std::endl;
  //std::cout << gizmoCenterView[1] << std::endl;
  //std::cout << gizmoCenterView[2] << std::endl;
  Vector3D viewUp;
  camera->GetViewUp(viewUp.GetDataPointer());
  Vector3D cameraDirection;
  camera->GetDirectionOfProjection(cameraDirection.GetDataPointer());
  //std::cout << "cameraDirection"<< std::endl;
  //std::cout << cameraDirection[0] << std::endl;
  //std::cout << cameraDirection[1] << std::endl;
  //std::cout << cameraDirection[2] << std::endl;

  Vector3D viewRight = itk::CrossProduct(viewUp, cameraDirection);
  Vector3D viewCam = itk::CrossProduct(viewUp, viewRight);
  auto appender = vtkSmartPointer<vtkAppendPolyData>::New();

  double diagonal = std::min(renderer->GetSizeX(), renderer->GetSizeY()) * renderer->GetScaleFactorMMPerDisplayUnit();
  double arrowLength = 0.3 * diagonal; // fixed in relation to window size
  auto disk = Create2DDisk(cameraDirection,
                           viewRight,
                           viewUp, gizmo_noscaleCenterView - cameraDirection,
                           0.1 * arrowLength);

  AssignScalarValueTo(disk, Gizmo_noscale::MoveFreely);
  if (moveable)
  {
    appender->AddInputData(disk);
  }
  
  auto center = gizmo_noscaleCenterView - cameraDirection;
  auto center_move = center;

  // 旋转部分
  auto ringSource_X =
    Create2DRing(cameraDirection, viewRight, viewUp, gizmo_noscaleCenterView - cameraDirection, 0.3 * arrowLength);
  ringSource_X->Update();
  AssignScalarValueTo(ringSource_X->GetOutput(), Gizmo_noscale::RotateAroundAxisZ);
  if (rotatable)
  {
    appender->AddInputData(ringSource_X->GetOutput());
  }
  

  // 缩放部分
  //auto scaleLine = Create2DScale(cameraDirection, center);
  //scaleLine->Update();
  //AssignScalarValueTo(scaleLine->GetOutput(), Gizmo::ScaleY);
  //appender->AddInputData(scaleLine->GetOutput());
  // loop over directions -1 and +1 for arrows
  if (scalable)
  {
  
   for (double direction = -1.0; direction < 2.0; direction += 2.0)
  {
   auto axisX =
    Create2DArrow(cameraDirection,
                    gizmo_noscaleCenterView,
      plane->ProjectPointOntoPlane(gizmo_noscale->GetCenter() + (gizmo_noscale->GetAxisX() * arrowLength) * direction),
                    Gizmo_noscale::MoveAlongAxisX,
      Gizmo_noscale::ScaleX);
   appender->AddInputData(axisX);

    auto axisY =
      Create2DArrow(cameraDirection,
                   gizmo_noscaleCenterView,
     plane->ProjectPointOntoPlane(gizmo_noscale->GetCenter() + (gizmo_noscale->GetAxisY() * arrowLength) * direction),
                   Gizmo_noscale::MoveAlongAxisY,
     Gizmo_noscale::ScaleY);
    appender->AddInputData(axisY);

    auto axisZ =
      Create2DArrow(cameraDirection,
                    gizmo_noscaleCenterView,
      plane->ProjectPointOntoPlane(gizmo_noscale->GetCenter() + (gizmo_noscale->GetAxisZ() * arrowLength) * direction),
                    Gizmo_noscale::MoveAlongAxisZ,
                    Gizmo_noscale::ScaleZ);
    appender->AddInputData(axisZ);
  }
  }
  ls->m_VtkPolyDataMapper->SetInputConnection(appender->GetOutputPort());

  ApplyVisualProperties(renderer);
  
}

void mitk::Gizmo_noscaleMapper2D::ApplyVisualProperties(BaseRenderer *renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);

  float lineWidth = 3.0f;
  ls->m_Actor->GetProperty()->SetLineWidth(lineWidth);

  mitk::LookupTableProperty::Pointer lookupTableProp;
  this->GetDataNode()->GetProperty(lookupTableProp, "LookupTable", renderer);
  if (lookupTableProp.IsNotNull())
  {
    ls->m_VtkPolyDataMapper->SetLookupTable(lookupTableProp->GetLookupTable()->GetVtkLookupTable());
  }

  bool scalarVisibility = false;
  this->GetDataNode()->GetBoolProperty("scalar visibility", scalarVisibility);
  ls->m_VtkPolyDataMapper->SetScalarVisibility((scalarVisibility ? 1 : 0));

  if (scalarVisibility)
  {
    mitk::VtkScalarModeProperty *scalarMode;
    if (this->GetDataNode()->GetProperty(scalarMode, "scalar mode", renderer))
      ls->m_VtkPolyDataMapper->SetScalarMode(scalarMode->GetVtkScalarMode());
    else
      ls->m_VtkPolyDataMapper->SetScalarModeToDefault();

    bool colorMode = false;
    this->GetDataNode()->GetBoolProperty("color mode", colorMode);
    ls->m_VtkPolyDataMapper->SetColorMode((colorMode ? 1 : 0));

    double scalarsMin = 0;
    this->GetDataNode()->GetDoubleProperty("ScalarsRangeMinimum", scalarsMin, renderer);

    double scalarsMax = 1.0;
    this->GetDataNode()->GetDoubleProperty("ScalarsRangeMaximum", scalarsMax, renderer);

    ls->m_VtkPolyDataMapper->SetScalarRange(scalarsMin, scalarsMax);
  }
  this->GetDataNode()->SetProperty("layer", mitk::IntProperty::New(65536));
}

void mitk::Gizmo_noscaleMapper2D::SetDefaultProperties(mitk::DataNode *node,
                                               mitk::BaseRenderer *renderer /*= nullptr*/,
                                               bool /*= false*/)
{
  node->SetProperty("color", ColorProperty::New(0.3, 0.3, 0.3)); // a little lighter than the
                                                                 // "plane widgets" of
                                                                 // QmitkStdMultiWidget
  node->SetProperty("scalar visibility", BoolProperty::New(true), renderer);
  node->SetProperty("ScalarsRangeMinimum", DoubleProperty::New(0), renderer);
  node->SetProperty("ScalarsRangeMaximum", DoubleProperty::New((int)Gizmo_noscale::NoHandle), renderer);

  double colorMoveFreely[] = {1, 0, 0, 1}; // RGBA
  double colorAxisX[] = {0.753, 0, 0, 1};  // colors copied from QmitkStdMultiWidget to
  double colorAxisY[] = {0, 0.69, 0, 1};   // look alike
  double colorAxisZ[] = {0, 0.502, 1, 1};
  double colorInactive[] = {0.7, 0.7, 0.7, 1};

  // build a nice color table
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues((int)Gizmo_noscale::NoHandle + 1);
  lut->SetTableRange(0, (int)Gizmo_noscale::NoHandle);
  lut->SetTableValue(Gizmo_noscale::MoveFreely, colorMoveFreely);
  lut->SetTableValue(Gizmo_noscale::MoveAlongAxisX, colorAxisX);
  lut->SetTableValue(Gizmo_noscale::MoveAlongAxisY, colorAxisY);
  lut->SetTableValue(Gizmo_noscale::MoveAlongAxisZ, colorAxisZ);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisX, colorAxisX);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisY, colorAxisY);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisZ, colorAxisZ);
  lut->SetTableValue(Gizmo_noscale::ScaleX, colorAxisX);
  lut->SetTableValue(Gizmo_noscale::ScaleY, colorAxisY);
  lut->SetTableValue(Gizmo_noscale::ScaleZ, colorAxisZ);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisX3D, colorAxisX);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisY3D, colorAxisY);
  lut->SetTableValue(Gizmo_noscale::RotateAroundAxisZ3D, colorAxisZ);
  lut->SetTableValue(Gizmo_noscale::NoHandle, colorInactive);

  mitk::LookupTable::Pointer mlut = mitk::LookupTable::New();
  mlut->SetVtkLookupTable(lut);

  mitk::LookupTableProperty::Pointer lutProp = mitk::LookupTableProperty::New();
  lutProp->SetLookupTable(mlut);
  node->SetProperty("LookupTable", lutProp, renderer);

  node->SetProperty("helper object", BoolProperty::New(true), renderer);
  node->SetProperty("visible", BoolProperty::New(true), renderer);
  node->SetProperty("show in 2D", BoolProperty::New(true), renderer);
  // no "show in 3D" because this would require a specialized mapper for gizmos in 3D
}
