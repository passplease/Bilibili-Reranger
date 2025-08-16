cmake_minimum_required(VERSION 3.27)
project(ExamplePlugin)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILEON ON)# 输出构建日志

add_library(${PROJECT_NAME} SHARED main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${API})
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

# Plugin Interface (For Test)
set(DLL_DIR "${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}" PARENT_SCOPE)
set(DLL_NAME ${PROJECT_NAME} PARENT_SCOPE)

# Copy .dll and .pdb under /plugins, act after each build task
add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/Debug/plugins/${PROJECT_NAME}.dll"
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_BINARY_DIR}/Debug/plugins" # copy
        COMMENT "dealing with plugin dll files ..."
)
add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/Debug/plugins/${PROJECT_NAME}.pdb"
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/Debug/${PROJECT_NAME}.pdb" "${CMAKE_BINARY_DIR}/Debug/plugins" # copy
        COMMENT "dealing with plugin pdb files ..."
)# .pdb file for debug set break point

add_custom_target(
        "${PROJECT_NAME}_COPY_DLL"
        DEPENDS "${CMAKE_BINARY_DIR}/Debug/plugins/${PROJECT_NAME}.dll"
)
add_custom_target(
        "${PROJECT_NAME}_COPY_PDB"
        DEPENDS "${CMAKE_BINARY_DIR}/Debug/plugins/${PROJECT_NAME}.pdb"
)
add_dependencies("${PROJECT_NAME}_COPY_DLL" ${PROJECT_NAME})
add_dependencies("${PROJECT_NAME}_COPY_PDB" "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_DLLS "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_PDBS "${PROJECT_NAME}_COPY_PDB")