set(H_FILES
  DataManagement/lancetNavigationObject.h
  DataManagement/lancetTrackingDeviceSourceConfigurator.h
  
  IO/lancetNavigationObjectWriter.h

  Algorithms/lancetNavigationDataInReferenceCoordFilter.h
  Rendering/lancetNavigationObjectVisualizationFilter.h

  TrackingDevices/lancetRobotTrackingTool.h
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.h
  TrackingDevices/kukaRobotDevice.h
)

set(CPP_FILES
  DataManagement/lancetNavigationObject.cpp
  DataManagement/lancetTrackingDeviceSourceConfigurator.cpp
  
  IO/lancetNavigationObjectWriter.cpp
  
  Algorithms/lancetNavigationDataInReferenceCoordFilter.cpp
  Rendering/lancetNavigationObjectVisualizationFilter.cpp

  TrackingDevices/lancetRobotTrackingTool.cpp
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.cpp
  TrackingDevices/kukaRobotDevice.cpp
)

set(UI_FILES
)

set(MOC_H_FILES
  TrackingDevices/kukaRobotDevice.h
)

set(RESOURCE_FILES
)
