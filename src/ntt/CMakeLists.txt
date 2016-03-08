add_library(sqlite3 sqlite3.c)
add_executable (mgen ToLite.cpp)
target_link_libraries (sqlite3 pthread dl)
target_link_libraries (mgen ScheduleReader sqlite3)
