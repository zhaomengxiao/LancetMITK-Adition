/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#include "org_mitk_lancet_linkinghardware_Activator.h"
#include "QLinkingHardware.h"

namespace mitk
{
    struct PluginActivator::PluginActivatorPrivateImp
    {
        static ctkPluginContext* context;
    };
    ctkPluginContext* PluginActivator::PluginActivatorPrivateImp::context = nullptr;

    org_mitk_lancet_linkinghardware_Activator::org_mitk_lancet_linkinghardware_Activator()
        : imp(std::make_shared<PluginActivatorPrivateImp>())
    {
        qDebug() << "";
    }

    void org_mitk_lancet_linkinghardware_Activator::start(ctkPluginContext* context)
    {
        this->imp->context = context;
        BERRY_REGISTER_EXTENSION_CLASS(QLinkingHardware, context)
    }

    void org_mitk_lancet_linkinghardware_Activator::stop(ctkPluginContext* context)
    {
        Q_UNUSED(context);
        this->imp->context = nullptr;
    }

    ctkPluginContext* org_mitk_lancet_linkinghardware_Activator::GetPluginContext()
    {
        return PluginActivatorPrivateImp::context;
    }
}
