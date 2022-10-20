set(MOC_H_FILES
  src/internal/lancetTest.h
  src/internal/lancetPluginActivator.h
)

set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
  lancetTest.cpp
  lancetPluginActivator.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
