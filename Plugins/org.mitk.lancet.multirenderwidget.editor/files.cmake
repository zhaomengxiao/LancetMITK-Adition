set(SRC_CPP_FILES
  QMultiRenderWidgetEditor.cpp
)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_multirenderwidget_editor_Activator.cpp
)

set(MOC_H_FILES
  src/QMultiRenderWidgetEditor.h
  src/internal/org_mitk_lancet_multirenderwidget_editor_Activator.h
)

set(UI_FILES
  src/QMultiRenderWidgetEditor.ui
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/icon.xpm
)

set(QRC_FILES
  resources/resources.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
