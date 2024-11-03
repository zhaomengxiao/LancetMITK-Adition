set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_NeurosurgicalPunctureRobot_Activator.cpp
  NeurosurgicalPunctureRobot.cpp
  MPRMaker.cpp
  vtkResliceCallBack.cpp
  vtkResliceCursorCallBack.cpp
  vtkResliceWidget.cpp
  vtkWidgetBase.cpp
  vtkInclude.h
  Utility.h
  vtkWidgetMPR.h
)

set(UI_FILES
  src/internal/NeurosurgicalPunctureRobotControls.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_NeurosurgicalPunctureRobot_Activator.h
  src/internal/NeurosurgicalPunctureRobot.h
  src/internal/MPRMaker.h
  src/internal/vtkResliceCallBack.cpp
  src/internal/vtkResliceCursorCallBack.cpp
  src/internal/vtkResliceWidget.cpp
  src/internal/vtkWidgetBase.cpp
  src/internal/vtkWidgetMPR.cpp
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  resources/icon.xpm
  plugin.xml
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
