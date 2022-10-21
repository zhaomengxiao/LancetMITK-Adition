/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#include "org_mitk_lancet_contusion_Activator.h"
#include "QContusion.h"

namespace mitk
{
  void org_mitk_lancet_contusion_Activator::start(ctkPluginContext *context)
  {
    BERRY_REGISTER_EXTENSION_CLASS(QContusion, context)
  }

  void org_mitk_lancet_contusion_Activator::stop(ctkPluginContext *context) { Q_UNUSED(context) }
}
