MITK_CREATE_MODULE(
  INCLUDE_DIRS Qmitk
  DEPENDS MitkIGT MitkIGTUI MitkLancetIGT MitkQtWidgetsExt MitkPersistence
)

add_subdirectory(autoload/LancetConfigurationWidgetRegistry)

## create IGTUI config
configure_file(mitkLancetIGTUIConfig.h.in ${PROJECT_BINARY_DIR}/mitkLancetIGTUIConfig.h @ONLY)


