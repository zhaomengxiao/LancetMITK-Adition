project(org_mitk_lancet_CZXtest)

mitk_create_plugin(
  EXPORT_DIRECTIVE CZXTEST_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt MitkIGTUI MitkLancetIGT MitkLancetRobot MitkLancetAlgo MitkGizmo MitkBoundingShape
  MODULE_DEPENDS MitkLancetPrintDataHelper MitkLancetFileIO MitkLancetHardwareDevice
  PACKAGE_DEPENDS PRIVATE VTK|FiltersFlowPaths
)

set(NLOHMANN_INCLUDE "")

set(NLOHMANN_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann")
include_directories(${NLOHMANN_INCLUDE}) 

target_compile_features(org_mitk_lancet_CZXtest PUBLIC cxx_std_20)

target_compile_definitions(org_mitk_lancet_CZXtest
    PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>
)
set(ZIP_FILE "${MITK_LOCAL_EP_PATH}/MeshLibDist_v2.4.2.198.zip")
set(OUTPUT_DIR ${MITK_BINARY_DIR})

message("MeshLib unzip OUTPUT_DIR: ${MITK_BINARY_DIR}")

execute_process(
    COMMAND tar -xvf ${ZIP_FILE} -C ${OUTPUT_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(LANCET_MESHLIB "${MITK_BINARY_DIR}/MeshLibDist_v2.4.2.198")

set(LANCET_MESHLIB_INCLUDE "")
set(LANCET_MESHLIB_LIB_DEBUG "")
set(LANCET_MESHLIB_LIB_RELEASE "")
set(LANCET_MESHLIB_LIB "")
set(LANCET_MESHLIB_BIN "")
set(LANCET_MESHLIB_DLLS_DEBUG "")
set(LANCET_MESHLIB_DLLS_RELEASE "")

set(LANCET_ARCHITECTURE_BIT "")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(LANCET_ARCHITECTURE_BIT "X64")
else()
  set(LANCET_ARCHITECTURE_BIT "X86")
endif()

set(LANCET_MESHLIB_INCLUDE "${LANCET_MESHLIB}/install/include")
include_directories(${LANCET_MESHLIB_INCLUDE})

set(LANCET_MESHLIB_BIN "${LANCET_MESHLIB}/install/app")
set(LANCET_MESHLIB_LIB "${LANCET_MESHLIB}/install/lib")

file(GLOB LANCET_MESHLIB_LIB_DEBUG "${LANCET_MESHLIB_LIB}/Debug/*.lib")
file(GLOB LANCET_MESHLIB_LIB_RELEASE "${LANCET_MESHLIB_LIB}/Release/*.lib")

file(GLOB LANCET_MESHLIB_DLLS_DEBUG "${LANCET_MESHLIB_BIN}/Debug/*.dll")
file(GLOB LANCET_MESHLIB_DLLS_RELEASE "${LANCET_MESHLIB_BIN}/Release/*.dll")

foreach(lib ${LANCET_MESHLIB_LIB_DEBUG})
    target_link_libraries(org_mitk_lancet_CZXtest PRIVATE
$<$<CONFIG:Debug>:${lib}>)
endforeach()

foreach(lib ${LANCET_MESHLIB_LIB_RELEASE})
    target_link_libraries(org_mitk_lancet_CZXtest PRIVATE
$<$<CONFIG:Release>:${lib}>)
endforeach()

#message("LANCET_MESHLIB_LIB_DEBUG: ${LANCET_MESHLIB_LIB_DEBUG}")
#message("LANCET_MESHLIB_LIB_RELEASE: ${LANCET_MESHLIB_LIB_RELEASE}")

#target_link_libraries(${PLUGIN_TARGET} PUBLIC ${LANCET_MESHLIB_LIB_RELEASE})

foreach(dll ${LANCET_MESHLIB_DLLS_RELEASE})
    file(COPY ${dll} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Release")
endforeach()

foreach(dll ${LANCET_MESHLIB_DLLS_DEBUG})
    file(COPY ${dll} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Debug")
endforeach()

#file(COPY ${LANCET_MESHLIB_DLLS_DEBUG} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Debug")  
#file(COPY ${LANCET_MESHLIB_DLLS_RELEASE} DESTINATION "${MITK_BINARY_DIR}/bin/plugins/Release")