set(MOC_H_FILES
  src/internal/lancetPluginActivator.h
  src/lancetSpatialFittingAbstractService.h
  src/internal/lancetSpatialFittingService.h
  src/core/lancetSpatialFittingNavigationToolCollector.h
  src/internal/moduls/lancetSpatialFittingPelvisCheckPointModel.h
  src/internal/moduls/lancetSpatialFittingProbeCheckPointModel.h
  src/internal/moduls/lancetSpatialFittingRoboticsRegisterModel.h
  src/internal/moduls/lancetSpatialFittingPelvicRoughRegistrationsModel.h
  src/internal/moduls/lancetSpatialFittingPelvicPrecisionRegistrationsModel.h
)

set(SRC_CPP_FILES
  lancetSpatialFittingAbstractService.cpp
  lancetSpatialFittingAbstractDirector.cpp
  lancetSpatialFittingAbstractPipelineBuilder.cpp
  lancetSpatialFittingAbstractModel.cpp
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
  core/lancetSpatialFittingPelvicRoughRegistrationsDirector.cpp
  core/lancetSpatialFittingPelvicCheckPointVerifyDirector.cpp
  core/lancetSpatialFittingPelvicRoughRegistrationsVerifyDirector.cpp
)

set(INTERNAL_CPP_FILES
  lancetPluginActivator.cpp
  lancetSpatialFittingService.cpp
  lancetSpatiaFittingModulsFactor.cpp
  moduls/lancetSpatialFittingPelvisCheckPointModel.cpp
  moduls/lancetSpatialFittingProbeCheckPointModel.cpp
  moduls/lancetSpatialFittingRoboticsRegisterModel.cpp
  moduls/lancetSpatialFittingAbstractPelvicRegistrationsModel.cpp
  moduls/lancetSpatialFittingPelvicRoughRegistrationsModel.cpp
  moduls/lancetSpatialFittingPelvicPrecisionRegistrationsModel.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
