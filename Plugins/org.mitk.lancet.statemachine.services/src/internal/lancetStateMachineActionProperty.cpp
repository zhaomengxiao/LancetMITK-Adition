#include "lancetStateMachineActionProperty.h"
#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	StateMachineActionProperty::StateMachineActionProperty()
		: IStateMachineActionProperty()
	{

	}

	StateMachineActionProperty::StateMachineActionProperty(const QString& id)
		: IStateMachineActionProperty(id)
	{
	}

	StateMachineActionProperty::StateMachineActionProperty(
		const berry::SmartPointer<IScxmlStateMachineState>& state)
		: IStateMachineActionProperty(state)
	{
	}
}