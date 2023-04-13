project(org_mitk_lancet_robot)
#set(qt5_depends Qt5|Widgets+Network)
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${MITK_VEGA_TRACKER_INCLUDE_DIR})
set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ${MITK_VEGA_TRACKER_LIB})

mitk_create_plugin(
  EXPORT_DIRECTIVE ROBOT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkLancetIGT
  #PACKAGE_DEPENDS ${qt5_depends}
)
#add_subdirectory(src/api/staubliapi)

#target_link_libraries(org_mitk_lancet_robot PRIVATE staubliapi)