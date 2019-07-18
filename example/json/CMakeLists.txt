# Example project

include_directories(${CMAKE_SOURCE_DIR}/include
					${CMAKE_SOURCE_DIR}/include/json
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})
					
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIR_SRCS)

add_executable(example-json
               ${DIR_SRCS})
