project(org_mitk_lancet_view_deviceconfig)

mitk_create_plugin(
  EXPORT_DIRECTIVE DEVICECONFIG_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
