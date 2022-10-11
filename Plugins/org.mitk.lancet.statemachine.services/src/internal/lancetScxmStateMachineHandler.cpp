#include "lancetScxmStateMachineHandler.h"

#include <QScxmlStateMachine>
#include <QScxmlStateMachine>

#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	ScxmStateMachineHandler::ScxmStateMachineHandler() 
		: IScxmStateMachineHandler(IScxmlStateMachineState::Pointer(nullptr))
	{
	}

	ScxmStateMachineHandler::ScxmStateMachineHandler(berry::SmartPointer<IScxmlStateMachineState> state)
		: IScxmStateMachineHandler(state)
	{
	}

	ScxmStateMachineHandler::~ScxmStateMachineHandler()
	{
	}

	bool ScxmStateMachineHandler::StartStateMachine()
	{
		if (this->GetAssignor().IsNotNull() && !this->GetAssignor()->GetQtStateMachine().isNull())
		{
			this->GetAssignor()->GetQtStateMachine()->start();
			return true;
		}
		return false;
	}

	bool ScxmStateMachineHandler::StopStateMachine()
	{
		if (this->GetAssignor().IsNotNull() && !this->GetAssignor()->GetQtStateMachine().isNull())
		{
			IScxmlStateMachineState::Pointer exitState;
			for (auto item : this->GetAssignor()->GetSubStateMachines())
			{
				if (item.IsNotNull() && item->GetStateId() == "exit")
				{
					exitState = item;
					break;
				}
			}
			this->ToTargetState(exitState);
			//this->GetAssignor()->GetQtStateMachine()->stop();
			return true;
		}
		return false;
	}

	bool ScxmStateMachineHandler::Init(berry::SmartPointer<IScxmlStateMachineState> state)
	{
		return this->InitQtEventForStateMachine(state);
	}
}