#! 
#! \brief Create a Qt style unit test project.
#!
#! 
#!
#!
#! This function should be called from the test project CMakeLists. After the macro 
#！call, the target group name can be used as ${TESTTING_TARGET_GROUP_NAME} to add 
#！other libraries in CMakeLists.txt. Include paths and links.
#!
#! Options:
#!
#! \param PLUGIN_UNIT_TESTTING Mark the project as a unit test.
#!
#! Parameters:
#!
#! \param PLUGIN_TESTTING_GROUP_NAME Group name declared in VS project.
#! \param PLUGIN_TESTTING_LINK_LIBRARIE Specify the link library address of the test 
#!        plug-in.
#! \param PLUGIN_TESTTING_LINK_INCLUDE Specify the header file address of the test 
#!        plug-in.
#!
#! Multi-value parameters (all optional):
#!
#! \param PLUGIN_TESTTING_SOURCES Specify the source code of the test plug-in.
#! \param PLUGIN_TESTTING_DATA_DIRS Specify the external test data address of the test 
#!        plug-in, Generate all files under the external test data directory to 
#!        testing_data.qrc file.
cmake_minimum_required(VERSION 3.18)
function(lancetFunctionCreateQtTest)

  # options
  set(arg_options PLUGIN_UNIT_TESTTING)
  
  # single value arguments
  set(arg_single 
    PLUGIN_TESTTING_GROUP_NAME
	PLUGIN_TESTTING_LINK_LIBRARIE
	PLUGIN_TESTTING_LINK_INCLUDE
  )
  
  # multiple value arguments
  set(arg_multiple
    PLUGIN_TESTTING_SOURCES
    PLUGIN_TESTTING_DATA_DIRS
  )

  cmake_parse_arguments(_TESTTING "${arg_options}" "${arg_single}" "${arg_multiple}" ${ARGN})

  set(_TESTTING_PLUGIN_UNIT_TESTTING 1)
  
  # ------------------ Input parameter security check ------------------------------
  if(NOT _TESTTING_PLUGIN_TESTTING_LINK_LIBRARIE)
    message(SEND_ERROR "PLUGIN_TESTTING_LINK_LIBRARIE is empty!")
	return()
  endif()
  
  if(NOT _TESTTING_PLUGIN_TESTTING_LINK_INCLUDE)
    message(SEND_ERROR "PLUGIN_TESTTING_LINK_INCLUDE is empty!")
	return()
  endif()
  
  if(NOT _TESTTING_PLUGIN_TESTTING_SOURCES)
    message(WARNING "_TESTTING_PLUGIN_TESTTING_SOURCES is empty! The default is ${PROJECT_NAME}.cpp")
	set(_TESTTING_PLUGIN_TESTTING_SOURCES ${PROJECT_NAME}.cpp)
  endif()
  
  if(_TESTTING_PLUGIN_TESTTING_GROUP_NAME)
    set(_vs_project_target_group_name ${_TESTTING_PLUGIN_TESTTING_GROUP_NAME})
  else()
    set(_vs_project_target_group_name LancetTestting)
    message(WARNING "The target output VS project group name is not found. The default is [LancetTestting] ")
  endif()
  
  # -----------------------
  target_include_directories(${_TESTTING_PLUGIN_TESTTING_LINK_LIBRARIE} PUBLIC ${_TESTTING_PLUGIN_TESTTING_LINK_INCLUDE})
  enable_testing()
  
  # ---------------------------------- Qt ------------------------------------
  
  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)
  
  if(_TESTTING_PLUGIN_TESTTING_DATA_DIRS)
    set(CMAKE_AUTORCC ON)
  else()
    # Set to null by default.
    set(_TESTTING_PLUGIN_TESTTING_DATA_DIRS )
  endif()
  
  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  
  find_package(Qt5Test REQUIRED)
  
  # ------------------------ Project ---------------------------
  add_executable(${PROJECT_NAME} ${_TESTTING_PLUGIN_TESTTING_SOURCES} ${_TESTTING_PLUGIN_TESTTING_DATA_DIRS})
  add_test(${PROJECT_NAME} ${PROJECT_NAME})
  target_link_libraries(${PROJECT_NAME} ${_TESTTING_PLUGIN_TESTTING_LINK_LIBRARIE} Qt5::Test)
  
  # ---------------------------- VS Project ------------------------------------
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${_vs_project_target_group_name})
  set_property(GLOBAL PROPERTY PREDEFINED_TARGET_FOLDER ${_vs_project_target_group_name})
  
endfunction()