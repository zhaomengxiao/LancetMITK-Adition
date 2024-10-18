project(org_mitk_lancet_CZXtest)

mitk_create_plugin(
  EXPORT_DIRECTIVE CZXTEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkIGTUI MitkLancetIGT MitkLancetRobot MitkLancetAlgo MitkGizmo MitkBoundingShape
  MODULE_DEPENDS MitkLancetHardwareDevice MitkLancetRobotRegistration MitkLancetPrintDataHelper MitkLancetFileIO
  PACKAGE_DEPENDS PRIVATE VTK|FiltersFlowPaths
)

set(NLOHMANN_INCLUDE "")

set(NLOHMANN_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann")
include_directories(${NLOHMANN_INCLUDE}) 
