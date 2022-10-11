project(org_mitk_lancet_contusion)

mitk_create_plugin(
  EXPORT_DIRECTIVE CONTUSION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
