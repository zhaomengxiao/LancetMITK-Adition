project(org_mitk_lancet_cameratest)

mitk_create_plugin(
  EXPORT_DIRECTIVE CAMERATEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkIGTUI MitkLancetIGT MitkLancetRobot
  PACKAGE_DEPENDS Poco|Net 
)
