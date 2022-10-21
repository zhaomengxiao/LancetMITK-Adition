project(org_mitk_lancet_disclaimeragreement)

mitk_create_plugin(
  EXPORT_DIRECTIVE DISCLAIMERAGREEMENT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
