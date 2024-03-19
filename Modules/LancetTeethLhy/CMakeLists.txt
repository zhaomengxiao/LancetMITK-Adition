#include(${MITK_EXTENSION_DIRS}/CMake/RobotHardware.cmake)

set(MITK_DAZU_ROBOT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/robotapi/include")
set(MITK_DAZU_ROBOT_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/robotapi/lib/HR_Pro.lib")


set(qt5_depends Qt5|Core+Widgets+Network)

if(MITK_USE_KUKA_ROBOT)
  set(ADDITIONAL_INCLUDE_DIRS 
  ${ADDITIONAL_INCLUDE_DIRS} 
  ${MITK_DAZU_ROBOT_INCLUDE_DIR}
)

  message("MITK_DAZU_ROBOT_INCLUDE_DIR = ${MITK_DAZU_ROBOT_INCLUDE_DIR}")
  set(ADDITIONAL_LIBS 
  ${CMAKE_CURRENT_SOURCE_DIR}/robotapi/lib/RRTC_Staubli.lib
  ${CMAKE_CURRENT_SOURCE_DIR}/robotapi/lib/robotapi_Staubli.lib
  ${MITK_DAZU_ROBOT_LIBRARY_DIR}
  )
  message("MITK_DAZU_ROBOT_LIBRARY_DIR is ${MITK_DAZU_ROBOT_LIBRARY_DIR}")
endif()

#mitk_create_module(LancetStaubliRobot
#INCLUDE_DIRS
#    PUBLIC ${ADDITIONAL_INCLUDE_DIRS} Algorithms Common DataManagement ExceptionHandling IO Rendering TrackingDevices TestingHelper
#  DEPENDS PUBLIC MitkSceneSerialization MitkIGTBase MitkIGT MitkLancetRobot MitkLancetRegistration
#  PACKAGE_DEPENDS
#    PRIVATE ITK VTK 
#    PUBLIC ${qt5_depends} Eigen
#  ADDITIONAL_LIBS PUBLIC "${ADDITIONAL_LIBS}"
#  #FORCE_STATIC
#)

MITK_CREATE_MODULE(LancetTeethLhy
  INCLUDE_DIRS
    PUBLIC ${ADDITIONAL_INCLUDE_DIRS} TrackingDevices
  DEPENDS MitkCore MitkIGT
  PACKAGE_DEPENDS PUBLIC ${qt5_depends} Poco|Net Eigen
  ADDITIONAL_LIBS PUBLIC "${ADDITIONAL_LIBS}"
  #FORCE_STATIC
)