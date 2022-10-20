#include "LancetScxmlStateMachineService.h"

// Qt
#include <QDir>
#include <QScxmlStateMachine>

// Unit
#include "lancetNamingFormats.h"

#include "lancetScxmStateMachineState.h"
#include "lancetScxmlStateMachineLoader.h"
#include "lancetScxmStateMachineHandler.h"
#include "lancetScxmlStateMachineService.h"

namespace lancet
{
	struct ScxmlStateMachineService::ScxmlStateMachineServicePrivateImp
	{
		IScxmlStateMachineState::Pointer state;
	};

	ScxmlStateMachineService::ScxmlStateMachineService()
		: imp(std::make_shared<ScxmlStateMachineServicePrivateImp>())
	{
		qDebug() << "ScxmlStateMachineService::ScxmlStateMachineService";
	}

	ScxmlStateMachineService::~ScxmlStateMachineService()
	{
	}

	IScxmlStateMachineState::Pointer 
		ScxmlStateMachineService::CreateScxmlStateMachine(const QString& scxmlfile,
		const QString& propertyFileName, int type, berry::SmartPointer<IScxmlStateMachineState> manage, 
		const QString& id)
	{
		IScxmlStateMachineState::Pointer stateMachineState;
		IScxmlStateMachineState::StateType inputStateType = IScxmlStateMachineState::StateType(type);

		// loading
		ScxmlStateMachineLoader loader(scxmlfile, propertyFileName, inputStateType, manage, id);
		stateMachineState = loader.GetOutput();

		///< Start debug
		//if (loader.GetErrorState())
		//{
		//	qDebug() << loader.GetErrorStringList();
		//}
		//if (loader.GetWarningState())
		//{
		//	qDebug() << loader.GetWarningStringList();
		//}
		/// End debug

		if (inputStateType == IScxmlStateMachineState::StateType::Admin)
		{
			this->SetAdminState(stateMachineState);
		}
		// Module or Ordinary
		if (false == stateMachineState->GetQtStateMachine().isNull())
		{
				for (auto substate_id : stateMachineState->GetQtStateMachine()->stateNames())
				{
					QString sub_scxmlfile = 
						state_machine::GetStateScxmlFileName(scxmlfile, substate_id, 
							stateMachineState->GetSubStateType());

					// propertyfile.json
					// You may be interested in naming rules. You can look at 
					// \f lancet::state_machine::GetStatePropertyFileName
					QString sub_propertyfile = 
						state_machine::GetStatePropertyFileName(scxmlfile, substate_id, 
							stateMachineState->GetSubStateType());

					auto substate = this->CreateScxmlStateMachine(sub_scxmlfile, sub_propertyfile,
						stateMachineState->GetSubStateType(), stateMachineState, substate_id);

					stateMachineState->AppendSubStateMachineItem(substate);
				}
		}

		// link handler
		ScxmStateMachineHandler::Pointer handle =
			ScxmStateMachineHandler::Pointer(new ScxmStateMachineHandler(stateMachineState));
		handle->Init(stateMachineState);
		stateMachineState->SetStateMachineHandler(handle);

		return stateMachineState;
	}

	bool ScxmlStateMachineService::StartStateMachine()
	{
		if (this->GetAdminState().IsNotNull()
			&& this->GetAdminState()->GetStateMachineHandler().IsNotNull())
		{
			return this->GetAdminState()->GetStateMachineHandler()->StartStateMachine();
		}
		return false;
	}

	bool ScxmlStateMachineService::StopStateMachine()
	{
		if (this->GetAdminState().IsNotNull()
			&& this->GetAdminState()->GetStateMachineHandler().IsNotNull())
		{
			return this->GetAdminState()->GetStateMachineHandler()->StopStateMachine();
		}
		return false;
	}

	//IScxmlStateMachineState::Pointer ScxmlStateMachineService::GetLastActiveScxmlStateMachine() const
	//{
	//	return IScxmlStateMachineState::Pointer();
	//}
}