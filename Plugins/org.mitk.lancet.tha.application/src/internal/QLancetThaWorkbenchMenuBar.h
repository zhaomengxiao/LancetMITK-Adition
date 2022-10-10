#ifndef QLancetThaWorkbenchMenuBar_H
#define QLancetThaWorkbenchMenuBar_H

// Qt
#include <QToolBar>
#include <QMenuBar>

// berry
#include <berrySmartPointer.h>

namespace Ui
{
	class QLancetThaWorkbenchMenuBarForm;
}

namespace lancet
{
	class IScxmlStateMachineState;
	class IScxmlStateMachineService;
}

class QResizeEvent;
class QLancetThaWorkbenchMenuBar : public QToolBar
{
	Q_OBJECT
		typedef lancet::IScxmlStateMachineState IScxmlStateMachineState;
public:
	explicit QLancetThaWorkbenchMenuBar(QWidget* = 0);
	virtual ~QLancetThaWorkbenchMenuBar();

public:
	virtual QMenuBar* GetModuleMenuBar() const;
	virtual QToolBar* GetApplicationToolBar() const;

	virtual bool OpenStateWidget(berry::SmartPointer<lancet::IScxmlStateMachineState>);
	virtual bool CloseStateWidget(berry::SmartPointer<lancet::IScxmlStateMachineState>);

	virtual berry::SmartPointer<lancet::IScxmlStateMachineService> 
		GetStateMachineService() const;

	virtual bool InitializeStateMachineForUi();
protected:
	virtual void resizeEvent(QResizeEvent* event) override;

protected slots:
	void onStateMachineModuleEnter(IScxmlStateMachineState*);
	void onStateMachineModuleExit(IScxmlStateMachineState*);

	void onStateMachineElementEnter(IScxmlStateMachineState*);
	void onStateMachineElementExit(IScxmlStateMachineState*);

	void onActionClicked(bool);
	void on_pushbtnApplicationIcon_clicked();
private:
	virtual bool InitializeUiOfStateMachine(QMenu* menu,
		berry::SmartPointer<lancet::IScxmlStateMachineState> state);

	virtual QMenu* InitializeMenu(QMenu* menu,
		berry::SmartPointer<lancet::IScxmlStateMachineState>, bool recursion = true);

	virtual QAction* InitializeAction(QMenu* menu,
		berry::SmartPointer<lancet::IScxmlStateMachineState>);

	virtual bool SwitchToSubState(berry::SmartPointer<lancet::IScxmlStateMachineState>);

	virtual void ConnectToElementStateMachine(
		berry::SmartPointer<lancet::IScxmlStateMachineState>);
private:
	Ui::QLancetThaWorkbenchMenuBarForm* ui;
	struct QLancetThaWorkbenchMenuBarPrivateImp;
	std::shared_ptr<QLancetThaWorkbenchMenuBarPrivateImp> imp;
};

#endif // !QLancetThaWorkbenchMenuBar_H
