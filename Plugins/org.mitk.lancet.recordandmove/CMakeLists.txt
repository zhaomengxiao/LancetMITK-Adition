project(org_mitk_lancet_recordandmove)

mitk_create_plugin(
  EXPORT_DIRECTIVE RECORDANDMOVE_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkIGTUI MitkLancetIGT MitkLancetKukaFRI MitkLancetRobot
  PACKAGE_DEPENDS Poco|Net 
)
