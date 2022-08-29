#file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
set(H_FILES
  include/robotUtil.h
  include/robotRegistration.h
  include/udpmessage.h
  include/udpsocketrobotheartbeat.h
)

set(MOC_H_FILES
  include/udpsocketrobotheartbeat.h
)

set(CPP_FILES
  robotUtil.cpp
  robotRegistration.cpp
  udpmessage.cpp
  udpsocketrobotheartbeat.cpp
#  robotcontroler.cpp
#  robotsocket.cpp
)

#[[set(RESOURCE_FILES
)]]