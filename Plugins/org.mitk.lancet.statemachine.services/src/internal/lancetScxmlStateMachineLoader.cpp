#include "lancetScxmlStateMachineLoader.h"
#include "lancetScxmStateMachineState.h"
#include "lancetStateMachineActionProperty.h"

// Qt
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QScxmlStateMachine>

namespace lancet
{
	struct ScxmlStateMachineLoader::ScxmlStateMachineLoaderPrivateImp
	{
		QJsonDocument jsonDoc;
		QJsonParseError jsonError;
	};

	ScxmlStateMachineLoader::ScxmlStateMachineLoader()
		: IScxmlStateMachineLoader("empty", "empty", IScxmlStateMachineState::Ordinary, 
			IScxmlStateMachineState::Pointer(nullptr), "empty")
		, imp(std::make_shared<ScxmlStateMachineLoaderPrivateImp>())
	{
	}

	ScxmlStateMachineLoader::ScxmlStateMachineLoader(const QString& filename, const QString& propertyFileName,
		IScxmlStateMachineState::StateType type,
		IScxmlStateMachineState::Pointer manage, const QString& id)
		: IScxmlStateMachineLoader(filename, propertyFileName, type, manage, id)
		, imp(std::make_shared<ScxmlStateMachineLoaderPrivateImp>())
	{

	}

	berry::SmartPointer<IScxmlStateMachineState> ScxmlStateMachineLoader::GetOutput()
	{
		QString readall = QString("empty");
		berry::SmartPointer<IScxmlStateMachineState> retval_state =
			berry::SmartPointer<ScxmlStateMachineState>(new ScxmlStateMachineState(this->GetManageStateMachine(),
				this->GetStateType(), this->GetStateId()));

		auto scxmlStateMachine  = QSharedPointer<QScxmlStateMachine>();

		// If loading the. scxml file fails, exit the stack of the current function.
		if (false == this->GetQtScxmlStateMachineOutput(scxmlStateMachine))
		{
			// skipping the attribute resolution phase.
			goto QUIT_FUNCTION;
		}
		retval_state->SetQtStateMachine(scxmlStateMachine);
		retval_state->SetSubStateMachineScxmlAbsoluteFile(this->GetFileName());

		// Other ancillary information of resolution status.
		// format for ${ScxmlStateMachineState::GetStateId()}_subsidiary.json
		// eg. org_mitk_lancet_tka_medicalrecordmanagement_subsidiary.json
		if (!this->OpenSubsidiaryFile(this->GetPropertyFileName(), readall))
		{
			//skipping the property resolution phase.
			goto QUIT_FUNCTION;
		}
		
		// Check .json format.
		this->imp->jsonDoc = QJsonDocument::fromJson(readall.toUtf8(), &this->imp->jsonError);
		if (this->imp->jsonError.error != QJsonParseError::NoError)
		{
			// .json file format parsing error,skipping the property resolution phase.
			this->SetWarningState(true);
			this->AppendWarningStringItem("[loading-json-file] format error " + this->imp->jsonError.errorString());
		}
		retval_state->SetActionProperty(this->ParseSubsidiaryDescriptionInfo(this->imp->jsonDoc));

		QUIT_FUNCTION:
		return retval_state;
	}

	bool ScxmlStateMachineLoader::GetQtScxmlStateMachineOutput(QSharedPointer<QScxmlStateMachine>& pointer)
	{
		if (false == QFile::exists(this->GetFileName()))
		{
			this->SetErrorState(true);
			this->AppendErrorStringItem("[loading-open-scxml-file] Not find file " + this->GetFileName());
			return false;
		}

		pointer =
			QSharedPointer<QScxmlStateMachine>(QScxmlStateMachine::fromFile(this->GetFileName()));

		// Sync. scxml file load error information.
		this->SetErrorState(pointer.isNull() || false == pointer->parseErrors().isEmpty());

		for (auto err_loading_item : pointer->parseErrors())
		{
			this->AppendErrorStringItem("[loading-open-scxml-file] " + err_loading_item.toString());
		}

		return !this->GetErrorState();
	}

	bool ScxmlStateMachineLoader::OpenSubsidiaryFile(const QString& filename, QString& readall)
	{
		QString subsidiaryFile = filename;

		// Check whether the file exists and the disk.
		if (false == QFile::exists(subsidiaryFile))
		{
			// The target attribute description file cannot be found
			this->SetWarningState(true);
			this->AppendWarningStringItem("[loading-open-json-file] Target file not found: " + subsidiaryFile);
			return false;
		}

		QFile fp(subsidiaryFile);
		if (false == fp.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			// Failed to open the target property description file
			this->SetWarningState(true);
			this->AppendWarningStringItem("[loading-open-json-file] Fail to open file: " + subsidiaryFile);
			this->AppendWarningStringItem("[loading-open-json-file] Detailed error information: " + fp.errorString());
			fp.close();
			return false;
		}
		readall = fp.readAll();

		this->SetWarningState(true);
		this->AppendWarningStringItem("[loading-open-json-file] file found: " + subsidiaryFile);
		return true;
	}

	berry::SmartPointer<StateMachineActionProperty> 
		ScxmlStateMachineLoader::ParseSubsidiaryDescriptionInfo(const QJsonDocument& qjsonDoc)
	{
		berry::SmartPointer<StateMachineActionProperty> retval_property(new StateMachineActionProperty());

		if (qjsonDoc.isNull() || qjsonDoc.isEmpty())
		{
			this->SetWarningState(true);
			this->AppendWarningStringItem("[laoding-property-file] Enter an empty JSON format object.");
			return berry::SmartPointer<StateMachineActionProperty>();
		}

		auto jsonRootObject = qjsonDoc.object().value("properties");
		//auto jsonRootObject = qjsonDoc.object()["properties"];
		if (jsonRootObject.isNull() || false == jsonRootObject.isObject())
		{
			this->SetWarningState(true);
			this->AppendWarningStringItem("[laoding-property-file] File format error, skip status attribute loading.");
			return retval_property;
		}

		retval_property->SetStateId(jsonRootObject.toObject()["id"].toString());
		retval_property->SetStateObjectName(jsonRootObject.toObject()["objectName"].toString());
		retval_property->SetStateUiName(jsonRootObject.toObject()["uiName"].toString());

		return retval_property;
	}

}