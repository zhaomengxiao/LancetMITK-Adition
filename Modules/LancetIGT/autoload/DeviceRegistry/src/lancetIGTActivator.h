/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef lancetIGTActivator_H
#define lancetIGTActivator_H

#include <usModuleActivator.h>


namespace lancet
{

  /** Documentation:
  *   \brief  The load function of this class is executed every time, the module is loaded.
  *           Attention: no static variables of any class in IGT Module are initialized at this moment!
  *   \ingroup IGT
  */
  class LancetIGTActivator : public us::ModuleActivator
  {
    public:
      LancetIGTActivator();
      ~LancetIGTActivator() override;
      void Load(us::ModuleContext*) override;
      void Unload(us::ModuleContext*) override;

    private:
      //mitk::TrackingDeviceTypeCollection m_DeviceTypeCollection;

  };
}

#endif // lancetIGTActivator_H
