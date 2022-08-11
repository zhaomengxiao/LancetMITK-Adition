project(org_mitk_lancet_htotest)

mitk_create_plugin(
  EXPORT_DIRECTIVE HTOTEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
