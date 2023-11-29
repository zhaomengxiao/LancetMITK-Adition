project(org_mitk_lancet_spinedemo)

mitk_create_plugin(
  EXPORT_DIRECTIVE SPINEDEMO_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkBoundingShape MitkLancetAlgo MitkLancetGeoUtil MitkDICOM   MitkLancetRegistration MitkGizmo
  MitkCore MitkCLCore MitkCommandLine MitkCLUtilities MitkMatchPointRegistration MitkIGTUI MitkLancetIGT MitkLancetRobot
  PACKAGE_DEPENDS PRIVATE ITK VTK VTK|CommonComputationalGeometry
)
