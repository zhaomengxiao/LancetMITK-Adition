cmake_minimum_required(VERSION 3.18)
project(tst_pipelineManager)

if(NOT TESTTING_TARGET_PLUGIN)
  message(SEND_ERROR "TESTTING_TARGET_PLUGIN is empty!")
  return()
endif()

if(NOT TESTTING_TARGET_PLUGIN_INCLUDE)
  message(SEND_ERROR "TESTTING_TARGET_PLUGIN_INCLUDE is empty!")
  return()
endif()

if(NOT TESTTING_TARGET_PLUGIN_LIBRARIES)
  message(SEND_ERROR "TESTTING_TARGET_PLUGIN_LIBRARIES is empty!")
  return()
endif()

# ------------------ files -----------------------
set(files ${PROJECT_NAME}.cpp)
set(test_datas )

lancetFunctionCreateQtTest(
  PLUGIN_TESTTING_GROUP_NAME LancetTestting/tst_spatialFitting
  PLUGIN_TESTTING_LINK_INCLUDE ${TESTTING_TARGET_PLUGIN_INCLUDE}
  PLUGIN_TESTTING_LINK_LIBRARIE ${TESTTING_TARGET_PLUGIN_LIBRARIES}
  PLUGIN_TESTTING_SOURCES ${files}
  PLUGIN_TESTTING_DATA_DIRS ${test_datas}
)