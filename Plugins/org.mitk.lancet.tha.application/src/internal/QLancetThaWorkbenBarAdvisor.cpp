#include "QLancetThaWorkbenBarAdvisor.h"

#include <berryPlatformUI.h>
#include <berryIWorkbenchWindow.h>
#include <berryIWorkbenchWindowConfigurer.h>
#include <berryIWorkbenchCommandConstants.h>

#include <QMainWindow>
#include <QmitkFileOpenAction.h>

// tha
#include "QLancetThaWorkbenchMenuBar.h"

struct QLancetThaWorkbenBarAdvisor::QLancetThaWorkbenBarAdvisorPrivateImp
{
	berry::SmartPointer<berry::IWorkbenchWindow> window;
};

QLancetThaWorkbenBarAdvisor::QLancetThaWorkbenBarAdvisor(
	berry::SmartPointer<berry::IActionBarConfigurer> configurer)
	: berry::ActionBarAdvisor(configurer)
	, imp(std::make_shared<QLancetThaWorkbenBarAdvisorPrivateImp>())
{
	this->imp->window = 
		berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow();
}

void QLancetThaWorkbenBarAdvisor::MakeActions(berry::IWorkbenchWindow* window)
{

}

void QLancetThaWorkbenBarAdvisor::FillMenuBar(berry::IMenuManager* menuBar)
{

}

void QLancetThaWorkbenBarAdvisor::FillToolBar(berry::IToolBarManager* toolBar)
{

}

berry::SmartPointer<berry::IWorkbenchWindow> QLancetThaWorkbenBarAdvisor::GetWindow() const
{
	return this->imp->window;
}
