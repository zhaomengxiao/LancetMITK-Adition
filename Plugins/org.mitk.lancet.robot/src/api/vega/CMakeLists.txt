cmake_minimum_required(VERSION 3.0)

add_subdirectory(capisample)

project(vega)

# find_package(Eigen3 REQUIRED)
find_package(Qt5Core REQUIRED)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE}) 

set(CMAKE_AUTOMOC ON)

file(
  GLOB
  VEGA_C
  ${PROJECT_SOURCE_DIR}/*.c*
)

file(
  GLOB
  VEGA_H
  ${PROJECT_SOURCE_DIR}/*.h*
)

add_library(
  ${PROJECT_NAME}
  STATIC
  ${VEGA_C}
  ${VEGA_H}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  ${VTK_LIBRARIES} 
  CAPIcommon
  Qt5::Core
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  # ${EIGEN3_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)

# Copy .rom files so the built binaries can use them
file(
  COPY
  sroms
  DESTINATION
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
)

file(
  COPY
  sroms
  DESTINATION
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
)