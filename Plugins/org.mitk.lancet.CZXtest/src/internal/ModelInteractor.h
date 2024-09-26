//#pragma once
//#include "ModelUserControl.h"
//// MITK includes
//#include <mitkDataInteractor.h>
//#include <mitkDataNode.h>
//#include <mitkGeometry3D.h>
//#include <mitkInteractionPositionEvent.h>
//
//// VTK includes
//#include <vtkCellPicker.h>
//#include <vtkSmartPointer.h>
//
//// System includes
//#include <memory>
//#include <qobject.h>
//
//using namespace mitk;
//class InteractionPositionEvent;
//class ModelInteractor : public QObject, public DataInteractor
//{
//    Q_OBJECT
//public:
//    mitkClassMacro(ModelInteractor, DataInteractor);
//    itkFactorylessNewMacro(Self);
//    itkCloneMacro(Self);
//
//    //! The node holding the gizmo for visual feedback.
//    //! This is the node that the interactor is primarily working on
//    //! (calls DataInteractor::SetDataNode).
//    void SetGizmoNode(DataNode* node);
//
//    //! The node that shall be manipulated in function of the user
//    //! interaction on the gizmo.
//    void SetManipulatedObjectNode(DataNode* node);
//signals:
//     void InteractoChanged();
//private:
//    ModelInteractor();
//    ~ModelInteractor() override;
//
//    //! Setup the relation between the XML state machine and this object's methods.
//    void ConnectActionsAndFunctions() override;
//
//    //! State machine condition: successful gizmo picking
//    //! \return true when any part of the gizmo has been picked.
//    bool HasPickedHandle(const InteractionEvent*);
//
//    //! State machine action: Advances state machine by generating a new event
//    //! in function of the picked part of the gizmo.
//    void DecideInteraction(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! State machine action: Translates the manipulated object along the picked axis
//    //! in function of mouse movement.
//    void MoveAlongAxis(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! State machine action: Scale the manipulated object equally along all axes
//    //! in function of mouse movement.
//    void ScaleEqually(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! State machine action: Rotates the manipulated object around the picked axis
//    //! in function of mouse movement.
//    void RotateAroundAxis(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! State machine action: Moves the manipulated object parallel to the camera view plane
//    //! in function of mouse movement.
//    void MoveFreely(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! Add the final transformation operations to the undo stack
//    void FeedUndoStack(StateMachineAction*, InteractionEvent* interactionEvent);
//
//    //! Applies a calculated translation vector to the manipulated object.
//    void ApplyTranslationToManipulatedObject(const Vector3D& projectedMovement);
//
//    //! Applies a calculated scaling factor the manipulated object.
//    void ApplyEqualScalingToManipulatedObject(double scalingFactor);
//
//    //! Applies a calculated rotation angle to the manipulated object.
//    void ApplyRotationToManipulatedObject(double angle_deg);
//
//    //! Pick a gizmo handle from a 2D event (passing by the 2D mapper)
//    ModelUserControl::HandleType PickFrom2D(const mitk::InteractionPositionEvent* positionEvent);
//
//    //! Pick a gizmo handle from a 3D event
//    //! (passing by the general surface mapper and the gizmo object)
//    ModelUserControl::HandleType PickFrom3D(const mitk::InteractionPositionEvent* positionEvent);
//
//    void UpdateHandleHighlight();
//
//    //! the Gizmo used for visual feedback and picking
//    ModelUserControl::Pointer m_Gizmo;
//
//    //! The manipulated object's geometry
//    BaseGeometry::Pointer m_ManipulatedObjectGeometry;
//
//    //! For picking on the vtkPolyData representing the gizmo
//    std::map<BaseRenderer*, vtkSmartPointer<vtkCellPicker>> m_Picker;
//
//    //! Part of the gizmo that was picked on last check
//    ModelUserControl::HandleType m_PickedHandle = ModelUserControl::NoHandle;
//
//    //! Part of the gizmo that is currently highlighted
//    ModelUserControl::HandleType m_HighlightedHandle = ModelUserControl::NoHandle;
//
//    //! Color (RGBA) used for highlighting
//    double m_ColorForHighlight[4];
//
//    //! Color (RGBA) that has been replaced by m_ColorForHighlight
//    double m_ColorReplacedByHighlight[4];
//
//    Point2D m_InitialClickPosition2D; //< Initial screen click position
//    double m_InitialClickPosition2DZ; //< Z value of the initial screen click position
//    Point3D m_InitialClickPosition3D; //< Initial 3D click position
//
//    Point2D m_InitialGizmoCenter2D; //< Initial position of the gizmo's center in screen coordinates
//    Point3D m_InitialGizmoCenter3D; //< Initial 3D position of the gizmo's center
//
//    Vector3D m_AxisOfMovement; //< Axis along which we move when translating
//    Vector3D m_AxisOfRotation; //< Axis around which we turn when rotating
//
//    std::unique_ptr<Operation> m_FinalDoOperation;   //< Operation for the undo-stack
//    std::unique_ptr<Operation> m_FinalUndoOperation; //< Operation for the undo-stack
//
//    //! A copy of the origin geometry, to avoid accumulation of tiny errors
//    BaseGeometry::Pointer m_InitialManipulatedObjectGeometry;
//};
//
