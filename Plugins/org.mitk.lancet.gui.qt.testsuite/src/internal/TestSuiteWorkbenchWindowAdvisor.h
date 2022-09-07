/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QMITKEXTWORKBENCHWINDOWADVISOR_H_
#define QMITKEXTWORKBENCHWINDOWADVISOR_H_

#include <berryWorkbenchWindowAdvisor.h>

#include <berryIPartListener.h>
#include <berryIEditorPart.h>
#include <berryIWorkbenchPage.h>
#include <berryWorkbenchAdvisor.h>
#include <berryWorkbenchWindowAdvisor.h>

#include <org_mitk_lancet_gui_qt_testsuite_EXPORT.h>

#include <QList>

class QAction;
class QMenu;

class org_mitk_lancet_gui_qt_testsuite_EXPORT TestSuiteWorkbenchWindowAdvisor : public QObject, public berry::WorkbenchWindowAdvisor
{
  Q_OBJECT

public:

    TestSuiteWorkbenchWindowAdvisor(berry::WorkbenchAdvisor* wbAdvisor,
        berry::IWorkbenchWindowConfigurer::Pointer configurer);

    ~TestSuiteWorkbenchWindowAdvisor() override;

   

    void PostWindowOpen() override;

};

#endif /*QMITKEXTWORKBENCHWINDOWADVISOR_H_*/
