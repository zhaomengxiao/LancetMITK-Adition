/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "TestSuiteWorkbenchWindowAdvisor.h"

#include <QString>

#include <service/event/ctkEventAdmin.h>

#include <berryIPerspectiveDescriptor.h>
#include <berryIWorkbenchPage.h>
#include <berryIWorkbenchWindow.h>
#include <berryPlatform.h>
#include <berryPlatformUI.h>
#include <berryWorkbenchPlugin.h>

#include <internal/berryQtShowViewAction.h>


#include <QMouseEvent>

#include "mitkDataStorageEditorInput.h"
#include "mitkIDataStorageService.h"
#include "mitkWorkbenchUtil.h"
#include "internal/QmitkCommonExtPlugin.h"


TestSuiteWorkbenchWindowAdvisor::TestSuiteWorkbenchWindowAdvisor(berry::WorkbenchAdvisor* wbAdvisor,
                                                               berry::IWorkbenchWindowConfigurer::Pointer configurer)
  : berry::WorkbenchWindowAdvisor(configurer)
  
{

}

TestSuiteWorkbenchWindowAdvisor::~TestSuiteWorkbenchWindowAdvisor()
{
}


void TestSuiteWorkbenchWindowAdvisor::PostWindowOpen()
{
  // berry::WorkbenchWindowAdvisor::PostWindowOpen();
  // // Force Rendering Window Creation on startup.
  // berry::IWorkbenchWindowConfigurer::Pointer configurer = GetWindowConfigurer();
  //
  // ctkPluginContext* context = QmitkCommonExtPlugin::getContext();
  // ctkServiceReference serviceRef = context->getServiceReference<mitk::IDataStorageService>();
  // if (serviceRef)
  // {
  //   mitk::IDataStorageService *dsService = context->getService<mitk::IDataStorageService>(serviceRef);
  //   if (dsService)
  //   {
  //     mitk::IDataStorageReference::Pointer dsRef = dsService->GetDataStorage();
  //     mitk::DataStorageEditorInput::Pointer dsInput(new mitk::DataStorageEditorInput(dsRef));
  //     mitk::WorkbenchUtil::OpenEditor(configurer->GetWindow()->GetActivePage(),dsInput);
  //   }
  // }
  //
  // auto introPart = configurer->GetWindow()->GetWorkbench()->GetIntroManager()->GetIntro();
  // if (introPart.IsNotNull())
  // {
  //   configurer->GetWindow()->GetWorkbench()->GetIntroManager()->ShowIntro(GetWindowConfigurer()->GetWindow(), false);
  // }
}

