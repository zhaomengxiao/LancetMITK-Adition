//#include "ModelInteractor.h"
//#include "ModelMapper2D.h"
//// MITK includes
//#include <mitkInteractionConst.h>
//#include <mitkInteractionPositionEvent.h>
//#include <mitkInternalEvent.h>
//#include <mitkLookupTableProperty.h>
//#include <mitkOperationEvent.h>
//#include <mitkRotationOperation.h>
//#include <mitkScaleOperation.h>
//#include <mitkSurface.h>
//#include <mitkUndoController.h>
//#include <mitkVtkMapper.h>
//
//// VTK includes
//#include <vtkCamera.h>
//#include <vtkInteractorObserver.h>
//#include <vtkInteractorStyle.h>
//#include <vtkMath.h>
//#include <vtkPointData.h>
//#include <vtkPolyData.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkVector.h>
//#include <vtkVectorOperators.h>
//
//ModelInteractor::ModelInteractor()
//{
//    m_ColorForHighlight[0] = 1.0;
//    m_ColorForHighlight[1] = 0.5;
//    m_ColorForHighlight[2] = 0.0;
//    m_ColorForHighlight[3] = 1.0;
//
//    // TODO if we want to get this configurable, the this is the recipe:
//    // - make the 2D mapper add corresponding properties to control "enabled" and "color"
//    // - make the interactor evaluate those properties
//    // - in an ideal world, modify the state machine on the fly and skip mouse move handling
//}
//
//ModelInteractor::~ModelInteractor()
//{
//}
//
//void ModelInteractor::ConnectActionsAndFunctions()
//{
//    CONNECT_CONDITION("PickedHandle", HasPickedHandle);
//
//    CONNECT_FUNCTION("DecideInteraction", DecideInteraction);
//    CONNECT_FUNCTION("MoveAlongAxis", MoveAlongAxis);
//    CONNECT_FUNCTION("RotateAroundAxis", RotateAroundAxis);
//    CONNECT_FUNCTION("MoveFreely", MoveFreely);
//    CONNECT_FUNCTION("ScaleEqually", ScaleEqually);
//    CONNECT_FUNCTION("FeedUndoStack", FeedUndoStack);
//}
//
//void ModelInteractor::SetGizmoNode(DataNode* node)
//{
//    DataInteractor::SetDataNode(node);
//
//    m_Gizmo = dynamic_cast<ModelUserControl*>(node->GetData());
//
//    // setup picking from just this object
//    m_Picker.clear();
//}
//
//void ModelInteractor::SetManipulatedObjectNode(DataNode* node)
//{
//    if (node && node->GetData())
//    {
//        m_ManipulatedObjectGeometry = node->GetData()->GetGeometry();
//    }
//}
//
//bool ModelInteractor::HasPickedHandle(const InteractionEvent* interactionEvent)
//{
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr ||
//        m_Gizmo.IsNull() ||
//        m_ManipulatedObjectGeometry.IsNull() ||
//        interactionEvent->GetSender()->GetRenderWindow()->GetNeverRendered())
//    {
//        return false;
//    }
//
//    if (interactionEvent->GetSender()->GetMapperID() == BaseRenderer::Standard2D)
//    {
//        m_PickedHandle = PickFrom2D(positionEvent);
//    }
//    else
//    {
//        m_PickedHandle = PickFrom3D(positionEvent);
//    }
//
//    UpdateHandleHighlight();
//
//    return m_PickedHandle != ModelUserControl::NoHandle;
//}
//
//void ModelInteractor::DecideInteraction(StateMachineAction*, InteractionEvent* interactionEvent)
//{
//    assert(m_PickedHandle != Gizmo::NoHandle);
//
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr)
//    {
//        return;
//    }
//
//    // if something relevant was picked, we calculate a number of
//    // important points and axes for the upcoming geometry manipulations
//
//    // note initial state
//    m_InitialClickPosition2D = positionEvent->GetPointerPositionOnScreen();
//    m_InitialClickPosition3D = positionEvent->GetPositionInWorld();
//
//    auto renderer = positionEvent->GetSender()->GetVtkRenderer();
//    renderer->SetWorldPoint(m_InitialClickPosition3D[0], m_InitialClickPosition3D[1], m_InitialClickPosition3D[2], 0);
//    renderer->WorldToDisplay();
//    m_InitialClickPosition2DZ = renderer->GetDisplayPoint()[2];
//
//    m_InitialGizmoCenter3D = m_Gizmo->GetCenter();
//    positionEvent->GetSender()->WorldToDisplay(m_InitialGizmoCenter3D, m_InitialGizmoCenter2D);
//
//    m_InitialManipulatedObjectGeometry = m_ManipulatedObjectGeometry->Clone();
//
//    switch (m_PickedHandle) {
//    case ModelUserControl::MoveAlongAxisX:
//    case ModelUserControl::RotateAroundAxisX:
//    case ModelUserControl::ScaleX:
//        m_AxisOfMovement = m_InitialManipulatedObjectGeometry->GetAxisVector(0);
//        break;
//    case ModelUserControl::MoveAlongAxisY:
//    case ModelUserControl::RotateAroundAxisY:
//    case ModelUserControl::ScaleY:
//        m_AxisOfMovement = m_InitialManipulatedObjectGeometry->GetAxisVector(1);
//        break;
//    case ModelUserControl::MoveAlongAxisZ:
//    case ModelUserControl::RotateAroundAxisZ:
//    case ModelUserControl::ScaleZ:
//        m_AxisOfMovement = m_InitialManipulatedObjectGeometry->GetAxisVector(2);
//        break;
//    default:
//        break;
//    }
//    m_AxisOfMovement.Normalize();
//    m_AxisOfRotation = m_AxisOfMovement;
//
//    // for translation: test whether the user clicked into the "object's real" axis direction
//    //                  or into the other one
//    Vector3D intendedAxis = m_InitialClickPosition3D - m_InitialGizmoCenter3D;
//
//    if (intendedAxis * m_AxisOfMovement < 0) {
//        m_AxisOfMovement *= -1.0;
//    }
//
//    // for rotation: test whether the axis of rotation is more looking in the direction
//    //               of the camera or in the opposite
//    vtkCamera* camera = renderer->GetActiveCamera();
//    vtkVector3d cameraDirection(camera->GetDirectionOfProjection());
//
//    double angle_rad = vtkMath::AngleBetweenVectors(cameraDirection.GetData(), m_AxisOfRotation.GetDataPointer());
//
//    if (angle_rad < vtkMath::Pi() / 2.0) {
//        m_AxisOfRotation *= -1.0;
//    }
//
//    InternalEvent::Pointer decision;
//    switch (m_PickedHandle)
//    {
//    case ModelUserControl::MoveAlongAxisX:
//    case ModelUserControl::MoveAlongAxisY:
//    case ModelUserControl::MoveAlongAxisZ:
//        decision = InternalEvent::New(interactionEvent->GetSender(), this, "StartTranslationAlongAxis");
//        break;
//    case ModelUserControl::RotateAroundAxisX:
//    case ModelUserControl::RotateAroundAxisY:
//    case ModelUserControl::RotateAroundAxisZ:
//        decision = InternalEvent::New(interactionEvent->GetSender(), this, "StartRotationAroundAxis");
//        break;
//    case ModelUserControl::MoveFreely:
//        decision = InternalEvent::New(interactionEvent->GetSender(), this, "MoveFreely");
//        break;
//    case ModelUserControl::ScaleX:
//    case ModelUserControl::ScaleY:
//    case ModelUserControl::ScaleZ:
//        // Implementation note: Why didn't we implement per-axis scaling yet?
//        // When this was implemented, the mitk::ScaleOperation was able to only describe
//        // uniform scaling around a central point. Since we use operations for all modifications
//        // in order to have undo/redo working, any axis-specific scaling should also
//        // use operations.
//        // Consequence: enhance ScaleOperation when there is need to have scaling per axis.
//        decision = InternalEvent::New(interactionEvent->GetSender(), this, "ScaleEqually");
//        break;
//    default:
//        break;
//    }
//
//    interactionEvent->GetSender()->GetDispatcher()->QueueEvent(decision);
//}
//
//void ModelInteractor::MoveAlongAxis(StateMachineAction*, InteractionEvent* interactionEvent)
//{
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr)
//    {
//        return;
//    }
//
//    Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();
//
//    // re-use the initial z value to calculate movements parallel to the camera plane
//    auto renderer = positionEvent->GetSender()->GetVtkRenderer();
//    renderer->SetDisplayPoint(currentPosition2D[0], currentPosition2D[1], m_InitialClickPosition2DZ);
//    renderer->DisplayToWorld();
//    vtkVector3d worldPointVTK(renderer->GetWorldPoint());
//    Point3D worldPointITK(worldPointVTK.GetData());
//    Vector3D freeMouseMovement3D(worldPointITK - m_InitialClickPosition3D);
//
//    double projectedMouseMovement3D = freeMouseMovement3D * m_AxisOfMovement;
//    Vector3D appliedMovement3D = projectedMouseMovement3D * m_AxisOfMovement;
//
//    ApplyTranslationToManipulatedObject(appliedMovement3D);
//    RenderingManager::GetInstance()->ForceImmediateUpdateAll();
//    emit InteractoChanged();
//}
//
//void ModelInteractor::RotateAroundAxis(StateMachineAction*, InteractionEvent* interactionEvent)
//{
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr)
//    {
//        return;
//    }
//
//    Vector2D originalVector = m_InitialClickPosition2D - m_InitialGizmoCenter2D;
//    Vector2D currentVector = positionEvent->GetPointerPositionOnScreen() - m_InitialGizmoCenter2D;
//
//    originalVector.Normalize();
//    currentVector.Normalize();
//
//    double angle_rad = std::atan2(currentVector[1], currentVector[0]) - std::atan2(originalVector[1], originalVector[0]);
//
//    ApplyRotationToManipulatedObject(vtkMath::DegreesFromRadians(angle_rad));
//    RenderingManager::GetInstance()->ForceImmediateUpdateAll();
//    emit InteractoChanged();
//}
//
//void ModelInteractor::MoveFreely(StateMachineAction*, InteractionEvent* interactionEvent)
//{
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr)
//    {
//        return;
//    }
//
//    Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();
//
//    // re-use the initial z value to really move parallel to the camera plane
//    auto renderer = positionEvent->GetSender()->GetVtkRenderer();
//    renderer->SetDisplayPoint(currentPosition2D[0], currentPosition2D[1], m_InitialClickPosition2DZ);
//    renderer->DisplayToWorld();
//    vtkVector3d worldPointVTK(renderer->GetWorldPoint());
//    Point3D worldPointITK(worldPointVTK.GetData());
//    Vector3D movementITK(worldPointITK - m_InitialClickPosition3D);
//
//    ApplyTranslationToManipulatedObject(movementITK);
//    RenderingManager::GetInstance()->ForceImmediateUpdateAll();
//    emit InteractoChanged();
//}
//
//void ModelInteractor::ScaleEqually(StateMachineAction*, InteractionEvent* interactionEvent)
//{
//    auto positionEvent = dynamic_cast<const mitk::InteractionPositionEvent*>(interactionEvent);
//    if (positionEvent == nullptr)
//    {
//        return;
//    }
//
//    Point2D currentPosition2D = positionEvent->GetPointerPositionOnScreen();
//    double relativeSize = (currentPosition2D - m_InitialGizmoCenter2D).GetNorm() /
//        (m_InitialClickPosition2D - m_InitialGizmoCenter2D).GetNorm();
//
//    ApplyEqualScalingToManipulatedObject(relativeSize);
//    RenderingManager::GetInstance()->ForceImmediateUpdateAll();
//    emit InteractoChanged();
//}
//
//void ModelInteractor::ApplyTranslationToManipulatedObject(const Vector3D& translation)
//{
//    assert(m_ManipulatedObjectGeometry.IsNotNull());
//
//    auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();
//    m_FinalDoOperation.reset(new PointOperation(OpMOVE, Point3D(translation)));
//    if (m_UndoEnabled)
//    {
//        m_FinalUndoOperation.reset(new PointOperation(OpMOVE, Point3D(-translation)));
//    }
//
//    manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
//    m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
//}
//
//void ModelInteractor::ApplyEqualScalingToManipulatedObject(double scalingFactor)
//{
//    assert(m_ManipulatedObjectGeometry.IsNotNull());
//    auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();
//
//    m_FinalDoOperation.reset(new ScaleOperation(OpSCALE, scalingFactor - 1.0, m_InitialGizmoCenter3D));
//    if (m_UndoEnabled)
//    {
//        m_FinalUndoOperation.reset(new ScaleOperation(OpSCALE, -(scalingFactor - 1.0), m_InitialGizmoCenter3D));
//    }
//
//    manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
//    m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
//}
//
//void ModelInteractor::ApplyRotationToManipulatedObject(double angle_deg)
//{
//    assert(m_ManipulatedObjectGeometry.IsNotNull());
//
//    auto manipulatedGeometry = m_InitialManipulatedObjectGeometry->Clone();
//
//    m_FinalDoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmoCenter3D, m_AxisOfRotation, angle_deg));
//    if (m_UndoEnabled)
//    {
//        m_FinalUndoOperation.reset(new RotationOperation(OpROTATE, m_InitialGizmoCenter3D, m_AxisOfRotation, -angle_deg));
//    }
//
//    manipulatedGeometry->ExecuteOperation(m_FinalDoOperation.get());
//    m_ManipulatedObjectGeometry->SetIndexToWorldTransform(manipulatedGeometry->GetIndexToWorldTransform());
//}
//
//void ModelInteractor::FeedUndoStack(StateMachineAction*, InteractionEvent*)
//{
//    if (m_UndoEnabled)
//    {
//        OperationEvent* operationEvent = new OperationEvent(m_ManipulatedObjectGeometry,
//            // OperationEvent will destroy operations!
//            // --> release() and not get()
//            m_FinalDoOperation.release(),
//            m_FinalUndoOperation.release(),
//            "Direct geometry manipulation");
//        mitk::OperationEvent::IncCurrObjectEventId(); // save each modification individually
//        m_UndoController->SetOperationEvent(operationEvent);
//    }
//}
//
//ModelUserControl::HandleType ModelInteractor::PickFrom2D(const mitk::InteractionPositionEvent* positionEvent)
//{
//    BaseRenderer* renderer = positionEvent->GetSender();
//
//    auto mapper = GetDataNode()->GetMapper(BaseRenderer::Standard2D);
//    auto model_mapper = dynamic_cast<ModelMapper2D*>(mapper);
//    auto& picker = m_Picker[renderer];
//
//    if (picker == nullptr)
//    {
//        picker = vtkSmartPointer<vtkCellPicker>::New();
//        picker->SetTolerance(0.005);
//
//        if (model_mapper)
//        { // doing this each time is bizarre
//            picker->AddPickList(model_mapper->GetVtkProp(renderer));
//            picker->PickFromListOn();
//        }
//    }
//
//    auto displayPosition = positionEvent->GetPointerPositionOnScreen();
//    picker->Pick(displayPosition[0], displayPosition[1], 0, positionEvent->GetSender()->GetVtkRenderer());
//
//    vtkIdType pickedPointID = picker->GetPointId();
//    if (pickedPointID == -1)
//    {
//        return ModelUserControl::NoHandle;
//    }
//
//    vtkPolyData* polydata = model_mapper->GetVtkPolyData(renderer);
//
//    if (polydata && polydata->GetPointData() && polydata->GetPointData()->GetScalars())
//    {
//        double dataValue = polydata->GetPointData()->GetScalars()->GetTuple1(pickedPointID);
//        return m_Gizmo->GetHandleFromPointDataValue(dataValue);
//    }
//
//    return ModelUserControl::NoHandle;
//}
//
//ModelUserControl::HandleType ModelInteractor::PickFrom3D(const mitk::InteractionPositionEvent* positionEvent)
//{
//    BaseRenderer* renderer = positionEvent->GetSender();
//    auto& picker = m_Picker[renderer];
//    if (picker == nullptr)
//    {
//        picker = vtkSmartPointer<vtkCellPicker>::New();
//        picker->SetTolerance(0.005);
//        auto mapper = GetDataNode()->GetMapper(BaseRenderer::Standard3D);
//        auto vtk_mapper = dynamic_cast<VtkMapper*>(mapper);
//        if (vtk_mapper)
//        { // doing this each time is bizarre
//            picker->AddPickList(vtk_mapper->GetVtkProp(renderer));
//            picker->PickFromListOn();
//        }
//    }
//
//    auto displayPosition = positionEvent->GetPointerPositionOnScreen();
//    picker->Pick(displayPosition[0], displayPosition[1], 0, positionEvent->GetSender()->GetVtkRenderer());
//
//    vtkIdType pickedPointID = picker->GetPointId();
//    if (pickedPointID == -1)
//    {
//        return ModelUserControl::NoHandle;
//    }
//
//    // _something_ picked
//    return m_Gizmo->GetHandleFromPointID(pickedPointID);
//}
//
//void ModelInteractor::UpdateHandleHighlight()
//{
//    if (m_HighlightedHandle != m_PickedHandle) {
//
//        auto node = GetDataNode();
//        if (node == nullptr) return;
//
//        auto base_prop = node->GetProperty("LookupTable");
//        if (base_prop == nullptr) return;
//
//        auto lut_prop = dynamic_cast<LookupTableProperty*>(base_prop);
//        if (lut_prop == nullptr) return;
//
//        auto lut = lut_prop->GetLookupTable();
//        if (lut == nullptr) return;
//
//        // Table size is expected to constructed as one entry per gizmo-part enum value
//        assert(lut->GetVtkLookupTable()->GetNumberOfTableValues() > std::max(m_PickedHandle, m_HighlightedHandle));
//
//        // Reset previously overwritten color
//        if (m_HighlightedHandle != ModelUserControl::NoHandle)
//        {
//            lut->SetTableValue(m_HighlightedHandle, m_ColorReplacedByHighlight);
//        }
//
//        // Overwrite currently highlighted color
//        if (m_PickedHandle != ModelUserControl::NoHandle)
//        {
//            lut->GetTableValue(m_PickedHandle, m_ColorReplacedByHighlight);
//            lut->SetTableValue(m_PickedHandle, m_ColorForHighlight);
//        }
//
//        // Mark node modified to allow repaint
//        node->Modified();
//        RenderingManager::GetInstance()->RequestUpdateAll(RenderingManager::REQUEST_UPDATE_ALL);
//        emit InteractoChanged();
//        m_HighlightedHandle = m_PickedHandle;
//    }
//}