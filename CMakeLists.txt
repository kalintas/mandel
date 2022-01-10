
cmake_minimum_required(VERSION 3.19.3)

project(MANDEL)

include(cmake/CompilerWarnings.cmake)

set(MANDEL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(MANDEL_SRC_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(
    HEADER_FILES 
    "${MANDEL_INCLUDE_DIR}/utility.hpp" 
    "${MANDEL_INCLUDE_DIR}/buffer_object.hpp" 
    "${MANDEL_INCLUDE_DIR}/mandel.hpp" 
    "${MANDEL_INCLUDE_DIR}/shader.hpp" 
    "${MANDEL_INCLUDE_DIR}/uniform.hpp" 
    "${MANDEL_INCLUDE_DIR}/vertex_array_object.hpp"
    "${MANDEL_INCLUDE_DIR}/mandel_handler.hpp"
    "${MANDEL_INCLUDE_DIR}/pch.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_backend/imgui_impl_glfw.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_backend/imgui_impl_opengl3.h"
)

set(
    SRC_FILES 
    "${MANDEL_SRC_DIR}/buffer_object.cpp" 
    "${MANDEL_SRC_DIR}/main.cpp" 
    "${MANDEL_SRC_DIR}/mandel.cpp" 
    "${MANDEL_SRC_DIR}/shader.cpp" 
    "${MANDEL_SRC_DIR}/utility.cpp" 
    "${MANDEL_SRC_DIR}/vertex_array_object.cpp"
    "${MANDEL_SRC_DIR}/mandel_handler.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_backend/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_backend/imgui_impl_opengl3.cpp"
)

add_executable(mandel ${SRC_FILES} ${HEADER_FILES} ${GLEW_SRC_FILES} ${IMGUI_SRC_FILES})

set_project_warnings(mandel OFF)

target_compile_definitions(mandel PRIVATE _DEBUG=1)
target_compile_definitions(mandel PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW=1) # setup imgui with glew
target_compile_definitions(mandel PRIVATE GLEW_STATIC=1)                   # use static

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
target_link_libraries(mandel PRIVATE GLEW::GLEW)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(mandel PRIVATE imgui::imgui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(mandel PRIVATE glfw)


target_include_directories(mandel PRIVATE 
    ${MANDEL_INCLUDE_DIR} 
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_backend"
)

set_target_properties(
    mandel PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# copy res file to executable directory
add_custom_command(TARGET mandel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:mandel>/res")


if (CMAKE_CXX_COMPILE_ID STREQUAL "GNU")
    target_compile_options(mandel PRIVATE "-static-libgcc -static-libstdc++ -mwindows")
endif()

