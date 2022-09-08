 #ifndef QLancetExampleWorkbenchWindowAdvisor_H
 #define QLancetExampleWorkbenchWindowAdvisor_H


 #include <berryWorkbenchAdvisor.h>
 #include <berryWorkbenchWindowAdvisor.h>

namespace berry
{
	struct IActionBarConfigurer;
	//struct IMemento;
	struct IWorkbenchWindowConfigurer;

	class ActionBarAdvisor;
	//class Shell;
}; // !berry

 class QLancetExampleWorkbenchWindowAdvisor : public QObject, public berry::WorkbenchWindowAdvisor
 {
	 Q_OBJECT
 public:
	 QLancetExampleWorkbenchWindowAdvisor(berry::WorkbenchAdvisor* wbAdvisor, 
		 const berry::SmartPointer<berry::IWorkbenchWindowConfigurer>& configurer);
	 virtual ~QLancetExampleWorkbenchWindowAdvisor() override;

	 berry::SmartPointer<berry::ActionBarAdvisor> CreateActionBarAdvisor(
		 berry::SmartPointer<berry::IActionBarConfigurer> configurer) override;

	 virtual QWidget* CreateEmptyWindowContents(QWidget* parent) override;

	 virtual void PostWindowCreate() override;
 private:
	 //struct QLancetExampleWorkbenchWindowAdvisorPrivateImp;
	 //std::shared_ptr<QLancetExampleWorkbenchWindowAdvisorPrivateImp> imp;
 };

 #endif // !QLancetExampleWorkbenchWindowAdvisor