//#include "QLancetExampleWorkbenchAdvisor.h"
//#include "QLancetExampleWorkbenchWindowAdvisor.h"
//
//void QLancetExampleWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
//{
//	berry::QtWorkbenchAdvisor::Initialize(configurer);
//
//	configurer->SetSaveAndRestore(true);
//}
//
//berry::WorkbenchWindowAdvisor* QLancetExampleWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(
//	berry::IWorkbenchWindowConfigurer::Pointer configurer)
//{
//	// Set an individual initial size
//	configurer->SetInitialSize(QPoint(600, 400));
//	// Set an individual title
//	configurer->SetTitle("Lancet Example Application");
//	// Enable or disable the perspective bar
//	configurer->SetShowMenuBar(true);
//	configurer->SetShowPerspectiveBar(false);
//
//	// MainWindow style
//	qDebug() << "configurer.GetWindowFlags " << configurer->GetWindowFlags();
//	configurer->SetWindowFlags(configurer->GetWindowFlags() | Qt::FramelessWindowHint);
//
//
//	return new QLancetExampleWorkbenchWindowAdvisor(this, configurer); 
//	//return new berry::WorkbenchWindowAdvisor(configurer);
//}
//
//QString QLancetExampleWorkbenchAdvisor::GetInitialWindowPerspectiveId()
//{
//	return DEFAULT_PERSPECTIVE_ID;
//}
//
//const QString QLancetExampleWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID = "org.lancet.example.applicationperspective";