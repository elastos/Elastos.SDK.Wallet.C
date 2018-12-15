cmake_minimum_required(VERSION 3.12)

include(config/cmake/CMakeLists.txt)

project(Elastos.SDK.Wallet.C)
add_subdirectory(src)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Elastos.SDK.Wallet.C.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/Elastos.SDK.Wallet.C.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Elastos.SDK.Wallet.C.pc DESTINATION lib/pkgconfig)
