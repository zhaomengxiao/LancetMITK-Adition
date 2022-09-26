#ifndef LancetIStateMachineActionProperty_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

#include "org_mitk_lancet_statemachine_services_Export.h"

namespace lancet
{
	class IScxmlStateMachineState;
	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 * \brief Attribute abstract class of state machine element.
	 * 
	 * The attribute data class of the state machine element with a custom style is 
	 * required. Please inherit this class to implement.
	 *
	 * \author Sun
	 * \version V1.0.0
	 * \date 2022-09-23 18:11:22
	 * \remark Nothing.
	 *
	 * Contact: sh4a01@163.com
	 *
	 */
	class LANCET_STATEMACHINE_SERVICES_PLUGIN IStateMachineActionProperty 
		: public berry::Object
	{
		berryObjectMacro(lancet::IStateMachineActionProperty);
	public:
		IStateMachineActionProperty();
		IStateMachineActionProperty(const QString&);
		IStateMachineActionProperty(const berry::SmartPointer<IScxmlStateMachineState>&);

	public:
		virtual QString GetStateId() const;
		virtual void SetStateId(const QString&);

		virtual QString GetStateObjectName() const;
		virtual void SetStateObjectName(const QString&);

		virtual QString GetStateUiName() const;
		virtual void SetStateUiName(const QString&);

		// todo
		// theme
	private:
		struct IStateMachineActionPropertyPrivateImp;
		std::shared_ptr<IStateMachineActionPropertyPrivateImp> imp;
	};
}

#define LancetIStateMachineActionProperty_H
#endif // !LancetIStateMachineActionProperty_H
