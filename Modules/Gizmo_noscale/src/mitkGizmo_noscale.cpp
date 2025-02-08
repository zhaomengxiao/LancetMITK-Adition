/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkGizmo_noscale.h"
#include "mitkGizmo_noscaleInteractor.h"

// MITK includes
#include <mitkBaseRenderer.h>
#include <mitkLookupTableProperty.h>
#include <mitkNodePredicateDataType.h>
#include <mitkRenderingManager.h>
#include <mitkVtkInterpolationProperty.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkCharArray.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>

#include <iostream>
#include <random>
// ITK includes
#include <itkCommand.h>

// MicroServices
#include <usGetModuleContext.h>

namespace
{
  const char *PROPERTY_KEY_ORIGINAL_OBJECT_OPACITY = "gizmo_noscale.originalObjectOpacity";
}

namespace mitk
{
  //! Private object, removing the Gizmo from data storage along with its manipulated object.
  class Gizmo_noscaleRemover
  {
  public:
    Gizmo_noscaleRemover()
      : m_Storage(nullptr), m_Gizmo_noscaleNode(nullptr), m_ManipulatedNode(nullptr), m_StorageObserverTag(0)
    {
    }
    //! Tell the object about the storage that contains both the nodes
    //! containing the gizmo and the manipulated object.
    //!
    //! The method sets up observation of
    //! - removal of the manipulated node from storage
    //! - destruction of storage itself
    void UpdateStorageObservation(mitk::DataStorage *storage,
                                  mitk::DataNode *gizmo_noscale_node,
                                  mitk::DataNode *manipulated_node)
    {
      if (m_Storage != nullptr)
      {
        m_Storage->RemoveNodeEvent.RemoveListener(mitk::MessageDelegate1<Gizmo_noscaleRemover, const mitk::DataNode *>(
          this, &Gizmo_noscaleRemover::OnDataNodeHasBeenRemoved));
        m_Storage->RemoveObserver(m_StorageObserverTag);
      }

      m_Storage = storage;
      m_Gizmo_noscaleNode = gizmo_noscale_node;
      m_ManipulatedNode = manipulated_node;

      if (m_Storage != nullptr)
      {
        m_Storage->RemoveNodeEvent.AddListener(mitk::MessageDelegate1<Gizmo_noscaleRemover, const mitk::DataNode *>(
          this, &Gizmo_noscaleRemover::OnDataNodeHasBeenRemoved));

        itk::SimpleMemberCommand<Gizmo_noscaleRemover>::Pointer command =
          itk::SimpleMemberCommand<Gizmo_noscaleRemover>::New();
        command->SetCallbackFunction(this, &mitk::Gizmo_noscaleRemover::OnDataStorageDeleted);
        m_StorageObserverTag = m_Storage->AddObserver(itk::ModifiedEvent(), command);
      }
    }

    //! Callback notified on destruction of DataStorage
    void OnDataStorageDeleted() { m_Storage = nullptr; }
    //! Callback notified on removal of _any_ object from data storage
    void OnDataNodeHasBeenRemoved(const mitk::DataNode *node)
    {
      if (node == m_ManipulatedNode)
      {
        // m_Storage is still alive because it is the emitter
        if (m_Storage->Exists(m_Gizmo_noscaleNode))
        {
          m_Storage->Remove(m_Gizmo_noscaleNode);
          // normally, gizmo will be deleted here (unless somebody
          // still holds a reference to it)
        }
      }
    }

    //! Clean up our observer registrations
    ~Gizmo_noscaleRemover()
    {
      if (m_Storage)
      {
        m_Storage->RemoveNodeEvent.RemoveListener(mitk::MessageDelegate1<Gizmo_noscaleRemover, const mitk::DataNode *>(
          this, &Gizmo_noscaleRemover::OnDataNodeHasBeenRemoved));
        m_Storage->RemoveObserver(m_StorageObserverTag);
      }
    }

  private:
    mitk::DataStorage *m_Storage;
    mitk::DataNode *m_Gizmo_noscaleNode;
    mitk::DataNode *m_ManipulatedNode;
    unsigned long m_StorageObserverTag;
  };

} // namespace mitk

bool mitk::Gizmo_noscale::HasGizmo_noscaleAttached(DataNode *node, DataStorage *storage)
{
  auto typeCondition = TNodePredicateDataType<Gizmo_noscale>::New();
  auto gizmo_noscaleChildren = storage->GetDerivations(node, typeCondition);
  return !gizmo_noscaleChildren->empty();
}

bool mitk::Gizmo_noscale::RemoveGizmo_noscaleFromNode(DataNode *node, DataStorage *storage)
{
  if (node == nullptr || storage == nullptr)
  {
    return false;
  }

  auto typeCondition = TNodePredicateDataType<Gizmo_noscale>::New();
  auto gizmo_noscaleChildren = storage->GetDerivations(node, typeCondition);

  for (auto &gizmo_noscaleChild : *gizmo_noscaleChildren)
  {
    auto *gizmo_noscale = dynamic_cast<Gizmo_noscale *>(gizmo_noscaleChild->GetData());
    if (gizmo_noscale)
    {
      storage->Remove(gizmo_noscaleChild);
      gizmo_noscale->m_Gizmo_noscaleRemover->UpdateStorageObservation(nullptr, nullptr, nullptr);
    }
  }

  //--------------------------------------------------------------
  // Restore original opacity if we changed it
  //--------------------------------------------------------------
  float originalOpacity = 1.0;
  if (node->GetFloatProperty(PROPERTY_KEY_ORIGINAL_OBJECT_OPACITY, originalOpacity))
  {
    node->SetOpacity(originalOpacity);
    node->GetPropertyList()->DeleteProperty(PROPERTY_KEY_ORIGINAL_OBJECT_OPACITY);
  }

  return !gizmo_noscaleChildren->empty();
}

mitk::DataNode::Pointer mitk::Gizmo_noscale::AddGizmo_noscaleToNode(
  DataNode *node, DataStorage *storage, bool Moveable, bool Rotatable, bool Scalable)
{
  assert(node);
   if (node->GetData() == nullptr || node->GetData()->GetGeometry() == nullptr)
  {
    return nullptr;
  }
  //--------------------------------------------------------------
  // Add visual gizmo that follows the node to be manipulated
  //--------------------------------------------------------------

  std::cout << "3d" << std::endl;
  auto gizmo_noscale = Gizmo_noscale::NewWithBoolean(Moveable, Rotatable, Scalable);
  auto gizmo_noscaleNode = DataNode::New();
  gizmo_noscaleNode->SetBoolProperty("Moveable", Moveable);
  gizmo_noscaleNode->SetBoolProperty("Rotatable", Rotatable);
  gizmo_noscaleNode->SetBoolProperty("Scalable", Scalable);
  gizmo_noscaleNode->SetName("Gizmo");
  gizmo_noscaleNode->SetData(gizmo_noscale);
  gizmo_noscaleNode->SetIntProperty("layer", 1000);
  gizmo_noscale->FollowGeometry(node->GetData()->GetGeometry(), Moveable, Rotatable, Scalable);

  //--------------------------------------------------------------
  // Add interaction to the gizmo
  //--------------------------------------------------------------

  mitk::Gizmo_noscaleInteractor::Pointer interactor = mitk::Gizmo_noscaleInteractor::New();
  interactor->LoadStateMachine("Gizmo3DStates.xml", us::GetModuleContext()->GetModule());
  interactor->SetEventConfig("Gizmo3DConfig.xml", us::ModuleRegistry::GetModule("MitkGizmo_noscale"));

  interactor->SetGizmo_noscaleNode(gizmo_noscaleNode);
  interactor->SetManipulatedObjectNode(node);

  ////--------------------------------------------------------------
  //// Note current opacity for later restore and lower it
  ////--------------------------------------------------------------

  float currentNodeOpacity = 1.0;
  if (node->GetOpacity(currentNodeOpacity, nullptr))
  {
    if (currentNodeOpacity > 0.5f)
    {
      node->SetFloatProperty(PROPERTY_KEY_ORIGINAL_OBJECT_OPACITY, currentNodeOpacity);
      node->SetOpacity(0.5f);
    }
  }

  if (storage && Moveable)
  {
    node->SetIntProperty("layer", 1000);
    storage->Add(gizmo_noscaleNode, node);
    gizmo_noscale->m_Gizmo_noscaleRemover->UpdateStorageObservation(storage, gizmo_noscaleNode, node);
  }
  else
  {
    node->SetIntProperty("layer", 1000);
    storage->Add(gizmo_noscaleNode, node);
    gizmo_noscale->m_Gizmo_noscaleRemover->UpdateStorageObservation(storage, gizmo_noscaleNode, node);
  }

  return gizmo_noscaleNode;
}

namespace mitk
{
  Gizmo_noscale::Gizmo_noscale()
    : Surface(),
      m_AllowTranslation(true),
      m_AllowRotation(true),
      m_AllowScaling(true),
      // m_ScaleFactor_init(0.0),
      m_ConstantValueInitialized(false), 
      m_Gizmo_noscaleRemover(new Gizmo_noscaleRemover())
  {
    Initialize(false, false, false);
  }

  Gizmo_noscale::Gizmo_noscale(bool Moveable, bool Rotatable, bool Scalable)
    : Surface(),
      m_AllowTranslation(true),
      m_AllowRotation(true),
      m_AllowScaling(true),
      // m_ScaleFactor_init(matrix[11]),
      m_ConstantValueInitialized(false), // 
      m_Gizmo_noscaleRemover(new Gizmo_noscaleRemover())
  {
    Initialize(Moveable, Rotatable, Scalable);
  }

  void Gizmo_noscale::Initialize(bool Moveable, bool Rotatable, bool Scalable)
  {
    m_Center.Fill(0);

    m_AxisX.Fill(0);
    m_AxisX[0] = 1;
    m_AxisY.Fill(0);
    m_AxisY[1] = 1;
    m_AxisZ.Fill(0);
    m_AxisZ[2] = 1;

    m_Radius.Fill(1);
    std::cout << "Moveable is " << Moveable << std::endl;
    std::cout << "Rotatable is " << Rotatable << std::endl;
    std::cout << "Scalable is " << Scalable << std::endl;
    if (Moveable = false)
    {
      std::cout << "Moveable is false" << std::endl;
      UpdateRepresentation(false, true, true);
    }
  }

  //mitk::Gizmo_noscale::Pointer Gizmo_noscale::NewWithMatrix(const double matrix[16])
  //{
  //  Pointer smartPtr = new Gizmo_noscale(matrix);

  //  smartPtr->UnRegister();
  //  return smartPtr;
  //}
} // namespace mitk

mitk::Gizmo_noscale::~Gizmo_noscale()
{
  if (m_FollowedGeometry.IsNotNull())
  {
    m_FollowedGeometry->RemoveObserver(m_FollowerTag);
  }
}

void mitk::Gizmo_noscale::UpdateRepresentation(bool Moveable, bool Rotatable, bool Scalable)
{
  /* bounding box around the unscaled bounding object */
  ScalarType bounds[6] = {-m_Radius[0] * 1.2,
                          +m_Radius[0] * 1.2,
                          -m_Radius[1] * 1.2,
                          +m_Radius[1] * 1.2,
                          -m_Radius[2] * 1.2,
                          +m_Radius[2] * 1.2};
  GetGeometry()->SetBounds(bounds);
  GetTimeGeometry()->Update();
 //  scaleFactor_tmp = 0.5;
  std::cout << "here" << std::endl;
  SetVtkPolyData(BuildGizmo_noscale());
  
}

namespace
{
  void AssignScalarValueTo(vtkPolyData *polydata, char value)
  {
    vtkSmartPointer<vtkCharArray> pointData = vtkSmartPointer<vtkCharArray>::New();

    int numberOfPoints = polydata->GetNumberOfPoints();
    pointData->SetNumberOfComponents(1);
    pointData->SetNumberOfTuples(numberOfPoints);
    pointData->FillComponent(0, value);
    polydata->GetPointData()->SetScalars(pointData);
  }

  vtkSmartPointer<vtkPolyData> BuildAxis(const mitk::Point3D &center,
                                         const mitk::Vector3D &axis,
                                         double halflength,
                                         bool drawRing,
                                         char vertexValueAxis,
                                         char vertexValueRing,
                                         char vertexValueScale)
  {
    // Define all sizes relative to absolute size (thus that the gizmo will appear
    // in the same relative size for huge (size >> 1) and tiny (size << 1) objects).
    // This means that the gizmo will appear very different when a scene contains _both_
    // huge and tiny objects at the same time, but when the users zooms in on his
    // object of interest, the gizmo will always have the same relative size.
    const double shaftRadius = halflength * 0.02;
    const double arrowHeight = shaftRadius * 6;
    const int tubeSides = 15;

    // poly data appender to collect cones and tube that make up the axis
    vtkSmartPointer<vtkAppendPolyData> axisSource = vtkSmartPointer<vtkAppendPolyData>::New();

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
      // arrow tips at 110% of radius
      cone->SetCenter(center[0] + sign * axis[0] * (halflength * 1.1 + arrowHeight * 0.5),
                      center[1] + sign * axis[1] * (halflength * 1.1 + arrowHeight * 0.5),
                      center[2] + sign * axis[2] * (halflength * 1.1 + arrowHeight * 0.5));
      cone->SetDirection(sign * axis[0], sign * axis[1], sign * axis[2]);
      cone->SetRadius(shaftRadius * 3);
      cone->SetHeight(arrowHeight);
      cone->SetResolution(tubeSides);
      cone->CappingOn();
      cone->Update();

      AssignScalarValueTo(cone->GetOutput(), vertexValueScale);
      axisSource->AddInputData(cone->GetOutput());
    }

    // build the axis itself (as a tube around the line defining the axis)
    vtkSmartPointer<vtkPolyData> shaftSkeleton = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints = vtkSmartPointer<vtkPoints>::New();
    shaftPoints->InsertPoint(0, (center + axis * halflength * 1.1).GetDataPointer());
    shaftPoints->InsertPoint(1, (center + axis * halflength * 1.1 * 0.7).GetDataPointer());
    shaftSkeleton->SetPoints(shaftPoints);

    vtkSmartPointer<vtkCellArray> shaftLines = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints[] = {0, 1};
    shaftLines->InsertNextCell(2, shaftLinePoints);
    shaftSkeleton->SetLines(shaftLines);

    vtkSmartPointer<vtkTubeFilter> shaftSource = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource->SetInputData(shaftSkeleton);
    shaftSource->SetNumberOfSides(tubeSides);
    shaftSource->SetVaryRadiusToVaryRadiusOff();
    shaftSource->SetRadius(shaftRadius);
    shaftSource->Update();
    AssignScalarValueTo(shaftSource->GetOutput(), vertexValueAxis);

    vtkSmartPointer<vtkPolyData> shaftSkeleton2 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints2 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints2->InsertPoint(0, (center - axis * halflength * 1.1 * 0.7).GetDataPointer());
    shaftPoints2->InsertPoint(1, (center - axis * halflength * 1.1).GetDataPointer());
    shaftSkeleton2->SetPoints(shaftPoints2);

    vtkSmartPointer<vtkCellArray> shaftLines2 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints2[] = {0, 1};
    shaftLines2->InsertNextCell(2, shaftLinePoints2);
    shaftSkeleton2->SetLines(shaftLines2);
    vtkSmartPointer<vtkTubeFilter> shaftSource2 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource2->SetInputData(shaftSkeleton2);
    shaftSource2->SetNumberOfSides(tubeSides);
    shaftSource2->SetVaryRadiusToVaryRadiusOff();
    shaftSource2->SetRadius(shaftRadius);
    shaftSource2->Update();
    AssignScalarValueTo(shaftSource2->GetOutput(), vertexValueAxis);

    axisSource->AddInputData(shaftSource->GetOutput());
    axisSource->AddInputData(shaftSource2->GetOutput());
    axisSource->Update();

    // vtkSmartPointer<vtkTubeFilter> ringSource; // used after if block, so declare it here
    // if (drawRing)
    //{
    //  // build the ring orthogonal to the axis (as another tube)
    //  vtkSmartPointer<vtkPolyData> ringSkeleton = vtkSmartPointer<vtkPolyData>::New();
    //  vtkSmartPointer<vtkPoints> ringPoints = vtkSmartPointer<vtkPoints>::New();
    //  ringPoints->SetDataTypeToDouble(); // just some decision (see cast below)
    //  unsigned int numberOfRingPoints = 100;
    //  vtkSmartPointer<vtkCellArray> ringLines = vtkSmartPointer<vtkCellArray>::New();
    //  ringLines->InsertNextCell(numberOfRingPoints + 1);
    //  mitk::Vector3D ringPointer;
    //  for (unsigned int segment = 0; segment < numberOfRingPoints; ++segment)
    //  {
    //    ringPointer[0] = 0;
    //    ringPointer[1] = std::cos((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());
    //    ringPointer[2] = std::sin((double)(segment) / (double)numberOfRingPoints * 2.0 * vtkMath::Pi());

    //    ringPoints->InsertPoint(segment, (ringPointer * halflength).GetDataPointer());

    //    ringLines->InsertCellPoint(segment);
    //  }
    //  ringLines->InsertCellPoint(0);

    //  // transform ring points (copied from vtkConeSource)
    //  vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    //  t->Translate(center.GetDataPointer());
    //  double vMag = vtkMath::Norm(axis.GetDataPointer());
    //  if (axis[0] < 0.0)
    //  {
    //    // flip x -> -x to avoid instability
    //    t->RotateWXYZ(180.0, (axis[0] - vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
    //    t->RotateWXYZ(180.0, 0, 1, 0);
    //  }
    //  else
    //  {
    //    t->RotateWXYZ(180.0, (axis[0] + vMag) / 2.0, axis[1] / 2.0, axis[2] / 2.0);
    //  }

    //  double thisPoint[3];
    //  for (unsigned int i = 0; i < numberOfRingPoints; ++i)
    //  {
    //    ringPoints->GetPoint(i, thisPoint);
    //    t->TransformPoint(thisPoint, thisPoint);
    //    ringPoints->SetPoint(i, thisPoint);
    //  }

    //  ringSkeleton->SetPoints(ringPoints);
    //  ringSkeleton->SetLines(ringLines);

    //  ringSource = vtkSmartPointer<vtkTubeFilter>::New();
    //  ringSource->SetInputData(ringSkeleton);
    //  ringSource->SetNumberOfSides(tubeSides);
    //  ringSource->SetVaryRadiusToVaryRadiusOff();
    //  ringSource->SetRadius(shaftRadius);
    //  ringSource->Update();
    //  AssignScalarValueTo(ringSource->GetOutput(), vertexValueRing);
    //}

    // assemble axis and ring
    vtkSmartPointer<vtkAppendPolyData> appenderGlobal = vtkSmartPointer<vtkAppendPolyData>::New();
    appenderGlobal->AddInputData(axisSource->GetOutput());
    /* if (drawRing)
     {
       appenderGlobal->AddInputData(ringSource->GetOutput());
     }*/
    appenderGlobal->Update();

    // make everything shiny by adding normals
    vtkSmartPointer<vtkPolyDataNormals> normalsSource = vtkSmartPointer<vtkPolyDataNormals>::New();
    normalsSource->SetInputConnection(appenderGlobal->GetOutputPort());
    normalsSource->ComputePointNormalsOn();
    normalsSource->ComputeCellNormalsOff();
    normalsSource->SplittingOn();
    normalsSource->Update();

    vtkSmartPointer<vtkPolyData> result = normalsSource->GetOutput();
    return result;
  }

} // unnamed namespace

double mitk::Gizmo_noscale::GetLongestRadius() const
{
  double longestAxis = std::max(m_Radius[0], m_Radius[1]);
  longestAxis = std::max(longestAxis, m_Radius[2]);
  return longestAxis;
}

vtkSmartPointer<vtkPolyData> mitk::Gizmo_noscale::BuildGizmo_noscale()
{
  double longestAxis = GetLongestRadius();
  vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer<vtkAppendPolyData>::New();
  if (m_AllowMOVE && !m_AllowSCALE)
  {
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisX,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? MoveAlongAxisX : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisX3D : NoHandle,
                                     m_AllowScaling ? MoveAlongAxisX : NoHandle));
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisY,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? MoveAlongAxisY : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisY3D : NoHandle,
                                     m_AllowScaling ? MoveAlongAxisY : NoHandle));
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisZ,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? MoveAlongAxisZ : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisZ3D : NoHandle,
                                     m_AllowScaling ? MoveAlongAxisZ : NoHandle));
  }
  else if (!m_AllowMOVE && m_AllowSCALE)
  {
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisX,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? ScaleX : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisX3D : NoHandle,
                                     m_AllowScaling ? ScaleX : NoHandle));
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisY,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? ScaleY : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisY3D : NoHandle,
                                     m_AllowScaling ? ScaleY : NoHandle));
    appender->AddInputData(BuildAxis(m_Center,
                                     m_AxisZ,
                                     longestAxis,
                                     m_AllowRotation,
                                     m_AllowTranslation ? ScaleZ : NoHandle,
                                     m_AllowRotation ? RotateAroundAxisZ3D : NoHandle,
                                     m_AllowScaling ? ScaleZ : NoHandle));
  }
    else if (m_AllowMOVE && m_AllowSCALE)
    {
      appender->AddInputData(BuildAxis(m_Center,
                                       m_AxisX,
                                       longestAxis,
                                       m_AllowRotation,
                                       m_AllowTranslation ? MoveAlongAxisX : NoHandle,
                                       m_AllowRotation ? RotateAroundAxisX3D : NoHandle,
                                       m_AllowScaling ? ScaleX : NoHandle));
      appender->AddInputData(BuildAxis(m_Center,
                                       m_AxisY,
                                       longestAxis,
                                       m_AllowRotation,
                                       m_AllowTranslation ? MoveAlongAxisY : NoHandle,
                                       m_AllowRotation ? RotateAroundAxisY3D : NoHandle,
                                       m_AllowScaling ? ScaleY : NoHandle));
      appender->AddInputData(BuildAxis(m_Center,
                                       m_AxisZ,
                                       longestAxis,
                                       m_AllowRotation,
                                       m_AllowTranslation ? MoveAlongAxisZ : NoHandle,
                                       m_AllowRotation ? RotateAroundAxisZ3D : NoHandle,
                                       m_AllowScaling ? ScaleZ : NoHandle));
    }
  
 

  auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(m_Center[0], m_Center[1], m_Center[2]);
  sphereSource->SetRadius(longestAxis * 0.1);
  // sphereSource->
  sphereSource->Update();
  AssignScalarValueTo(sphereSource->GetOutput(), MoveFreely);
  appender->AddInputData(sphereSource->GetOutput());
  // Set drag line

  const double shaftRadius = longestAxis * 0.02;
  const double arrowHeight = shaftRadius * 6;
  const int tubeSides = 15;
  if (m_AllowROTATE)
  {
    auto sphereSource_Y1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_Y1->SetCenter(m_Center[0] + 1 * m_AxisZ[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] + 1 * m_AxisZ[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] + 1 * m_AxisZ[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_Y1->SetRadius(longestAxis * 0.05);
    sphereSource_Y1->Update();
    mitk::Point3D center_Y1;
    std::cout << "LongestAxis is :" << std::endl;
    std::cout << longestAxis << std::endl;
    center_Y1[0] = m_Center[0] + 1 * m_AxisZ[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Y1[1] = m_Center[1] + 1 * m_AxisZ[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Y1[2] = m_Center[2] + 1 * m_AxisZ[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_Y1 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_Y1 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_Y1->InsertPoint(0, (center_Y1 + longestAxis * m_AxisX * 0.2).GetDataPointer());
    shaftPoints_Y1->InsertPoint(1, (center_Y1 - longestAxis * m_AxisX * 0.2).GetDataPointer());
    shaftSkeleton_Y1->SetPoints(shaftPoints_Y1);

    // std::cout << "scaleFactor_reslut is : " << scaleFactor_reslut << std::endl;
    vtkSmartPointer<vtkCellArray> shaftLines_Y1 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_Y1[] = {0, 1};
    shaftLines_Y1->InsertNextCell(2, shaftLinePoints_Y1);
    shaftSkeleton_Y1->SetLines(shaftLines_Y1);

    vtkSmartPointer<vtkTubeFilter> shaftSource_Y1 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_Y1->SetInputData(shaftSkeleton_Y1);
    shaftSource_Y1->SetNumberOfSides(tubeSides);
    shaftSource_Y1->SetVaryRadiusToVaryRadiusOff();
    shaftSource_Y1->SetRadius(shaftRadius);
    shaftSource_Y1->Update();
    AssignScalarValueTo(shaftSource_Y1->GetOutput(), RotateAroundAxisY3D);
    AssignScalarValueTo(sphereSource_Y1->GetOutput(), RotateAroundAxisY3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_Y1 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_Y1;
      cone_center_Y1 = center_Y1 + 0.2 * longestAxis * sign * m_AxisX * 1.1;
      cone_Y1->SetCenter(cone_center_Y1[0], cone_center_Y1[1], cone_center_Y1[2]);
      cone_Y1->SetDirection(sign * m_AxisX[0], sign * m_AxisX[1], sign * m_AxisX[2]);
      cone_Y1->SetRadius(shaftRadius * 3);
      cone_Y1->SetHeight(arrowHeight);
      cone_Y1->SetResolution(tubeSides);
      cone_Y1->CappingOn();
      cone_Y1->Update();
      AssignScalarValueTo(cone_Y1->GetOutput(), RotateAroundAxisY3D);
      appender->AddInputData(cone_Y1->GetOutput());
    }
    appender->AddInputData(shaftSource_Y1->GetOutput());
    appender->AddInputData(sphereSource_Y1->GetOutput());

    auto sphereSource_Y2 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_Y2->SetCenter(m_Center[0] - 1 * m_AxisZ[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] - 1 * m_AxisZ[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] - 1 * m_AxisZ[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_Y2->SetRadius(longestAxis * 0.05);
    sphereSource_Y2->Update();
    mitk::Point3D center_Y2;
    center_Y2[0] = m_Center[0] - 1 * m_AxisZ[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Y2[1] = m_Center[1] - 1 * m_AxisZ[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Y2[2] = m_Center[2] - 1 * m_AxisZ[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_Y2 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_Y2 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_Y2->InsertPoint(0, (center_Y2 + longestAxis * m_AxisX * 0.2).GetDataPointer());
    shaftPoints_Y2->InsertPoint(1, (center_Y2 - longestAxis * m_AxisX * 0.2).GetDataPointer());
    shaftSkeleton_Y2->SetPoints(shaftPoints_Y2);

    vtkSmartPointer<vtkCellArray> shaftLines_Y2 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_Y2[] = {0, 1};
    shaftLines_Y2->InsertNextCell(2, shaftLinePoints_Y2);
    shaftSkeleton_Y2->SetLines(shaftLines_Y2);

    vtkSmartPointer<vtkTubeFilter> shaftSource_Y2 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_Y2->SetInputData(shaftSkeleton_Y2);
    shaftSource_Y2->SetNumberOfSides(tubeSides);
    shaftSource_Y2->SetVaryRadiusToVaryRadiusOff();
    shaftSource_Y2->SetRadius(shaftRadius);
    shaftSource_Y2->Update();
    AssignScalarValueTo(shaftSource_Y2->GetOutput(), RotateAroundAxisY3D);
    AssignScalarValueTo(sphereSource_Y2->GetOutput(), RotateAroundAxisY3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_Y2 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_Y2;
      cone_center_Y2 = center_Y2 + 0.2 * longestAxis * sign * m_AxisX * 1.1;
      cone_Y2->SetCenter(cone_center_Y2[0], cone_center_Y2[1], cone_center_Y2[2]);
      cone_Y2->SetDirection(sign * m_AxisX[0], sign * m_AxisX[1], sign * m_AxisX[2]);
      cone_Y2->SetRadius(shaftRadius * 3);
      cone_Y2->SetHeight(arrowHeight);
      cone_Y2->SetResolution(tubeSides);
      cone_Y2->CappingOn();
      cone_Y2->Update();
      AssignScalarValueTo(cone_Y2->GetOutput(), RotateAroundAxisY3D);
      appender->AddInputData(cone_Y2->GetOutput());
    }
    appender->AddInputData(shaftSource_Y2->GetOutput());
    appender->AddInputData(sphereSource_Y2->GetOutput());

    // add X_axis gizmo
    auto sphereSource_X1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_X1->SetCenter(m_Center[0] + 1 * m_AxisY[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] + 1 * m_AxisY[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] + 1 * m_AxisY[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_X1->SetRadius(longestAxis * 0.05);
    sphereSource_X1->Update();
    mitk::Point3D center_X1;
    center_X1[0] = m_Center[0] + 1 * m_AxisY[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_X1[1] = m_Center[1] + 1 * m_AxisY[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_X1[2] = m_Center[2] + 1 * m_AxisY[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_X1 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_X1 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_X1->InsertPoint(0, (center_X1 + longestAxis * m_AxisZ * 0.2).GetDataPointer());
    shaftPoints_X1->InsertPoint(1, (center_X1 - longestAxis * m_AxisZ * 0.2).GetDataPointer());
    shaftSkeleton_X1->SetPoints(shaftPoints_X1);

    vtkSmartPointer<vtkCellArray> shaftLines_X1 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_X1[] = {0, 1};
    shaftLines_X1->InsertNextCell(2, shaftLinePoints_X1);
    shaftSkeleton_X1->SetLines(shaftLines_X1);

    vtkSmartPointer<vtkTubeFilter> shaftSource_X1 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_X1->SetInputData(shaftSkeleton_X1);
    shaftSource_X1->SetNumberOfSides(tubeSides);
    shaftSource_X1->SetVaryRadiusToVaryRadiusOff();
    shaftSource_X1->SetRadius(shaftRadius);
    shaftSource_X1->Update();
    AssignScalarValueTo(shaftSource_X1->GetOutput(), RotateAroundAxisX3D);
    AssignScalarValueTo(sphereSource_X1->GetOutput(), RotateAroundAxisX3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_X1 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_X1;
      cone_center_X1 = center_X1 + 0.2 * longestAxis * sign * m_AxisZ * 1.1;
      cone_X1->SetCenter(cone_center_X1[0], cone_center_X1[1], cone_center_X1[2]);
      cone_X1->SetDirection(sign * m_AxisZ[0], sign * m_AxisZ[1], sign * m_AxisZ[2]);
      cone_X1->SetRadius(shaftRadius * 3);
      cone_X1->SetHeight(arrowHeight);
      cone_X1->SetResolution(tubeSides);
      cone_X1->CappingOn();
      cone_X1->Update();
      AssignScalarValueTo(cone_X1->GetOutput(), RotateAroundAxisX3D);
      appender->AddInputData(cone_X1->GetOutput());
    }
    appender->AddInputData(shaftSource_X1->GetOutput());
    appender->AddInputData(sphereSource_X1->GetOutput());

    auto sphereSource_X2 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_X2->SetCenter(m_Center[0] - 1 * m_AxisY[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] - 1 * m_AxisY[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] - 1 * m_AxisY[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_X2->SetRadius(longestAxis * 0.05);
    sphereSource_X2->Update();
    mitk::Point3D center_X2;
    center_X2[0] = m_Center[0] - 1 * m_AxisY[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_X2[1] = m_Center[1] - 1 * m_AxisY[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_X2[2] = m_Center[2] - 1 * m_AxisY[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_X2 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_X2 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_X2->InsertPoint(0, (center_X2 + longestAxis * m_AxisZ * 0.2).GetDataPointer());
    shaftPoints_X2->InsertPoint(1, (center_X2 - longestAxis * m_AxisZ * 0.2).GetDataPointer());
    shaftSkeleton_X2->SetPoints(shaftPoints_X2);

    vtkSmartPointer<vtkCellArray> shaftLines_X2 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_X2[] = {0, 1};
    shaftLines_X2->InsertNextCell(2, shaftLinePoints_X2);
    shaftSkeleton_X2->SetLines(shaftLines_X2);

    vtkSmartPointer<vtkTubeFilter> shaftSource_X2 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_X2->SetInputData(shaftSkeleton_X2);
    shaftSource_X2->SetNumberOfSides(tubeSides);
    shaftSource_X2->SetVaryRadiusToVaryRadiusOff();
    shaftSource_X2->SetRadius(shaftRadius);
    shaftSource_X2->Update();
    AssignScalarValueTo(shaftSource_X2->GetOutput(), RotateAroundAxisX3D);
    AssignScalarValueTo(sphereSource_X2->GetOutput(), RotateAroundAxisX3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_X2 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_X2;
      cone_center_X2 = center_X2 + 0.2 * longestAxis * sign * m_AxisZ * 1.1;
      cone_X2->SetCenter(cone_center_X2[0], cone_center_X2[1], cone_center_X2[2]);
      cone_X2->SetDirection(sign * m_AxisZ[0], sign * m_AxisZ[1], sign * m_AxisZ[2]);
      cone_X2->SetRadius(shaftRadius * 3);
      cone_X2->SetHeight(arrowHeight);
      cone_X2->SetResolution(tubeSides);
      cone_X2->CappingOn();
      cone_X2->Update();
      AssignScalarValueTo(cone_X2->GetOutput(), RotateAroundAxisX3D);
      appender->AddInputData(cone_X2->GetOutput());
    }
    appender->AddInputData(shaftSource_X2->GetOutput());
    appender->AddInputData(sphereSource_X2->GetOutput());

    // add Z_axis gizmo
    auto sphereSource_Z1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_Z1->SetCenter(m_Center[0] + 1 * m_AxisX[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] + 1 * m_AxisX[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] + 1 * m_AxisX[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_Z1->SetRadius(longestAxis * 0.05);
    sphereSource_Z1->Update();
    mitk::Point3D center_Z1;
    center_Z1[0] = m_Center[0] + 1 * m_AxisX[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Z1[1] = m_Center[1] + 1 * m_AxisX[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Z1[2] = m_Center[2] + 1 * m_AxisX[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_Z1 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_Z1 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_Z1->InsertPoint(0, (center_Z1 + longestAxis * m_AxisY * 0.2).GetDataPointer());
    shaftPoints_Z1->InsertPoint(1, (center_Z1 - longestAxis * m_AxisY * 0.2).GetDataPointer());
    shaftSkeleton_Z1->SetPoints(shaftPoints_Z1);

    vtkSmartPointer<vtkCellArray> shaftLines_Z1 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_Z1[] = {0, 1};
    shaftLines_Z1->InsertNextCell(2, shaftLinePoints_Z1);
    shaftSkeleton_Z1->SetLines(shaftLines_Z1);

    vtkSmartPointer<vtkTubeFilter> shaftSource_Z1 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_Z1->SetInputData(shaftSkeleton_Z1);
    shaftSource_Z1->SetNumberOfSides(tubeSides);
    shaftSource_Z1->SetVaryRadiusToVaryRadiusOff();
    shaftSource_Z1->SetRadius(shaftRadius);
    shaftSource_Z1->Update();
    AssignScalarValueTo(shaftSource_Z1->GetOutput(), RotateAroundAxisZ3D);
    AssignScalarValueTo(sphereSource_Z1->GetOutput(), RotateAroundAxisZ3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_Z1 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_Z1;
      cone_center_Z1 = center_Z1 + 0.2 * longestAxis * sign * m_AxisY * 1.1;
      cone_Z1->SetCenter(cone_center_Z1[0], cone_center_Z1[1], cone_center_Z1[2]);
      cone_Z1->SetDirection(sign * m_AxisY[0], sign * m_AxisY[1], sign * m_AxisY[2]);
      cone_Z1->SetRadius(shaftRadius * 3);
      cone_Z1->SetHeight(arrowHeight);
      cone_Z1->SetResolution(tubeSides);
      cone_Z1->CappingOn();
      cone_Z1->Update();
      AssignScalarValueTo(cone_Z1->GetOutput(), RotateAroundAxisZ3D);
      appender->AddInputData(cone_Z1->GetOutput());
    }
    appender->AddInputData(shaftSource_Z1->GetOutput());
    appender->AddInputData(sphereSource_Z1->GetOutput());

    auto sphereSource_Z2 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource_Z2->SetCenter(m_Center[0] - 1 * m_AxisX[0] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[1] - 1 * m_AxisX[1] * (longestAxis * 1.5 + arrowHeight * 0.5),
                               m_Center[2] - 1 * m_AxisX[2] * (longestAxis * 1.5 + arrowHeight * 0.5));
    sphereSource_Z2->SetRadius(longestAxis * 0.05);
    sphereSource_Z2->Update();
    mitk::Point3D center_Z2;
    center_Z2[0] = m_Center[0] - 1 * m_AxisX[0] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Z2[1] = m_Center[1] - 1 * m_AxisX[1] * (longestAxis * 1.5 + arrowHeight * 0.5);
    center_Z2[2] = m_Center[2] - 1 * m_AxisX[2] * (longestAxis * 1.5 + arrowHeight * 0.5);
    vtkSmartPointer<vtkPolyData> shaftSkeleton_Z2 = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> shaftPoints_Z2 = vtkSmartPointer<vtkPoints>::New();
    shaftPoints_Z2->InsertPoint(0, (center_Z2 + longestAxis * m_AxisY * 0.2).GetDataPointer());
    shaftPoints_Z2->InsertPoint(1, (center_Z2 - longestAxis * m_AxisY * 0.2).GetDataPointer());
    shaftSkeleton_Z2->SetPoints(shaftPoints_Z2);

    vtkSmartPointer<vtkCellArray> shaftLines_Z2 = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType shaftLinePoints_Z2[] = {0, 1};
    shaftLines_Z2->InsertNextCell(2, shaftLinePoints_Z2);
    shaftSkeleton_Z2->SetLines(shaftLines_Z2);

    vtkSmartPointer<vtkTubeFilter> shaftSource_Z2 = vtkSmartPointer<vtkTubeFilter>::New();
    shaftSource_Z2->SetInputData(shaftSkeleton_Z2);
    shaftSource_Z2->SetNumberOfSides(tubeSides);
    shaftSource_Z2->SetVaryRadiusToVaryRadiusOff();
    shaftSource_Z2->SetRadius(shaftRadius);
    shaftSource_Z2->Update();
    AssignScalarValueTo(shaftSource_Z2->GetOutput(), RotateAroundAxisZ3D);
    AssignScalarValueTo(sphereSource_Z2->GetOutput(), RotateAroundAxisZ3D);

    // build two cones at the end of axis
    for (double sign = -1.0; sign < 3.0; sign += 2)
    {
      vtkSmartPointer<vtkConeSource> cone_Z2 = vtkConeSource::New();
      // arrow tips at 110% of radius
      mitk::Point3D cone_center_Z2;
      cone_center_Z2 = center_Z2 + 0.2 * longestAxis * sign * m_AxisY * 1.1;
      cone_Z2->SetCenter(cone_center_Z2[0], cone_center_Z2[1], cone_center_Z2[2]);
      cone_Z2->SetDirection(sign * m_AxisY[0], sign * m_AxisY[1], sign * m_AxisY[2]);
      cone_Z2->SetRadius(shaftRadius * 3);
      cone_Z2->SetHeight(arrowHeight);
      cone_Z2->SetResolution(tubeSides);
      cone_Z2->CappingOn();
      cone_Z2->Update();
      AssignScalarValueTo(cone_Z2->GetOutput(), RotateAroundAxisZ3D);
      appender->AddInputData(cone_Z2->GetOutput());
    }
    appender->AddInputData(shaftSource_Z2->GetOutput());
    appender->AddInputData(sphereSource_Z2->GetOutput());
  }
  appender->Update();

  return appender->GetOutput();
}




void mitk::Gizmo_noscale::FollowGeometry(BaseGeometry *geom, bool Moveable, bool Rotatable, bool Scalable)
{
  auto observer = itk::SimpleMemberCommand<Gizmo_noscale>::New();
  observer->SetCallbackFunction(this, &Gizmo_noscale::OnFollowedGeometryModified);
  m_AllowMOVE = Moveable;
  m_AllowROTATE = Rotatable;
  m_AllowSCALE = Scalable;
  if (m_FollowedGeometry.IsNotNull())
  {
    m_FollowedGeometry->RemoveObserver(m_FollowerTag);
  }

  m_FollowedGeometry = geom;
  m_FollowerTag = m_FollowedGeometry->AddObserver(itk::ModifiedEvent(), observer);

  // initial adjustment
  OnFollowedGeometryModified();
}

void mitk::Gizmo_noscale::OnFollowedGeometryModified()
{
  m_Center = m_FollowedGeometry->GetCenter();

  m_AxisX = m_FollowedGeometry->GetAxisVector(0);
  m_AxisY = m_FollowedGeometry->GetAxisVector(1);
  m_AxisZ = m_FollowedGeometry->GetAxisVector(2);

  m_AxisX.Normalize();
  m_AxisY.Normalize();
  m_AxisZ.Normalize();

  for (int dim = 0; dim < 3; ++dim)
  {
    m_Radius[dim] = 0.5 * m_FollowedGeometry->GetExtentInMM(dim);
  }

  UpdateRepresentation(m_AllowMOVE, m_AllowROTATE, m_AllowSCALE);
}

mitk::Gizmo_noscale::HandleType mitk::Gizmo_noscale::GetHandleFromPointDataValue(double value)
{
#define CheckHandleType(type)                                                                                          \
  if (static_cast<int>(value) == static_cast<int>(type))                                                               \
    return type;

  CheckHandleType(MoveFreely);
  CheckHandleType(MoveAlongAxisX);
  CheckHandleType(MoveAlongAxisY);
  CheckHandleType(MoveAlongAxisZ);
  CheckHandleType(RotateAroundAxisX);
  CheckHandleType(RotateAroundAxisY);
  CheckHandleType(RotateAroundAxisZ);
  CheckHandleType(RotateAroundAxisX3D);
  CheckHandleType(RotateAroundAxisY3D);
  CheckHandleType(RotateAroundAxisZ3D);
  CheckHandleType(ScaleX);
  CheckHandleType(ScaleY);
  CheckHandleType(ScaleZ);
  return NoHandle;
#undef CheckHandleType
}

mitk::Gizmo_noscale::HandleType mitk::Gizmo_noscale::GetHandleFromPointID(vtkIdType id)
{
  assert(GetVtkPolyData());
  assert(GetVtkPolyData()->GetPointData());
  assert(GetVtkPolyData()->GetPointData()->GetScalars());
  double dataValue = GetVtkPolyData()->GetPointData()->GetScalars()->GetTuple1(id);
  return GetHandleFromPointDataValue(dataValue);
}

std::string mitk::Gizmo_noscale::HandleTypeToString(HandleType type)
{
#define CheckHandleType(candidateType)                                                                                 \
  if (type == candidateType)                                                                                           \
    return std::string(#candidateType);

  CheckHandleType(MoveFreely);
  CheckHandleType(MoveAlongAxisX);
  CheckHandleType(MoveAlongAxisY);
  CheckHandleType(MoveAlongAxisZ);
  CheckHandleType(RotateAroundAxisX);
  CheckHandleType(RotateAroundAxisY);
  CheckHandleType(RotateAroundAxisZ);
  CheckHandleType(RotateAroundAxisX3D);
  CheckHandleType(RotateAroundAxisY3D);
  CheckHandleType(RotateAroundAxisZ3D);
  CheckHandleType(ScaleX);
  CheckHandleType(ScaleY);
  CheckHandleType(ScaleZ);
  CheckHandleType(NoHandle);
  return "InvalidHandleType";
#undef CheckHandleType
}

//void mitk::Gizmo_noscale::SetViewTransformMatrix(vtkSmartPointer<vtkMatrix4x4> viewTransformMatrix)
//{
//  m_ViewTransformMatrix->DeepCopy(viewTransformMatrix);
//}
//
//void mitk::Gizmo_noscale::GetRotationMatrix(const double matrix[16])
//{
//  for (int i = 0; i < 16; ++i)
//  {
//    m_RotationMatrix_cam[i] = matrix[i];
//  }
//  std::cout << "Rotation Matrix gizmo_noscale:" << std::endl;
//  for (int i = 0; i < 16; ++i)
//  {
//    std::cout << m_RotationMatrix_cam[i] << " ";
//  }
//  std::cout << std::endl;
//}
