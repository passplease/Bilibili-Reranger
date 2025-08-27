cmake_minimum_required(VERSION 3.27)
project(ExamplePlugin)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILEON ON)# 输出构建日志
add_compile_options(/utf-8)

add_library(${PROJECT_NAME} SHARED main.cpp
        interface.h
        tasks.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${API})
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

# Plugin Interface (For Test)
set(DLL_DIR "${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}" PARENT_SCOPE)
set(DLL_NAME ${PROJECT_NAME} PARENT_SCOPE)

# Copy .dll and .pdb under /plugins, act after each build task
add_custom_target(
        "${PROJECT_NAME}_COPY_DLL"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Debug/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_BINARY_DIR}/Debug/plugins" # copy
        COMMENT "dealing with plugin dll files ..."
)
add_custom_target(
        "${PROJECT_NAME}_COPY_PDB"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CMAKE_BINARY_DIR}/Debug/plugins" # copy
        COMMENT "dealing with plugin pdb files ..."
)# .pdb file for debug set break point

add_dependencies("${PROJECT_NAME}_COPY_DLL" ${PROJECT_NAME})
add_dependencies("${PROJECT_NAME}_COPY_PDB" "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_DLLS "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_PDBS "${PROJECT_NAME}_COPY_PDB")