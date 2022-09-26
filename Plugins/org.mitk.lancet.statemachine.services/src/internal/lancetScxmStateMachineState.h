#ifndef LancetScxmStateMachineState_H
#define LancetScxmStateMachineState_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

#include <src/lancetIScxmlStateMachineState.h>
#include <org_mitk_lancet_statemachine_services_Export.h>

namespace lancet
{
class IScxmlStateMachineState;

/**
 * \ingroup org_mitk_lancet_statemachine_services
 * \brief Customized state machine elements.
 *
 * This class is inherited from lancet::IScxmlStateMachineState. Please refer to
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
class LANCET_STATEMACHINE_SERVICES_PLUGIN ScxmlStateMachineState 
	: public IScxmlStateMachineState
{
	Q_OBJECT
	
public:
	berryObjectMacro(lancet::ScxmlStateMachineState);

	ScxmlStateMachineState(berry::SmartPointer<IScxmlStateMachineState> manage,
		StateType type, const QString& id);
};
}

#endif // !LancetScxmStateMachineState_H
