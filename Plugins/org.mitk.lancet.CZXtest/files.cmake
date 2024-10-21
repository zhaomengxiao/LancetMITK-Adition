set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_CZXtest_Activator.cpp
  CzxTest.cpp
  ModelRegistration.cpp
  AngleCalculationHelper.cpp
  PKARenderHelper.cpp
  PKAData.cpp
  CalculationHelper.cpp
  PKAPrePlanBoneModel.cpp
  ChunLiXGImplant.cpp
  ChunLiXKImplant.cpp
  PKAEnum.h
  JsonSerailizeHelper.h
  PKAHardwareDevice.cpp
  ChunLITray.cpp
  PKADianaAimHardwareDevice.cpp
  ToolDisplayHelper.cpp
  IntraOsteotomy.cpp
  PreoPreparation.cpp
  RobotJoint.cpp
  JointPartDescription.cpp
  RobotFrame.cpp
  RobotBase.cpp
  BoundingBoxInteraction.cpp
  RobotFrame2.cpp
  DianaAimHardwareService.cpp
  #PrePlanData.h
  #IntraBone.cpp
  #PKAProthesisModel.cpp
  #PKADataBase.cpp
  #PKACalculator.cpp
  #PKAModel.cpp
  #ModelUserControl.cpp
  #ModelInteractor.cpp
  #ModelMapper2D.cpp
  #ModelInterFactory.cpp
  #us_init.cpp
)

set(UI_FILES
  src/internal/CzxTestControls.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_CZXtest_Activator.h
  src/internal/CzxTest.h
  src/internal/ModelRegistration.h
  src/internal/AngleCalculationHelper.h
  src/internal/PKARenderHelper.h
  src/internal/PKAData.h
  src/internal/PKAPrePlanBoneModel.h
  src/internal/CalculationHelper.h
  src/internal/ChunLiXGImplant.h
  src/internal/ChunLiXKImplant.h
  src/internal/PKAHardwareDevice.h
  src/internal/ChunLITray.h
  src/internal/PKADianaAimHardwareDevice.h
  src/internal/ToolDisplayHelper.h
  src/internal/IntraOsteotomy.h
  src/internal/PreoPreparation.h
  src/internal/RobotJoint.h
  src/internal/JointPartDescription.h
  src/internal/RobotFrame.h
  src/internal/RobotBase.h
  src/internal/BoundingBoxInteraction.h
  src/internal/RobotFrame2.h
  src/internal/DianaAimHardwareService.h
  #src/internal/IntraBone.h
  #src/internal/PrePlanData.cpp
  #src/internal/PKAModel.h
  #src/internal/PKACalculator.h
  #src/internal/PKADataBase.h
  #src/internal/PKAProthesisModel.h
  #src/internal/PKAEnum.h
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  resources/icon.xpm
  plugin.xml
)

set(RESOURCE_FILES
  # "Interactions" prefix forced by mitk::StateMachine
  Interactions/Gizmo3DStates.xml
  Interactions/Gizmo3DConfig.xml
  Marker/PKADrill.aimtool
  Marker/PKAFemurRF.aimtool
  Marker/PKAProbe.aimtool
  Marker/PKARobotBaseRF.aimtool
  Marker/PKARobotEndRF.aimtool
  Marker/PKARobotEndRF_old.aimtool
  Marker/PKATibiaRF.aimtool
)


# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES

)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
