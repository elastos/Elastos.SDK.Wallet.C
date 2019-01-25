file( GLOB COMMON_SOURCES "*.cpp" )

add_library(common OBJECT ${COMMON_SOURCES})
target_include_directories(common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	target_link_libraries(common log)
endif()
