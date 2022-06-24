project(org_mitk_lancet_accuracytest)

mitk_create_plugin(
  EXPORT_DIRECTIVE ACCURACYTEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PRIVATE MitkQtWidgetsExt MitkIGT MitkIGTUI MitkLancetRegistration
)
