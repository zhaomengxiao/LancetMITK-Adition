/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "surfaceboolean.h"
#include "mitkDataNode.h"
#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"
#include "mitkSurface.h"
#include "mitkSurfaceOperation.h"
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkBooleanOperationPolyDataFilter.h>

void SurfaceBoolean::Execute(itk::Object *caller, const itk::EventObject &event)
{
  Execute((const itk::Object *)caller, event);
}

void SurfaceBoolean::Execute(const itk::Object *caller, const itk::EventObject &event)
{
    assert(m_RefNode);
    assert(m_MoveNode);
    auto refsurface = dynamic_cast<mitk::Surface*> (m_RefNode->GetData());
    assert(refsurface);
    auto movesurface = dynamic_cast<mitk::Surface*> (m_MoveNode->GetData());

  const clock_t cleanPolydata_start  = clock();
  //clean polydata first
  vtkNew<vtkTriangleFilter> tri1;
  tri1->SetInputData(movesurface->GetVtkPolyData());
  vtkNew<vtkCleanPolyData> clean1;
  clean1->SetInputConnection(tri1->GetOutputPort());
  clean1->Update();
  auto input1 = clean1->GetOutput();

  vtkNew<vtkTriangleFilter> tri2;
  tri2->SetInputData(refsurface->GetVtkPolyData());
  vtkNew<vtkCleanPolyData> clean2;
  clean2->SetInputConnection(tri2->GetOutputPort());
  clean2->Update();
  auto input2 = clean2->GetOutput();

  float cleanPolydata_end = float(clock() - cleanPolydata_start) / CLOCKS_PER_SEC;
  MITK_INFO << "image clean time is " << cleanPolydata_end ;
  //boolean
  vtkNew<vtkBooleanOperationPolyDataFilter> booleanOperation;
  // if (operation == "union")
  // {
  //     booleanOperation->SetOperationToUnion();
  // }
  // else if (operation == "intersection")
  // {
  //     booleanOperation->SetOperationToIntersection();
  // }
  // else if (operation == "difference")
  // {
  //     booleanOperation->SetOperationToDifference();
  // }
  // else
  // {
  //     std::cout << "Unknown operation: " << operation << std::endl;
  //     return EXIT_FAILURE;
  // }
  const clock_t boolean_start = clock();
  booleanOperation->SetOperationToDifference();
  booleanOperation->SetInputData(0, input1);
  booleanOperation->SetInputData(1, input2);
  booleanOperation->Update();

  float boolean_end = float(clock() - boolean_start) / CLOCKS_PER_SEC;
  MITK_INFO << "boolean time is " << boolean_end;

  auto op = new mitk::SurfaceOperation(mitk::OpSURFACECHANGED, booleanOperation->GetOutput(),0);
  refsurface->ExecuteOperation(op);
  delete op;
}

void SurfaceBoolean::SetMovingNode(mitk::DataNode *move_node)
{
    m_MoveNode = move_node;
}

void SurfaceBoolean::SetReferenceNode(mitk::DataNode *ref_node)
{
  m_RefNode = ref_node;
}

void SurfaceBoolean::Update()
{
  if (m_RefNode != nullptr&&m_MoveNode!=nullptr)
  {
      MITK_INFO << "Cutter" << m_MoveNode->GetName();
      MITK_INFO << "refnode" << m_RefNode->GetName();
  }
    
  // if (m_refNode == nullptr)
  // {
  //   MITK_ERROR << "binding node failed: Call SetReferenceNode before UpdateBinding";
  //   return;
  // }
  // MITK_INFO << "[nodeBinder]";
  // MITK_INFO << "--------------------------------------------";
  // for (auto &kv : m_attachmentList)
  // {
  //   MITK_INFO << kv.first << " -> " << m_refNode->GetName();
  //
  //   //Cal Transform Matrix
  //   //get global coords of reference node
  //   auto Tg2ref = m_refNode->GetData()->GetGeometry()->GetVtkMatrix();
  //   //get global coords of attachment node
  //   auto Tg2attach = kv.second->GetData()->GetGeometry()->GetVtkMatrix();
  //
  //   vtkMatrix4x4 *Tref2g = vtkMatrix4x4::New();
  //   vtkMatrix4x4::Invert(Tg2ref, Tref2g);
  //
  //   vtkMatrix4x4 *Tref2attach = vtkMatrix4x4::New();
  //   vtkMatrix4x4::Multiply4x4(Tref2g, Tg2attach, Tref2attach);
  //
  //   m_matrixList.insert_or_assign(kv.first, Tref2attach);
  // }
  // MITK_INFO << "--------------------------------------------";
}

void SurfaceBoolean::Disable()
{
  if (m_RefNode == nullptr)
  {
    return;
  }
  m_MoveNode->RemoveObserver(m_commandTag);
  m_IsEnable = false;
}

void SurfaceBoolean::Enable()
{
  if (m_MoveNode == nullptr)
  {
    MITK_ERROR << "SurfaceBoolean Enable faild: move node null";
    return;
  }
  Update();
  m_MoveNode->GetData()->GetGeometry()->AddObserver(itk::ModifiedEvent(), this);
  m_IsEnable = true;
}
