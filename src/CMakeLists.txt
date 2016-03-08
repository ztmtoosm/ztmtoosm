cmake_minimum_required (VERSION 2.6)
project (ztmtoosm C CXX)    
set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -std=c++11")

include_directories (xml RareToThickRoutes rapidjson stringspecial osmbase HTMLGenerator RouteInfoGenerator ntt ${CMAKE_BINARY_DIR})


get_filename_component(MYPROJECT_DIR ${CMAKE_BINARY_DIR} PATH)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MYPROJECT_DIR}/bin)
configure_file(${CMAKE_SOURCE_DIR}/conf.h.in ${CMAKE_BINARY_DIR}/conf.h)

add_subdirectory (stringspecial)
add_subdirectory (osmbase)
add_subdirectory (HTMLGenerator)
add_subdirectory (RouteInfoGenerator)
add_subdirectory (ScheduleReader)
add_subdirectory (testparse)
add_subdirectory (RareToThickRoutes)
add_subdirectory (ntt)

# add_library (ztmtoosm_general ${ALL_SOURCES})

#find_package (Doxygen)
#if (DOXYGEN_FOUND)
#    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#    add_custom_target(doc
#            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#            COMMENT "Generating API documentation with Doxygen"
#            )
#endif (DOXYGEN_FOUND)
