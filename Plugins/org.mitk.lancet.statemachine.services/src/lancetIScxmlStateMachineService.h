/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare a state machine service object.
 * \ingroup org_mitk_lancet_statemachine_services
 * \version V1.0.0
 * \data 2022-09-23 17:44:54
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-09-23 17:44:54
 * \remark Create initialize version.
 */
#ifndef LancetIScxmlStateMachineService_H
#define LancetIScxmlStateMachineService_H

// Qt
#include <QVariant>

#include <lancetIScxmlStateMachineState.h>
#include <org_mitk_lancet_statemachine_services_Export.h>

namespace lancet
{
	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 * \brief This is the abstract class of state machine service.
	 *
	 * It manages and loads multi-dimensional state and state machine, and provides
	 * the entry point of state management module to the outside.
	 * Derived classes should inherit \c QObject on the basis of abstract classes.
	 * 
	 * \pre 
	 * 
	 * The middleware must be loaded using the or Qt plug-in framework.
	 * 
	 * \see lancet::IScxmlStateMachineState
	 *
	 * \author Sun
	 * \version V1.0.0
	 * \date 2022-09-23 18:11:22
	 * \remark Nothing.
	 *
	 * Contact: sh4a01@163.com
	 *
	 */
	class LANCET_STATEMACHINE_SERVICES_PLUGIN IScxmlStateMachineService 
		: public berry::Object
	{
	public:
		berryObjectMacro(lancet::IScxmlStateMachineService);

		IScxmlStateMachineService();
		virtual ~IScxmlStateMachineService();

		/**
		* \brief Return to the top manager of the state machine management module.
		*/
		virtual IScxmlStateMachineState::Pointer GetAdminState() const;
		virtual void SetAdminState(const IScxmlStateMachineState::Pointer&);

		/**
		* \brief Load external configuration files to the state machine service module.
		* 
		* The derivation should pay attention to the multidimensional existence of 
		* state machine or sub state.
		* 
		* \example
		* 
		* \see lancet::IScxmlStateMachineState
		*/
		virtual IScxmlStateMachineState::Pointer CreateScxmlStateMachine(const QString& scxmlfile,
			const QString& propertyFileName, int, berry::SmartPointer<IScxmlStateMachineState>,
			const QString&) = 0;

		/**
		 * \brief Start or stop the <p>state machine</p>.
		 *
		 * The state machine service is multi-dimensional. Here, the state machine of 
		 * the top-level state object is operated. Starting is only effective for the 
		 * state machine of the top-level state, and stopping recursively stops all 
		 * started sub state machines.
		 *
		 * \pre The state machine service can effectively trigger this method function 
		 *      only after initialization, otherwise it is invalid.
		 * 
		 * \return Returns true if successful, otherwise false.
		 * 
		 * \see lancet::IScxmlStateMachineService::CreateScxmlStateMachine()
		 */
		
		virtual bool StartStateMachine() = 0;
		virtual bool StopStateMachine() = 0;

		/**
		 * \brief Set or obtain the state set under the module state machine object.
		 *
		 * This is just a convenient method. The data source is provided by the state 
		 * object.
		 *
		 * \pre The state machine service can effectively trigger this method function
		 *      only after initialization, otherwise it is invalid.
		 *
		 * \see lancet::IScxmlStateMachineState
		 */
		
		virtual std::vector<IScxmlStateMachineState::Pointer> GetScxmlModuleStateMachines() const;
		virtual void SetScxmlModuleStateMachines(const std::vector<IScxmlStateMachineState::Pointer>&);
		virtual void SetScxmlModuleStateMachines(const QString& id,
			const std::vector<IScxmlStateMachineState::Pointer>&);

		/**
		 * \brief Get or set the sub level state set. For details, refer to
		 * <p>lancet::IScxmlStateMachineState::SetSubStateMachines()</p> or
		 * <p>lancet::IScxmlStateMachineState::GetSubStateMachines()</p>
		 *
		 * Get or set the sub level state set of the target state object.
		 * The service object does not record any data of this type, but is
		 * an operation state object. Then think backward. When you need
		 * this type of operation, you should also pay attention to the
		 * properties of the state object.
		 *
		 * \pre The state machine service can effectively trigger this method function
		 *      only after initialization, otherwise it is invalid.
		 *
		 * \return Child State Object Set.
		 */
		virtual IScxmlStateMachineState::Pointer GetScxmlStateMachine(const QString&) const;
		virtual IScxmlStateMachineState::Pointer
			GetScxmlStateMachine(IScxmlStateMachineState::Pointer, const QString&) const;

	protected:
		std::vector<IScxmlStateMachineState::Pointer>
			GetScxmlStateMachines(IScxmlStateMachineState::Pointer, IScxmlStateMachineState::StateType) const;

		std::vector<IScxmlStateMachineState::Pointer>
			GetScxmlStateMachines(IScxmlStateMachineState::Pointer, IScxmlStateMachineState::StateType,
				std::vector<IScxmlStateMachineState::Pointer>&) const;
	public:
		static void Initialize();
		static berry::SmartPointer<IScxmlStateMachineService> GetScxmlStateMachineService() /*final*/;
	private:
		struct IScxmlStateMachineServicePrivateImp;
		std::shared_ptr<IScxmlStateMachineServicePrivateImp> imp;
	};

}
Q_DECLARE_INTERFACE(lancet::IScxmlStateMachineService, "org.mitk.lancet.statemachine.services.IScxmlStateMachineService")

#endif // !LancetIScxmlStateMachineService_H