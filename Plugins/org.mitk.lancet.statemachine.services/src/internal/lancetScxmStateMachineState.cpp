#include "lancetScxmStateMachineState.h"


namespace lancet
{
	ScxmlStateMachineState::ScxmlStateMachineState(berry::SmartPointer<IScxmlStateMachineState> manage, 
		IScxmlStateMachineState::StateType type, const QString& id)
		: IScxmlStateMachineState(manage, type, id)
	{

	}
}