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
	 * \note Keyword List
	 *		-# id: Unique ID identification code of the status.
	 *		-# objectName: Object name of interface mapping object.
	 *		-# uiName: Text information of interface mapping object.
	 *		-# editorWidgetIdentify: Editor plug-in ID ID code of the status.
	 *		-# fucntionWidgetIdentify: Status feature plug-in ID identifier.
	 *		-# themes: Style style sheet for status.
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

		struct LANCET_STATEMACHINE_SERVICES_PLUGIN Keywords
		{
			static const char* const ID;
			static const char* const OBJECT_NAME;
			static const char* const UI_NAME;
			static const char* const EDITOR_ID;
			static const char* const FUNCTION_ID;
		};

		class LANCET_STATEMACHINE_SERVICES_PLUGIN IQActionStyleProperty
			: public berry::Object
		{
			berryObjectMacro(lancet::IStateMachineActionProperty::IQActionStyleProperty);
		public:
			IQActionStyleProperty();
			struct LANCET_STATEMACHINE_SERVICES_PLUGIN IThemeStyle
			{
				QString editorWidgetQssFilePath;
				QString fucntionWidgetQssFilePath;
			};

			virtual QList<QString> GetStyleThemes() const;

			virtual bool HasStyle(const QString&) const;
			virtual IThemeStyle GetStyle(const QString& theme) const;
			virtual void SetStyle(const QString&, const IThemeStyle&);
		private:
			struct IQActionStylePropertyPrivateImp;
			std::shared_ptr<IQActionStylePropertyPrivateImp> imp;
		};
	public:
		virtual bool IsValidKeywordKey(const QString&) const;
		virtual QVariant GetKeywordValue(const QString&) const;
		virtual void SetKeywordValue(const QString& ,const QVariant&);

		virtual QString GetStateId() const;
		virtual void SetStateId(const QString&);

		virtual QString GetStateObjectName() const;
		virtual void SetStateObjectName(const QString&);

		virtual QString GetStateUiName() const;
		virtual void SetStateUiName(const QString&);

		virtual QString GetStateEditorWidgetIdentify() const;
		virtual void SetStateEditorWidgetIdentify(const QString&);

		virtual QString GetStateFucntionWidgetIdentify() const;
		virtual void SetStateFucntionWidgetIdentify(const QString&);

		virtual berry::SmartPointer<IQActionStyleProperty> GetIQActionStyleProperty() const;
		virtual void SetIQActionStyleProperty(const berry::SmartPointer<IQActionStyleProperty>&);
	protected:
		virtual void Initialize() = 0;
	private:
		struct IStateMachineActionPropertyPrivateImp;
		std::shared_ptr<IStateMachineActionPropertyPrivateImp> imp;
	};
}

#define LancetIStateMachineActionProperty_H
#endif // !LancetIStateMachineActionProperty_H
