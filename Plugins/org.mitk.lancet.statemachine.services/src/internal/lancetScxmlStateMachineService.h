#ifndef LancetScxmlStateMachineService_H
#define LancetScxmlStateMachineService_H

#include <org_mitk_lancet_statemachine_services_Export.h>

#include <src/lancetIScxmlStateMachineService.h>

namespace lancet
{

	/**
	 * \ingroup org_mitk_lancet_statemachine_services
	 * \brief Private customized state machine service module entry.
	 *
	 * This class is inherited from lancet::IScxmlStateMachineService. Please refer to
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
	class LANCET_STATEMACHINE_SERVICES_PLUGIN ScxmlStateMachineService
		: public QObject, public IScxmlStateMachineService
	{
		Q_OBJECT
		Q_INTERFACES(lancet::IScxmlStateMachineService)
	public:
		ScxmlStateMachineService();

	public:
		virtual ~ScxmlStateMachineService() override;

		virtual IScxmlStateMachineState::Pointer CreateScxmlStateMachine(const QString& scxmlfile,
			const QString& propertyFileName, int, berry::SmartPointer<IScxmlStateMachineState>,
			const QString&) override;

		virtual bool StartStateMachine() override;
		virtual bool StopStateMachine() override;

		//virtual IScxmlStateMachineState::Pointer GetLastActiveScxmlStateMachine() const override;
	private:
		struct ScxmlStateMachineServicePrivateImp;
		std::shared_ptr<ScxmlStateMachineServicePrivateImp> imp;
	};
}

#endif // !LancetScxmlStateMachineService_H
