project(org_mitk_lancet_imageregistration)

mitk_create_plugin(
  EXPORT_DIRECTIVE IMAGEREGISTRATION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
