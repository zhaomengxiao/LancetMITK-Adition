#include "LancetIScxmlStateMachineService.h"
#include "lancetIScxmStateMachineHandler.h"

// Qt
#include <QScxmlStateMachine.h>

namespace lancet
{
	struct IScxmlStateMachineService::IScxmlStateMachineServicePrivateImp
	{
		IScxmlStateMachineState::Pointer adminState;

		static berry::SmartPointer<IScxmlStateMachineService> staticStateMachineService;
	};

	// Initialize static service object.
	berry::SmartPointer<IScxmlStateMachineService>
		IScxmlStateMachineService::IScxmlStateMachineServicePrivateImp::staticStateMachineService =
		berry::SmartPointer<IScxmlStateMachineService>();

	IScxmlStateMachineService::IScxmlStateMachineService()
		:imp(std::make_shared<IScxmlStateMachineServicePrivateImp>())
	{
		// Binding service object
		this->imp->staticStateMachineService = berry::SmartPointer<IScxmlStateMachineService>(this);
	}

	IScxmlStateMachineService::~IScxmlStateMachineService()
	{
		// Unbind service object
		this->imp->staticStateMachineService = berry::SmartPointer<IScxmlStateMachineService>(nullptr);
	}

	IScxmlStateMachineState::Pointer IScxmlStateMachineService::GetAdminState() const
	{
		return this->imp->adminState;
	}

	void IScxmlStateMachineService::SetAdminState(const IScxmlStateMachineState::Pointer& state)
	{
		this->imp->adminState = state;
	}

	std::vector<IScxmlStateMachineState::Pointer> IScxmlStateMachineService::GetScxmlModuleStateMachines() const
	{
		IScxmlStateMachineState::Pointer adminState = this->GetAdminState();
		std::vector<IScxmlStateMachineState::Pointer> retval = std::vector<IScxmlStateMachineState::Pointer>();

		if (adminState.IsNotNull())
		{
			retval = this->GetScxmlStateMachines(adminState, IScxmlStateMachineState::StateType::Module);
		}
		return retval;
	}

	void IScxmlStateMachineService::SetScxmlModuleStateMachines(const std::vector<IScxmlStateMachineState::Pointer>& vec)
	{
		if (this->GetAdminState().IsNotNull())
		{
			return this->SetScxmlModuleStateMachines(this->GetAdminState()->GetStateId(), vec);
		}
	}

	void IScxmlStateMachineService::SetScxmlModuleStateMachines(const QString& id,
		const std::vector<IScxmlStateMachineState::Pointer>& vec)
	{
		IScxmlStateMachineState::Pointer findState = this->GetScxmlStateMachine(id);
		if (findState.IsNotNull())
		{
			findState->SetSubStateMachines(vec);
		}
		else
		{
			// Print The state machine may be in an unloaded state and admin state is empty.
		}
	}

	IScxmlStateMachineState::Pointer IScxmlStateMachineService::GetScxmlStateMachine(const QString& id) const
	{
		if (this->GetAdminState().IsNotNull())
		{
			if (id == this->GetAdminState()->GetStateId())
			{
				return this->GetAdminState();
			}
			return this->GetScxmlStateMachine(this->GetAdminState(), id);
		}

		return IScxmlStateMachineState::Pointer();
	}

	IScxmlStateMachineState::Pointer
		IScxmlStateMachineService::GetScxmlStateMachine(IScxmlStateMachineState::Pointer state, const QString& id) const
	{
		if (state.IsNotNull())
		{
			for (auto item_state : state->GetSubStateMachines())
			{
				if (item_state.IsNotNull() 
					&& item_state->GetStateId().toLower() == id.toLower()
					&& state->GetSubStateType() == item_state->GetStateType())
				{
					return item_state;
				}
			}
		}
		return IScxmlStateMachineState::Pointer();
	}

	std::vector<IScxmlStateMachineState::Pointer>
		IScxmlStateMachineService::GetScxmlStateMachines(IScxmlStateMachineState::Pointer state,
			IScxmlStateMachineState::StateType type) const
	{
		std::vector<IScxmlStateMachineState::Pointer> retval = std::vector<IScxmlStateMachineState::Pointer>();
		return this->GetScxmlStateMachines(state, type, retval);
	}

	std::vector<IScxmlStateMachineState::Pointer>
		IScxmlStateMachineService::GetScxmlStateMachines(IScxmlStateMachineState::Pointer state,
			IScxmlStateMachineState::StateType type,
			std::vector<IScxmlStateMachineState::Pointer>& out) const
	{
		if (state.IsNotNull())
		{
			for (auto item_state : state->GetSubStateMachines())
			{
				if (item_state.IsNotNull() && item_state->GetStateType() == type)
				{
					out.push_back(item_state);
					return this->GetScxmlStateMachines(state, type, out);
				}
			}
		}
		return out;
	}

	void IScxmlStateMachineService::Initialize()
	{
		//qRegisterMetaType<IScxmlStateMachineState>();
	}

	berry::SmartPointer<IScxmlStateMachineService> 
		IScxmlStateMachineService::GetScxmlStateMachineService()
	{
		return IScxmlStateMachineServicePrivateImp::staticStateMachineService;
	}
}