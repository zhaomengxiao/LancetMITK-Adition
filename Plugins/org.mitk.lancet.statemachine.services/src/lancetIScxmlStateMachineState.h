/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Declare a state machine object.
 * \ingroup org_mitk_lancet_statemachine_services
 * \version V1.0.0
 * \data 2022-09-26 10:18:08
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-09-26 10:18:08
 * \remark Nothing.
 */
#ifndef LancetIScxmlStateMachineState_H
#define LancetIScxmlStateMachineState_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

// Qt
#include <QObject>
#include <QMetaType>
#include <QSharedPointer>

#include <org_mitk_lancet_statemachine_services_Export.h>

class QScxmlStateMachine;
namespace lancet
{
	class IScxmStateMachineHandler;
	class IScxmlStateMachineService;
	class IStateMachineActionProperty;
	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 *
	 * \brief State description object of state machine.
	 *
	 * The state machine is generally presented as follows and managed in a tree structure.
	 * - StateType::Admin
	 *		- StateType::Module
	 *			- StateType::Ordinary
	 *			- StateType::Ordinary
	 *		- StateType::Module
	 *			- StateType::Ordinary
	 *			- ...
	 *		- ...
	 *
	 * \warning Only one StateType::Admin instance object can exist in a process.
	 * Otherwise, the previous instance will be overwritten
	 */
	class LANCET_STATEMACHINE_SERVICES_PLUGIN IScxmlStateMachineState 
		: public QObject, public berry::Object
	{
		Q_OBJECT
	public:
		berryObjectMacro(lancet::IScxmlStateMachineState);
		/**
		* \brief State type description of state machine.
		*
		*/
		enum StateType
		{
			Admin,			///< Top level state machine state type. 
			Module,			///< Module state machine state type.
			Ordinary		///< Common state machine state type.
		};

		/**
		* \brief IScxmlStateMachineState Class Construction Method.
		*
		* \param manage
		*					State Manager Instance Object.If the new target state type is Admin, please leave
		*					it blank.
		* \param type
		*					The type of state. Admin has uniqueness and coverage.
		* \param id
		*					The ID of the status. The ID value should be the unique value of the <p>mitk</p>
		*					plug-in style.
		*/
		IScxmlStateMachineState(IScxmlStateMachineState::Pointer manage, 
			StateType type, const QString& id);
		virtual ~IScxmlStateMachineState();

		/**
		* \brief Type of return status.
		*/
		virtual StateType GetStateType() const;
		virtual StateType GetSubStateType() const;

		/**
		* \brief Returns the unique ID of the status object.
		*
		* \note You should probably guide mitk's plug-in id naming style.
		*/
		virtual QString GetStateId() const;

		/**
		* \brief Returns a Qt style state machine instance.
		*
		* \see QScxmlStateMachine(<p>Qt 5.12.10</p>)
		*/
		virtual QSharedPointer<QScxmlStateMachine> GetQtStateMachine() const;
		virtual void SetQtStateMachine(const QSharedPointer<QScxmlStateMachine>&);

		/**
		* \brief The last activated sub state of the return state.
		*
		* Because the state machine module is managed in a tree structure, the returned
		* object may be a Module or an Ordinance attribute. Users can flexibly use this
		* state object according to their business.
		*
		* \see lancet::IScxmlStateMachineState::Pointer
		*			 lancet::IScxmlStateMachineState::GetActiveState()
		*/
		virtual IScxmlStateMachineState::Pointer GetLastActiveState() const final;
		virtual void SetLastActiveState(IScxmlStateMachineState::Pointer) final;

		/**
		* \brief The current activated sub state of the return state.
		* 
		* \see lancet::IScxmlStateMachineState::Pointer
		*			 lancet::IScxmlStateMachineState::GetLastActiveState()
		*/
		virtual IScxmlStateMachineState::Pointer GetActiveState() const final;
		virtual void SetActiveState(IScxmlStateMachineState::Pointer) final;

		/**
		* \brief Returns the SCXML configuration file name.
		*
		* \see lancet::IScxmlStateMachineState::GetSubStateMachineScxmlAbsoluteFile()
		*/
		virtual QString GetSubStateMachineScxmlFile() const final;

		/**
		* \brief Returns the absolute path of the SCXML configuration file.
		*
		* \see lancet::IScxmlStateMachineState::GetSubStateMachineScxmlFile()
		*/
		virtual QString GetSubStateMachineScxmlAbsoluteFile() const final;
		virtual void SetSubStateMachineScxmlAbsoluteFile(const QString&) final;

		/**
		* \brief Return Status Manager Object.
		*
		* \see lancet::IScxmlStateMachineState, berry::SmartPointer
		*/
		virtual berry::SmartPointer<IScxmlStateMachineState> GetManageStateMachine() const;
		virtual void SetManageStateMachine(const berry::SmartPointer<IScxmlStateMachineState>&);


		/**
		* \brief Return Status Manager Object.
		*
		* \see lancet::IScxmlStateMachineState, berry::SmartPointer
		*/
		virtual std::vector<berry::SmartPointer<IScxmlStateMachineState>> GetSubStateMachines() const;
		virtual void AppendSubStateMachineItem(const berry::SmartPointer<IScxmlStateMachineState>&);
		virtual bool RemoveSubStateMachineItem(const QString&);
		virtual bool RemoveSubStateMachineItem(const berry::SmartPointer<IScxmlStateMachineState>&);
		virtual void SetSubStateMachines(std::vector<berry::SmartPointer<IScxmlStateMachineState>>);

		/**
		* \brief Return the attribute representation object of components in Qt interface.
		*
		* \see lancet::IStateMachineActionProperty, berry::SmartPointer
		*/
		virtual berry::SmartPointer<IStateMachineActionProperty> GetActionProperty() const;
		virtual void SetActionProperty(const berry::SmartPointer<IStateMachineActionProperty>&);

		virtual bool GetActive() const;
		virtual void SetActive(bool);

		virtual berry::SmartPointer<IScxmStateMachineHandler> GetStateMachineHandler() const;
		virtual void SetStateMachineHandler(berry::SmartPointer<IScxmStateMachineHandler>);
	public:

		/**
		* \brief Override berry::ToString().
		*
		* \see berry::ToString()
		* \todo To be realized IScxmlStateMachineState::ToString(bool) const
		*/
		virtual QString ToString() const override;
		virtual QString ToString(bool isDebug) const;
	public:
		/**
		* \brief Returns the state machine service object.
		*
		* This method shields derived classes from re implementation.
		*
		* \see lancet::IScxmlStateMachineService, berry::SmartPointer
		*/
		virtual berry::SmartPointer<IScxmlStateMachineService> GetStateMachineService() const final;

	private:
		struct IScxmlStateMachineStatePrivateImp;
		std::shared_ptr<IScxmlStateMachineStatePrivateImp> imp;
	};
}
//Q_DECLARE_METATYPE(lancet::IScxmlStateMachineState)
#endif // !LancetIScxmlStateMachineState_H