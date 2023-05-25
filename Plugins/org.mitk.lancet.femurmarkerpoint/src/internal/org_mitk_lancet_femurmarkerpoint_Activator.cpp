/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#include "org_mitk_lancet_femurmarkerpoint_Activator.h"
#include "QFemurMarkerPoint.h"

namespace mitk
{
  static ctkPluginContext* staticCtkPluginContext = nullptr;
  void org_mitk_lancet_femurmarkerpoint_Activator::start(ctkPluginContext *context)
  {
    staticCtkPluginContext = context;
    BERRY_REGISTER_EXTENSION_CLASS(QFemurMarkerPoint, context)
  }

  void org_mitk_lancet_femurmarkerpoint_Activator::stop(ctkPluginContext *context) 
  {
	Q_UNUSED(context);
	staticCtkPluginContext = nullptr;
  }
  ctkPluginContext* org_mitk_lancet_femurmarkerpoint_Activator::GetPluginContext()
  {
	  return staticCtkPluginContext;
  }
}
