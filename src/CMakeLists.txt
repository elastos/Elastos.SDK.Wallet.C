include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(wrapper/httpclient)
add_subdirectory(wrapper/database)
