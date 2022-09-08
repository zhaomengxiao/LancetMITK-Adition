/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "org_mitk_lancet_example_application_Activator.h"

#include "QLancetExampleApplication.h"
#include "QLancetExamplePerspective.h"

ctkPluginContext* org_mitk_lancet_example_application_Activator::staticPluginContext = nullptr;
void org_mitk_lancet_example_application_Activator::start(ctkPluginContext *context)
{
  staticPluginContext = context;
  BERRY_REGISTER_EXTENSION_CLASS(QLancetExampleApplication, context)
  BERRY_REGISTER_EXTENSION_CLASS(QLancetExamplePerspective, context)
}

void org_mitk_lancet_example_application_Activator::stop(ctkPluginContext *context)
{
  Q_UNUSED(context)
}

ctkPluginContext* org_mitk_lancet_example_application_Activator::GetPluginContext()
{
	return staticPluginContext;
}