aux_source_directory(. MODULE_SOURCES)
add_library(httpclient OBJECT ${MODULE_SOURCES})

add_executable(test-httpget test/HttpGet.cpp)
target_link_libraries(test-httpget httpclient)
