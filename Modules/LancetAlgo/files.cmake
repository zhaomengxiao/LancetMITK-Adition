file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*" "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/include/*" "${CMAKE_CURRENT_SOURCE_DIR}/Physiology/include/*" "${CMAKE_CURRENT_SOURCE_DIR}/Navigation/include/*")
set(H_FILES
  Physiology/include/physioConst.h
  Physiology/include/physioModelFactory.h
  Physiology/include/physioModels.h
  Geometry/include/basic.h
  Geometry/include/leastsquaresfit.h
)
set(CPP_FILES
  Physiology/src/physioModelFactory.cpp
  Physiology/src/physioModels.cpp
  Geometry/src/basic.cpp
  Geometry/src/leastsquaresfit.cpp
)

#[[set(RESOURCE_FILES
)]]