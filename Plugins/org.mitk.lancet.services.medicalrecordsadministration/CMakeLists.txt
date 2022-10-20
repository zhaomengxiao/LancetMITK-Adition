project(org_mitk_lancet_services_medicalrecordsadministration)

mitk_create_plugin(
  EXPORT_DIRECTIVE ORG_MITK_LANCET_SERVICES_MEDICALRECORDSADMINISTRATION_PLUGIN
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PUBLIC MitkCore
)

if (BUILD_TESTING)
  add_subdirectory(tests)
  enable_testing()
  add_test(NAME org_mitk_lancet_services_medicalrecordsadministration_test COMMAND 1 2)
endif()