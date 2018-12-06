aux_source_directory(. MODULE_SOURCES)
add_library(database STATIC ${MODULE_SOURCES})
target_link_libraries(database sqlite3 common)


add_executable(test-db test/DatabaseTest.cpp)
target_link_libraries(test-db database)

