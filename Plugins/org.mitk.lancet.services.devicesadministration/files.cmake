set(MOC_H_FILES
  src/lancetIDevicesAdministrationService.h
  src/lancetIDevicesScanner.h
  src/internal/lancetPluginActivator.h
  src/internal/lancetDevicesAdministrationService.h
  src/internal/lancetDevicesScanner.h
  
)

set(SRC_CPP_FILES
  lancetIDevicesAdministrationService.cpp
  lancetIDevicesScanner.cpp
)

set(INTERNAL_CPP_FILES
  lancetPluginActivator.cpp
  lancetDevicesAdministrationService.cpp
  lancetDevicesScanner.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
