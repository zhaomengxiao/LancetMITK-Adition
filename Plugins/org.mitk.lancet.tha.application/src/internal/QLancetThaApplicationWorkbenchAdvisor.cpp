#include "QLancetThaApplicationWorkbenchAdvisor.h"
#include "QLancetThaApplicationWorkbenchWindowAdvisor.h"
#include "org_mitk_lancet_tha_application_Activator.h"

#include <src/lancetIScxmlStateMachineState.h>
#include <src/lancetIScxmlStateMachineService.h>

void QLancetThaApplicationWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
{
	qDebug() << "QLancetThaApplicationWorkbenchAdvisor::Initialize";
	berry::QtWorkbenchAdvisor::Initialize(configurer);

	configurer->SetSaveAndRestore(false);

	auto context = org_mitk_lancet_tha_application_Activator::GetPluginContext();
	auto stateMachineServiceRef = context->getServiceReference<lancet::IScxmlStateMachineService>();
	auto stateMachineService = context->getService<lancet::IScxmlStateMachineService>(stateMachineServiceRef);
	// F:/Examples/ExampleQtScxmlStateMachine/LancetThaScxmlStateMachine_debug.scxml
	if (nullptr != stateMachineService)
	{
		stateMachineService->Initialize();
		stateMachineService->CreateScxmlStateMachine(
			"Y:/LancetMitk_SBD/MITK-build/bin/resources_temp/stateMachine/tha/org_mitk_lancet_tha_application.scxml",
			"Y:/LancetMitk_SBD/MITK-build/bin/resources_temp/stateMachine/tha/application_subsidiary.json",
			lancet::IScxmlStateMachineState::StateType::Admin,
			lancet::IScxmlStateMachineState::Pointer(nullptr), "org_mitk_lancet_tha_application");

		//std::cout << stateMachineService->GetAdminState()->ToString().toStdString();
	}
}

berry::WorkbenchWindowAdvisor* QLancetThaApplicationWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(
	berry::IWorkbenchWindowConfigurer::Pointer configurer)
{
	// Set an individual initial size
	configurer->SetInitialSize(QPoint(600, 400));
	// Set an individual title
	configurer->SetTitle("Lancet Hip Application");
	// Enable or disable the perspective bar
	configurer->SetShowMenuBar(true);
	configurer->SetShowPerspectiveBar(false);

	// MainWindow style
	configurer->SetWindowFlags(configurer->GetWindowFlags() | Qt::FramelessWindowHint);


	return new QLancetThaApplicationWorkbenchWindowAdvisor(this, configurer);
}

QString QLancetThaApplicationWorkbenchAdvisor::GetInitialWindowPerspectiveId()
{
	return QLancetThaApplicationWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID;
}

const QString QLancetThaApplicationWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID = "org.mitk.qlancetthaapplicationperspective";