/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "org_mitk_lancet_gui_qt_testsuite_Activator.h"

#include "TestSuiteApplication.h"
#include "DevicePerspective.h"
#include "ImageRegistPerspective.h"

void org_mitk_lancet_gui_qt_testsuit_Activator::start(ctkPluginContext *context)
{
  BERRY_REGISTER_EXTENSION_CLASS(TestSuiteApplication, context)
  BERRY_REGISTER_EXTENSION_CLASS(DevicePerspective, context)
  BERRY_REGISTER_EXTENSION_CLASS(ImageRegistPerspective, context)
}

void org_mitk_lancet_gui_qt_testsuit_Activator::stop(ctkPluginContext *context)
{
  Q_UNUSED(context)
}
