project(org_mitk_lancet_movedata)

mitk_create_plugin(
  EXPORT_DIRECTIVE MOVEDATA_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration MitkGizmo
  MitkCore MitkCLCore MitkCommandLine MitkCLUtilities
  PACKAGE_DEPENDS PRIVATE ITK VTK VTK|CommonComputationalGeometry VTK|FiltersGeneral VTK|FiltersSources VTK|FiltersExtraction VTK|FiltersFlowPaths VTK|FiltersModeling VTK|FiltersPoints
)
