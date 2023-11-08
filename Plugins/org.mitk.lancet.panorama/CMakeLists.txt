project(org_mitk_panorama)

mitk_create_plugin(
    EXPORT_DIRECTIVE Panorama_EXPORT
	EXPORTED_INCLUDE_SUFFIXES src
	MODULE_DEPENDS MitkQtWidgetsExt MitkTestingHelper MitkModelFit MitkModelFitUI
	PACKAGE_DEPENDS PRIVATE ITK VTK ITK|IOGDCM ITK|Smoothing VTK|ImagingGeneral ITK|IOPNG ITK|LabelMap ITK|IONIFTI
	)
