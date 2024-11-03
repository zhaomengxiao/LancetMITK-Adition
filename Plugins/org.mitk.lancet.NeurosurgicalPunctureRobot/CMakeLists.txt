project(org_mitk_lancet_NeurosurgicalPunctureRobot)

mitk_create_plugin(
  EXPORT_DIRECTIVE NEUROSURGICALPUNCTUREROBOT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
  PACKAGE_DEPENDS PRIVATE VTK|RenderingImage+ImagingCore+IOImage+ImagingMorphological+FiltersGeneral+FiltersCore+CommonExecutionModel+CommonDataModel+CommonMisc+CommonTransforms+CommonCore
  PACKAGE_DEPENDS PRIVATE ITK|DistanceMap
)

# Include VMTK headers
set(VMTK_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/VMTK/include")
include_directories(${VMTK_INCLUDE})

# Locate VTK DICOM libraries and DLLs
file(GLOB VMTK_LIB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/VMTK/lib/Release/*.lib")
file(GLOB VMTK_DLLS_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/VMTK/bin/Release/*.dll")

# Link against VMTK libraries
target_link_libraries(org_mitk_lancet_NeurosurgicalPunctureRobot PRIVATE
  ${VMTK_LIB_RELEASE}
)

# Copy DLLs to the MITK binary directory
file(COPY ${VMTK_DLLS_RELEASE} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Release")
###############################################################################

# Include VTK DICOM headers
set(VTKDICOM_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/vtkDicom/include")
include_directories(${VTKDICOM_INCLUDE})

# Locate VTK DICOM libraries and DLLs
file(GLOB VTKDICOM_LIB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/vtkDicom/lib/Release/*.lib")
file(GLOB VTKDICOM_DLLS_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/vtkDicom/bin/Release/*.dll")

# Link against VTK DICOM libraries
target_link_libraries(org_mitk_lancet_NeurosurgicalPunctureRobot PRIVATE
  ${VTKDICOM_LIB_RELEASE}
)

# Copy DLLs to the MITK binary directory
file(COPY ${VTKDICOM_DLLS_RELEASE} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Release")

