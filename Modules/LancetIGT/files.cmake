set(H_FILES
  DataManagement/lancetNavigationObject.h
  DataManagement/lancetTrackingDeviceSourceConfigurator.h
  DataManagement/lancetPathPoint.h
  DataManagement/lancetThaPelvisObject.h
  DataManagement/lancetThaFemurObject.h
  DataManagement/lancetThaReductionObject.h
  
  IO/lancetNavigationObjectWriter.h

  Algorithms/lancetNavigationDataInReferenceCoordFilter.h
  Algorithms/lancetApplyDeviceRegistratioinFilter.h
  Algorithms/lancetApplySurfaceRegistratioinFilter.h
  Algorithms/lancetApplySurfaceRegistratioinStaticImageFilter.h
  Algorithms/lancetTreeCoords.h
  
  Rendering/lancetNavigationObjectVisualizationFilter.h

  TrackingDevices/lancetRobotTrackingTool.h
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.h
  TrackingDevices/kukaRobotDevice.h
  TrackingDevices/lancetKukaRobotDevice.h
  #UI/QmitkLancetKukaWidget.cpp
)

set(CPP_FILES
  DataManagement/lancetNavigationObject.cpp
  DataManagement/lancetTrackingDeviceSourceConfigurator.cpp
  DataManagement/lancetPathPoint.cpp
  DataManagement/lancetThaPelvisObject.cpp
  DataManagement/lancetThaFemurObject.cpp
  DataManagement/lancetThaReductionObject.cpp
  
  IO/lancetNavigationObjectWriter.cpp
  
  Algorithms/lancetNavigationDataInReferenceCoordFilter.cpp
  Algorithms/lancetApplyDeviceRegistratioinFilter.cpp
  Algorithms/lancetApplySurfaceRegistratioinFilter.cpp
  Algorithms/lancetApplySurfaceRegistratioinStaticImageFilter.cpp
  Algorithms/lancetTreeCoords.cpp

  Rendering/lancetNavigationObjectVisualizationFilter.cpp

  TrackingDevices/lancetRobotTrackingTool.cpp
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.cpp
  TrackingDevices/kukaRobotDevice.cpp
  TrackingDevices/lancetKukaRobotDevice.cpp
  #UI/QmitkLancetKukaWidget.cpp
)

set(UI_FILES
  #UI/QmitkLancetKukaWidget.ui
)

set(MOC_H_FILES
  #UI/QmitkLancetKukaWidget.h
  TrackingDevices/kukaRobotDevice.h
)

set(RESOURCE_FILES
)
