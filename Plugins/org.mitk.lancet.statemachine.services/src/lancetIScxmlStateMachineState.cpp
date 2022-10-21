#include "lancetIScxmlStateMachineState.h"
#include "lancetIScxmlStateMachineService.h"
#include "lancetIStateMachineActionProperty.h"
#include "lancetIScxmStateMachineHandler.h"
#include "internal/lancetNamingFormats.h"

// Qt
#include <QScxmlStateMachine>

namespace lancet
{
	struct IScxmlStateMachineState::IScxmlStateMachineStatePrivateImp
	{
		bool active;
		QString stateId;
		StateType stateType;
		QString subStateMachineScxmlAbsoluteFile;
		berry::SmartPointer<IScxmStateMachineHandler> stateMachineHandler;
		//berry::SmartPointer<const IScxmlStateMachineState> lastActiveState;
		berry::SmartPointer<IScxmlStateMachineState> parentStateMachine;
		std::vector<berry::SmartPointer<IScxmlStateMachineState>> subStateMachines;

		QSharedPointer<QScxmlStateMachine> qtStateMachine;

		berry::SmartPointer<IStateMachineActionProperty> actionProperty;
	};

	IScxmlStateMachineState::IScxmlStateMachineState(IScxmlStateMachineState::Pointer manage,
		StateType type, const QString& id)
		: imp(std::make_shared<IScxmlStateMachineStatePrivateImp>())
	{
		this->imp->stateId = id;
		this->imp->stateType = type;
		this->SetManageStateMachine(manage);
	}

	IScxmlStateMachineState::~IScxmlStateMachineState()
	{
	}

	IScxmlStateMachineState::StateType IScxmlStateMachineState::GetStateType() const
	{
		return this->imp->stateType;
	}

	IScxmlStateMachineState::StateType IScxmlStateMachineState::GetSubStateType() const
	{
			return this->GetStateType() == IScxmlStateMachineState::StateType::Admin ?
				IScxmlStateMachineState::StateType::Module : IScxmlStateMachineState::StateType::Ordinary;
	}

	QString IScxmlStateMachineState::GetStateId() const
	{
		return this->imp->stateId;
	}

	QSharedPointer<QScxmlStateMachine> IScxmlStateMachineState::GetQtStateMachine() const
	{
		return this->imp->qtStateMachine;
	}

	void IScxmlStateMachineState::SetQtStateMachine(const QSharedPointer<QScxmlStateMachine>& stateMachine)
	{
		this->imp->qtStateMachine = stateMachine;
	}

	QString IScxmlStateMachineState::ToString() const
	{
		return this->ToString(false);
	}

	QString IScxmlStateMachineState::ToString(bool isDebug) const
	{
		/**
		AdminState
			-MouduleState
				-OrdinaryState
			-MouduleState
				-...
		*/
		QString info;

		info += "-------------------------------------------------------------------------\n";
		info += QString("StateId: %1\n").arg(this->GetStateId());
		info += QString("StateType: %1\n").arg(this->GetStateType());
		info += QString("StateActive: %1\n").arg(this->GetActive());
		info += QString("StateParent: id-%1\n").arg(this->GetManageStateMachine().IsNotNull() 
			? this->GetManageStateMachine()->GetStateId(): "empty");
		info += QString("LastActiveState: id-%1\n").arg(this->GetLastActiveState().IsNotNull()
			? this->GetLastActiveState()->GetStateId() : "empty");
		info += QString("QtScxmlStateMachine: name-%1\n").arg(!this->GetQtStateMachine().isNull()
			? this->GetQtStateMachine()->name() : "empty");
		info += QString("ScxmlFilePath: %1\n").arg(this->GetSubStateMachineScxmlFile());
		info += QString("ScxmlAbsoluteFilePath: %1\n").arg(this->GetSubStateMachineScxmlAbsoluteFile());

		info += QString("Property: todo\n");

		info += "\n\n";
		for (auto item : this->GetSubStateMachines())
		{
			if (item.IsNotNull())
			{
				info += item->ToString();
			}
		}
		info += "-------------------------------------------------------------------------\n";

		return info;
	}

	berry::SmartPointer<IScxmlStateMachineService> IScxmlStateMachineState::GetStateMachineService() const
	{
		return IScxmlStateMachineService::GetScxmlStateMachineService();
	}

	IScxmlStateMachineState::Pointer IScxmlStateMachineState::GetLastActiveState() const
	{
		if (this->GetStateMachineHandler().IsNotNull())
		{
			return this->GetStateMachineHandler()->GetLastActiveStateMachine();
		}
		return IScxmlStateMachineState::Pointer();
	}

	void IScxmlStateMachineState::SetLastActiveState(IScxmlStateMachineState::Pointer state)
	{
		if (this->GetStateMachineHandler().IsNotNull())
		{
			this->GetStateMachineHandler()->SetLastActiveStateMachine(state);
		}
		else
		{
			// Warning nullptr
		}
	}

	IScxmlStateMachineState::Pointer IScxmlStateMachineState::GetActiveState() const
	{
		if (this->GetStateMachineHandler().IsNotNull())
		{
			return this->GetStateMachineHandler()->GetActiveStateMachine();
		}
		return IScxmlStateMachineState::Pointer();
	}

	void IScxmlStateMachineState::SetActiveState(IScxmlStateMachineState::Pointer state)
	{
		if (this->GetStateMachineHandler().IsNotNull())
		{
			this->GetStateMachineHandler()->SetActiveStateMachine(state);
		}
		else
		{
			// Warning nullptr
		}
	}

	QString IScxmlStateMachineState::GetSubStateMachineScxmlFile() const
	{
		return this->GetSubStateMachineScxmlAbsoluteFile().left(this->GetSubStateMachineScxmlAbsoluteFile().lastIndexOf("/"));
	}

	QString IScxmlStateMachineState::GetSubStateMachineScxmlAbsoluteFile() const
	{
		return this->imp->subStateMachineScxmlAbsoluteFile;
	}

	void IScxmlStateMachineState::SetSubStateMachineScxmlAbsoluteFile(const QString& file)
	{
		this->imp->subStateMachineScxmlAbsoluteFile = QString(file).replace("\\", "/");
	}

	berry::SmartPointer<IScxmlStateMachineState> IScxmlStateMachineState::GetManageStateMachine() const
	{
		return this->imp->parentStateMachine;
	}

	void IScxmlStateMachineState::SetManageStateMachine(const berry::SmartPointer<IScxmlStateMachineState>& state)
	{
		this->imp->parentStateMachine = state;
	}

	std::vector<berry::SmartPointer<IScxmlStateMachineState>> IScxmlStateMachineState::GetSubStateMachines() const
	{
		return this->imp->subStateMachines;
	}

	void IScxmlStateMachineState::AppendSubStateMachineItem(const berry::SmartPointer<IScxmlStateMachineState>& state)
	{
		if (state.IsNotNull())
		{
			this->imp->subStateMachines.push_back(state);
		}
	}

	bool IScxmlStateMachineState::RemoveSubStateMachineItem(const QString& stateId)
	{
		for (auto item = this->imp->subStateMachines.begin(); item != this->imp->subStateMachines.end(); ++item)
		{
			if ((*item).IsNotNull() && (*item)->GetStateId() == stateId)
			{
				this->imp->subStateMachines.erase(item);
				return true;
			}
		}
		return false;
	}

	bool IScxmlStateMachineState::RemoveSubStateMachineItem(const berry::SmartPointer<IScxmlStateMachineState>& state)
	{
		return state.IsNotNull() ? this->RemoveSubStateMachineItem(state->GetStateId()) : false;
	}

	void IScxmlStateMachineState::SetSubStateMachines(std::vector<berry::SmartPointer<IScxmlStateMachineState>> vecStates)
	{
		this->imp->subStateMachines = vecStates;
	}

	berry::SmartPointer<IStateMachineActionProperty> IScxmlStateMachineState::GetActionProperty() const
	{
		return this->imp->actionProperty;
	}

	void IScxmlStateMachineState::SetActionProperty(const berry::SmartPointer<IStateMachineActionProperty>& property)
	{
		this->imp->actionProperty = property;
	}
	bool IScxmlStateMachineState::GetActive() const
	{
		return this->imp->active;
	}

	void IScxmlStateMachineState::SetActive(bool active)
	{
		this->imp->active = active;
	}

	IScxmlStateMachineState::StateFeatures 
		IScxmlStateMachineState::GetStateFeatures() const
	{
		int counter = 0;
		for (auto item : this->GetSubStateMachines())
		{
			if (item.IsNotNull() && plugin::IsLancetModuleFormat(item->GetStateId()))
			{
				++counter;
			}
		}
		switch (counter)
		{
		case 0:
			return IScxmlStateMachineState::StateFeatures::Unknown;
		case 1:
			return IScxmlStateMachineState::StateFeatures::Single;
		}
		return IScxmlStateMachineState::StateFeatures::Multiple;
	}

	berry::SmartPointer<IScxmStateMachineHandler> 
		IScxmlStateMachineState::GetStateMachineHandler() const
	{
		return this->imp->stateMachineHandler;
	}

	void IScxmlStateMachineState::SetStateMachineHandler(
		berry::SmartPointer<IScxmStateMachineHandler> handler)
	{
		this->imp->stateMachineHandler = handler;
	}
}