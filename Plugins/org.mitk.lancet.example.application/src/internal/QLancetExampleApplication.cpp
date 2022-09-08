/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QLancetExampleApplication.h"
#include "QLancetExampleWorkbenchWindowAdvisor.h"

// Berry
#include <berryPlatformUI.h>
#include <berryQtWorkbenchAdvisor.h>

class QLancetExampleWorkbenchAdvisor
	: public berry::QtWorkbenchAdvisor
{
public:
	static const QString DEFAULT_PERSPECTIVE_ID;

	void Initialize(berry::IWorkbenchConfigurer::Pointer configurer) override;

	berry::WorkbenchWindowAdvisor* CreateWorkbenchWindowAdvisor(
		berry::IWorkbenchWindowConfigurer::Pointer configurer) override;

	virtual QString GetInitialWindowPerspectiveId() override;
};

void QLancetExampleWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
{
	berry::QtWorkbenchAdvisor::Initialize(configurer);

	configurer->SetSaveAndRestore(true);
}

berry::WorkbenchWindowAdvisor* QLancetExampleWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(
	berry::IWorkbenchWindowConfigurer::Pointer configurer)
{
	// Set an individual initial size
	configurer->SetInitialSize(QPoint(600, 400));
	// Set an individual title
	configurer->SetTitle("Lancet Example Application");
	// Enable or disable the perspective bar
	configurer->SetShowMenuBar(true);
	configurer->SetShowPerspectiveBar(false);

	// MainWindow style
	qDebug() << "configurer.GetWindowFlags " << configurer->GetWindowFlags();
	configurer->SetWindowFlags(configurer->GetWindowFlags() | Qt::FramelessWindowHint);


	return new QLancetExampleWorkbenchWindowAdvisor(this, configurer);
	//return new berry::WorkbenchWindowAdvisor(configurer);
}

QString QLancetExampleWorkbenchAdvisor::GetInitialWindowPerspectiveId()
{
	return DEFAULT_PERSPECTIVE_ID;
}

const QString QLancetExampleWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID = "org.mitk.qlancetexampleperspective";

QLancetExampleApplication::QLancetExampleApplication()
{
}

QLancetExampleApplication::~QLancetExampleApplication()
{
}

QVariant QLancetExampleApplication::Start(berry::IApplicationContext * /*context*/)
{
  QScopedPointer<berry::Display> display(berry::PlatformUI::CreateDisplay());

  QScopedPointer<QLancetExampleWorkbenchAdvisor> wbAdvisor(new QLancetExampleWorkbenchAdvisor());
  int code = berry::PlatformUI::CreateAndRunWorkbench(display.data(), wbAdvisor.data());

  // exit the application with an appropriate return code
  return code == berry::PlatformUI::RETURN_RESTART ? EXIT_RESTART : EXIT_OK;
}

void QLancetExampleApplication::Stop()
{
}
