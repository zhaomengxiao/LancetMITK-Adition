#ifndef QLancetThaApplicationWorkbenchAdvisor_H
#define QLancetThaApplicationWorkbenchAdvisor_H

// Berry
#include <berryQtWorkbenchAdvisor.h>
#include "org_mitk_lancet_tha_application_Export.h"

class org_mitk_lancet_tha_application_EXPORT QLancetThaApplicationWorkbenchAdvisor 
	: public berry::QtWorkbenchAdvisor
{
public:
	static const QString DEFAULT_PERSPECTIVE_ID;

	void Initialize(berry::IWorkbenchConfigurer::Pointer configurer) override;

	berry::WorkbenchWindowAdvisor* CreateWorkbenchWindowAdvisor(
		berry::IWorkbenchWindowConfigurer::Pointer configurer) override;

	virtual QString GetInitialWindowPerspectiveId() override;
};
#endif // !QLancetThaApplicationWorkbenchAdvisor_H
