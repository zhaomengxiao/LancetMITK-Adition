project(org_mitk_lancet_tha_application)

mitk_create_plugin(
  EXPORT_DIRECTIVE org_mitk_lancet_tha_application_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  NO_INSTALL
  )
  
if (BUILD_TESTING)
  add_subdirectory(tests)
  enable_testing()
  add_test(NAME org_mitk_lancet_tha_application_test COMMAND 1 2)
endif()