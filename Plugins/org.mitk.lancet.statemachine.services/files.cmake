set(MOC_H_FILES
  src/lancetIScxmlStateMachineState.h
  src/internal/lancetScxmStateMachineState.h
  src/lancetIScxmStateMachineHandler.h
  src/internal/lancetScxmStateMachineHandler.h
  src/internal/lancetScxmlStateMachineService.h
  src/internal/lancetPluginActivator.h
)

set(SRC_CPP_FILES
  lancetIScxmlStateMachineState.cpp
  lancetIScxmlStateMachineLoader.cpp
  lancetIScxmStateMachineHandler.cpp
  lancetIStateMachineActionProperty.cpp
  lancetIScxmlStateMachineService.cpp
)

set(INTERNAL_CPP_FILES
  lancetNamingFormats.cpp
  lancetPluginActivator.cpp
  lancetScxmStateMachineState.cpp
  lancetScxmStateMachineHandler.cpp
  lancetScxmlStateMachineService.cpp
  lancetStateMachineActionProperty.cpp
  lancetScxmlStateMachineLoader.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
