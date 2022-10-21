cmake_minimum_required(VERSION 3.18)
project(tst_thaWorkbenchMenuBar)

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


#target_include_directories(${TESTTING_TARGET_PLUGIN_LIBRARIES} PUBLIC ${TESTTING_TARGET_PLUGIN_INCLUDE})
# ------------------ files -----------------------
set(files ${PROJECT_NAME}.cpp tst_thaWorkbenchWindow.cpp)
message("xxxx CMAKE_CURRENT_SOURCE_DIR= ${CMAKE_CURRENT_SOURCE_DIR}")
set(test_datas ${CMAKE_CURRENT_SOURCE_DIR}/tst_datas/test_datas.qrc)

lancetFunctionCreateQtTest(
  PLUGIN_TESTTING_GROUP_NAME LancetTestting/tst_ThaApplication
  PLUGIN_TESTTING_LINK_INCLUDE ${TESTTING_TARGET_PLUGIN_INCLUDE}
  PLUGIN_TESTTING_LINK_LIBRARIE ${TESTTING_TARGET_PLUGIN_LIBRARIES}
  PLUGIN_TESTTING_SOURCES ${files}
  PLUGIN_TESTTING_DATA_DIRS ${test_datas}
)