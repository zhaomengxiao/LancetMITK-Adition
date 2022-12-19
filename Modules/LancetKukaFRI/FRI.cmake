#Begin KUKA FRI
option(MITK_USE_KUKA_FRI "Enable support for KUKA Fast Robot Interface" ON)
#only if MITK_USE_KUKA_ROBOT is enabled
if(MITK_USE_KUKA_FRI)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    find_library(MITK_KUKA_FRI_LIB libFRIClientd HINTS "${CMAKE_CURRENT_SOURCE_DIR}/FastRobotInterface/lib" DOC "Path which contains the libFRIClientd.lib")
    message(WARNING "fri debug")
  else()
    find_library(MITK_KUKA_FRI_LIB libFRIClient HINTS "${CMAKE_CURRENT_SOURCE_DIR}/FastRobotInterface/lib" DOC "Path which contains the libFRIClientd.lib")
    message(WARNING "fri release")
  endif()

  Set(MITK_KUKA_FRI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FastRobotInterface/include/")
ENDIF(MITK_USE_KUKA_FRI)

#End KUKA Hardware