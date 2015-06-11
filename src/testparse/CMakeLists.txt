add_library (testparse2 szkielet.cpp testparse.cpp)
add_executable (testparse testparse.cpp)
target_link_libraries (testparse testparse2 osmbase ScheduleReader stringspecial RareToThickRoutes2)
