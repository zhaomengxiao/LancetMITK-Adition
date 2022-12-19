file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(CPP_FILES
  lancetBackendBase.cpp
  lancetLogginBackend.cpp
  widgets/lancetDeviceTrackingWidget.cpp
)

set(MOC_H_FILES
  include/widgets/lancetDeviceTrackingWidget.h
)

set(UI_FILES 
  src/widgets/lancetDeviceTrackingWidget.ui
)

set(RESOURCE_FILES
  robotEndRFWidget.png
  robotBaseRFWidget.png
  pelvisRFWidget.png
  femurRFWidget.png
  probeRFWidget.png
)
