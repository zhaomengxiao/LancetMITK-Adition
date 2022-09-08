set(SRC_CPP_FILES
  QLancetExampleWidgetEditor.cpp
)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_example_editor_Activator.cpp
  QLancetExampleWidgetEditorPreferencePage.cpp
  acetabularpreparationwidget.cpp
  QLancetExampleEditorControl.cpp
)

set(MOC_H_FILES
  src/QLancetExampleWidgetEditor.h

  src/internal/org_mitk_lancet_example_editor_Activator.h
  src/internal/QLancetExampleWidgetEditorPreferencePage.h
  src/internal/acetabularpreparationwidget.h
  src/internal/QLancetExampleEditorControl.h
)

set(UI_FILES
  src/internal/QmitkStdMultiWidgetEditorPreferencePage.ui
  src/internal/acetabularpreparationwidget.ui
  src/internal/qlancetexampleeditorfrom.ui
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/StdMultiWidgetEditor.svg
)

set(QRC_FILES
  resources/QmitkStdMultiWidgetEditor.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
