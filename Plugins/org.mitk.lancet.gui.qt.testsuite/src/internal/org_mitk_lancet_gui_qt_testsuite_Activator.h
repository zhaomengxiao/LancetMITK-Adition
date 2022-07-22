/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef ORG_MITK_LANCET_GUI_QT_TESTSUITE_ACTIVATOR_H
#define ORG_MITK_LANCET_GUI_QT_TESTSUITE_ACTIVATOR_H

#include <ctkPluginActivator.h>

class org_mitk_lancet_gui_qt_testsuit_Activator : public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org_mitk_example_gui_multipleperspectives")
  Q_INTERFACES(ctkPluginActivator)

public:
  void start(ctkPluginContext *context) override;
  void stop(ctkPluginContext *context) override;
};

#endif // ORG_MITK_LANCET_GUI_QT_TESTSUITE_ACTIVATOR_H
