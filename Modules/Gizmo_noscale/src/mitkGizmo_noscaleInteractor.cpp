/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkGizmo_noscaleInteractor.h"
#include "mitkGizmo_noscaleMapper2D.h"

// MITK includes
#include <mitkInteractionConst.h>
#include <mitkInteractionPositionEvent.h>
#include <mitkInternalEvent.h>
#include <mitkLookupTableProperty.h>
#include <mitkOperationEvent.h>
#include <mitkRotationOperation.h>
#include <mitkScaleOperation.h>
#include <mitkSurface.h>
#include <mitkUndoController.h>
#include <mitkVtkMapper.h>

// VTK includes
#include "mitkMatrixConvert.h"
#include <vtkCamera.h>
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyle.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

mitk::Gizmo_noscaleInteractor::Gizmo_noscaleInteractor()
{
    m_ColorForHighlight[0] = 1.0;
    m_ColorForHighlight[1] = 0.5;
    m_ColorForHighlight[2] = 0.0;
    m_ColorForHighlight[3] = 1.0;

    // TODO if we want to get this configurable, the this is the recipe:
    // - make the 2D mapper add corresponding properties to control "enabled" and "color"
    // - make the interactor evaluate those properties
    // - in an ideal world, modify the state machine on the fly and skip mouse move handling
}

mitk::Gizmo_noscaleInteractor::~Gizmo_noscaleInteractor()
{
}

void mitk::Gizmo_noscaleInteractor::ConnectActionsAndFunctions()
{
  CONNECT_CONDITION("PickedHandle", HasPickedHandle);

  CONNECT_FUNCTION("DecideInteraction", DecideInteraction);
  CONNECT_FUNCTION("MoveAlongAxis", MoveAlongAxis);
  CONNECT_FUNCTION("RotateAroundAxis", RotateAroundAxis);
  CONNECT_FUNCTION("MoveFreely", MoveFreely);
  CONNECT_FUNCTION("ScaleEqually", ScaleEqually);
  CONNECT_FUNCTION("FeedUndoStack", FeedUndoStack);
  CONNECT_FUNCTION("MoveAlongAxis3D", MoveAlongAxis3D);
  CONNECT_FUNCTION("RotateAroundAxis3D", RotateAroundAxis3D);
}

void mitk::Gizmo_noscaleInteractor::SetGizmo_noscaleNode(DataNode *node)
{
  DataInteractor::SetDataNode(node);

  m_Gizmo_noscale = dynamic_cast<Gizmo_noscale *>(node->GetData());

  // setup picking from just this object
  m_Picker.clear();
}

void mitk::Gizmo_noscaleInteractor::SetManipulatedObjectNode(DataNode *node)
{
  if (node && node->GetData())
  {
    m_ManipulatedObjectGeometry = node->GetData()->GetGeometry();
  }
}

bool mitk::Gizmo_noscaleInteractor::HasPickedHandle(const InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr ||
      m_Gizmo_noscale.IsNull() ||
      m_ManipulatedObjectGeometry.IsNull() ||
      interactionEvent->GetSender()->GetRenderWindow()->GetNeverRendered())
  {
    return false;
  }

  if (interactionEvent->GetSender()->GetMapperID() == BaseRenderer::Standard2D)
  {
    m_PickedHandle = PickFrom2D(positionEvent);
  }
  else
  {
    m_PickedHandle = PickFrom3D(positionEvent);
  }

  UpdateHandleHighlight();

  return m_PickedHandle != Gizmo_noscale::NoHandle;
}

void mitk::Gizmo_noscaleInteractor::DecideInteraction(StateMachineAction *, InteractionEvent *interactionEvent)
{
  assert(m_PickedHandle != Gizmo_noscale::NoHandle);

  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  // if something relevant was picked, we calculate a number of
  // important points and axes for the upcoming geometry manipulations

  // note initial state
  m_InitialClickPosition2D = positionEvent->GetPointerPositionOnScreen();
  m_InitialClickPosition3D = positionEvent->GetPositionInWorld();

  auto renderer = positionEvent->GetSender()->GetVtkRenderer();
  renderer->SetWorldPoint(m_InitialClickPosition3D[0], m_InitialClickPosition3D[1], m_InitialClickPosition3D[2], 0);
  renderer->WorldToDisplay();
  m_InitialClickPosition2DZ = renderer->GetDisplayPoint()[2];

  m_InitialGizmo_noscaleCenter3D = m_Gizmo_noscale->GetCenter();
  positionEvent->GetSender()->WorldToDisplay(m_InitialGizmo_noscaleCenter3D, m_InitialGizmo_noscaleCenter2D);

  m_InitialManipulatedObjectGeometry = m_ManipulatedObjectGeometry->Clone();

  switch ( m_PickedHandle ) {
  case Gizmo_noscale::MoveAlongAxisX:
  case Gizmo_noscale::RotateAroundAxisX:
  case Gizmo_noscale::RotateAroundAxisX3D:
  case Gizmo_noscale::ScaleX:
    m_AxisOfMovement = m_ManipulatedObjectGeometry->GetAxisVector(0);
    m_AxisOfMovement3D = m_InitialManipulatedObjectGeometry->GetAxisVector(0);
      break;
  case Gizmo_noscale::MoveAlongAxisY:
  case Gizmo_noscale::RotateAroundAxisY:
  case Gizmo_noscale::RotateAroundAxisY3D:
  case Gizmo_noscale::ScaleY:
    m_AxisOfMovement = m_ManipulatedObjectGeometry->GetAxisVector(1);
    m_AxisOfMovement3D = m_InitialManipulatedObjectGeometry->GetAxisVector(1);
      break;
  case Gizmo_noscale::MoveAlongAxisZ:
  case Gizmo_noscale::RotateAroundAxisZ:
  case Gizmo_noscale::RotateAroundAxisZ3D:
  case Gizmo_noscale::ScaleZ:
    m_AxisOfMovement = m_ManipulatedObjectGeometry->GetAxisVector(2);
    m_AxisOfMovement3D = m_InitialManipulatedObjectGeometry->GetAxisVector(2);
      break;

  default:
      break;
  }
  m_AxisOfMovement.Normalize();
  m_AxisOfMovement3D.Normalize();
  m_AxisOfRotation3D = m_AxisOfMovement3D;
  // for translation: test whether the user clicked into the "object's real" axis direction
  //                  or into the other one
  Vector3D intendedAxis = m_InitialClickPosition3D - m_InitialGizmo_noscaleCenter3D;

  if ( intendedAxis * m_AxisOfMovement < 0 ) {
      m_AxisOfMovement *= -1.0;
  }

    if (intendedAxis * m_AxisOfMovement3D < 0)
  {
    m_AxisOfMovement3D *= -1.0;
  }

  // for rotation: test whether the axis of rotation is more looking in the direction
  //               of the camera or in the opposite
  vtkCamera *camera = renderer->GetActiveCamera();
  vtkVector3d cameraDirection(camera->GetDirectionOfProjection());
  m_AxisOfRotation = camera->GetDirectionOfProjection();
  
  double angle_rad = vtkMath::AngleBetweenVectors(cameraDirection.GetData(), m_AxisOfRotation.GetDataPointer());
  double angle_rad3D = vtkMath::AngleBetweenVectors(cameraDirection.GetData(), m_AxisOfRotation3D.GetDataPointer());
  if ( angle_rad < vtkMath::Pi() / 2.0 ) {
      m_AxisOfRotation *= -1.0;
  }

    if (angle_rad3D < vtkMath::Pi() / 2.0)
  {
    m_AxisOfRotation3D *= -1.0;
  }

  InternalEvent::Pointer decision;
  switch (m_PickedHandle)
  {
    case Gizmo_noscale::MoveAlongAxisX:
    case Gizmo_noscale::MoveAlongAxisY:
    case Gizmo_noscale::MoveAlongAxisZ:
      decision = InternalEvent::New(interactionEvent->GetSender(), this, "StartTranslationAlongAxis");
      break;
    case Gizmo_noscale::RotateAroundAxisX:
    case Gizmo_noscale::RotateAroundAxisY:
    case Gizmo_noscale::RotateAroundAxisZ:
      decision = InternalEvent::New(interactionEvent->GetSender(), this, "StartRotationAroundAxis");
      break;
    case Gizmo_noscale::MoveFreely:
      decision = InternalEvent::New(interactionEvent->GetSender(), this, "MoveFreely");
      break;
    case Gizmo_noscale::ScaleX:
    case Gizmo_noscale::ScaleY:
    case Gizmo_noscale::ScaleZ:
      // Implementation note: Why didn't we implement per-axis scaling yet?
      // When this was implemented, the mitk::ScaleOperation was able to only describe
      // uniform scaling around a central point. Since we use operations for all modifications
      // in order to have undo/redo working, any axis-specific scaling should also
      // use operations.
      // Consequence: enhance ScaleOperation when there is need to have scaling per axis.
      decision = InternalEvent::New(interactionEvent->GetSender(), this, "ScaleEqually");
      break;
    case Gizmo_noscale::RotateAroundAxisX3D:
    case Gizmo_noscale::RotateAroundAxisY3D:
    case Gizmo_noscale::RotateAroundAxisZ3D:
      decision = InternalEvent::New(interactionEvent->GetSender(), this, "StartRotationAroundAxis3D");
      break;

    default:
      break;
  }

  interactionEvent->GetSender()->GetDispatcher()->QueueEvent(decision);
}

void mitk::Gizmo_noscaleInteractor::MoveAlongAxis(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();

  // re-use the initial z value to calculate movements parallel to the camera plane
  auto renderer = positionEvent->GetSender()->GetVtkRenderer();
  renderer->SetDisplayPoint(currentPosition2D[0], currentPosition2D[1], m_InitialClickPosition2DZ);
  renderer->DisplayToWorld();
  vtkVector3d worldPointVTK(renderer->GetWorldPoint());
  Point3D worldPointITK(worldPointVTK.GetData());
  Vector3D freeMouseMovement3D(worldPointITK - m_InitialClickPosition3D);

  double projectedMouseMovement3D = freeMouseMovement3D * m_AxisOfMovement;
  Vector3D appliedMovement3D = projectedMouseMovement3D * m_AxisOfMovement;

  ApplyTranslationToManipulatedObject(appliedMovement3D);
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::MoveAlongAxis3D(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();

  // re-use the initial z value to calculate movements parallel to the camera plane
  auto renderer = positionEvent->GetSender()->GetVtkRenderer();
  renderer->SetDisplayPoint(currentPosition2D[0], currentPosition2D[1], m_InitialClickPosition2DZ);
  renderer->DisplayToWorld();
  vtkVector3d worldPointVTK(renderer->GetWorldPoint());
  Point3D worldPointITK(worldPointVTK.GetData());
  Vector3D freeMouseMovement3D(worldPointITK - m_InitialClickPosition3D);

  double projectedMouseMovement3D = freeMouseMovement3D * m_AxisOfMovement3D;
  Vector3D appliedMovement3D = projectedMouseMovement3D * m_AxisOfMovement3D;

  ApplyTranslationToManipulatedObject3D(appliedMovement3D);
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::RotateAroundAxis(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Vector2D originalVector = m_InitialClickPosition2D - m_InitialGizmo_noscaleCenter2D;
  Vector2D currentVector = positionEvent->GetPointerPositionOnScreen() - m_InitialGizmo_noscaleCenter2D;

  originalVector.Normalize();
  currentVector.Normalize();

  double angle_rad = std::atan2(currentVector[1], currentVector[0]) - std::atan2(originalVector[1], originalVector[0]);

  ApplyRotationToManipulatedObject(vtkMath::DegreesFromRadians(angle_rad));
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::RotateAroundAxis3D(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Vector2D originalVector = m_InitialClickPosition2D - m_InitialGizmo_noscaleCenter2D;
  Vector2D currentVector = positionEvent->GetPointerPositionOnScreen() - m_InitialGizmo_noscaleCenter2D;

  originalVector.Normalize();
  currentVector.Normalize();

  double angle_rad = std::atan2(currentVector[1], currentVector[0]) - std::atan2(originalVector[1], originalVector[0]);

  ApplyRotationToManipulatedObject3D(vtkMath::DegreesFromRadians(angle_rad));
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::MoveFreely(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();

  // re-use the initial z value to really move parallel to the camera plane
  auto renderer = positionEvent->GetSender()->GetVtkRenderer();
  renderer->SetDisplayPoint(currentPosition2D[0], currentPosition2D[1], m_InitialClickPosition2DZ);
  renderer->DisplayToWorld();
  vtkVector3d worldPointVTK(renderer->GetWorldPoint());
  Point3D worldPointITK(worldPointVTK.GetData());
  Vector3D movementITK(worldPointITK - m_InitialClickPosition3D);

  ApplyTranslationToManipulatedObject(movementITK);
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::ScaleEqually(StateMachineAction *, InteractionEvent *interactionEvent)
{
  auto positionEvent = dynamic_cast<const InteractionPositionEvent *>(interactionEvent);
  if (positionEvent == nullptr)
  {
    return;
  }

  Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();
  double relativeSize = (currentPosition2D - m_InitialGizmo_noscaleCenter2D).GetNorm() /
                        (m_InitialClickPosition2D - m_InitialGizmo_noscaleCenter2D).GetNorm();

  // ApplyEqualScalingToManipulatedObject(relativeSize);

  ApplyXScalingToManipulatedObject(relativeSize);
  RenderingManager::GetInstance()->ForceImmediateUpdateAll();
}

void mitk::Gizmo_noscaleInteractor::ApplyTranslationToManipulatedObject(const Vector3D &translation)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());

  auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();
  m_FinalDoOperation.reset(new PointOperation(OpMOVE, Point3D(translation)));
  if (m_UndoEnabled)
  {
    m_FinalUndoOperation.reset(new PointOperation(OpMOVE, Point3D(-translation)));
  }

  manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
}

void mitk::Gizmo_noscaleInteractor::ApplyTranslationToManipulatedObject3D(const Vector3D &translation)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());

  auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();
  m_FinalDoOperation.reset(new PointOperation(OpMOVE, Point3D(translation)));
  if (m_UndoEnabled)
  {
    m_FinalUndoOperation.reset(new PointOperation(OpMOVE, Point3D(-translation)));
  }

  manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
}

void mitk::Gizmo_noscaleInteractor::ApplyEqualScalingToManipulatedObject(double scalingFactor)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());
  auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();

  m_FinalDoOperation.reset(new ScaleOperation(OpSCALE, scalingFactor - 1.0, m_InitialGizmo_noscaleCenter3D));
  if (m_UndoEnabled)
  {
    m_FinalUndoOperation.reset(new ScaleOperation(OpSCALE, -(scalingFactor - 1.0), m_InitialGizmo_noscaleCenter3D));
  }

  manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
}

void mitk::Gizmo_noscaleInteractor::ApplyXScalingToManipulatedObject(double scalingFactor)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());

  // Obtain the rotation matrix from the initial object transform
  auto initTransMatrix = vtkMatrix4x4::New();

  auto rotMatrix = vtkMatrix4x4::New();
  rotMatrix->Identity();

  auto scaleMatrix = vtkMatrix4x4::New();
  scaleMatrix->Identity();

  auto translateMatrix = vtkMatrix4x4::New();
  translateMatrix->Identity();
  translateMatrix->SetElement(0, 3, m_InitialGizmo_noscaleCenter3D[0]);
  translateMatrix->SetElement(1, 3, m_InitialGizmo_noscaleCenter3D[1]);
  translateMatrix->SetElement(2, 3, m_InitialGizmo_noscaleCenter3D[2]);

  auto initItkTrans = m_InitialManipulatedObjectGeometry->GetIndexToWorldTransform();

  mitk::TransferItkTransformToVtkMatrix(initItkTrans, initTransMatrix);

  Eigen::Vector3d x;
  Eigen::Vector3d y;
  Eigen::Vector3d z;

  for (int i{0}; i < 3; i++)
  {
    x[i] = initTransMatrix->GetElement(i, 0);
    y[i] = initTransMatrix->GetElement(i, 1);
    z[i] = initTransMatrix->GetElement(i, 2);
  }

  double x_scale = 1;
  double y_scale = 1;
  double z_scale = 1;

  switch (m_PickedHandle)
  {
    case Gizmo_noscale::ScaleX:
      x_scale *= scalingFactor;
      break;
    case Gizmo_noscale::ScaleY:
      y_scale *= scalingFactor;
      break;
    case Gizmo_noscale::ScaleZ:
      z_scale *= scalingFactor;
      break;
  }

  scaleMatrix->SetElement(0, 0, x_scale);
  scaleMatrix->SetElement(1, 1, y_scale);
  scaleMatrix->SetElement(2, 2, z_scale);

  x.normalize();
  y.normalize();
  z.normalize();

  for (int i{0}; i < 3; i++)
  {
    rotMatrix->SetElement(i, 0, x[i]);
    rotMatrix->SetElement(i, 1, y[i]);
    rotMatrix->SetElement(i, 2, z[i]);
  }

  auto inverseRotMatrix = vtkMatrix4x4::New();
  auto inverseTranslateMatrix = vtkMatrix4x4::New();

  inverseRotMatrix->DeepCopy(rotMatrix);
  inverseRotMatrix->Invert();
  inverseTranslateMatrix->DeepCopy(translateMatrix);
  inverseTranslateMatrix->Invert();

  auto tmpTrans = vtkTransform::New();
  tmpTrans->Identity();
  tmpTrans->PostMultiply();
  tmpTrans->Concatenate(initTransMatrix);
  tmpTrans->Concatenate(inverseTranslateMatrix);
  tmpTrans->Concatenate(inverseRotMatrix);
  tmpTrans->Concatenate(scaleMatrix);
  tmpTrans->Concatenate(rotMatrix);
  tmpTrans->Concatenate(translateMatrix);
  tmpTrans->Update();

  auto finalAffineTrans = mitk::AffineTransform3D::New();

  mitk::TransferVtkMatrixToItkTransform(tmpTrans->GetMatrix(), finalAffineTrans.GetPointer());

  // m_ManipulatedObjectGeometry->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix()); // if use this line, Gizmo
  // cannot be updated in real time
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(finalAffineTrans);
}

void mitk::Gizmo_noscaleInteractor::ApplyRotationToManipulatedObject(double angle_deg)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());

  auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();

  m_FinalDoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmo_noscaleCenter3D, m_AxisOfRotation, angle_deg));
  if (m_UndoEnabled)
  {
    m_FinalUndoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmo_noscaleCenter3D, m_AxisOfRotation, -angle_deg));
  }

  manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
}

void mitk::Gizmo_noscaleInteractor::ApplyRotationToManipulatedObject3D(double angle_deg)
{
  assert(m_ManipulatedObjectGeometry.IsNotNull());

  auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();

  m_FinalDoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmo_noscaleCenter3D, m_AxisOfRotation3D, angle_deg));
  if (m_UndoEnabled)
  {
    m_FinalUndoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmo_noscaleCenter3D, m_AxisOfRotation3D, -angle_deg));
  }

  manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
  m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
}

void mitk::Gizmo_noscaleInteractor::FeedUndoStack(StateMachineAction *, InteractionEvent *interactionEvent)
{
  if (m_UndoEnabled)
  {
    OperationEvent *operationEvent = new OperationEvent(m_ManipulatedObjectGeometry,
                                                        // OperationEvent will destroy operations!
                                                        // --> release() and not get()
                                                        m_FinalDoOperation.release(),
                                                        m_FinalUndoOperation.release(),
                                                        "Direct geometry manipulation");
    mitk::OperationEvent::IncCurrObjectEventId(); // save each modification individually
    m_UndoController->SetOperationEvent(operationEvent);
  }
}

mitk::Gizmo_noscale::HandleType mitk::Gizmo_noscaleInteractor::PickFrom2D(const InteractionPositionEvent *positionEvent)
{
  BaseRenderer *renderer = positionEvent->GetSender();

  auto mapper = GetDataNode()->GetMapper(BaseRenderer::Standard2D);
  auto gizmo_noscale_mapper = dynamic_cast<Gizmo_noscaleMapper2D *>(mapper);
  auto &picker = m_Picker[renderer];

  if (picker == nullptr)
  {
    picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.02);

    if (gizmo_noscale_mapper)
    { // doing this each time is bizarre
      picker->AddPickList(gizmo_noscale_mapper->GetVtkProp(renderer));
      picker->PickFromListOn();
    }
  }

  auto displayPosition = positionEvent->GetPointerPositionOnScreen();
  picker->Pick(displayPosition[0], displayPosition[1], 0, positionEvent->GetSender()->GetVtkRenderer());

  vtkIdType pickedPointID = picker->GetPointId();
  if (pickedPointID == -1)
  {
    return Gizmo_noscale::NoHandle;
  }

  vtkPolyData *polydata = gizmo_noscale_mapper->GetVtkPolyData(renderer);

  if (polydata && polydata->GetPointData() && polydata->GetPointData()->GetScalars())
  {
    double dataValue = polydata->GetPointData()->GetScalars()->GetTuple1(pickedPointID);
    return m_Gizmo_noscale->GetHandleFromPointDataValue(dataValue);
  }

  return Gizmo_noscale::NoHandle;
}

mitk::Gizmo_noscale::HandleType mitk::Gizmo_noscaleInteractor::PickFrom3D(const InteractionPositionEvent *positionEvent)
{
  BaseRenderer *renderer = positionEvent->GetSender();
  auto &picker = m_Picker[renderer];
  if (picker == nullptr)
  {
    picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);
    auto mapper = GetDataNode()->GetMapper(BaseRenderer::Standard3D);
    auto vtk_mapper = dynamic_cast<VtkMapper *>(mapper);
    if (vtk_mapper)
    { // doing this each time is bizarre
      picker->AddPickList(vtk_mapper->GetVtkProp(renderer));
      picker->PickFromListOn();
    }
  }

  auto displayPosition = positionEvent->GetPointerPositionOnScreen();
  picker->Pick(displayPosition[0], displayPosition[1], 0, positionEvent->GetSender()->GetVtkRenderer());

  vtkIdType pickedPointID = picker->GetPointId();
  if (pickedPointID == -1)
  {
    return Gizmo_noscale::NoHandle;
  }

  // _something_ picked
  return m_Gizmo_noscale->GetHandleFromPointID(pickedPointID);
}

void mitk::Gizmo_noscaleInteractor::UpdateHandleHighlight()
{
  if (m_HighlightedHandle != m_PickedHandle) {

    auto node = GetDataNode();
    if (node == nullptr) return;

    auto base_prop = node->GetProperty("LookupTable");
    if (base_prop == nullptr) return;

    auto lut_prop = dynamic_cast<LookupTableProperty*>(base_prop);
    if (lut_prop == nullptr) return;

    auto lut = lut_prop->GetLookupTable();
    if (lut == nullptr) return;

    // Table size is expected to constructed as one entry per gizmo-part enum value
    assert(lut->GetVtkLookupTable()->GetNumberOfTableValues() > std::max(m_PickedHandle, m_HighlightedHandle));

    // Reset previously overwritten color
    if (m_HighlightedHandle != Gizmo_noscale::NoHandle)
    {
        lut->SetTableValue(m_HighlightedHandle, m_ColorReplacedByHighlight);
    }

    // Overwrite currently highlighted color
    if (m_PickedHandle != Gizmo_noscale::NoHandle)
    {
      lut->GetTableValue(m_PickedHandle, m_ColorReplacedByHighlight);
      lut->SetTableValue(m_PickedHandle, m_ColorForHighlight);
    }

    // Mark node modified to allow repaint
    node->Modified();
    RenderingManager::GetInstance()->RequestUpdateAll(RenderingManager::REQUEST_UPDATE_ALL);

    m_HighlightedHandle = m_PickedHandle;
  }
}
