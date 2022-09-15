/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// berry includes
#ifndef TESTSUITEAPPLICATION_H
#define TESTSUITEAPPLICATION_H

#include <berryIApplication.h>

// Qt includes
#include <QObject>
#include <QScopedPointer>

class TestSuiteWorkbenchAdvisor;

class TestSuiteApplication : public QObject, public berry::IApplication
{
  Q_OBJECT
  Q_INTERFACES(berry::IApplication)

public:
  TestSuiteApplication();
  ~TestSuiteApplication() override;

  QVariant Start(berry::IApplicationContext *context) override;
  void Stop() override;

private:
  QScopedPointer<TestSuiteWorkbenchAdvisor> wbAdvisor;
};

#endif // TESTSUITEAPPLICATION_H
