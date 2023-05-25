include(${MITK_EXTENSION_DIRS}/CMake/lancetFunctionCreateMitkProvisioning.cmake)

set(arg_read_plugins_dll 
  
)
set(arg_start_plugins_dll 
  org.mitk.lancet.statemachine.services
  org.mitk.lancet.services.medicalrecords
  org.mitk.lancet.services.spatialfitting
  org.mitk.lancet.services.spatialfitting.ex
  org.mitk.lancet.services.devicesadministration
  org.mitk.lancet.tha.application
  org.mitk.lancet.medicalrecordmanagement
  org.mitk.lancet.medicalrecordmanagement.editor
  org.mitk.lancet.linkinghardware
  org.mitk.lancet.linkinghardware.editor
  org.mitk.lancet.multirenderwidget.editor
  org.mitk.lancet.qctmark
  org.mitk.lancet.implantplanning
  org.mitk.lancet.disclaimeragreement
  org.mitk.lancet.roboticsregistrations
  org.mitk.lancet.roboticsregistrations.editor
  org.mitk.lancet.roboticsregistrationsaccuracy
  org.mitk.lancet.roboticsregistrationsaccuracy.editor
  org.mitk.lancet.femurmarkerpoint
  org.mitk.lancet.femurmarkerpoint.editor
  org.mitk.lancet.femurroughregistrations
  org.mitk.lancet.femurroughregistrations.editor
  org.mitk.lancet.femurprecisionregistrations
  org.mitk.lancet.femurprecisionregistrations.editor
  org.mitk.lancet.pelvismarkerpoint
  org.mitk.lancet.pelvismarkerpoint.editor
  org.mitk.lancet.pelvisprecisionregistrations
  org.mitk.lancet.pelvisprecisionregistrations.editor
  org.mitk.lancet.pelvisroughregistrations
  org.mitk.lancet.pelvisroughregistrations.editor
  org.mitk.lancet.contusion
  org.mitk.lancet.contusion.editor
  org.mitk.lancet.contusionaccuracy
  org.mitk.lancet.contusionaccuracy.editor
  org.mitk.lancet.putmolarcupprosthesisaccuracy
  org.mitk.lancet.putmolarcupprosthesisaccuracy.editor
  org.mitk.lancet.putmolarcupprosthesis
  org.mitk.lancet.putmolarcupprosthesis.editor
  org.mitk.lancet.femurcontusion
  org.mitk.lancet.femurcontusion.editor
  org.mitk.lancet.putfemurprosthesisaccuracy
  org.mitk.lancet.putfemurprosthesisaccuracy.editor
  org.mitk.lancet.postoperativeoutcome
)
set(arg_install_plugins_dll 
  
)
set(arg_stop_plugins_dll 
  
)

lancetFunctionCreateMitkProvisioning(
  RUNTIME_PROVISIONING_PUBLIC_PLUGIN_MAKE
  RUNTIME_PROVISIONING_NAME LancetLauncher_THA # LancetLauncher_THA.provisioning
  RUNTIME_PROVISIONING_SOURCE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
  RUNTIME_PROVISIONING_TARGET_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  RUNTIME_READ_SUB_PLUGIN_LIST ${arg_read_plugins_dll}
  RUNTIME_START_SUB_PLUGIN_LIST ${arg_start_plugins_dll}
  RUNTIME_INSTALL_SUB_PLUGIN_LIST ${arg_install_plugins_dll}
  RUNTIME_STOP_SUB_PLUGIN_LIST ${arg_stop_plugins_dll}
)