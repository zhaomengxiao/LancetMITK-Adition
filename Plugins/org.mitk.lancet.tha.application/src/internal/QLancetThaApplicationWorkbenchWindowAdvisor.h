#ifndef QLancetThaApplicationWorkbenchWindowAdvisor_H
#define QLancetThaApplicationWorkbenchWindowAdvisor_H

//berry
#include <berryWorkbenchAdvisor.h>
#include <berryWorkbenchWindowAdvisor.h>

namespace lancet
{
	class IScxmlStateMachineState;
}

class QLancetThaApplicationWorkbenchWindowAdvisor 
	: public QObject, public berry::WorkbenchWindowAdvisor
{
	Q_OBJECT
	using IScxmlStateMachineState = lancet::IScxmlStateMachineState;
public:
	QLancetThaApplicationWorkbenchWindowAdvisor(berry::WorkbenchAdvisor* wbAdvisor,
		const berry::SmartPointer<berry::IWorkbenchWindowConfigurer>& configurer);
	virtual ~QLancetThaApplicationWorkbenchWindowAdvisor() override;

	virtual berry::SmartPointer<berry::ActionBarAdvisor> CreateActionBarAdvisor(
		berry::SmartPointer<berry::IActionBarConfigurer> configurer) override;
public:
	virtual void PostWindowCreate() override;

	virtual void PostWindowOpen() override;
};
#endif // !QLancetThaApplicationWorkbenchWindowAdvisor_H
