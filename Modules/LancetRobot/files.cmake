#file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
set(H_FILES
  include/robotUtil.h
  include/robotRegistration.h
  include/udpmessage.h
  include/udpsocketrobotheartbeat.h
  include/kukaRobotAPI/kukaRobotAPI.h
  include/kukaRobotAPI/udpRobotInfoClient.h
  include/kukaRobotAPI/tcpRobotCommandServer.h
  include/kukaRobotAPI/defaultProtocol.h
  include/kukaRobotAPI/robotInfoProtocol.h
)

set(MOC_H_FILES
  include/udpsocketrobotheartbeat.h
)

set(CPP_FILES
  robotUtil.cpp
  robotRegistration.cpp
  udpmessage.cpp
  udpsocketrobotheartbeat.cpp
  kukaRobotAPI/kukaRobotAPI.cpp
  kukaRobotAPI/udpRobotInfoClient.cpp
  kukaRobotAPI/tcpRobotCommandServer.cpp
  kukaRobotAPI/defaultProtocol.cpp
  kukaRobotAPI/robotInfoProtocol.cpp
)

#[[set(RESOURCE_FILES
)]]