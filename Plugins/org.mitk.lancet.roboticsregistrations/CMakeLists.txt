project(org_mitk_lancet_roboticsregistrations)

mitk_create_plugin(
  EXPORT_DIRECTIVE ROBOTICSREGISTRATIONS_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
