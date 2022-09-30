project(org_mitk_lancet_kukarobotcontrol)

mitk_create_plugin(
  EXPORT_DIRECTIVE KUKAROBOTCONTROL_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkLancetIGT MitkLancetRobot
)
