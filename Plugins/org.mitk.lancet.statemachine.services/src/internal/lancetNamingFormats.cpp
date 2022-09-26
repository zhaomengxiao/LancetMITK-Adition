#include "lancetNamingFormats.h"

#include <QDir>
#include <QRegExp>
#include <QStringList>

#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	QString plugin::ToMitkId(const QString& mitkPluginId)
	{
		return QString(mitkPluginId).replace("_", ".");
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		plugin::GetMitkPluginName(const QString& mitkPluginId)
	{
		return mitkPluginId.split(QRegExp("(\\.|_)")).back();
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		plugin::GetMitkPluginProductName(const QString& mitkPluginId)
	{
		QStringList listSplitString = mitkPluginId.split(QRegExp("(\\.|_)"));
		return listSplitString.size() > 4 ? listSplitString[3]: "empty";
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		state_machine::GetStateName(const QString& stateId)
	{
		return plugin::GetMitkPluginName(stateId);
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		state_machine::GetModuleStateName(const QString& stateId)
	{
		return GetStateName(stateId);
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		state_machine::GetStateScxmlFileName(const QString& parentFileName,
			const QString& stateId, int type)
	{		
		return QDir(GetStateScxmlFolderName(parentFileName, stateId, type))
			.absoluteFilePath(QString("%1.scxml").arg(GetStateName(stateId)));
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		state_machine::GetStateScxmlFolderName(const QString& parentFileName,
		const QString& stateId, int type)
	{
		QDir dir(parentFileName.left(parentFileName.lastIndexOf("/")));

		// append ${product_id}
		if (type == IScxmlStateMachineState::StateType::Module)
		{
			dir.setPath(dir.absoluteFilePath(plugin::GetMitkPluginProductName(stateId)));
		}
		return dir.absoluteFilePath(GetStateName(stateId));
	}

	QString LANCET_STATEMACHINE_SERVICES_PLUGIN 
		state_machine::GetStatePropertyFileName(const QString& parentFileName,
		const QString& stateId, int type, const QString& format)
	{
		return QDir(GetStateScxmlFolderName(parentFileName, stateId, type))
			.absoluteFilePath(QString("%1%2").arg(GetStateName(stateId)).arg(format));
	}

}

