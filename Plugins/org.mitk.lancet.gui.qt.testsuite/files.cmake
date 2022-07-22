set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_gui_qt_testsuite_Activator.cpp
  DevicePerspective.cpp
  ImageRegistPerspective.cpp
  TestSuiteApplication.cpp
  TestSuiteWorkbenchWindowAdvisor.cpp
)

set(UI_FILES
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_gui_qt_testsuite_Activator.h
  src/internal/DevicePerspective.h
  src/internal/ImageRegistPerspective.h
  src/internal/TestSuiteApplication.h
  src/internal/TestSuiteWorkbenchWindowAdvisor.h
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/perspectiveIcon1.png
  resources/perspectiveIcon2.png
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
