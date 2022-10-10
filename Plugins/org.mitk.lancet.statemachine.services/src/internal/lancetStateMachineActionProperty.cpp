#include "lancetStateMachineActionProperty.h"
#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	StateMachineActionProperty::StateMachineActionProperty()
		: IStateMachineActionProperty()
	{
		this->Initialize();
	}

	StateMachineActionProperty::StateMachineActionProperty(const QString& id)
		: IStateMachineActionProperty(id)
	{
		this->Initialize();
	}

	StateMachineActionProperty::StateMachineActionProperty(
		const berry::SmartPointer<IScxmlStateMachineState>& state)
		: IStateMachineActionProperty(state)
	{
		this->Initialize();
	}

	void StateMachineActionProperty::Initialize()
	{
		this->SetKeywordValue(Keywords::ID, "");
		this->SetKeywordValue(Keywords::UI_NAME, "");
		this->SetKeywordValue(Keywords::EDITOR_ID, "");
		this->SetKeywordValue(Keywords::FUNCTION_ID, "");
		this->SetKeywordValue(Keywords::OBJECT_NAME, "");
	}
}