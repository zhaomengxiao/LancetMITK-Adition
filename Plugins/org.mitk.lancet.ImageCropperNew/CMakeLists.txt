project(org_mitk_lancet_ImageCropperNew)

mitk_create_plugin(
  EXPORT_DIRECTIVE IMAGECROPPERNEW_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkGizmo_noscale MitkMoveObject
)
