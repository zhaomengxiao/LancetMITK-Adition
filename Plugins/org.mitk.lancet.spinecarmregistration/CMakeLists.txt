project(org_mitk_lancet_spinecarmregistration)

mitk_create_plugin(
  EXPORT_DIRECTIVE SPINECARMREGISTRATION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration   
)
