cmake_minimum_required(VERSION 3.8)
project(EocSystem)
option(EocSystem_BUILD_TESTS "Build tests." OFF)

aux_source_directory(${EocSystem_SOURCE_DIR}/src/e/system EocSystem_DIR_SRCS)
add_library(EocSystem ${EocSystem_DIR_SRCS})
target_include_directories(EocSystem PUBLIC ${EocSystem_SOURCE_DIR}/src)
set_property(TARGET EocSystem PROPERTY CXX_STANDARD 17)
set_property(TARGET EocSystem PROPERTY CXX_STANDARD_REQUIRED ON)

if(EocSystem_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()