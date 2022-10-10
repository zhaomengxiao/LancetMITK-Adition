#include "lancetIStateMachineActionProperty.h"
#include "lancetIScxmlStateMachineState.h"

namespace lancet
{
	struct IStateMachineActionProperty::IStateMachineActionPropertyPrivateImp
	{
		QMap<QString, QVariant> mapKeywords;
	};

	struct IStateMachineActionProperty::IQActionStyleProperty::IQActionStylePropertyPrivateImp
	{
		QMap<QString, IThemeStyle> mapThemes;
	};
	const char* const IStateMachineActionProperty::Keywords::ID = "id";
	const char* const IStateMachineActionProperty::Keywords::OBJECT_NAME = "ObjectName";
	const char* const IStateMachineActionProperty::Keywords::UI_NAME = "UiName";
	const char* const IStateMachineActionProperty::Keywords::EDITOR_ID = "editorWidgetIdentify";
	const char* const IStateMachineActionProperty::Keywords::FUNCTION_ID = "fucntionWidgetIdentify";

	IStateMachineActionProperty::IStateMachineActionProperty()
		: imp(std::make_shared<IStateMachineActionPropertyPrivateImp>())
	{
		
	}

	IStateMachineActionProperty::IStateMachineActionProperty(const QString& id)
		: IStateMachineActionProperty()
	{
		this->SetStateId(id);
	}

	IStateMachineActionProperty::IStateMachineActionProperty(
		const berry::SmartPointer<IScxmlStateMachineState>& state)
		: IStateMachineActionProperty()
	{
		if (state.IsNotNull())
		{
			this->SetStateId(state->GetStateId());
		}
	}

	bool IStateMachineActionProperty::IsValidKeywordKey(const QString& key) const
	{
		return this->imp->mapKeywords.find(key) != this->imp->mapKeywords.end();
	}

	QVariant IStateMachineActionProperty::GetKeywordValue(const QString& key) const
	{
		if (this->IsValidKeywordKey(key))
		{
			return this->imp->mapKeywords[key];
		}
		return QVariant();
	}

	void IStateMachineActionProperty::SetKeywordValue(const QString& key, 
		const QVariant& v)
	{
		this->imp->mapKeywords[key] = v;
	}

	QString IStateMachineActionProperty::GetStateId() const
	{
		return this->GetKeywordValue(Keywords::ID).toString();
	}

	void IStateMachineActionProperty::SetStateId(const QString& state)
	{
		this->SetKeywordValue(Keywords::ID, state);
	}

	QString IStateMachineActionProperty::GetStateObjectName() const
	{
		return this->GetKeywordValue(Keywords::OBJECT_NAME).toString();
	}

	void IStateMachineActionProperty::SetStateObjectName(const QString& name)
	{
		this->SetKeywordValue(Keywords::OBJECT_NAME, name);
	}

	QString IStateMachineActionProperty::GetStateUiName() const
	{
		return this->GetKeywordValue(Keywords::UI_NAME).toString();
	}

	void IStateMachineActionProperty::SetStateUiName(const QString& name)
	{
		this->SetKeywordValue(Keywords::UI_NAME, name);
	}
	IStateMachineActionProperty::IQActionStyleProperty::IQActionStyleProperty()
		: imp(std::make_shared<IQActionStylePropertyPrivateImp>())
	{
	}
	QList<QString> IStateMachineActionProperty::IQActionStyleProperty::GetStyleThemes() const
	{
		return this->imp->mapThemes.keys();
	}
	bool IStateMachineActionProperty::IQActionStyleProperty::HasStyle(const QString& key) const
	{
		return this->imp->mapThemes.find(key) != this->imp->mapThemes.end();
	}
	IStateMachineActionProperty::IQActionStyleProperty::IThemeStyle
		IStateMachineActionProperty::IQActionStyleProperty::GetStyle(const QString& theme) const
	{
		if (this->HasStyle(theme))
		{
			return this->imp->mapThemes[theme];
		}
		return IThemeStyle();
	}
	void IStateMachineActionProperty::IQActionStyleProperty::SetStyle(const QString& theme,
		const IThemeStyle& style)
	{
		this->imp->mapThemes[theme] = style;
	}
}