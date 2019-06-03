cmake_minimum_required(VERSION 3.14)
project("ziopp")

execute_process(COMMAND conan install -if "${CMAKE_BINARY_DIR}" ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt
		        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(ZIOPP_TESTS_HEADERS )
set(ZIOPP_TESTS_SOURCE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/test_upath.cpp)

add_executable(ziopp-tests ${ZIOPP_TESTS_HEADERS} ${ZIOPP_TESTS_SOURCE_CODE})
set_target_properties(ziopp-tests PROPERTIES
		LINKER_LANGUAGE CXX
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		MAP_IMPORTED_CONFIG_MINSIZEREL Release
		MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		VERSION 1.0.0.0)
target_include_directories(ziopp-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ziopp-tests PUBLIC CONAN_PKG::gtest ziopp)