/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkMoveObjectObjectFactory.h"
#include "mitkMoveObjectVtkMapper2D.h"
#include "mitkMoveObjectVtkMapper3D.h"
#include <mitkCoreObjectFactory.h>

mitk::MoveObjectObjectFactory::MoveObjectObjectFactory()
{
}

mitk::MoveObjectObjectFactory::~MoveObjectObjectFactory()
{
}

mitk::Mapper::Pointer mitk::MoveObjectObjectFactory::CreateMapper(mitk::DataNode *node, MapperSlotId slotId)
{
  Mapper::Pointer mapper;

  if (dynamic_cast<GeometryData *>(node->GetData()) != nullptr)
  {
    if (slotId == BaseRenderer::Standard2D)
    {
      mapper = MoveObjectVtkMapper2D::New();
    }
    else if (slotId == BaseRenderer::Standard3D)
    {
      mapper = MoveObjectVtkMapper3D::New();
    }

    if (mapper.IsNotNull())
      mapper->SetDataNode(node);
  }

  return mapper;
}

const char *mitk::MoveObjectObjectFactory::GetDescription() const
{
  return "MoveObject Object Factory";
}

std::string mitk::MoveObjectObjectFactory::GetFileExtensions()
{
  return "";
}

mitk::CoreObjectFactoryBase::MultimapType mitk::MoveObjectObjectFactory::GetFileExtensionsMap()
{
  return MultimapType();
}

std::string mitk::MoveObjectObjectFactory::GetSaveFileExtensions()
{
  return "";
}

mitk::CoreObjectFactoryBase::MultimapType mitk::MoveObjectObjectFactory::GetSaveFileExtensionsMap()
{
  return MultimapType();
}

void mitk::MoveObjectObjectFactory::SetDefaultProperties(mitk::DataNode *node)
{
  if (node == nullptr)
    return;

  if (dynamic_cast<GeometryData *>(node->GetData()) != nullptr)
  {
    MoveObjectVtkMapper2D::SetDefaultProperties(node);
    MoveObjectVtkMapper3D::SetDefaultProperties(node);
  }
}

void mitk::RegisterMoveObjectObjectFactory()
{
  static bool alreadyRegistered = false;

  if (!alreadyRegistered)
  {
    CoreObjectFactory::GetInstance()->RegisterExtraFactory(MoveObjectObjectFactory::New());
    alreadyRegistered = true;
  }
}
