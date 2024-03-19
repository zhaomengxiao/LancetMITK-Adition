/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#include "org_mitk_lancet_lhytest_Activator.h"
#include "LhyTest.h"

namespace mitk
{
  void org_mitk_lancet_lhytest_Activator::start(ctkPluginContext *context)
  {
	qRegisterMetaType<LToolAttitudeMessage>("LToolAttitudeMessage");
    BERRY_REGISTER_EXTENSION_CLASS(LhyTest, context)
  }

  void org_mitk_lancet_lhytest_Activator::stop(ctkPluginContext *context) { Q_UNUSED(context) }
}
