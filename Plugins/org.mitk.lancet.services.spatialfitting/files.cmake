set(MOC_H_FILES
  src/internal/lancetPluginActivator.h
  src/core/lancetSpatialFittingNavigationToolCollector.h
)

set(SRC_CPP_FILES
  lancetSpatialFittingAbstractDirector.cpp
  lancetSpatialFittingAbstractPipelineBuilder.cpp
  core/lancetSpatialFittingNavigationToolCollector.cpp
  core/lancetSpatialFittingNavigationToolToNavigationToolFilter.cpp
  core/lancetSpatialFittingNavigationToolToSpaceFilter.cpp
  core/lancetSpatialFittingPipelineBuilder.cpp
  core/lancetSpatialFittingPipelineManager.cpp
  core/lancetSpatialFittingPointAccuracyDate.cpp
  core/lancetSpatialFittingRoboticsRegisterDirector.cpp
)

set(INTERNAL_CPP_FILES
  lancetPluginActivator.cpp
  lancetSpatialFittingRoboticsRegisterModel.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
