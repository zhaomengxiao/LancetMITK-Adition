#include "QLancetThaApplication.h"

// Berry
#include <berryPlatformUI.h>

// internal
#include "QLancetThaApplicationWorkbenchAdvisor.h"

#include <mitkLog.h>
#include <lancetLogginBackend.h>

QLancetThaApplication::QLancetThaApplication(QObject* parent)
	: QObject(parent)
{
}

QLancetThaApplication::~QLancetThaApplication()
{
}

QVariant QLancetThaApplication::Start(berry::IApplicationContext* context)
{
	lancet::LogginBackend::Register();
	mitk::LoggingBackend::Unregister();

	QScopedPointer<berry::Display> display(berry::PlatformUI::CreateDisplay());

	QScopedPointer<QLancetThaApplicationWorkbenchAdvisor> wbAdvisor(new QLancetThaApplicationWorkbenchAdvisor());
	int code = berry::PlatformUI::CreateAndRunWorkbench(display.data(), wbAdvisor.data());

	// exit the application with an appropriate return code
	return code == berry::PlatformUI::RETURN_RESTART ? EXIT_RESTART : EXIT_OK;
}

void QLancetThaApplication::Stop()
{
}
