set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
  org_mitk_lancet_linkinghardware_Activator.cpp
  QLinkingHardware.cpp
)

set(UI_FILES
  src/internal/QLinkingHardwareControls.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_lancet_linkinghardware_Activator.h
  src/internal/QLinkingHardware.h
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  resources/icon.xpm
  plugin.xml
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
   resources/resources.qrc
)

set(CPP_FILES )

#! Copy runtime dependent resource files.
set(resources_dir ${CMAKE_CURRENT_SOURCE_DIR}/resources/IGTToolStorage)
set(resources_target_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/IGTToolStorage)
message("QLinkingHardware: resources_dir >> ${resources_dir}")
message("QLinkingHardware: resources_target_dir >> ${resources_target_dir}")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${resources_dir} ${resources_target_dir})

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
