pkg_search_module(pkg-sqlite3 REQUIRED sqlite3)
message(STATUS "sqlite3 library path: ${pkg-sqlite3_LDFLAGS}")

file(GLOB DATABASE_SOURCES "*.cpp")
add_library(database STATIC ${DATABASE_SOURCES})
target_link_libraries(database ${pkg-sqlite3_LDFLAGS} common)


add_executable(test-db test/DatabaseTest.cpp)
target_link_libraries(test-db database)

