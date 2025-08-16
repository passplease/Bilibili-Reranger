cmake_minimum_required(VERSION 3.27)
project(BiliBili_Reranger_API)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILEON ON)# 输出构建日志
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")# 正常输出日志
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(API ${PROJECT_NAME} PARENT_SCOPE)

add_library(${PROJECT_NAME} SHARED
        utils.cpp
        Util.h
        pluginInterface.h
        APIStatus.h
        pluginInterface.cpp
)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)# Allow export

target_compile_definitions(${PROJECT_NAME} PRIVATE API_DLL)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/Debug/${PROJECT_NAME}.dll"
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_BINARY_DIR}/Debug" # copy
        COMMENT "dealing with plugin dll files ..."
)
add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/Debug/${PROJECT_NAME}.pdb"
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/api/Debug/${PROJECT_NAME}.pdb" "${CMAKE_BINARY_DIR}/Debug" # copy
        COMMENT "dealing with plugin pdb files ..."
)

add_custom_target(
        "${PROJECT_NAME}_COPY_DLL"
        DEPENDS "${CMAKE_BINARY_DIR}/Debug/${PROJECT_NAME}.dll"
)
add_custom_target(
        "${PROJECT_NAME}_COPY_PDB"
        DEPENDS "${CMAKE_BINARY_DIR}/Debug/${PROJECT_NAME}.pdb"
)
add_dependencies("${PROJECT_NAME}_COPY_DLL" ${PROJECT_NAME})
add_dependencies("${PROJECT_NAME}_COPY_PDB" "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_DLLS "${PROJECT_NAME}_COPY_DLL")
set_property(GLOBAL APPEND PROPERTY COPY_ALL_PDBS "${PROJECT_NAME}_COPY_PDB")