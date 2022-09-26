#include "LancetIScxmStateMachineHandler.h"
#include "LancetIScxmlStateMachineState.h"

#include <QStringList>
#include <QScxmlStateMachine>
#include <src/lancetIScxmlStateMachineState.h>
#include <src/lancetIScxmlStateMachineService.h>
#include <QDebug>

namespace lancet
{
	struct IScxmStateMachineHandler::IScxmStateMachineHandlerPrivateImp
	{
		IScxmlStateMachineState::Pointer stateAssignor;
		berry::SmartPointer<IScxmlStateMachineState> stateActive;
		berry::SmartPointer<IScxmlStateMachineState> lastStateActive;
	};

	IScxmStateMachineHandler::IScxmStateMachineHandler()
		: IScxmStateMachineHandler(berry::SmartPointer<IScxmlStateMachineState>(nullptr))
	{
	}

	IScxmStateMachineHandler::IScxmStateMachineHandler(berry::SmartPointer<IScxmlStateMachineState> o)
		: imp(std::make_shared<IScxmStateMachineHandlerPrivateImp>())
	{
		this->SetAssignor(o);
	}

	IScxmStateMachineHandler::~IScxmStateMachineHandler()
	{
	}

	bool IScxmStateMachineHandler::InitQtEventForStateMachine(berry::SmartPointer<IScxmlStateMachineState> state)
	{
		if (state.IsNotNull() && false == state->GetQtStateMachine().isNull())
		{
			// dataModelChanged
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(dataModelChanged(QScxmlDataModel*)), 
				this, SLOT(onScxmlStateMachine_dataModelChanged(QScxmlDataModel*)));
			// initialValuesChanged
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(initialValuesChanged(const QVariantMap&)),
				this, SLOT(onScxmlStateMachine_initialValuesChanged(const QVariantMap&)));
			// initializedChanged
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(initializedChanged(bool)),
				this, SLOT(onScxmlStateMachine_initializedChanged(bool)));
			// invokedServicesChanged
			QObject::connect(state->GetQtStateMachine().data(), 
				SIGNAL(invokedServicesChanged(const QVector<QScxmlInvokableService*>&)),
				this, SLOT(onScxmlStateMachine_invokedServicesChanged(const QVector<QScxmlInvokableService*>&)));
			// loaderChanged
			//QObject::connect(state->GetQtStateMachine().data(), SIGNAL(loaderChanged(QScxmlCompiler::Loader*)),
			//	this, SLOT(onScxmlStateMachine_loaderChanged(QScxmlCompiler::Loader*)));
			// log
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(log(const QString&, const QString&)),
				this, SLOT(onScxmlStateMachine_log(const QString&, const QString&)));
			// reachedStableState
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(reachedStableState()),
				this, SLOT(onScxmlStateMachine_reachedStableState()));
			// runningChanged
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(runningChanged(bool)),
				this, SLOT(onScxmlStateMachine_runningChanged(bool)));
			// tableDataChanged
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(tableDataChanged(QScxmlTableData*)),
				this, SLOT(onScxmlStateMachine_tableDataChanged(QScxmlTableData*)));
			// finished
			QObject::connect(state->GetQtStateMachine().data(), SIGNAL(finished()),
				this, SLOT(onScxmlStateMachine_finished()));

			for (IScxmlStateMachineState::Pointer item_state : state->GetSubStateMachines())
			{
				if (item_state.IsNotNull())
				{
					auto retval =
					state->GetQtStateMachine()->connectToState(item_state->GetStateId(),
						this, SLOT(onScxmlStateMachine_stateChange(bool)));
					if (retval == false) 
					{
						// Can't connect to state
						qDebug() << "IScxmStateMachineHandler::InitQtEventForStateMachine Not " << item_state->GetStateId();
					}
				}
			}

			return true;
		}
		return false;
	}

	bool IScxmStateMachineHandler::IsDispatchableTarget(const QString& sessionid)
	{
		if (this->GetAssignor().IsNotNull()
			&& false == this->GetAssignor()->GetQtStateMachine().isNull())
		{
			return this->GetAssignor()->GetQtStateMachine()->isDispatchableTarget(sessionid);
		}
		return false;
	}

	bool IScxmStateMachineHandler::IsDispatchableTarget(berry::SmartPointer<IScxmlStateMachineState> state)
	{
		if (state.IsNotNull())
		{
			return this->IsDispatchableTarget(state->GetStateId());
		}
		return false;
	}

	berry::SmartPointer<IScxmlStateMachineState> IScxmStateMachineHandler::GetAssignor() const
	{
		return this->imp->stateAssignor;
	}

	void IScxmStateMachineHandler::SetAssignor(berry::SmartPointer<IScxmlStateMachineState> as)
	{
		this->imp->stateAssignor = as;
	}

	berry::SmartPointer<IScxmlStateMachineState> 
		IScxmStateMachineHandler::GetActiveStateMachine() const
	{
		return this->imp->stateActive;
	}

	void IScxmStateMachineHandler::SetActiveStateMachine(berry::SmartPointer<IScxmlStateMachineState> state)
	{
		this->imp->stateActive = state;
	}

	berry::SmartPointer<IScxmlStateMachineState> IScxmStateMachineHandler::GetLastActiveStateMachine() const
	{
		return this->imp->lastStateActive;
	}

	void IScxmStateMachineHandler::SetLastActiveStateMachine(berry::SmartPointer<IScxmlStateMachineState> state)
	{
		this->imp->lastStateActive = state;
	}

	bool IScxmStateMachineHandler::ToTargetState(const QString& id)
	{
		berry::SmartPointer<IScxmlStateMachineState> targetState;
		auto stateMachineService = IScxmlStateMachineService::GetScxmlStateMachineService();
		if (stateMachineService.IsNotNull())
		{
			targetState = stateMachineService->GetScxmlStateMachine(this->GetAssignor(), id);
		}
		return this->ToTargetState(targetState);
	}

	bool IScxmStateMachineHandler::ToTargetState(const berry::SmartPointer<IScxmlStateMachineState>& state)
	{
		if (/*this->IsDispatchableTarget(state) && */this->GetAssignor().IsNotNull()
			&& false == this->GetAssignor()->GetQtStateMachine().isNull())
		{
			QString submit = QString("to_%1").arg(state->GetStateId());
			qDebug() << submit;
			qDebug() << this->GetAssignor()->GetQtStateMachine()->activeStateNames();
			this->GetAssignor()->GetQtStateMachine()->submitEvent(submit);
			return true;
		}
		return false;
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_stateChange(bool active)
	{
		if (false == active)
		{
			this->SetLastActiveStateMachine(this->GetActiveStateMachine());
			emit this->StateExit(this->GetLastActiveStateMachine().GetPointer());
			return;
		}

		if (this->GetAssignor().IsNotNull() && !this->GetAssignor()->GetQtStateMachine().isNull())
		{
			auto stateMachineService = IScxmlStateMachineService::GetScxmlStateMachineService();
			if (stateMachineService.IsNull())
			{
				// Do nothing
				return;
			}

			// find active state and update it.
			for (auto substate : this->GetAssignor()->GetSubStateMachines())
			{
				bool is_active = this->GetAssignor()->GetQtStateMachine()->activeStateNames()
					.contains(substate->GetStateId().toLower());
				if (active && substate.IsNotNull()	&& is_active)
				{
					this->SetActiveStateMachine(substate);
					emit this->StateEnter(this->GetActiveStateMachine().GetPointer());
					break;
				}
			}
		}
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_finished()
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_dataModelChanged(QScxmlDataModel* model)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_initialValuesChanged(const QVariantMap& initialValues)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_initializedChanged(bool initialized)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_invokedServicesChanged(const QVector<QScxmlInvokableService*>& invokedServices)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	//void IScxmStateMachineHandler::onScxmlStateMachine_loaderChanged(QScxmlCompiler::Loader* loader)
	//{
	//	qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	//}

	void IScxmStateMachineHandler::onScxmlStateMachine_log(const QString& label, const QString& msg)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_reachedStableState()
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_runningChanged(bool running)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}

	void IScxmStateMachineHandler::onScxmlStateMachine_tableDataChanged(QScxmlTableData* tableData)
	{
		qDebug() << "\033[0;34m" << QString("file(%1) line(%2) func(%3)").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__) << QString("log") << "\033[0m";
	}
}
