project(org_mitk_lancet_example_editor)

mitk_create_plugin(
  EXPORT_DIRECTIVE ORG_MITK_LANCET_EXAMPLE_EDITOR
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgets
  )
