file( GLOB COMMON_SOURCES "*.cpp" )

add_library(common OBJECT ${COMMON_SOURCES})
set_target_properties(common PROPERTIES HEADERDIR "${CMAKE_CURRENT_SOURCE_DIR}")
