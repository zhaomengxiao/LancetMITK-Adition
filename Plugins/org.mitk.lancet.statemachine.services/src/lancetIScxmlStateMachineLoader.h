/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief State Machine Object Loader.
 * \ingroup org_mitk_lancet_statemachine_services
 * \version V1.0.0
 * \data 2022-09-26 10:16:48
 * 
 * \par Modify History
 *
 * \author Sun
 * \data 2022-09-26 10:16:48
 * \remark Nothing
 */
#ifndef LancetIScxmlStateMachineLoader_H
#define LancetIScxmlStateMachineLoader_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

#include <lancetIScxmlStateMachineState.h>
#include <org_mitk_lancet_statemachine_services_Export.h>

namespace lancet
{
	/**
		* \ingroup org_mitk_lancet_statemachine_services
		* 
		* \brief State Machine Loader, A complete state consists of attributes, SCXML, 
		*				 and events.
		* 
		*				 This is implemented based on the QtScxml module, which converts the 
		*				 state of the state machine from the configuration file to the property 
		*				 objects of the <p>Qt state machine</p> and the lancet state machine.
		* 
		*				 This class is not responsible for state machine event processing.
		* 
		* \see lancet::IScxmlStateMachineState, QScxmlStateMachine, 
		*			 lancet::IScxmlStateMachineLoader#GetOutput()
		*			 
		* \note The user shall implement the following interfaces:
		*			 lancet::IScxmlStateMachineLoader#GetOutput()
		* 
		* \author Sun
		*/
	class LANCET_STATEMACHINE_SERVICES_PLUGIN IScxmlStateMachineLoader : public berry::Object
	{
	public:
		berryObjectMacro(lancet::IScxmlStateMachineLoader);

		IScxmlStateMachineLoader();
		/**
		* \brief Parametric construction method.
		* 
		* You should probably refer to the description of the \f SetInput() interface, 
		* which is clearer than what is explained here.
		* 
		* \see lancet::IScxmlStateMachineLoader#SetInput()
		*/
		explicit IScxmlStateMachineLoader(const QString& filename, const QString& propertyFileName, 
			IScxmlStateMachineState::StateType type, IScxmlStateMachineState::Pointer manage, 
			const QString& id);

		/**
		* \brief Set the input parameter set of the loader.
		*
		* These parameters include the scxml state machine configuration file of Qt, the 
		* font attribute description file (\c json) customized by lancet, and the basic 
		* attributes of the state to be loaded. For these basic attributes, refer to 
		* <p>lancet::#IScxmlStateMachineState</p>
		*
		* \param filename
		*						Qt style files with scxml suffix. Please enter an absolute file address.
		* \param propertyFileName
		*						The json file described by the user-defined status attribute. Please
		*						enter the absolute file address.
		* \param type
		*						Definition property type of state.
		* \param manage
		*						Manager Object.
		* \param id
		*						Unique ID code of the status.
		*
		* \see lancet::IScxmlStateMachineLoader#SetFileName()
		*			 lancet::IScxmlStateMachineLoader#SetPropertyFileName()
		*			 lancet::IScxmlStateMachineLoader#SetStateType()
		*			 lancet::IScxmlStateMachineLoader#SetManageStateMachine()
		*			 lancet::IScxmlStateMachineLoader#SetStateId()
		*			 lancet::IScxmlStateMachineLoader#GetOutput()
		*/
		virtual void SetInput(const QString& filename, const QString& propertyFileName, 
			IScxmlStateMachineState::StateType type, IScxmlStateMachineState::Pointer manage, 
			const QString& id);

		/**
		* \brief Output the loaded state object according to the configuration of the 
		*				 loader.
		* 
		* The derived class must implement this interface from the new one, which is 
		* also the only interface extended by the derived class.
		* 
		* \see lancet::IScxmlStateMachineState
		*/
		virtual berry::SmartPointer<IScxmlStateMachineState> GetOutput() = 0;

		/**
		* \brief Qt style scxml configuration file. You must enter an absolute path.
		*/
		virtual QString GetFileName() const;
		virtual void SetFileName(const QString&);

		/**
		* \brief The json file described by the user-defined status attribute. 
		*				 You must enter an absolute path.
		*/
		virtual QString GetPropertyFileName() const;
		virtual void SetPropertyFileName(const QString&) const;

		/**
		* \brief Definition property type of state.
		*/
		virtual IScxmlStateMachineState::StateType GetStateType() const;
		virtual void SetStateType(const IScxmlStateMachineState::StateType&);

		/**
		* \brief Manager Object.
		* \see lancet::IScxmlStateMachineState
		*/
		virtual IScxmlStateMachineState::Pointer GetManageStateMachine() const;
		virtual void SetManageStateMachine(IScxmlStateMachineState::Pointer);

		/**
		* \brief Load the unique ID identifier of the target state.
		*/
		virtual QString GetStateId() const;
		virtual void SetStateId(const QString&);

		/**
		* \brief Abnormal status and information produced by the loader.
		*/
		virtual bool GetErrorState() const;
		virtual bool GetWarningState() const;
		virtual QStringList GetErrorStringList() const;
		virtual QStringList GetWarningStringList() const;
	protected:
		virtual void SetErrorState(bool);
		virtual void SetErrorStringList(const QStringList&);
		virtual void AppendErrorStringItem(const QString&);
		virtual void RemoveErrorStringItem();

		virtual void SetWarningState(bool);
		virtual void SetWarningStringList(const QStringList&);
		virtual void AppendWarningStringItem(const QString&);
		virtual void RemoveWarningStringItem();
	private:
		struct IScxmlStateMachineLoaderPrivateImp;
		std::shared_ptr<IScxmlStateMachineLoaderPrivateImp> imp;
	};
}

#endif // !LancetIScxmlStateMachineLoader_H
