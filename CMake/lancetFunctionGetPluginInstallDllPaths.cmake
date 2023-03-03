#! 
#! \brief Get the installation path information of Mitk style plug-ins.
#!
#!
#! This method should be called from the plug-in CMakeList. Get the list of 
#! plug-in installation information. For the format, refer to Mitk style, 
#! such as operation(START|INSTALL|STOP|READ): file:///${INSTALL_DIR}/lib${plugin_name}.dll
#!
#! Options:
#!
#! Parameters:
#!
#! \param RUNTIME_PROVISIONING_OPERATION Operation type, It can only be START | 
#! INSTALL | STOP | READ.
#! 
#! \param RUNTIME_PROVISIONING_INSTALL_DIRECTORY Installation directory of 
#! dynamic library.
#!
#! Multi-value parameters (all optional):
#!
#! \param RUNTIME_SUB_PLUGIN_LIST Collection of plug-ins on which the runtime depends.
#! \param RUNTIME_OUT_SUB_PLUGIN_LIST Collection of plug-ins on which the output runtime depends.
#! 
cmake_minimum_required(VERSION 3.18)
function(lancetFunctionGetPluginInstallDllPaths PROPERTY_RUNTIME_PROVISIONING_OPERATION PROPERTY_RUNTIME_PROVISIONING_INSTALL_DIRECTORY PROPERTY_RUNTIME_OUT_SUB_PLUGIN_LIST PROPERTY_RUNTIME_SUB_PLUGIN_LIST)

# ------------------ Input parameter security check ------------------------------
if(NOT PROPERTY_RUNTIME_PROVISIONING_OPERATION)
  message(SEND_ERROR "Not define PROPERTY_RUNTIME_PROVISIONING_OPERATION.")
  return()
endif()
if(NOT PROPERTY_RUNTIME_PROVISIONING_INSTALL_DIRECTORY)
  message(SEND_ERROR "Not define PROPERTY_RUNTIME_PROVISIONING_INSTALL_DIRECTORY.")
  return()
endif()
if(NOT PROPERTY_RUNTIME_OUT_SUB_PLUGIN_LIST)
  message(SEND_ERROR "Not define RUNTIME_OUT_SUB_PLUGIN_LIST.")
  return()
endif()
if(NOT PROPERTY_RUNTIME_SUB_PLUGIN_LIST)
  message("Not define PROPERTY_RUNTIME_PROVISIONING_INSTALL_DIRECTORY or pass in a null value, and the event will be handled by default.")
endif()

# ------------------
set(install_dll_paths "# ----------------------${PROPERTY_RUNTIME_PROVISIONING_OPERATION} list: \n")
set(PROPERTY_RUNTIME_SUB_PLUGIN_LIST_COPY )
if(4 LESS_EQUAL ${ARGC})
  list(SUBLIST ARGV 3 -1 PROPERTY_RUNTIME_SUB_PLUGIN_LIST_COPY)
endif()

foreach(__plugin ${PROPERTY_RUNTIME_SUB_PLUGIN_LIST_COPY})
  string(REPLACE "." "_" __plugin_name ${__plugin})
  set(__plugin_name_dll lib${__plugin_name}.dll)
  list(APPEND install_dll_paths "${PROPERTY_RUNTIME_PROVISIONING_OPERATION} file:///${PROPERTY_RUNTIME_PROVISIONING_INSTALL_DIRECTORY}/${__plugin_name_dll}")
endforeach()

list(LENGTH install_dll_paths list_lenght)
if(NOT list_lenght EQUAL 1)
  list(APPEND install_dll_paths "\n")
endif()
set(PROPERTY_RUNTIME_OUT_SUB_PLUGIN_LIST ${install_dll_paths} PARENT_SCOPE)
endfunction()