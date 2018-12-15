include_directories(
	${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/thirdparty
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${CFG_BUILD_ROOT}/include)

add_subdirectory(common)
add_subdirectory(wrapper/httpclient)
add_subdirectory(wrapper/database)

pkg_search_module(pkg-Elastos.ORG.Wallet.Lib.C REQUIRED Elastos.ORG.Wallet.Lib.C)
message(STATUS "Elastos.ORG.Wallet.Lib.C include path: ${pkg-Elastos.ORG.Wallet.Lib.C_INCLUDE_DIRS}")
message(STATUS "Elastos.ORG.Wallet.Lib.C library path: ${pkg-Elastos.ORG.Wallet.Lib.C_STATIC_LDFLAGS}")

file( GLOB Elastos.SDK.Wallet.C-SOURCES "*.cpp" )
file( GLOB Elastos.SDK.Wallet.C-HEADERS "*.h" )
add_library(Elastos.SDK.Wallet.C ${Elastos.SDK.Wallet.C-SOURCES})
target_include_directories(Elastos.SDK.Wallet.C PRIVATE "${pkg-Elastos.ORG.Wallet.Lib.C_INCLUDE_DIRS}")
target_link_libraries(Elastos.SDK.Wallet.C ${pkg-Elastos.ORG.Wallet.Lib.C_STATIC_LDFLAGS})
target_link_libraries(Elastos.SDK.Wallet.C common database httpclient Elastos.ORG.Wallet.Lib.C)
set_target_properties(Elastos.SDK.Wallet.C PROPERTIES PUBLIC_HEADER "${Elastos.SDK.Wallet.C-HEADERS}")
install(TARGETS Elastos.SDK.Wallet.C
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/Elastos.SDK.Wallet.C)

if(DEFINED CFG_WITH_TEST)
    add_executable(test-wallet test/WalletTest.cpp)
    target_link_libraries(test-wallet Elastos.SDK.Wallet.C)
endif()
