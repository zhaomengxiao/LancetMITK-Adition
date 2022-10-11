project(org_mitk_lancet_pelvismarkerpoint)

mitk_create_plugin(
  EXPORT_DIRECTIVE PELVISMARKERPOINT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
