add_library  (Koordynacje2 md5.cpp)
add_executable (Koordynacje test.cpp)
target_link_libraries (Koordynacje Koordynacje2 ScheduleReader stringspecial)
# target_link_libraries (Generator2 RouteInfoGenerator2 HTMLGenerator stringspecial osmbase ScheduleReader)
