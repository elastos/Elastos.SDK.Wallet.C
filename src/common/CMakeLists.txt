file( GLOB COMMON_SOURCES "*.cpp" )

add_library(common ${COMMON_SOURCES})
set_target_properties(common PROPERTIES HEADERDIR "${CMAKE_CURRENT_SOURCE_DIR}")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	target_link_libraries(common log)
endif()
