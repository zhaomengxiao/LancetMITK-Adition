#ifndef MainPage_H
#define MainPage_H

/**
 * \defgroup org_mitk_lancet_statemachine_services
 *
 * \brief State Machine Service Module Data Model.
 *
 * The state machine service model has very high compatibility and extensibility.
 * Relatively speaking, there will be certain sacrifices in code understanding
 * and readability.
 * 
 * At the design stage, according to the positioning and requirements of the
 * module, the module has a high degree of scalability and configurability, that
 * is, the user configures different state machine <i>.scxml</i> files according
 * to different business needs, instead of rewriting the code of the state machine
 * module.
 * 
 * \section Description
 * 
 * <b>Structural design</b>
 * 
 * The state machine service module is composed of service, state, loader and 
 * handler elements, and follows the guidelines of boundary and singleness.If 
 * you need to extend it, please follow the style below. 
 * In theory, the state machine service can manage the state of a button.
 * 
 * State level diagram:
 * \image html StatesSketchMap.png
 * 
 * Class Diagram:
 * \image html StateMachineClass.png
 * 
 * \c Service: 
 * 
 * It is the only access to the communication between the inner boundary and the 
 * outside world. Because of the positioning of this module, the attributes of the 
 * inner boundary are basically transparent to the outside world. At the same time, 
 * the inner boundary completely ignores how the outside world operates, how to 
 * operate and other behaviors, and it is free to play by the outside world.
 * 
 * \c State:
 * 
 * It is the main role of the internal bound state machine management. It not only 
 * describes its own attributes in detail, but also has a close relationship with the 
 * above and the following. To sum up, the data structure of the state is a two-way 
 * linked list to contact the context, and uses its own attributes to strengthen the 
 * context association.
 * 
 * \c Loader:
 * 
 * The loader is responsible for parsing external parameters and converting them into 
 * universally recognizable data objects (<I>#IScxmlStateMachineState</I>) with 
 * internal bounds. The loader is driven by the service.The format of external files 
 * is explained in <b>Document structure design</b>.
 * 
 * \c Handler:
 * 
 * The state machine event processing role is responsible for processing the state machine 
 * events or monitoring the state of the state machine. When there are changes, it will 
 * feed back in the form of <I>Qt signals</I>.
 * 
 * <b>Document structure design</b>
 * 
 * External files consist of <I>state machine(.scxml)</I>, <I>state attribute(_subsidiary.json)</I>, 
 * and <I>state style file(*.qss)</I>.
 * 
 * <b>.scxml</b>:
 * 
 * An XML form that describes the transition process of the whole state. One project in 
 * <I>Qt labs</I> is <I>QScxml</I>, which is based on the upper layer of <I>QStateMachine</I>. 
 * It can directly read files in SCXML format to generate internal state objects and members, 
 * and can directly perform state transition in Qt, which is very convenient. You may refer to 
 * <I>Qt's SCXML</I> module.
 * 
 * \warning 
 * There are no explicit naming rules for background processing states, such as initialization 
 * and termination. However, if you need to map the state to the interface directly or briefly, 
 * please follow the naming style of the <I>mitk plug-in</I>. eg.<b>org_mitk_lancet_%*%</b>
 * 
 * <b>_subsidiary.json</b>:
 * 
 * The external file describing the status attribute is appended after the naming is 
 * restricted to the target status <B>_subsidiary</B>, which is mandatory. Otherwise, 
 * the state machine service module will not be able to effectively recognize or parse 
 * its contents. The final interpretation right of the elements in the file is left to 
 * the <I>Loader</I>.
 * 
 * \note 
 * If there is a new service access, you need to extend the configuration of the status 
 * attribute. Maybe you need a new <I>loader</I> to meet your needs.
 * 
 * <b>.qss</b>:
 * Qt layout style explanation file, which is to meet the frequent changes in the interface 
 * style. If you are interested in this, you may need to know the Qt QSS style.
 * 
 * <b>Why this design</b>
 * 
 * This design style may be too cumbersome, but the core idea of the main framework is 
 * high scalability, flexibility and other features. It may be a little far fetched to 
 * force the <I>UI</I> attributes here, but when the minimum UI attributes are placed 
 * here, the state mapping at all levels of the interface will be based on the state 
 * machine service. Imagine the following, with other business requirements involved, 
 * Maybe we can complete the interface mapping of the interface state without writing 
 * the code of the state machine and Ui. This should be the idea we follow. Extreme 
 * laziness is the best.
 * 
 * <b>How to use</b>
 * 
 * Scxml sample file:
 * 
 * The Qt SCXML module provides functionality to create state machines from SCXML 
 * files. This includes both dynamically creating state machines (loading the SCXML 
 * file and instantiating states and transitions) and generating a C++ file that has 
 * a class implementing the state machine. It also contains functionality to support 
 * data models and executable content.
 * 
 * \code
 * <?xml version="1.0" encoding="UTF-8"?>
 * <!--
 * 	Hip software flow status machine definition
 * 	-Initialization
 * 	-MedicalrecordManagement
 * 	-StDevelopmentSurgicalProtocol
 * 	-HardwarePreparation
 * 	-SurgicalRegistration
 * 	-AcetabularPreparation
 * 	-FemurPreparation
 * 	-SurgicalOutcomes
 *
 * 	Rules for state name definition <Process editor plugin name> (all lower case letters)
 * 	.eg <org_mitk_lancet_tha_medicalrecordmanagement>.
 *
 * 	Jump event name definition rules: to_<Process editor plugin name> (all lower case letters)
 * 	.eg to_org_mitk_lancet_tha_medicalrecordmanagement.
 * -->
 * <scxml
 * 	xmlns="http://www.w3.org/2005/07/scxml"
 * 	version="1.0"
 * 	binding="early"
 * 	xmlns:qt="http://www.qt.io/2015/02/scxml-ext"
 * 	datamodel="ecmascript"
 * 	name="LancetThaScxmlStateMachineDebug"
 * 	qt:editorversion="4.13.1"
 * 	initial="initialization"
 * >
 * 	<!--
 * 		name: initialization
 * 		Sub processes Scxml files: initialization.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 	-->
 * 	<state id="initialization">
 * 		<transition type="internal" event="to_applicationquit" target="applicationquit"/>
 * 		<transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 * 	</state>
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_medicalrecordmanagement
 * 		Sub processes Scxml files: medicalrecordmanagement.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 *     <state id="org_mitk_lancet_tha_medicalrecordmanagement">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_femurpreparation" target="org_mitk_lancet_tha_femurpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 		Sub processes Scxml files: medicalrecordmanagement.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 *     <state id="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_femurpreparation" target="org_mitk_lancet_tha_femurpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_linkinghardware
 * 		Sub processes Scxml files: linkinghardware.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 * 	<state id="org_mitk_lancet_tha_linkinghardware">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_femurpreparation" target="org_mitk_lancet_tha_femurpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_surgicalregistration
 * 		Sub processes Scxml files: surgicalregistration.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 * 	<state id="org_mitk_lancet_tha_surgicalregistration">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_femurpreparation" target="org_mitk_lancet_tha_femurpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_acetabularpreparation
 * 		Sub processes Scxml files: acetabularpreparation.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 * 	<state id="org_mitk_lancet_tha_acetabularpreparation">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_femurpreparation" target="org_mitk_lancet_tha_femurpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_femurpreparation
 * 		Sub processes Scxml files: femurpreparation.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_surgicaloutcomes							org_mitk_lancet_tha_surgicaloutcomes
 * 	-->
 * 	<state id="org_mitk_lancet_tha_femurpreparation">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 * 	<!--
 * 		name: org_mitk_lancet_tha_surgicaloutcomes
 * 		Sub processes Scxml files: surgicaloutcomes.scxml
 * 		events:
 * 			event name														target state
 * 			to_applicationquit												applicationquit
 * 			to_org_mitk_lancet_tha_medicalrecordmanagement					org_mitk_lancet_tha_medicalrecordmanagement
 * 			to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol			org_mitk_lancet_tha_stdevelopmentsurgicalprotocol
 * 			to_org_mitk_lancet_tha_linkinghardware							org_mitk_lancet_tha_linkinghardware
 * 			to_org_mitk_lancet_tha_surgicalregistration						org_mitk_lancet_tha_surgicalregistration
 * 			to_org_mitk_lancet_tha_acetabularpreparation					org_mitk_lancet_tha_acetabularpreparation
 * 			to_org_mitk_lancet_tha_femurpreparation							org_mitk_lancet_tha_femurpreparation
 * 	-->
 * 	<state id="org_mitk_lancet_tha_surgicaloutcomes">
 *         <transition type="internal" event="to_applicationquit" target="applicationquit"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_medicalrecordmanagement" target="org_mitk_lancet_tha_medicalrecordmanagement"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_stdevelopmentsurgicalprotocol" target="org_mitk_lancet_tha_stdevelopmentsurgicalprotocol"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_linkinghardware" target="org_mitk_lancet_tha_linkinghardware"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicalregistration" target="org_mitk_lancet_tha_surgicalregistration"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_acetabularpreparation" target="org_mitk_lancet_tha_acetabularpreparation"/>
 *         <transition type="internal" event="to_org_mitk_lancet_tha_surgicaloutcomes" target="org_mitk_lancet_tha_surgicaloutcomes"/>
 *     </state>
 *
 *     <final id="applicationquit">
 *
 *     </final>
 * </scxml>
 * \endcode
 * 
 * Status Properties Sample File:
 * 
 * The attributes that describe the state (unit) in detail, including data structure, 
 * interface style, etc.
 * 
 * \code
 * {
 * 	"properties":
 * 	{
 * 		"id": "org_mitk_lancet_tka_medicalrecordmanagement",
 * 		"objectName": "ActionMedicalRecordManagement",
 * 		"uiName": "MedicalRecordManagement",
 * 		"theme":
 * 		{
 * 			"defaultStyle":
 * 			{
 * 				"actionQssFilePath": ":/resources/qss/tha/medicalrecordmanagement/action_medicalrecordmanagement_default.qss",
 * 				"editorQssFilePath": ":/resources/qss/tha/medicalrecordmanagement/editor_medicalrecordmanagement_default.qss",
 * 				"viewQssFilePath": ":/resources/qss/tha/medicalrecordmanagement/view_medicalrecordmanagement_default.qss"
 * 			}
 * 		}
 * 	}
 * }
 * \endcode
 * 
 * Qt Qss Sample File:
 * 
 * Formulate the display style of Qt components. Refer to Qt API document for 
 * detailed usage.
 * 
 * \code
 * *
 * {
 * 	color: beige;
 * }
 *
 * QLabel, QAbstractButton
 * {
 *     font: bold;
 * 	color: yellow;
 * }
 *
 * QFrame
 * {
 *     background-color: rgba(96,96,255,60%);
 *     border-color: rgb(32,32,196);
 *     border-width: 3px;
 *     border-style: solid;
 *     border-radius: 5;
 *     padding: 3px;
 * }
 *
 * QAbstractButton
 * {
 *     background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
 *                              stop:0 lightblue, stop:0.5 darkblue);
 *     border-width: 3px;
 *     border-color: darkblue;
 *     border-style: solid;
 *     border-radius: 5;
 *     padding: 3px;
 * }
 *
 * QAbstractButton:pressed
 * {
 *     background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
 *                              stop:0.5 darkblue, stop:1 lightblue);
 *     border-color: beige;
 * }
 * \endcode
 * 
 * 
 * Import External Files:
 * 
 * You should have <I>.scxml</I>, <I>_subsidiary.json</I>, <I>.qss</I> files before 
 * importing external files.Then you need to use the service import interface to 
 * parse the external file into a valid state machine service element.
 * 
 * \code
 * auto context = org_mitk_lancet_tha_application_Activator::GetPluginContext();
 * auto stateMachineServiceRef = context->getServiceReference<lancet::IScxmlStateMachineService>();
 * auto stateMachineService = context->getService<lancet::IScxmlStateMachineService>(stateMachineServiceRef);
 * // F:/Examples/ExampleQtScxmlStateMachine/LancetThaScxmlStateMachine_debug.scxml
 * if (nullptr != stateMachineService)
 * {
 * 	//qRegisterMetaType<lancet::IScxmlStateMachineState>("lancet::IScxmlStateMachineState");
 * 	stateMachineService->Initialize();
 * 	stateMachineService->CreateScxmlStateMachine(
 * 		"Y:/LancetMitk_SBD/MITK-build/bin/resources/org_mitk_lancet_tha_application.scxml",
 * 		"empty",
 * 		lancet::IScxmlStateMachineState::StateType::Admin,
 * 		lancet::IScxmlStateMachineState::Pointer(nullptr), "org_mitk_lancet_tha_application");
 *
 * 	std::cout << stateMachineService->GetAdminState()->ToString().toStdString();
 * }
 * \endcode
 *
 * How to map to interface:
 * 
 * Before operating this behavior, you should and must import an external file or 
 * create a state machine element. After that, you can traverse the state machine 
 * service element to generate a formatted interface. The style of the interface 
 * can be configured through the properties of the state element.
 * 
 * \code
 * auto context = org_mitk_lancet_tha_application_Activator::GetPluginContext();
 * auto stateMachineServiceRef = context->getServiceReference<lancet::IScxmlStateMachineService>();
 * auto stateMachineService = context->getService<lancet::IScxmlStateMachineService>(stateMachineServiceRef);
 * if (stateMachineService != nullptr && stateMachineService->GetAdminState().IsNotNull())
 * {
 * 	for (auto item : stateMachineService->GetAdminState()->GetSubStateMachines())
 * 	{
 * 		// proprety
 * 		//QString itemActionId = item->GetStateId();
 * 		QString itemActionUiName = item->GetActionProperty()->GetStateUiName();
 * 		QString itemActionObjectName = item->GetActionProperty()->GetStateObjectName();
 *
 * 		// qss
 * 		QString itemActionQSS = item->GetActionProperty()->GetActionQss();
 *
 * 		auto action = menuBar->addAction(itemActionUiName);
 * 		action->setObjectName(itemActionObjectName);
 *
 * 		// You may customize the interface status trigger unit components.
 * 		// Widget->setStyleSheet(itemActionQSS);
 *		connect(action, &QAction::triggered, this, &MyWidget::onStateMachine_ActionClicked);
 * 	}
 * }
 * \endcode
 * 
 * How to focus on events:
 * 
 * tate machine events provide the most basic state jump events. Of course, this 
 * module can realize more than these businesses. Other additional functions require 
 * selfless contributions from contributors.
 * 
 * \code
 * // connect to state machine for Qt.
 * auto adminStateHandle = stateMachineService->GetAdminState()->GetStateMachineHandler();
 * connect(adminStateHandle.GetPointer(), SIGNAL(StateEnter(IScxmlStateMachineState*)),
 * 	this, SLOT(onModuleStateMachineStateEnter(IScxmlStateMachineState*)));
 * connect(adminStateHandle.GetPointer(), SIGNAL(StateExit(IScxmlStateMachineState*)),
 * 	this, SLOT(onModuleStateMachineStateExit(IScxmlStateMachineState*)));
 * stateMachineService->StartStateMachine();
 * \endcode
 * 
 */

#endif // !MainPage_H