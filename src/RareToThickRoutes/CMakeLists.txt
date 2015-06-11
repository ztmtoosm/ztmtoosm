add_library (RareToThickRoutes2 dij_data.cpp dijkstra.cpp)
add_executable (RareToThickRoutes RareToThickRoutes.cpp)
target_link_libraries (RareToThickRoutes RareToThickRoutes2 fcgi stringspecial osmbase)
