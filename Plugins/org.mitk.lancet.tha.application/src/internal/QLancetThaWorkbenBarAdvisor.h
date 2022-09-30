#ifndef QLancetThaWorkbenBarAdvisor_H
#define QLancetThaWorkbenBarAdvisor_H

// berry
#include <berryActionBarAdvisor.h>
#include <berryIActionBarConfigurer.h>

class QLancetThaWorkbenBarAdvisor : public berry::ActionBarAdvisor 
{
public:
	QLancetThaWorkbenBarAdvisor(berry::SmartPointer<berry::IActionBarConfigurer> configurer);

protected:

	void MakeActions(berry::IWorkbenchWindow* window) override;

	void FillMenuBar(berry::IMenuManager* menuBar) override;

	void FillToolBar(berry::IToolBarManager* toolBar) override;

	berry::SmartPointer<berry::IWorkbenchWindow> GetWindow() const;
private:
	struct QLancetThaWorkbenBarAdvisorPrivateImp;
	std::shared_ptr<QLancetThaWorkbenBarAdvisorPrivateImp> imp;
};

#endif // !QLancetThaWorkbenBarAdvisor_H
