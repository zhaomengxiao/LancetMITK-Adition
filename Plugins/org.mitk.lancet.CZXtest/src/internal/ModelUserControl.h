//#pragma once
//#include <mitkDataNode.h>
//#include <mitkSurface.h>
//#include <mitkDataStorage.h>
//#include <MitkGizmoExports.h>
//
//using namespace mitk;
//class DataStorage;
//class ModelRemover;
//class ModelUserControl : public Surface
//{
//public:
//    //! Names for the three axes
//    enum AxisType
//    {
//        AxisX,
//        AxisY,
//        AxisZ
//    };
//
//    //! Names for the different parts of the gizmo
//    enum HandleType
//    {
//        MoveFreely, //< the central sphere
//        MoveAlongAxisX,
//        MoveAlongAxisY,
//        MoveAlongAxisZ,
//        RotateAroundAxisX,
//        RotateAroundAxisY,
//        RotateAroundAxisZ,
//        ScaleX,
//        ScaleY,
//        ScaleZ,
//        NoHandle //< to indicate picking failure
//    };
//
//    //! Conversion for any kind of logging/debug/... purposes
//    static std::string HandleTypeToString(HandleType type);
//
//    mitkClassMacro(ModelUserControl, Surface);
//    itkNewMacro(ModelUserControl);
//
//    itkGetConstMacro(Center, Point3D);
//    itkSetMacro(Center, Point3D);
//
//    itkGetConstMacro(AxisX, Vector3D);
//    itkSetMacro(AxisX, Vector3D);
//
//    itkGetConstMacro(AxisY, Vector3D);
//    itkSetMacro(AxisY, Vector3D);
//
//    itkGetConstMacro(AxisZ, Vector3D);
//    itkSetMacro(AxisZ, Vector3D);
//
//    itkGetConstMacro(Radius, Vector3D);
//    itkSetMacro(Radius, Vector3D);
//
//    itkGetConstMacro(AllowTranslation, bool);
//    itkSetMacro(AllowTranslation, bool);
//    itkBooleanMacro(AllowTranslation);
//
//    itkGetConstMacro(AllowRotation, bool);
//    itkSetMacro(AllowRotation, bool);
//    itkBooleanMacro(AllowRotation);
//
//    itkGetConstMacro(AllowScaling, bool);
//    itkSetMacro(AllowScaling, bool);
//    itkBooleanMacro(AllowScaling);
//
//    //! Return the longest of the three axes.
//    double GetLongestRadius() const;
//
//    //! Updates the representing surface object after changes to center, axes, or radius.
//    void UpdateRepresentation();
//
//    //! Setup the gizmo to follow any ModifiedEvents of the given geometry.
//    //! The object will adapt and update itself in function of the geometry's changes.
//    void FollowGeometry(BaseGeometry* geom);
//
//    //! The ITK callback to receive modified events of the followed geometry
//    void OnFollowedGeometryModified();
//
//    //! Determine the nature of the the given vertex id.
//    //! Can be used after picking a vertex id to determine what part of the
//    //! gizmo has been picked.
//    HandleType GetHandleFromPointID(vtkIdType id);
//
//    //! Determine the nature of the the given vertex data value.
//    //! Can be used after picking a vertex data value to determine what part of the
//    //! gizmo has been picked.
//    ModelUserControl::HandleType GetHandleFromPointDataValue(double value);
//
//    //! Convenience creation of a gizmo for given node
//    //! \param node The node holding the geometry to be visualized
//    //! \param storage The DataStorage where a node holding the gizmo
//    //!                shall be added to (ignored when nullptr)
//    //!
//    //! \return DataNode::Pointer containing the node used for vizualization of our gizmo
//    static DataNode::Pointer AddGizmoToNode(mitk::DataNode* node, mitk::DataStorage* storage);
//
//    //! Convenience removal of gizmo from given node
//    //! \param node The node being currently manipulated
//    //! \param storage The DataStorage where the gizmo has been added to
//    //!
//    //! \return true if the gizmo has been found and removed successfully
//    //!
//    //! Make sure to pass the same parameters here that you provided to a
//    //! previous call to AddGizmoToNode.
//    //!
//    //! \return DataNode::Pointer containing the node used for vizualization of our gizmo
//    static bool RemoveGizmoFromNode(mitk::DataNode* node, mitk::DataStorage* storage);
//
//    //! \return whether given node in given storage has a gizmo attached.
//    static bool HasGizmoAttached(mitk::DataNode* node, mitk::DataStorage* storage);
//
//protected:
//    ModelUserControl();
//    ~ModelUserControl() override;
//
//    ModelUserControl(const ModelUserControl&);            // = delete;
//    ModelUserControl& operator=(const ModelUserControl&); // = delete;
//
//    //! Creates a vtkPolyData representing the parameters defining the gizmo.
//    vtkSmartPointer<vtkPolyData> BuildGizmo();
//
//private:
//    Point3D m_Center;
//    Vector3D m_AxisX;
//    Vector3D m_AxisY;
//    Vector3D m_AxisZ;
//    Vector3D m_Radius;
//
//    bool m_AllowTranslation;
//    bool m_AllowRotation;
//    bool m_AllowScaling;
//
//    BaseGeometry::Pointer m_FollowedGeometry;
//    //! ITK tag for the observing of m_FollowedGeometry
//    unsigned long m_FollowerTag;
//
//    //! Observes a data storage for removal of the manipulated object.
//    //! Removes gizmo together with the manipulated object
//    std::unique_ptr<ModelRemover> m_ModelRemover;
//};
//
