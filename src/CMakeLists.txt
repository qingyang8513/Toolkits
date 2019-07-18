macro(source_group_by_dir all_files strip_path)
    #message("${strip_path}")
    if(MSVC)    
        foreach(single_file ${${all_files}})
            #message("${single_file}")
            string(REGEX REPLACE "(${strip_path})/(.*)$" \\2 relative_fpath ${single_file})
            string(REGEX MATCH "^(.*)/(.*)\\.[^.]*$" dummy ${relative_fpath})
            set(file_path ${CMAKE_MATCH_1})
            #message("${file_path}")
            string(REPLACE "/" "\\" file_group_name ${file_path})
            #message("group name: ${file_group_name}")
            source_group(${file_group_name} FILES ${single_file})
        endforeach(single_file)
    endif(MSVC)
endmacro(source_group_by_dir)


file(GLOB_RECURSE toolkits_include "${CMAKE_SOURCE_DIR}/include/*.h*")
source_group_by_dir(toolkits_include "${CMAKE_SOURCE_DIR}/include")

file(GLOB_RECURSE toolkits_cxx "${CMAKE_CURRENT_SOURCE_DIR}/*.c*")
source_group_by_dir(toolkits_cxx ${CMAKE_CURRENT_SOURCE_DIR})


set(SOURCE_FILES
    ${toolkits_include}
    ${toolkits_cxx})

add_library(toolkits STATIC ${SOURCE_FILES})

#######################
# install configuration
install(TARGETS toolkits
		DESTINATION ${OUTPUT_ROOT_LIB})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
		DESTINATION ${MY_OUTPUT_ROOT}/include/
		FILES_MATCHING PATTERN "*.h")
