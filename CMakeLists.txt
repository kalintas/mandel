
cmake_minimum_required(VERSION 3.19.3)

project(MANDEL)

include(cmake/CompilerWarnings.cmake)

find_package(OpenGL REQUIRED)

set(LIBRARY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../libs")

set(GLEW_SRC_FILES "${LIBRARY_SOURCE_DIR}/glew/glew.c")

set(
    IMGUI_SRC_FILES
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui.cpp"
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui_demo.cpp"
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui_draw.cpp"
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui_impl_glfw.cpp"
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui_impl_opengl3.cpp"
    "${LIBRARY_SOURCE_DIR}/ImGui/imgui_widgets.cpp"
)

set(MANDEL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(MANDEL_SRC_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/src    ")

set(
    HEADER_FILES 
    "${MANDEL_INCLUDE_DIR}/utility.h" 
    "${MANDEL_INCLUDE_DIR}/buffer_object.h" 
    "${MANDEL_INCLUDE_DIR}/mandel.h" 
    "${MANDEL_INCLUDE_DIR}/shader.h" 
    "${MANDEL_INCLUDE_DIR}/uniform.h" 
    "${MANDEL_INCLUDE_DIR}/vertex_array_object.h"
    "${MANDEL_INCLUDE_DIR}/mandel_handler.h"
    "${MANDEL_INCLUDE_DIR}/pch.h"
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
)

add_executable(mandel ${SRC_FILES} ${HEADER_FILES} ${GLEW_SRC_FILES} ${IMGUI_SRC_FILES})

set_project_warnings(mandel OFF)

target_compile_definitions(mandel PRIVATE _DEBUG=1)
target_compile_definitions(mandel PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW=1) # setup imgui with glew
target_compile_definitions(mandel PRIVATE GLEW_STATIC=1)                   # use static

target_link_libraries(
    mandel 
    ${OPENGL_LIBRARIES}
    "${LIBRARY_SOURCE_DIR}/glfw/libglfw3.a"
)


target_include_directories(
    mandel PUBLIC
    ${MANDEL_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${LIBRARY_SOURCE_DIR}
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

