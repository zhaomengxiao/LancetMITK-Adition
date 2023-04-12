project(org_mitk_lancet_medicalrecordmanagement)

mitk_create_plugin(
  EXPORT_DIRECTIVE MEDICALRECORDMANAGEMENT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)
