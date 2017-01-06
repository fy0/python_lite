
cmake_minimum_required(VERSION 2.8)

project (pylite)
set(CMAKE_BUILD_TYPE Debug)
#set(build_target shell)
set(build_target playground)

cmake_policy(SET CMP0015 OLD)

macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)

if (!MSVC)
#    set(CMAKE_C_FLAGS_DEBUG "/MTd")
#    set(CMAKE_C_FLAGS_RELEASE "/MT")
#    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
#    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
#else(MSVC)
    set(CMAKE_C_FLAGS "-Wall")
    set(CMAKE_C_FLAGS "-std=c99")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
endif(MSVC)

add_definitions(-DUNICODE -D_UNICODE)

IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(-DPYLT_DEBUG)
ENDIF()

file(GLOB_RECURSE project_headers src/*.h)
file(GLOB_RECURSE project_cpps src/*.c)
set (SRC_LIST ${project_headers} ${project_cpps})

source_group_by_dir(SRC_LIST)

IF(${build_target} MATCHES "shell")
    # shell
    add_definitions(-DSHELL)
    add_executable(pylite ${SRC_LIST})

    if (NOT MSVC)
        target_link_libraries(pylite m)
    endif(NOT MSVC)
ELSEIF(${build_target} MATCHES "playground")
    # playground
    add_definitions(-DPLAYGROUND)
    add_executable(pylite ${SRC_LIST})

    if (NOT MSVC)
        target_link_libraries(pylite m)
    endif(NOT MSVC)
ENDIF()

