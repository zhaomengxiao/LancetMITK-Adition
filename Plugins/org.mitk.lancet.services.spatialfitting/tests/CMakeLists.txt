cmake_minimum_required(VERSION 3.18)

# load lancetFunctionCreateQtTest.cmake
if(MITK_EXTENSION_DIRS)
  include(${MITK_EXTENSION_DIRS}/CMake/lancetFunctionCreateQtTest.cmake)
endif()

# TESTTING_TARGET_PLUGIN Target test plug-in name.
# TESTTING_TARGET_PLUGIN_INCLUDE Target test plug-in header file directory address.
# TESTTING_TARGET_PLUGIN Target test plug-in library file name.
set(TESTTING_TARGET_PLUGIN org_mitk_lancet_services_spatialfitting)
set(TESTTING_TARGET_PLUGIN_INCLUDE ${CMAKE_CURRENT_DIR}/../src)
set(TESTTING_TARGET_PLUGIN_LIBRARIES ${TESTTING_TARGET_PLUGIN})

# Add test set subunit.
add_subdirectory(tst_pointAccuracyDate)
add_subdirectory(tst_roboticsRegisterNavigationData)
add_subdirectory(tst_roboticsVerifyNavigationData)
