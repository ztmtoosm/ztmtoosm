#add_library  (Koordynacje ScheduleReacer.cpp)
add_executable (Koordynacje test.cpp)
target_link_libraries (Koordynacje ScheduleReader stringspecial)
# target_link_libraries (Generator2 RouteInfoGenerator2 HTMLGenerator stringspecial osmbase ScheduleReader)
