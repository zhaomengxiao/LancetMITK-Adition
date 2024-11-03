option(BUILD_HardwareDeviceCmdApps "Build command-line apps for the Example module" ON)

if(BUILD_HardwareDeviceCmdApps)
  mitkFunctionCreateCommandLineApp(
    NAME HardwareDeviceCmdApp
    DEPENDS MitkExampleModule
  )
endif()
