include(RobotHardware.cmake)

set(qt5_depends Qt5|Core+Widgets+Network)

if(MITK_USE_KUKA_ROBOT)
  set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${MITK_KUKA_ROBOT_INCLUDE_DIR})
  set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ${MITK_KUKA_ROBOT_LIB})
endif()

mitk_create_module(LancetIGT
INCLUDE_DIRS
    PUBLIC ${ADDITIONAL_INCLUDE_DIRS} Algorithms Common DataManagement ExceptionHandling IO Rendering TrackingDevices TestingHelper
  DEPENDS PUBLIC MitkSceneSerialization MitkCLUtilities MitkIGTBase MitkIGT MitkLancetRobot MitkLancetRegistration 
  PACKAGE_DEPENDS
    PRIVATE ITK VTK 
    PUBLIC ${qt5_depends}
  ADDITIONAL_LIBS PUBLIC "${ADDITIONAL_LIBS}"
)

if(MODULE_IS_ENABLED)
  #configure_file(mitkIGTConfig.h.in ${PROJECT_BINARY_DIR}/mitkIGTConfig.h @ONLY)

  add_subdirectory(autoload/DeviceRegistry)
  #add_subdirectory(Tutorial)
  add_subdirectory(test)
endif()
