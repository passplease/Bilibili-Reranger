cmake_minimum_required(VERSION 3.27)
project(BiliBili_Reranger)

set(CMAKE_CXX_STANDARD 20)
set(DCMAKE_VERBOSE_MAKEFILE ON)# 输出构建日志
add_compile_options(/utf-8)

add_executable(${PROJECT_NAME} src/main.cpp
        src/develop/flags.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Include
add_subdirectory(api)
if(DEFINED API)
    message(STATUS "Find API DLL, adding ...")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${API})
    add_dependencies(${PROJECT_NAME} ${API})
else ()
    message(FATAL_ERROR "Can not find API DLL, cmake failed")
endif ()

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

add_custom_target(
        COPY_ALL_DLLS
        COMMENT "Copying all plugin dlls to right path"
)
add_custom_target(
        COPY_ALL_PDBS
        COMMENT "Copying all plugin pdbs to right path (prepare for setting break point)"
)
# Subproject
add_subdirectory(plugins/ExamplePlugin)

# DLL Plugin (For Test)
message(STATUS "Finding Plugins...")
if(DEFINED DLL_DIR)
if(DEFINED DLL_NAME)
    message(STATUS "Adding Plugin: ${DLL_DIR}/${DLL_NAME}")
    include_directories(${DLL_DIR})
    link_directories(${DLL_DIR})
    #target_link_libraries(${PROJECT_NAME} PUBLIC "${DLL_NAME}.dll")加上这个意味着静态加载这个库
endif ()
endif ()

get_property(NEED_COPY GLOBAL PROPERTY COPY_ALL_DLLS)
if(NEED_COPY)
    add_dependencies(COPY_ALL_DLLS ${NEED_COPY})
endif ()

get_property(NEED_COPY_PDB GLOBAL PROPERTY COPY_ALL_PDBS)
if(NEED_COPY_PDB)
    add_dependencies(COPY_ALL_PDBS ${NEED_COPY_PDB})
endif ()

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEVELOP=true>
        $<$<NOT:$<CONFIG:Debug>>:DEVELOP=false>
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/DataFromBilibili.json")
    add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/test/DataFromBilibili.json" "${CMAKE_BINARY_DIR}/Debug/testing/DataFromBilibili.json"
            COMMENT "copying crawled data for test ..."
    )
endif ()