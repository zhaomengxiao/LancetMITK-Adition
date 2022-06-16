# Define the minimum CMake version that this project was developed for
cmake_minimum_required(VERSION 2.8)

# Set the project and build type
project(CAPIsample)

# Set the output directory structure
# set(BUILD_OUTPUT_DIRECTORY build-output)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DIRECTORY})
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIRECTORY})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DIRECTORY})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIRECTORY})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DIRECTORY})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIRECTORY})

# Set the preprocessor flag we need when creating the library
add_definitions(-DCAPICOMMON_EXPORTS)

# Set the $SOURCES_NDLINK variable using file(GLOB...) with wildcards
file(GLOB SOURCES_CAPICOMMON "CAPIcommon/src/*.cpp")

add_library(
  CAPIcommon
  SHARED
  ${SOURCES_CAPICOMMON}
)

# Include the header files
target_include_directories(
  CAPIcommon
  PUBLIC
  CAPIcommon/include
  PRIVATE
  CAPIcommon/src/include
)

# Generate the demo app executable
set(SOURCES CAPIsample/src/main.cpp)
add_executable(CAPIsample ${SOURCES})

# Link the shared library
target_link_libraries(
  CAPIsample
  PUBLIC
  CAPIcommon
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
