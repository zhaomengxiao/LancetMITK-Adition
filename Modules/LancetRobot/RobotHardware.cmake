#Begin KUKA ROBOT
option(MITK_USE_KUKA_ROBOT "Enable support for KUKA Robot hardware" OFF)

#only if MITK_USE_KUKA_ROBOT is enabled
if(MITK_USE_KUKA_ROBOT)
  find_library(MITK_KUKA_ROBOT_LIB robotapi DOC "Path which contains the robotapi.lib")
  find_path(MITK_KUKA_ROBOT_INCLUDE_DIR robotapi.h DOC  "Include directory of the robot api.")
ENDIF(MITK_USE_KUKA_ROBOT)

#End KUKA Hardware