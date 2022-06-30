mitk_create_module(LancetIGT
INCLUDE_DIRS
    PUBLIC Algorithms Common DataManagement ExceptionHandling IO Rendering TrackingDevices TestingHelper
  DEPENDS PUBLIC MitkSceneSerialization MitkIGTBase MitkIGT
  PACKAGE_DEPENDS PRIVATE ITK VTK 
)

#add_subdirectory(cmdapps)
