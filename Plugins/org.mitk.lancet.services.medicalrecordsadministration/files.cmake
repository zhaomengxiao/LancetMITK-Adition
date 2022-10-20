set(MOC_H_FILES
  src/internal/lancetPluginActivator.h
  src/lancetIMedicalRecordsScanner.h
  src/lancetIMedicalRecordsProperty.h
  src/lancetIMedicalRecordsAdministrationService.h
  src/internal/lancetMedicalRecordsScanner.h
  src/internal/lancetMedicalRecordsAdministrationService.h
)

set(SRC_CPP_FILES
  lancetIMedicalRecordsLoader.cpp
  lancetIMedicalRecordsScanner.cpp
  lancetIMedicalRecordsProperty.cpp
  lancetIMedicalRecordsAdministrationService.cpp
)

set(INTERNAL_CPP_FILES
  lancetPluginActivator.cpp
  lancetMedicalRecordsLoader.cpp
  lancetMedicalRecordsProperty.cpp
  lancetMedicalRecordsScanner.cpp
  lancetMedicalRecordsAdministrationService.cpp
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
