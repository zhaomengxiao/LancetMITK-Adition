#include "LancetIScxmlStateMachineLoader.h"

namespace lancet
{
	struct IScxmlStateMachineLoader::IScxmlStateMachineLoaderPrivateImp
	{
		bool isError;
		bool isWarning;
		QString fileName;
		QString propertyFileName;
		QString sid;
		QStringList listErrorMessage;
		QStringList listWarningMessage;
		IScxmlStateMachineState::StateType stateType = IScxmlStateMachineState::StateType::Ordinary;
		IScxmlStateMachineState::Pointer manageStateMachine = IScxmlStateMachineState::Pointer(nullptr);
	};

	IScxmlStateMachineLoader::IScxmlStateMachineLoader()
		: IScxmlStateMachineLoader("empty", "empty", IScxmlStateMachineState::Admin,
			IScxmlStateMachineState::Pointer(),"empty")
	{
	}

	IScxmlStateMachineLoader::IScxmlStateMachineLoader(const QString& filename,
		const QString& propertyFileName, IScxmlStateMachineState::StateType type, 
		IScxmlStateMachineState::Pointer manage, const QString& id)
		: imp(std::make_shared<IScxmlStateMachineLoaderPrivateImp>())
	{
		this->SetInput(filename, propertyFileName, type, manage, id);
	}

	void IScxmlStateMachineLoader::SetInput(const QString& filename, const QString& propertyFileName,
		IScxmlStateMachineState::StateType type, IScxmlStateMachineState::Pointer manage,
		const QString& id)
	{
		this->SetStateId(id);
		this->SetFileName(filename);
		this->SetStateType(type);
		this->SetManageStateMachine(manage);
		this->SetPropertyFileName(propertyFileName);
	}

	QString IScxmlStateMachineLoader::GetFileName() const
	{
		return this->imp->fileName;
	}

	void IScxmlStateMachineLoader::SetFileName(const QString& file)
	{
		this->imp->fileName = file;
	}

	QString IScxmlStateMachineLoader::GetPropertyFileName() const
	{
		return this->imp->propertyFileName;
	}

	void IScxmlStateMachineLoader::SetPropertyFileName(const QString& file) const
	{
		this->imp->propertyFileName = file;
	}

	IScxmlStateMachineState::StateType IScxmlStateMachineLoader::GetStateType() const
	{
		return this->imp->stateType;
	}

	void IScxmlStateMachineLoader::SetStateType(const IScxmlStateMachineState::StateType& type)
	{
		this->imp->stateType = type;
	}

	IScxmlStateMachineState::Pointer IScxmlStateMachineLoader::GetManageStateMachine() const
	{
		return this->imp->manageStateMachine;
	}

	void IScxmlStateMachineLoader::SetManageStateMachine(IScxmlStateMachineState::Pointer manage)
	{
		this->imp->manageStateMachine = manage;
	}

	QString IScxmlStateMachineLoader::GetStateId() const
	{
		return this->imp->sid;
	}

	void IScxmlStateMachineLoader::SetStateId(const QString& id)
	{
		this->imp->sid = id;
	}

	bool IScxmlStateMachineLoader::GetErrorState() const
	{
		return this->imp->isError;
	}

	bool IScxmlStateMachineLoader::GetWarningState() const
	{
		return this->imp->isWarning;
	}

	QStringList IScxmlStateMachineLoader::GetErrorStringList() const
	{
		return this->imp->listErrorMessage;
	}

	QStringList IScxmlStateMachineLoader::GetWarningStringList() const
	{
		return this->imp->listWarningMessage;
	}

	void IScxmlStateMachineLoader::SetErrorState(bool is)
	{
		this->imp->isError = is;
	}

	void IScxmlStateMachineLoader::SetErrorStringList(const QStringList& list)
	{
		this->imp->listErrorMessage = list;
	}

	void IScxmlStateMachineLoader::AppendErrorStringItem(const QString& message)
	{
		this->imp->listErrorMessage.push_back(message);
	}

	void IScxmlStateMachineLoader::RemoveErrorStringItem()
	{
		this->imp->listErrorMessage.clear();
	}

	void IScxmlStateMachineLoader::SetWarningState(bool warning)
	{
		this->imp->isWarning = warning;
	}

	void IScxmlStateMachineLoader::SetWarningStringList(const QStringList& list)
	{
		this->imp->listWarningMessage = list;
	}

	void IScxmlStateMachineLoader::AppendWarningStringItem(const QString& message)
	{
		this->imp->listWarningMessage.push_back(message);
	}

	void IScxmlStateMachineLoader::RemoveWarningStringItem()
	{
		this->imp->listWarningMessage.clear();
	}

}