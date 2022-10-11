project(org_mitk_lancet_femurroughregistrations)

mitk_create_plugin(
  EXPORT_DIRECTIVE FEMURROUGHREGISTRATIONS_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
