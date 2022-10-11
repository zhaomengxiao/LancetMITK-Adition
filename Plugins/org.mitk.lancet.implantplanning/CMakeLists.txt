project(org_mitk_lancet_implantplanning)

mitk_create_plugin(
  EXPORT_DIRECTIVE IMPLANTPLANNING_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
