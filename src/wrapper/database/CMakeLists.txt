pkg_search_module(pkg-sqlite3 REQUIRED sqlite3)
message(STATUS "sqlite3 include path: ${pkg-sqlite3_INCLUDE_DIRS}")
message(STATUS "sqlite3 library path: ${pkg-sqlite3_STATIC_LDFLAGS}")

file(GLOB DATABASE_SOURCES "*.cpp")
add_library(database STATIC ${DATABASE_SOURCES})
target_include_directories(database PRIVATE "${pkg-sqlite3_INCLUDE_DIRS}")
target_link_libraries(database common)
target_link_libraries(database ${pkg-sqlite3_STATIC_LDFLAGS})

if(DEFINED CFG_WITH_TEST)
	add_executable(test-db test/DatabaseTest.cpp)
	target_link_libraries(test-db database)
endif()

