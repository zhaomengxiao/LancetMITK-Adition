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
#include <mitkTrackingDeviceWidgetCollection.h>
//All Tracking devices, which should be available by default
#include "lancetKukaTrackingDeviceTypeInformation.h"
#include "UI/QmitkLancetKukaWidget.h"
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usServiceProperties.h>
#include <usModuleContext.h>

namespace mitk
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

    //widget
    std::vector<us::ServiceReference<mitk::TrackingDeviceWidgetCollection> > refs_widget = context->GetServiceReferences<mitk::TrackingDeviceWidgetCollection>();
    if (refs_widget.empty())
    {
      MITK_ERROR << "No tracking device widget service found!";
    }
    mitk::TrackingDeviceWidgetCollection* m_DeviceWidgetCollection = context->GetService<
      mitk::TrackingDeviceWidgetCollection>(refs_widget.front());
    m_DeviceWidgetCollection->RegisterTrackingDeviceWidget(lancet::KukaRobotTypeInformation::GetTrackingDeviceName(),new QmitkLancetKukaWidget);
  }

  void LancetIGTActivator::Unload(us::ModuleContext*)
  {
    
  }

}

US_EXPORT_MODULE_ACTIVATOR(mitk::LancetIGTActivator)
