project(org_mitk_lancet_dentalaccuracy)

mitk_create_plugin(
  EXPORT_DIRECTIVE DENTALACCURACY_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration MitkGizmo
  MitkCore MitkCLCore MitkCommandLine MitkCLUtilities MitkMatchPointRegistration
  PACKAGE_DEPENDS PRIVATE ITK VTK VTK|CommonComputationalGeometry
)
