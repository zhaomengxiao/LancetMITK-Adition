project(org_mitk_lancet_femurmarkerpoint)

mitk_create_plugin(
  EXPORT_DIRECTIVE FEMURMARKERPOINT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
