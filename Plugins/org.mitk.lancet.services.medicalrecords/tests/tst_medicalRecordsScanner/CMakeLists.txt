cmake_minimum_required(VERSION 3.18)
project(tst_medicalRecordsScanner)

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


#! Copy runtime dependent resource files.
set(tst_datas_dir ${CMAKE_CURRENT_SOURCE_DIR}/tst_datas)
set(tst_datas_target_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tst_datas)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${tst_datas_dir} ${tst_datas_target_dir})
add_definitions(-DTEST_DATAS_DIR="${tst_datas_target_dir}")

# ------------------ files -----------------------
set(files ${PROJECT_NAME}.cpp)
set(test_datas )

lancetFunctionCreateQtTest(
  PLUGIN_TESTTING_GROUP_NAME LancetTestting/tst_medicalRecordsServices
  PLUGIN_TESTTING_LINK_INCLUDE ${TESTTING_TARGET_PLUGIN_INCLUDE}
  PLUGIN_TESTTING_LINK_LIBRARIE ${TESTTING_TARGET_PLUGIN_LIBRARIES}
  PLUGIN_TESTTING_SOURCES ${files}
  PLUGIN_TESTTING_DATA_DIRS ${test_datas}
)