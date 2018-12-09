include_directories(
	${PROJECT_SOURCE_DIR}/src
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(wrapper/httpclient)
add_subdirectory(wrapper/database)

file( GLOB Elastos.SDK.Wallet.C-SOURCES "*.cpp" )
file( GLOB Elastos.SDK.Wallet.C-HEADERS "*.h" )
add_library(Elastos.SDK.Wallet.C ${Elastos.SDK.Wallet.C-SOURCES})
set_target_properties(Elastos.SDK.Wallet.C PROPERTIES PUBLIC_HEADER "${Elastos.SDK.Wallet.C-HEADERS}")
target_link_libraries(Elastos.SDK.Wallet.C common database httpclient)
install(TARGETS Elastos.SDK.Wallet.C
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/Elastos.SDK.Wallet.C)
