#file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
set(H_FILES
  include/robotUtil.h
  include/robotRegistration.h
  include/kukaRobotDevice.h
)

set(MOC_H_FILES
  include/kukaRobotDevice.h
)

set(CPP_FILES
  robotUtil.cpp
  robotRegistration.cpp
  kukaRobotDevice.cpp
#  robotcontroler.cpp
#  robotsocket.cpp
)

#[[set(RESOURCE_FILES
)]]