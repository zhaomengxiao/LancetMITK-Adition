project(org_mitk_lancet_movedata)

mitk_create_plugin(
  EXPORT_DIRECTIVE MOVEDATA_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration
)
