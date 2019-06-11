cmake_minimum_required(VERSION 3.14)
project("ziopp")

set(TEST_TARGET_NAME ziopp-tests)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
set(CMAKE_BUILD_TYPE ${CMAKE_CONFIGURATION_TYPES})
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

set(ZIOPP_TESTS_HEADERS )
set(ZIOPP_TESTS_SOURCE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/test_upath.cpp)

add_executable(${TEST_TARGET_NAME} ${ZIOPP_TESTS_HEADERS} ${ZIOPP_TESTS_SOURCE_CODE})
set_target_properties(${TEST_TARGET_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		MAP_IMPORTED_CONFIG_MINSIZEREL Release
		MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		VERSION 1.0.0.0)
target_include_directories(${TEST_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TEST_TARGET_NAME} PUBLIC CONAN_PKG::gtest ziopp)
