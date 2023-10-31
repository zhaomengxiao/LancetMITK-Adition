project(org_mitk_orthopedics_thaplan)

mitk_create_plugin(
  EXPORT_DIRECTIVE THAPLAN_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkOrthopedics MitkGizmo
)
