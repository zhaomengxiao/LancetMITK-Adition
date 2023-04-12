/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare the state machine service state event processing interface class
 * \ingroup org_mitk_lancet_statemachine_services
 * \version V1.0.0
 * \data 2022-09-26 16:27:04
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-09-26 16:27:04
 * \remark Nothing.
 */
#ifndef LancetIScxmStateMachineHandler_H
#define LancetIScxmStateMachineHandler_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

// Qt
#include <QObject>
#include <QVariantMap>

#include <org_mitk_lancet_statemachine_services_Export.h>

class QScxmlDataModel;
class QScxmlTableData;
class QScxmlInvokableService;

namespace lancet
{
	class IScxmlStateMachineState;
	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 * \brief This is the event processing abstract class of the state machine unit.
	 *
	 * Monitors various states and signal events of the state machine. This class
	 * exists to simplify the methods of external users, and is also one of the
	 * extension interfaces reserved for external users.
	 *
	 * When you try to extend this part, you should inherit this class and override
	 * its Init(), StartStateMachine(), StopStateMachine() methods. You can do whatever 
	 * you want with the rest of the extension parts.
	 *
	 * \pre
	 * The external state must be created or imported, otherwise the function is
	 * invalid.
	 *
	 * \author Sun
	 * \version V1.0.0
	 * \date 2022-09-26 16:28:30
	 * \remark Non
	 *
	 * Contact: sh4a01@163.com
	 */
	class LANCET_STATEMACHINE_SERVICES_PLUGIN 
		IScxmStateMachineHandler : public QObject, public berry::Object
	{
		Q_OBJECT
	public:
		berryObjectMacro(lancet::IScxmStateMachineHandler);
		IScxmStateMachineHandler();
		IScxmStateMachineHandler(berry::SmartPointer<IScxmlStateMachineState>);
		virtual ~IScxmStateMachineHandler();

	public:
		/**
		 * \brief Initialize the event processing class.
		 *
		 * This interface is a pure virtual property, and the inheritor needs to override 
		 * this method to achieve initialization.
		 *
		 * \warning 
		 * If the parameter input is empty, the event processing function is invalid.
		 *
		 * \param state
		 *              Monitoring state machine state objects.
		 * 
		 * \return Returns true if successful, otherwise false.
		 */
		virtual bool Init(berry::SmartPointer<IScxmlStateMachineState> state) = 0;

		/**
		 * \brief Initialize the event of Qt style state machine state object.
		 *
		 * This method is not automatic. The inheritor should call this method in the 
		 * init() method to initialize QT style state machine events.
		 * 
		 * This interface is not recommended to be rewritten. The attributes of this 
		 * class are for internal event business processing.
		 *
		 * \param state
		 *              Monitoring state machine state objects.
		 *
		 * \return Returns true if successful, otherwise false.
		 * 
		 * \see Init()
		 */
		virtual bool InitQtEventForStateMachine(berry::SmartPointer<IScxmlStateMachineState>);

		/**
		 * \brief Start or stop the state machine.
		 *
		 * Starting the work of Qt state machine will not affect or operate the embedded sub 
		 * state machine of Qt state machine.
		 * 
		 * Stopping the Qt state machine will stop all embedded sub Qt state machines.
		 *
		 * \pre 
		 * This class must be initialized before executing this operation.
		 *
		 * \return Returns true if successful, otherwise false.
		 * 
		 * \see Init(), InitQtEventForStateMachine()
		 */
		virtual bool StartStateMachine() = 0;
		virtual bool StopStateMachine() = 0;
		/**
		 * \brief Returns true if a message to target can be dispatched by this state machine.
		 *
		 * \deprecated 
		 * Qt does not provide an interface for this method, so this interface may be deleted 
		 * in future versions.
		 * 
		 * \param sessionid
		 *          is the session ID of the current state machine.
		 * \param state
		 *          is target state object.
		 *
		 * \see lancet::IScxmlStateMachineState, berry::SmartPointer
		 * 
		 * \link QScxmlStateMachine#isDispatchableTarget(const QString &target) const \endlink
		 */
		virtual bool IsDispatchableTarget(const QString& sessionid);
		virtual bool IsDispatchableTarget(berry::SmartPointer<IScxmlStateMachineState> state);

		virtual berry::SmartPointer<IScxmlStateMachineState> GetAssignor() const;
		virtual void SetAssignor(berry::SmartPointer<IScxmlStateMachineState>);

		virtual berry::SmartPointer<IScxmlStateMachineState> GetActiveStateMachine() const;
		virtual void SetActiveStateMachine(berry::SmartPointer<IScxmlStateMachineState>);

		virtual berry::SmartPointer<IScxmlStateMachineState> GetLastActiveStateMachine() const;
		virtual void SetLastActiveStateMachine(berry::SmartPointer<IScxmlStateMachineState>);

		/**
		 * \brief Jump to the state of the state machine.
		 * 
		 * \pre 
		 * The state machine is in working state.
		 * 
		 * \warning 
		 * This behavior may fail due to many factors, such as state machine logic control, 
		 * state input error, etc. When using this interface, be sure to monitor its return 
		 * value.
		 * 
		 * \param id
		 *              ID code of the status, corresponding to the.
		 * 
		 * \return Returns true if successful, otherwise false.
		 * 
		 * \see StartStateMachine()
		 */
		
		virtual bool ToTargetState(const QString& id);
		virtual bool ToTargetState(const berry::SmartPointer<IScxmlStateMachineState>&);
	Q_SIGNALS:
		/**
		 * \brief Event triggered when the sub state of the state machine enters.
		 *
		 * This event will only be triggered when the sub state enters for the first 
		 * time, and the state exit signal will be triggered when the state machine 
		 * switches the sub state.
		 *
		 * \pre
		 * The class is initialized and the state machine is working.
		 *
		 * \param state
		 *              Sub state object of trigger signal.
		 * 
		 * \see lancet::IScxmlStateMachineState, Init(), StartStateMachine()
		 */
		void StateEnter(IScxmlStateMachineState*);

		/**
		 * \brief Event triggered when the sub state of the state machine exits.
		 *
		 * This event will only be triggered when the sub state exits for the first
		 * time, and the state enters signal will be triggered when the state machine
		 * switches the sub state.
		 *
		 * \pre
		 * The class is initialized and the state machine is working.
		 *
		 * \param state
		 *              Sub state object of trigger signal.
		 *
		 * \see lancet::IScxmlStateMachineState, Init(), StartStateMachine()
		 */
		void StateExit(IScxmlStateMachineState*);
	private Q_SLOTS:
		virtual void onScxmlStateMachine_stateChange(bool);
		virtual void onScxmlStateMachine_finished();
		virtual void onScxmlStateMachine_dataModelChanged(QScxmlDataModel* model);
		virtual void onScxmlStateMachine_initialValuesChanged(const QVariantMap& initialValues);
		virtual void onScxmlStateMachine_initializedChanged(bool initialized);
		virtual void onScxmlStateMachine_invokedServicesChanged(const QVector<QScxmlInvokableService*>& invokedServices);
		//virtual void onScxmlStateMachine_loaderChanged(QScxmlCompiler::Loader* loader);
		virtual void onScxmlStateMachine_log(const QString& label, const QString& msg);
		virtual void onScxmlStateMachine_reachedStableState();
		virtual void onScxmlStateMachine_runningChanged(bool running);
		virtual void onScxmlStateMachine_tableDataChanged(QScxmlTableData* tableData);
	private:
		struct IScxmStateMachineHandlerPrivateImp;
		std::shared_ptr<IScxmStateMachineHandlerPrivateImp> imp;
	};
}
#endif // !LancetIScxmStateMachineHandler_H
