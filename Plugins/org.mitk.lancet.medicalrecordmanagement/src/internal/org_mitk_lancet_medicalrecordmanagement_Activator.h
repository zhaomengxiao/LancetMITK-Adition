/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef org_mitk_lancet_medicalrecordmanagement_Activator_h
#define org_mitk_lancet_medicalrecordmanagement_Activator_h

#include <ctkPluginActivator.h>

namespace mitk
{
  class org_mitk_lancet_medicalrecordmanagement_Activator : public QObject, public ctkPluginActivator
  {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org_mitk_lancet_medicalrecordmanagement")
    Q_INTERFACES(ctkPluginActivator)

  public:
    org_mitk_lancet_medicalrecordmanagement_Activator();
    void start(ctkPluginContext *context);
    void stop(ctkPluginContext *context);

    static ctkPluginContext* GetPluginContext();
  private:
    struct PluginActivatorPrivateImp;
    std::shared_ptr<PluginActivatorPrivateImp> imp;
  }; // org_mitk_lancet_medicalrecordmanagement_Activator

  typedef org_mitk_lancet_medicalrecordmanagement_Activator PluginActivator;
}

#endif // org_mitk_lancet_medicalrecordmanagement_Activator_h
