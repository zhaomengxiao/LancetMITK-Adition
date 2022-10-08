/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "lancetIGTUIActivator.h"

//All Tracking devices, which should be available by default
#include "lancetKukaTrackingDeviceTypeInformation.h"
#include "QmitkLancetKukaWidget.h"
//micro service
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usModuleContext.h>
#include <usServiceProperties.h>

#include "mitkTrackingDeviceWidgetCollection.h"

namespace lancet
{
  LancetIGTUIActivator::LancetIGTUIActivator()
  {
  }

  LancetIGTUIActivator::~LancetIGTUIActivator()
  {
  }

  void LancetIGTUIActivator::Load(us::ModuleContext*)
  {
    us::ModuleContext* context = us::GetModuleContext();
    //widget
    std::vector<us::ServiceReference<mitk::TrackingDeviceWidgetCollection> > refs_widget = context->GetServiceReferences<mitk::TrackingDeviceWidgetCollection>();
    if (refs_widget.empty())
    {
      MITK_ERROR << "No tracking device widget service found!";
    }
    mitk::TrackingDeviceWidgetCollection* m_DeviceWidgetCollection = context->GetService<
      mitk::TrackingDeviceWidgetCollection>(refs_widget.front());
    m_DeviceWidgetCollection->RegisterTrackingDeviceWidget(lancet::KukaRobotTypeInformation::GetTrackingDeviceName(), new QmitkLancetKukaWidget);
  }

  void LancetIGTUIActivator::Unload(us::ModuleContext*)
  {
    
  }
}

US_EXPORT_MODULE_ACTIVATOR(lancet::LancetIGTUIActivator)
