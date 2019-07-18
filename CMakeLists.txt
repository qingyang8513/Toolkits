# The minimum version required, but 2.8.11 is recommended.
cmake_minimum_required(VERSION 2.8.11)

project(toolkits-solution)

# Using C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set(CMAKE_BUILD_TYPE Release)

message("config types: ${CMAKE_CONFIGURATION_TYPES}")


include_directories(
        include
		include/timing
		include/json
        src
)

set(OUTPUT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(OUTPUT_ROOT_LIB ${OUTPUT_ROOT}/lib)
message(STATUS "OUTPUT_ROOT: " ${OUTPUT_ROOT})
message(STATUS "OUTPUT_ROOT_LIB: " ${OUTPUT_ROOT_LIB})
 
##############################
# build version
set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)
set(BUILD_NUMBER "${CMAKE_SOURCE_DIR}/attr/build_number")
set(VERSION_HEADER "${CMAKE_SOURCE_DIR}/include/version.h")

##############################
# add projects
add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(example/json)
