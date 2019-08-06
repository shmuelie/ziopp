cmake_minimum_required(VERSION 3.12)
project("ziopp")

set(ZIOPP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
set(ZIOPP_HEADERS ${ZIOPP_INCLUDE}/ziopp/upath.h ${ZIOPP_INCLUDE}/ziopp/filesystem.h ${ZIOPP_INCLUDE}/ziopp/upath_iterator.h ${ZIOPP_INCLUDE}/ziopp/filesystem_watcher.h)
set(ZIOPP_SOURCE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/src/ziopp/upath.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ziopp/filesystem.cpp)

add_library(ziopp ${ZIOPP_HEADERS} ${ZIOPP_SOURCE_CODE})

set_target_properties(ziopp PROPERTIES
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		MAP_IMPORTED_CONFIG_MINSIZEREL Release
		MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		VERSION 1.0.0.0)
target_include_directories(ziopp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)