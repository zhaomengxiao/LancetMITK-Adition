#!
#!
#! \param GROUP_NAME
#! \param TEST_PROJECTS
#! \param TEST_LINK_LIBRARIES
#! \param TEST_LINK_LIBRARIE_SOURCES
#! \param TEST_INPUT_DATA_DIRS
#!
#!
#!
#!
#!
function(lancetFunctionCreateQtTestGroup)

# options
set(arg_options EMPTY)

# single value arguments
set(arg_single PROJECT_GROUP_NAME)

# multiple value arguments
set(arg_multiple
  TEST_PROJECTS
  TEST_LINK_LIBRARIES
  TEST_LINK_LIBRARIE_SOURCES
  TEST_INPUT_DATA_DIRS
)

cmake_parse_arguments(_TESTTING "${arg_options}" "${arg_single}" "${arg_multiple}" ${ARGN})


if(_TESTTING_PROJECT_GROUP_NAME)
  message("_TESTTING_PROJECT_GROUP_NAME = ${_TESTTING_PROJECT_GROUP_NAME}")
else()
  message("Not input _TESTTING_PROJECT_GROUP_NAME")
endif()

if(_TESTTING_TEST_PROJECTS)
  message("_TESTTING_TEST_PROJECTS = ${_TESTTING_TEST_PROJECTS}")
  
  foreach(_test_project ${_TESTTING_TEST_PROJECTS})
    message("_TESTTING_TEST_PROJECTS::Item: ${_test_project}")
	add_subdirectory(${_test_project})
  endforeach()
else()
  message("Not input _TESTTING_TEST_PROJECTS")
endif()

if(_TESTTING_TEST_LINK_LIBRARIES)
  message("_TESTTING_TEST_LINK_LIBRARIES = ${_TESTTING_TEST_LINK_LIBRARIES}")
else()
  message("Not input _TESTTING_TEST_LINK_LIBRARIES")
endif()

if(_TESTTING_TEST_LINK_LIBRARIE_SOURCES)
  message("_TESTTING_TEST_LINK_LIBRARIE_SOURCES = ${_TESTTING_TEST_LINK_LIBRARIE_SOURCES}")
else()
  message("Not input _TESTTING_TEST_LINK_LIBRARIE_SOURCES")
endif()

if(_TESTTING_TEST_INPUT_DATA_DIRS)
  message("_TESTTING_TEST_INPUT_DATA_DIRS = ${_TESTTING_TEST_INPUT_DATA_DIRS}")
else()
  message("Not input _TESTTING_TEST_INPUT_DATA_DIRS")
endif()



endfunction()