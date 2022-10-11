#ifndef LancetScxmStateMachineHandler_H
#define LancetScxmStateMachineHandler_H

// berry
#include <berryObject.h>
#include <berryMacros.h>

#include "lancetIScxmStateMachineHandler.h"

namespace lancet
{
class IScxmlStateMachineState;

/**
 * \ingroup org_mitk_lancet_statemachine_services
 * \brief Event processing unit of private state machine element.
 *
 * This class is inherited from lancet::IScxmStateMachineHandler. Please refer to
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
	ScxmStateMachineHandler : public IScxmStateMachineHandler
{
	Q_OBJECT
public:
	berryObjectMacro(lancet::ScxmStateMachineHandler);

	ScxmStateMachineHandler();
	ScxmStateMachineHandler(berry::SmartPointer<IScxmlStateMachineState>);
	virtual ~ScxmStateMachineHandler() override;

	virtual bool StartStateMachine() override;
	virtual bool StopStateMachine() override;
public:
	virtual bool Init(berry::SmartPointer<IScxmlStateMachineState>) override;
};
}
#endif // !LancetScxmStateMachineHandler_H
