/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef mitkGizmo_noscale_h
#define mitkGizmo_noscale_h

#include <mitkDataNode.h>
#include <mitkSurface.h>

#include <MitkGizmo_noscaleExports.h>


namespace mitk
{
  class DataStorage;
  class Gizmo_noscaleRemover;

  //! A geometry manipulation "gizmo".
  //!
  //! This class represents the principal axes of some arbitrary BaseGeometry.
  //!
  //! The visualization shows the three axes x, y, and z along with a orthogonal ring around them.
  //! In its center, a small sphere is visualized.
  //!
  //! The class is intended to be visualized along with another data object that "owns" the followed
  //! BaseGeometry. The Gizmo will automatically update itself to all modifications to the
  //! followed base geometry. Interactive modifications to the geometry can thus be easily visualized.
  //!
  //! The gizmo is definded by
  //! - a center
  //! - three axes for x, y, and z
  //! - a radius
  //!
  //! The radius determines the size of the axes and the rings around them.
  //!
  //! A nice overview of similar / alternative representations can be found in
  //! "Schmidt R, Singh K, and Balakrishnan R. Sketching and Composing Widgets for 3D Manipulation.
  //!  EUROGRAPHICS 2008"
  //!
  //! \sa GizmoInteractor3D
  class MITKGIZMO_NOSCALE_EXPORT Gizmo_noscale : public Surface
  {
  public:
   //  double m_RotationMatrix_cam[16];
    // const double m_ScaleFactor_init;
    bool m_ConstantValueInitialized; 

    //! Names for the three axes
    //Gizmo() : viewTransformMatrix(vtkSmartPointer<vtkMatrix4x4>::New()) {}
    enum AxisType
    {
      AxisX,
      AxisY,
      AxisZ
    };

    //! Names for the different parts of the gizmo
    enum HandleType
    {
      MoveFreely, //< the central sphere
      MoveAlongAxisX,
      MoveAlongAxisY,
      MoveAlongAxisZ,
      RotateAroundAxisX,
      RotateAroundAxisY,
      RotateAroundAxisZ,
      RotateAroundAxisX3D,
      RotateAroundAxisY3D,
      RotateAroundAxisZ3D,
      ScaleX,
      ScaleY,
      ScaleZ,
      NoHandle //< to indicate picking failure
    };

    //! Conversion for any kind of logging/debug/... purposes
    static std::string HandleTypeToString(HandleType type);

    mitkClassMacro(Gizmo_noscale, Surface);
    itkNewMacro(Gizmo_noscale);


	    static Pointer NewWithBoolean(bool Moveable, bool Rotatable, bool Scalable)
    {
      // 调用带参数的构造函数
      Pointer smartPtr = new Gizmo_noscale(Moveable, Rotatable, Scalable);
      smartPtr->UnRegister();
      return smartPtr;
    }

    itkGetConstMacro(Center, Point3D);
    itkSetMacro(Center, Point3D);

    itkGetConstMacro(AxisX, Vector3D);
    itkSetMacro(AxisX, Vector3D);

    itkGetConstMacro(AxisY, Vector3D);
    itkSetMacro(AxisY, Vector3D);

    itkGetConstMacro(AxisZ, Vector3D);
    itkSetMacro(AxisZ, Vector3D);

    itkGetConstMacro(Radius, Vector3D);
    itkSetMacro(Radius, Vector3D);

    itkGetConstMacro(AllowTranslation, bool);
    itkSetMacro(AllowTranslation, bool);
    itkBooleanMacro(AllowTranslation);

    itkGetConstMacro(AllowRotation, bool);
    itkSetMacro(AllowRotation, bool);
    itkBooleanMacro(AllowRotation);

    itkGetConstMacro(AllowScaling, bool);
    itkSetMacro(AllowScaling, bool);
    itkBooleanMacro(AllowScaling);

    //! Return the longest of the three axes.
    double GetLongestRadius() const;

    //! Updates the representing surface object after changes to center, axes, or radius.
    void UpdateRepresentation(bool Moveable, bool Rotatable, bool Scalable);

    //! Setup the gizmo to follow any ModifiedEvents of the given geometry.
    //! The object will adapt and update itself in function of the geometry's changes.
    void FollowGeometry(BaseGeometry *geom, bool Moveable, bool Rotatable, bool Scalable);

    //! The ITK callback to receive modified events of the followed geometry
    void OnFollowedGeometryModified();

    //! Determine the nature of the the given vertex id.
    //! Can be used after picking a vertex id to determine what part of the
    //! gizmo has been picked.
    HandleType GetHandleFromPointID(vtkIdType id);

    //! Determine the nature of the the given vertex data value.
    //! Can be used after picking a vertex data value to determine what part of the
    //! gizmo has been picked.
    mitk::Gizmo_noscale::HandleType GetHandleFromPointDataValue(double value);

    //! Convenience creation of a gizmo for given node
    //! \param node The node holding the geometry to be visualized
    //! \param storage The DataStorage where a node holding the gizmo
    //!                shall be added to (ignored when nullptr)
    //!
    //! \return DataNode::Pointer containing the node used for vizualization of our gizmo
    static DataNode::Pointer AddGizmo_noscaleToNode(
      DataNode *node, DataStorage *storage, bool Moveable, bool Rotatable, bool Scalable);

    //! Convenience removal of gizmo from given node
    //! \param node The node being currently manipulated
    //! \param storage The DataStorage where the gizmo has been added to
    //!
    //! \return true if the gizmo has been found and removed successfully
    //!
    //! Make sure to pass the same parameters here that you provided to a
    //! previous call to AddGizmoToNode.
    //!
    //! \return DataNode::Pointer containing the node used for vizualization of our gizmo
    static bool RemoveGizmo_noscaleFromNode(DataNode *node, DataStorage *storage);

    //! \return whether given node in given storage has a gizmo attached.
    static bool HasGizmo_noscaleAttached(mitk::DataNode *node, DataStorage *storage);

	void SetViewTransformMatrix(vtkSmartPointer<vtkMatrix4x4> viewTransformMatrix);
  public:

    Gizmo_noscale();
    Gizmo_noscale(bool Moveable, bool Rotatable, bool Scalable);
    ~Gizmo_noscale() override;
  // void mitk::Gizmo_noscale::GetRotationMatrix(const double matrix[16]);
    Gizmo_noscale(const Gizmo_noscale &);            // = delete;
    Gizmo_noscale &operator=(const Gizmo_noscale &); // = delete;

    //! Creates a vtkPolyData representing the parameters defining the gizfmo.
    vtkSmartPointer<vtkPolyData> BuildGizmo_noscale();
    vtkSmartPointer<vtkPolyData> BuildGizmo_noscale_nomove();
    std::unique_ptr<Gizmo_noscaleRemover> m_Gizmo_noscaleRemover;
  private:
    Point3D m_Center;
    Vector3D m_AxisX;
    Vector3D m_AxisY;
    Vector3D m_AxisZ;
    Vector3D m_Radius;
    // double scaleFactor_tmp;
    void Initialize(bool Moveable , bool Rotatable, bool Scalable);
    bool m_AllowTranslation;
    bool m_AllowRotation;
    bool m_AllowScaling;
    bool m_AllowMOVE;
    bool m_AllowROTATE;
    bool m_AllowSCALE;

	vtkNew<vtkMatrix4x4> m_ViewTransformMatrix;


    BaseGeometry::Pointer m_FollowedGeometry;
    //! ITK tag for the observing of m_FollowedGeometry
    unsigned long m_FollowerTag;

    //! Observes a data storage for removal of the manipulated object.
    //! Removes gizmo together with the manipulated object


	std::vector<std::function<void()>> m_Observers;
  };

}
#endif /* BOUNDINGOBJECT_H_HEADER_INCLUDED */
