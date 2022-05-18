/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "nodebinder.h"
#include "mitkDataNode.h"
#include "mitkApplyTransformMatrixOperation.h"
#include "mitkInteractionConst.h"

void NodeBinder::Execute(itk::Object *caller, const itk::EventObject &event)
{
	mitk::BaseGeometry* callerGeometry = dynamic_cast<mitk::BaseGeometry*>(caller);
	if (callerGeometry == nullptr)
	{
		MITK_ERROR << "Execute nodeBinder Error: caller Geometry null";
		return;
	}
	//auto Tg2ref = basedata_caller->GetGeometry()->GetVtkMatrix();

	const auto Tg2ref = callerGeometry->GetVtkMatrix();
	// if (m_matrixList.empty())
	// {
	//   MITK_ERROR << "Execute nodeBinder Error: matrix null";
	//   return;
	// }
	for (auto& name_node_pair : m_attachmentList)
	{
		auto Tg2attach = vtkMatrix4x4::New();
		auto iter = m_matrixList.find(name_node_pair.first);
		if (iter == m_matrixList.end())
		{
			MITK_ERROR << "Execute nodeBinder Error: can not find Matrix match the node";
			return;
		}
		auto Tref2attach = iter->second;
		vtkMatrix4x4::Multiply4x4(Tg2ref, Tref2attach, Tg2attach);

		double ref[3]{ 0, 0, 0 };
		auto op = new mitk::ApplyTransformMatrixOperation(mitk::OpAPPLYTRANSFORMMATRIX, Tg2attach, ref);
		if (name_node_pair.second == nullptr)
		{
			MITK_ERROR << "Execute nodeBinder Error: attachmentNode null ";
			return;
		}
		name_node_pair.second->GetData()->GetGeometry()->ExecuteOperation(op);
		delete op;
	}
 
}

void NodeBinder::Execute(const itk::Object *caller, const itk::EventObject &event)
{
  // MITK_INFO << "nodeBinder Executed";
  /*auto callerNode = dynamic_cast<const mitk::DataNode *>(caller);
  if (callerNode == nullptr)
  {
    MITK_ERROR << "reference node null";
    return;
  }
  auto basedata_caller = callerNode->GetData();
  if (basedata_caller == nullptr)
  {
    MITK_ERROR << "reference node data null";
    return;
  }*/
	//Execute((const itk::Object*)caller, event);
  
}

void NodeBinder::AddBindingNode(mitk::DataNode *attachment)
{
  if (attachment == nullptr || attachment->GetData() == nullptr)
  {
    MITK_ERROR << "binding failed: bind null dataNode";
    return;
  }
  m_attachmentList.insert_or_assign(attachment->GetName(), attachment);
}

void NodeBinder::SetReferenceNode(mitk::DataNode *ref_node)
{
  m_refNode = ref_node;
}

void NodeBinder::UpdateBinding()
{
  if (m_refNode == nullptr)
  {
    MITK_ERROR << "binding node failed: Call SetReferenceNode before UpdateBinding";
    return;
  }
  MITK_INFO << "[nodeBinder]";
  MITK_INFO << "--------------------------------------------";
  for (auto &kv : m_attachmentList)
  {
    MITK_INFO << kv.first << " -> " << m_refNode->GetName();

    //Cal Transform Matrix
    //get global coords of reference node
    auto Tg2ref = m_refNode->GetData()->GetGeometry()->GetVtkMatrix();
    //get global coords of attachment node
    auto Tg2attach = kv.second->GetData()->GetGeometry()->GetVtkMatrix();

    vtkMatrix4x4 *Tref2g = vtkMatrix4x4::New();
    vtkMatrix4x4::Invert(Tg2ref, Tref2g);

    vtkMatrix4x4 *Tref2attach = vtkMatrix4x4::New();
    vtkMatrix4x4::Multiply4x4(Tref2g, Tg2attach, Tref2attach);

    m_matrixList.insert_or_assign(kv.first, Tref2attach);
  }
  MITK_INFO << "--------------------------------------------";
}

void NodeBinder::DisableBind()
{
  if (m_refNode == nullptr)
  {
    return;
  }
  m_refNode->RemoveObserver(m_commandTag);
  m_isBind = false;
}

void NodeBinder::EnableBind()
{
  if (m_refNode == nullptr)
  {
      MITK_ERROR << "Node Binder Enable faild: reference node null";
    return;
  }
  UpdateBinding();
  m_refNode->GetData()->GetGeometry()->AddObserver(itk::ModifiedEvent(), this);
  m_isBind = true;
}
