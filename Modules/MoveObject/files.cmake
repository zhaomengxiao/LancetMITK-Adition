file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(CPP_FILES
  DataManagement/mitkMoveObjectUtil.cpp
  DataManagement/mitkMoveObject.cpp
  DataManagement/mitkMoveObjectObjectFactory.cpp
  Interactions/mitkMoveObjectInteractor.cpp
  Rendering/mitkMoveObjectVtkMapper2D.cpp
  Rendering/mitkMoveObjectVtkMapper3D.cpp
)

set(RESOURCE_FILES
  Interactions/MoveObjectMouseConfig.xml
  Interactions/MoveObjectInteraction.xml
)
