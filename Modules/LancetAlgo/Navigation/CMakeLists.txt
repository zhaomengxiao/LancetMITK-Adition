include(files.cmake)

include_directories(./include)
include_directories(../Geometry/include)
include_directories(../Physiology/include)
add_library(navigation STATIC ${CPP_FILES} ${H_FILES})

target_link_libraries(navigation geometry physiology)

set_target_properties(navigation PROPERTIES DEBUG_POSTFIX d)

INSTALL(TARGETS navigation
		CONFIGURATIONS
		Debug
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/Debug
		RUNTIME DESTINATION Debug/bin
		ARCHIVE DESTINATION Debug/lib
		PUBLIC_HEADER DESTINATION Debug/include
		)


INSTALL(TARGETS navigation
		CONFIGURATIONS
		Release
		DESTINATION
		${CMAKE_INSTALL_PREFIX}/Release
		RUNTIME DESTINATION Release/bin
		ARCHIVE DESTINATION Release/lib
		PUBLIC_HEADER DESTINATION Release/include
		)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	install(FILES ${H_FILES} DESTINATION ${install_DIRECTORY}/include)
else()
	install(FILES ${H_FILES} DESTINATION ${install_DIRECTORY}/include)
endif()