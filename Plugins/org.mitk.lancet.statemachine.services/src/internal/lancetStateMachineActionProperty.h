#ifndef LancetStateMachineActionProperty_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

#include <src/lancetIStateMachineActionProperty.h>
#include <org_mitk_lancet_statemachine_services_Export.h>

namespace lancet
{
	class IScxmlStateMachineState;
	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 * \brief Attributes of a private custom state machine element.
	 *
	 * This class is inherited from lancet::IStateMachineActionProperty. Please refer to
	 * superclass for details.
	 *
	 * \author Sun
	 * \version V1.0.0
	 * \date 2022-09-26 17:31:18
	 * \remark Non
	 *
	 * Contact: sh4a01@163.com
	 *
	 */
	class LANCET_STATEMACHINE_SERVICES_PLUGIN
		StateMachineActionProperty : public IStateMachineActionProperty
	{
		berryObjectMacro(lancet::StateMachineActionProperty);
	public:
		StateMachineActionProperty();
		StateMachineActionProperty(const QString&);
		StateMachineActionProperty(const berry::SmartPointer<IScxmlStateMachineState>&);
	protected:
		virtual void Initialize() override;
	};
}

#define LancetStateMachineActionProperty_H
#endif // !LancetStateMachineActionProperty_H
