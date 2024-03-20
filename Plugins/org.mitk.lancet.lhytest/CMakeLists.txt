project(org_mitk_lancet_lhytest)

mitk_create_plugin(
  EXPORT_DIRECTIVE LHYTEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkLancetStaubliRobot MitkLancetIGT MitkIGTUI MitkLancetTeethLhy
)
