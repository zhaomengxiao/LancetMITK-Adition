cmake_minimum_required(VERSION 3.18)
project(tst_TrackingDeviceManage)

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
set(files ${PROJECT_NAME}.cpp)

set(test_datas )

#! Copy runtime dependent resource files.
set(test_datas_dir ${CMAKE_CURRENT_SOURCE_DIR}/tst_datas)
set(test_target_datas_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tst_datas)
message("tst_TrackingDeviceManage: test_datas_dir >> ${test_datas_dir}")
message("tst_TrackingDeviceManage: test_target_datas_dir >> ${test_target_datas_dir}")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${test_datas_dir} ${test_target_datas_dir})

lancetFunctionCreateQtTest(
  PLUGIN_TESTTING_GROUP_NAME LancetTestting/tst_TrackingDeviceManage
  PLUGIN_TESTTING_LINK_INCLUDE ${TESTTING_TARGET_PLUGIN_INCLUDE}
  PLUGIN_TESTTING_LINK_LIBRARIE ${TESTTING_TARGET_PLUGIN_LIBRARIES}
  PLUGIN_TESTTING_SOURCES ${files}
  PLUGIN_TESTTING_DATA_DIRS ${test_datas}
)