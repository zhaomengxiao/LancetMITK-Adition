#include "lancetIStateMachineActionProperty.h"
#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	struct IStateMachineActionProperty::IStateMachineActionPropertyPrivateImp
	{
		QString stateId;
		QString stateUiName;
		QString stateObjectName;
	};

	IStateMachineActionProperty::IStateMachineActionProperty()
		: imp(std::make_shared<IStateMachineActionPropertyPrivateImp>())
	{

	}

	IStateMachineActionProperty::IStateMachineActionProperty(const QString& id)
		: IStateMachineActionProperty()
	{
		this->SetStateId(id);
	}

	IStateMachineActionProperty::IStateMachineActionProperty(
		const berry::SmartPointer<IScxmlStateMachineState>& state)
		: IStateMachineActionProperty()
	{
		if (state.IsNotNull())
		{
			this->SetStateId(state->GetStateId());
		}
	}

	QString IStateMachineActionProperty::GetStateId() const
	{
		return this->imp->stateId;
	}

	void IStateMachineActionProperty::SetStateId(const QString& state)
	{
		this->imp->stateId = state;
	}

	QString IStateMachineActionProperty::GetStateObjectName() const
	{
		return this->imp->stateObjectName;
	}

	void IStateMachineActionProperty::SetStateObjectName(const QString& name)
	{
		this->imp->stateObjectName = name;
	}

	QString IStateMachineActionProperty::GetStateUiName() const
	{
		return this->imp->stateUiName;
	}

	void IStateMachineActionProperty::SetStateUiName(const QString& name)
	{
		this->imp->stateUiName = name;
	}
}