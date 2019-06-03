cmake_minimum_required(VERSION 3.14)
project("ziopp")

set(ZIOPP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
set(ZIOPP_HEADERS ${ZIOPP_INCLUDE}/ziopp/upath.h)
set(ZIOPP_SOURCE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/src/ziopp/upath.cpp)

add_library(ziopp ${ZIOPP_HEADERS} ${ZIOPP_SOURCE_CODE})

set_target_properties(ziopp PROPERTIES
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		MAP_IMPORTED_CONFIG_MINSIZEREL Release
		MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		VERSION 1.0.0.0)
target_include_directories(ziopp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)