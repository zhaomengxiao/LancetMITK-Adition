#ifndef LancetScxmlStateMachineLoader_H
#define LancetScxmlStateMachineLoader_H

#include <src/lancetIScxmlStateMachineLoader.h>

#include "org_mitk_lancet_statemachine_services_Export.h"

class QJsonDocument;
class QScxmlStateMachine;
namespace lancet
{
class StateMachineActionProperty;
/**
 * \ingroup org_mitk_lancet_statemachine_services
 * \brief Loader for state machine service to load external files.
 *
 * This class is inherited from lancet::IScxmlStateMachineLoader. Please refer to 
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
	ScxmlStateMachineLoader : public IScxmlStateMachineLoader
{
	berryObjectMacro(lancet::ScxmlStateMachineLoader)
public:
	ScxmlStateMachineLoader();

	explicit ScxmlStateMachineLoader(const QString& filename, const QString& propertyFileName, 
		IScxmlStateMachineState::StateType type,
		IScxmlStateMachineState::Pointer manage, const QString& id);

	virtual berry::SmartPointer<IScxmlStateMachineState> GetOutput() override;

protected:
	bool GetQtScxmlStateMachineOutput(QSharedPointer<QScxmlStateMachine>&);
	bool OpenSubsidiaryFile(const QString& filename, QString& readall);
	berry::SmartPointer<StateMachineActionProperty> ParseSubsidiaryDescriptionInfo(const QJsonDocument&);
private:
	struct ScxmlStateMachineLoaderPrivateImp;
	std::shared_ptr<ScxmlStateMachineLoaderPrivateImp> imp;
};
}

#endif // !LancetScxmlStateMachineLoader_H
