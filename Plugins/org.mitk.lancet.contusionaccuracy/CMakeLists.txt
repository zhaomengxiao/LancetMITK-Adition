project(org_mitk_lancet_contusionaccuracy)

mitk_create_plugin(
  EXPORT_DIRECTIVE CONTUSIONACCURACY_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
