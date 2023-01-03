file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(CPP_FILES
  lancetBackendBase.cpp
  lancetLogginBackend.cpp
  lancetMatrixConvert.cpp
  widgets/lancetDeviceTrackingWidget.cpp
)

set(MOC_H_FILES
  include/widgets/lancetDeviceTrackingWidget.h
)

set(UI_FILES 
  src/widgets/lancetDeviceTrackingWidget.ui
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
  resource/resources.qrc
)