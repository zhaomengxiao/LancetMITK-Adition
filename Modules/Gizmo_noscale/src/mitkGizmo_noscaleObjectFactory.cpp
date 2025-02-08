/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkGizmo_noscaleObjectFactory.h"

// Project includes
#include "mitkGizmo_noscale.h"
#include "mitkGizmo_noscaleMapper2D.h"

// MITK includes
#include <mitkBaseRenderer.h>
#include <mitkDataNode.h>
#include <mitkProperties.h>

mitk::Mapper::Pointer mitk::Gizmo_noscaleObjectFactory::CreateMapper(mitk::DataNode *node, MapperSlotId id)
{
  mitk::Mapper::Pointer newMapper = nullptr;

  if ((dynamic_cast<mitk::Gizmo_noscale *>(node->GetData()) != nullptr))
  {
    if (id == mitk::BaseRenderer::Standard2D)
    {
      newMapper = mitk::Gizmo_noscaleMapper2D::New();
      newMapper->SetDataNode(node);
    }
  }

  return newMapper;
}

void mitk::Gizmo_noscaleObjectFactory::SetDefaultProperties(mitk::DataNode *node)
{
  if ((dynamic_cast<mitk::Gizmo_noscale *>(node->GetData()) != nullptr))
  {
    Gizmo_noscaleMapper2D::SetDefaultProperties(node);
  }
}

struct RegisterGizmo_noscaleObjectFactory
{
  RegisterGizmo_noscaleObjectFactory() : m_Factory(mitk::Gizmo_noscaleObjectFactory::New())
  {
    mitk::CoreObjectFactory::GetInstance()->RegisterExtraFactory(m_Factory);
  }
  ~RegisterGizmo_noscaleObjectFactory() { mitk::CoreObjectFactory::GetInstance()->UnRegisterExtraFactory(m_Factory); }
  mitk::Gizmo_noscaleObjectFactory::Pointer m_Factory;
};

static RegisterGizmo_noscaleObjectFactory registerGizmo_noscaleObjectFactory;
