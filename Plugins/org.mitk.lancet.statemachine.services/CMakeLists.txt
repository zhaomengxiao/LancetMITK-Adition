project(org_mitk_lancet_statemachine_services)

mitk_create_plugin(
  EXPORT_DIRECTIVE LANCET_STATEMACHINE_SERVICES_PLUGIN
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PUBLIC MitkCore
  PACKAGE_DEPENDS PRIVATE Qt5|Scxml
)

if (BUILD_TESTING)
  add_subdirectory(tests)
  enable_testing()
  add_test(NAME org_mitk_lancet_statemachine_services_test COMMAND 1 2)
endif()