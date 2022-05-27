project(org_mitk_lancet_spinectregistration)

mitk_create_plugin(
  EXPORT_DIRECTIVE SPINECTREGISTRATION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration   
)
