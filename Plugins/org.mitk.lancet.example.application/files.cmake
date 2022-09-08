set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_example_application_Activator.cpp
  
  QLancetExampleApplication.cpp
  QLancetExamplePerspective.cpp
  QLancetExampleWorkbenchAdvisor.cpp
  QLancetExampleWorkbenchWindowAdvisor.cpp
  QLancetWorkbenchWindowBarWidget.cpp
  QLancetWorkbenchWindowToolBarWidget.cpp
  QLancetWorkbenchWindowLeftToolBarWidget.cpp
)

set(UI_FILES
  src/internal/lancetworkbenchwindowbarfrom.ui
  src/internal/qlancetworkbenchwindowlefttoolbarform.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_example_application_Activator.h

  src/internal/QLancetExampleApplication.h
  src/internal/QLancetExamplePerspective.h
  src/internal/QLancetExampleWorkbenchAdvisor.h
  src/internal/QLancetExampleWorkbenchWindowAdvisor.h
  src/internal/QLancetWorkbenchWindowBarWidget.h
  src/internal/QLancetWorkbenchWindowToolBarWidget.h
  src/internal/QLancetWorkbenchWindowLeftToolBarWidget.h
)

set(CACHED_RESOURCE_FILES
  plugin.xml
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
