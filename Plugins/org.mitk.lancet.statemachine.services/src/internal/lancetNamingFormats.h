#ifndef LancetNamingFormats_H
#define LancetNamingFormats_H

#include <QString>

#include "org_mitk_lancet_statemachine_services_Export.h"

/**
 * \namespace lancet
 * 
 * \brief Scope of Lancet Robot Co., Ltd.
 */
namespace lancet
{
	namespace plugin
	{
		/**
		 * \brief Change the state machine state ID to the plug-in ID in Mitk format.
		 *
		 * The style of Qt state machine state ID is underlined as a separator.
		 * eg. org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
		 * 
		 * The style of Mitk plug-in ID is to use the dot as the separator.
		 * eg. org.mitk.lancet.tha.stdevelopmentsurgicalprotocol
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 * \param qtStateId
		 *              Qt style state machine ID.
		 * \return Mitk plug-in ID.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN ToMitkId(const QString& qtStateId);

		/**
		 * \brief Returns the name of a Mitk style plug-in.
		 * 
		 * eg. Enter org_mitk_lancet_tha_stdevelopmentsurgicalprotocol, Output 
		 * stdevelopmentsurgicalprotocol
		 * 
		 * \ingroup org_mitk_lancet_statemachine_services
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetMitkPluginName(const QString&);

		/**
		 * \brief Returns the product ID of the Mitk plug-in.
		 *
		 * The plug-in ID format is org_ mitk_%{ProductID}_%{Item ID}_%{plug-in name}.
		 * 
		 * \ingroup org_mitk_lancet_statemachine_services
		 * 
		 * \return %{ProductID}.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetMitkPluginProductName(const QString&);

		/**
		 * \brief Returns whether the target plug-in ID conforms to the format of the Lancet.
		 *
		 * The plug-in ID format is org_ mitk_%{ProductID}_%{Item ID}_%{plug-in name}.
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 */
		bool LANCET_STATEMACHINE_SERVICES_PLUGIN IsLancetModuleFormat(const QString&);
	}

	namespace state_machine
	{
		/**
		 * \brief Returns the state name of the state machine ID.
		 *
		 * The plug-in ID format is org_ mitk_%{ProductID}_%{Item ID}_%{plug-in name}.
		 *
		 * \param qtStateId
		 *              Qt style state machine ID.
		 * 
		 * \ingroup org_mitk_lancet_statemachine_services
		 *
		 * \return %{plug-in name}.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetStateName(const QString&);

		/**
		 * \brief Returns the state module name of the state machine ID.
		 *
		 * The plug-in ID format is org_ mitk_%{ProductID}_%{Item ID}_%{plug-in name}.
		 *
		 * \param qtStateId
		 *              Qt style state machine ID.
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 *
		 * \return %{plug-in name}.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetModuleStateName(const QString&);

		/**
		 * \brief The path of the Qt state machine configuration file that returns the 
		 * state machine ID.
		 *
		 * Path is absolute.
		 *
		 * \param parentScxmlFile
		 *              SCXML file path of parent assembly.
		 * \param StateId
		 *              Qt style state machine ID.
		 * \param type
		 *              State type.
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 *
		 * \return SCXML file absolute path for status.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetStateScxmlFileName(const QString&,
			const QString&, int);

		/**
		 * \brief The directory of the Qt state machine configuration file that returns the
		 * state machine ID.
		 *
		 * Path is absolute.
		 *
		 * \param parentScxmlFile
		 *              SCXML file path of parent assembly.
		 * \param StateId
		 *              Qt style state machine ID.
		 * \param type
		 *              State type.
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 *
		 * \return SCXML file absolute directory for status.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetStateScxmlFolderName(const QString&, 
			const QString&, int);

		/**
		 * \brief The absolute path of the Qt state machine property configuration file 
		 * that returns the state machine ID.
		 *
		 * Path is absolute.
		 *
		 * \param parentScxmlFile
		 *              SCXML file path of parent assembly.
		 * \param StateId
		 *              Qt style state machine ID.
		 * \param type
		 *              State type.
		 * \param format
		 *              Suffix format of property configuration file.
		 *
		 * \ingroup org_mitk_lancet_statemachine_services
		 *
		 * \return The absolute path of the property configuration file.
		 */
		QString LANCET_STATEMACHINE_SERVICES_PLUGIN GetStatePropertyFileName(const QString&, 
			const QString&, int, const QString& = "_subsidiary.json");
	}
}

#endif // !LancetNamingFormats_H
