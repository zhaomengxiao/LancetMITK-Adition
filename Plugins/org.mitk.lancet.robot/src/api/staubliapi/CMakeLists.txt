cmake_minimum_required(VERSION 3.0)

project(staubliapi)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_library(
  RobotApi_Release_lib
  NAME
  robotapi
  PATHS
  ${PROJECT_SOURCE_DIR}
)
find_library(
  RobotApi_Debug_lib
  NAME
  robotapid
  PATHS
  ${PROJECT_SOURCE_DIR}
)

# set(
#   HEADER
#   ${PROJECT_SOURCE_DIR}/robotapi_include/robotapi.h
#   ${PROJECT_SOURCE_DIR}/robotapi_include/robotcontroler.h
#   ${PROJECT_SOURCE_DIR}/robotapi_include/robotsocket.h
# )

add_library(
  staubliapi
  STATIC
  IMPORTED
  GLOBAL
)

set_target_properties(
  staubliapi
  PROPERTIES
  IMPORTED_LOCATION_RELEASE
  ${RobotApi_Release_lib}
  IMPORTED_LOCATION_DEBUG
  ${RobotApi_Debug_lib}
  PUBLIC_HEADER
  ${PROJECT_SOURCE_DIR}/robotapi_include
)

target_include_directories(
  staubliapi
  INTERFACE
  ${PROJECT_SOURCE_DIR}/robotapi_include
)

target_link_libraries(
  staubliapi
  INTERFACE
  Qt5::Core
  Qt5::Network
)

OPTION(
  BUILD_EXAMPLE
  "Build Example"
  OFF
)

if(BUILD_EXAMPLE)
  find_package(Qt5Widgets REQUIRED)

  set(CMAKE_AUTOUIC ON)
  
  add_executable(
    staubliapi_example
    main.cpp
    widget.cpp
    widget.h
    widget.ui
  )
  
  target_link_libraries(
    staubliapi_example
    PUBLIC
    staubliapi
    Qt5::Widgets
  )
endif()