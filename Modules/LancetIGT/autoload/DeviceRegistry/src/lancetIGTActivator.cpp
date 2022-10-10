/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkIGTConfig.h"
#include "lancetIGTActivator.h"
#include <mitkTrackingDeviceTypeCollection.h>

//All Tracking devices, which should be available by default
#include "lancetKukaTrackingDeviceTypeInformation.h"


//micro service
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usServiceProperties.h>
#include <usModuleContext.h>

namespace lancet
{

  LancetIGTActivator::LancetIGTActivator()
  {
  }

  LancetIGTActivator::~LancetIGTActivator()
  {
  }

  void LancetIGTActivator::Load(us::ModuleContext*)
  {
    //type
    us::ModuleContext* context = us::GetModuleContext();
    std::vector<us::ServiceReference<mitk::TrackingDeviceTypeCollection> > refs = context->GetServiceReferences<mitk::TrackingDeviceTypeCollection>();
    if (refs.empty())
    {
      MITK_ERROR << "No tracking device service found!";
    }
    mitk::TrackingDeviceTypeCollection* m_DeviceTypeCollection = context->GetService<
      mitk::TrackingDeviceTypeCollection>(refs.front());
    m_DeviceTypeCollection->RegisterTrackingDeviceType(new lancet::KukaRobotTypeInformation());

    
  }

  void LancetIGTActivator::Unload(us::ModuleContext*)
  {
    
  }

}

US_EXPORT_MODULE_ACTIVATOR(lancet::LancetIGTActivator)
