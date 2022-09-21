project(org_mitk_lancet_surgicalsimulate)

mitk_create_plugin(
  EXPORT_DIRECTIVE SURGICALSIMULATE_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkLancetIGT MitkLancetRobot
)
