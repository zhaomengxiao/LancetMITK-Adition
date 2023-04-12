project(org_mitk_lancet_postoperativeoutcome)

mitk_create_plugin(
  EXPORT_DIRECTIVE POSTOPERATIVEOUTCOME_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
