file( GLOB HTTPCLIENT_SOURCES "*.cpp" )
add_library( httpclient STATIC ${HTTPCLIENT_SOURCES} )
get_target_property(common_headerdir common HEADERDIR)
target_include_directories( httpclient PRIVATE ${common_headerdir} )
target_link_libraries( httpclient common curl )

add_executable( test-httpget test/HttpGet.cpp )
target_link_libraries( test-httpget httpclient )

add_executable( test-httppost test/HttpPost.cpp )
target_link_libraries( test-httppost httpclient )
