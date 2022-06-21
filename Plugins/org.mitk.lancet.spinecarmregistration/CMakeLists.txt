project(org_mitk_lancet_spinecarmregistration)

mitk_create_plugin(
  EXPORT_DIRECTIVE SPINECARMREGISTRATION_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS PRIVATE Eigen OpenCV 
  MODULE_DEPENDS MitkLancetAlgo MitkLancetRegistration   
)
