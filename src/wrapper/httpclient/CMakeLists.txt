aux_source_directory(. MODULE_SOURCES)
add_library(httpclient OBJECT ${MODULE_SOURCES})
target_link_libraries(httpclient curl)

add_executable(test-httpget test/HttpGet.cpp)
target_link_libraries(test-httpget httpclient)

add_executable(test-httppost test/HttpPost.cpp)
target_link_libraries(test-httppost httpclient)
