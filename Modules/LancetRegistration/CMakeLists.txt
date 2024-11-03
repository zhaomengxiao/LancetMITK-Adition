mitk_create_module(LancetRegistration
  DEPENDS PUBLIC MitkCore
  PACKAGE_DEPENDS Qt5|Core+Widgets
  PACKAGE_DEPENDS PRIVATE VTK
)

#add_subdirectory(cmdapps)
