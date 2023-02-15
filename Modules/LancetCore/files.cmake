file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(CPP_FILES
  lancetBackendBase.cpp
  lancetLogginBackend.cpp
  lancetMatrixConvert.cpp
  lancetdevicetrackingwidget.cpp
)

set(MOC_H_FILES
  include/lancetdevicetrackingwidget.h
)

set(UI_FILES 
  src/lancetDeviceTrackingWidgetControls.ui
)

set(RESOURCE_FILES
  resources.qrc
)
