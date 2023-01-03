set(MOC_H_FILES
  src/lancetSpatialFittingAbstractService.h
  src/internal/lancetPluginActivator.h
  src/internal/lancetSpatialFittingService.h
  src/core/lancetSpatialFittingNavigationToolCollector.h
)

set(SRC_CPP_FILES
  lancetSpatialFittingAbstractService.cpp
  lancetSpatialFittingAbstractDirector.cpp
  lancetSpatialFittingAbstractPipelineBuilder.cpp
  core/lancetSpatialFittingNavigationToolCollector.cpp
  core/lancetSpatialFittingNavigationToolToNavigationToolFilter.cpp
  core/lancetSpatialFittingNavigationToolToSpaceFilter.cpp
  core/lancetSpatialFittingPipelineBuilder.cpp
  core/lancetSpatialFittingPipelineManager.cpp
  core/lancetSpatialFittingPointAccuracyDate.cpp
  core/lancetSpatialFittingRoboticsRegisterDirector.cpp
  core/lancetSpatialFittingRoboticsVerifyDirector.cpp
  core/lancetSpatialFittingProbeCheckPointDirector.cpp
  core/lancetSpatialFittingPelvicCheckPointDirector.cpp
  core/lancetSpatialFittingPelvicCheckPointVerifyDirector.cpp
  core/lancetSpatialFittingPelvicMarkPointDirector.cpp
  core/lancetSpatialFittingPelvicRegisterDirector.cpp
  core/lancetSpatialFittingPelvicVerifyDirector.cpp
  
)

set(INTERNAL_CPP_FILES
  lancetPluginActivator.cpp
  lancetSpatialFittingService.cpp
  lancetSpatialFittingRoboticsRegisterModel.cpp
  lancetSpatialFittingProbeCheckPointModel.cpp
  lancetSpatialFittingPelvisCheckPointModel.cpp
  lancetSpatialFittingPelvisMarkPointModel.cpp
  lancetSpatialFittingPelvisRegisterModel.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
