/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetPathPoint.h"





lancet::PointPath::PointPath()
{
}

lancet::PointPath::PointPath(const PointPath& other)
{
}

lancet::PointPath::~PointPath()
{
}

itk::LightObject::Pointer lancet::PointPath::InternalClone() const
{
  return DataObject::InternalClone();
}
