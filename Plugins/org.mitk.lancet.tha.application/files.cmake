set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_tha_application_Activator.cpp
  
  QLancetThaApplication.cpp
  QLancetThaWorkbenchMenuBar.cpp
  QLancetThaWorkbenBarAdvisor.cpp
  QLancetThaApplicationPerspective.cpp
  QLancetThaApplicationWorkbenchAdvisor.cpp
  QLancetThaApplicationWorkbenchWindowAdvisor.cpp
)

set(UI_FILES
  src/internal/qlancetthaworkbenchmenubarform.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_tha_application_Activator.h

  src/internal/QLancetThaApplication.h
  
  src/internal/QLancetThaWorkbenchMenuBar.h
  src/internal/QLancetThaWorkbenBarAdvisor.h
  src/internal/QLancetThaApplicationPerspective.h
  src/internal/QLancetThaApplicationWorkbenchWindowAdvisor.h
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
