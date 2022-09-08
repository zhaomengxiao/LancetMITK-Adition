/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "org_mitk_lancet_example_editor_Activator.h"

#include "QLancetExampleWidgetEditor.h"
//#include "acetabularpreparationwidget.h"
#include "QLancetExampleWidgetEditorPreferencePage.h"

ctkPluginContext* org_mitk_lancet_example_editor_Activator::m_Context = nullptr;

void
org_mitk_lancet_example_editor_Activator::start(ctkPluginContext* context)
{
  m_Context = context; 

  BERRY_REGISTER_EXTENSION_CLASS(QLancetExampleWidgetEditor, context)
  BERRY_REGISTER_EXTENSION_CLASS(QLancetExampleWidgetEditorPreferencePage, context)
  //BERRY_REGISTER_EXTENSION_CLASS(Acetabularpreparationwidgetview, context)
}

void
org_mitk_lancet_example_editor_Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  m_Context = nullptr;
}

ctkPluginContext *org_mitk_lancet_example_editor_Activator::GetContext()
{
  return m_Context;
}
