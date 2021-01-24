
cmake_minimum_required(VERSION 3.19.3)

project(MANDEL)

include(cmake/CompilerWarnings.cmake)

add_compile_definitions(_DEBUG=1)
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLEW=1) # setup imgui with glew
add_compile_definitions(GLEW_STATIC=1)                   # use static

# -mwindows turns off the console and others make it shareble or whatever
#SET(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -mwindows")


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

#FindGLFW.cmake
find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)

find_library(GLFW_LIBRARY NAMES glfw glfw3)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)
#~FindGLFW.cmake

set(LIBRARY_SOURCE_DIR ../dev)

set(GLEW_SRC_DIR  ${LIBRARY_SOURCE_DIR}/glew)
set(IMGUI_SRC_DIR ${LIBRARY_SOURCE_DIR}/ImGui)

set(GLEW_SRC_FILES ${GLEW_SRC_DIR}/glew.c)

set(
    IMGUI_SRC_FILES
    ${IMGUI_SRC_DIR}/imgui.cpp
    ${IMGUI_SRC_DIR}/imgui_demo.cpp
    ${IMGUI_SRC_DIR}/imgui_draw.cpp
    ${IMGUI_SRC_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_SRC_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_SRC_DIR}/imgui_widgets.cpp
)


set(MANDEL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MANDEL_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/src    )

set(
    HEADER_FILES 
    ${MANDEL_INCLUDE_DIR}/utility.h 
    ${MANDEL_INCLUDE_DIR}/buffer_object.h 
    ${MANDEL_INCLUDE_DIR}/mandel.h 
    ${MANDEL_INCLUDE_DIR}/shader.h 
    ${MANDEL_INCLUDE_DIR}/uniform.h 
    ${MANDEL_INCLUDE_DIR}/vertex_array_object.h
    ${MANDEL_INCLUDE_DIR}/mandel_handler.h
)

set(
    SRC_FILES 
    ${MANDEL_SRC_DIR}/buffer_object.cpp 
    ${MANDEL_SRC_DIR}/main.cpp 
    ${MANDEL_SRC_DIR}/mandel.cpp 
    ${MANDEL_SRC_DIR}/shader.cpp 
    ${MANDEL_SRC_DIR}/utility.cpp 
    ${MANDEL_SRC_DIR}/vertex_array_object.cpp
    ${MANDEL_SRC_DIR}/mandel_handler.cpp
)

add_executable(mandel ${SRC_FILES} ${HEADER_FILES} ${GLEW_SRC_FILES} ${IMGUI_SRC_FILES})

target_link_libraries(
    mandel 
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARY}
)

# #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  # 
# # for precompiled
#
#   add_executable(mandel ${SRC_FILES} ${HEADER_FILES})
#
#   #link glew before opengl
#   target_link_libraries(
#       mandel 
#       ${GLEW_STATIC_LIBRARIES}
#       ${OPENGL_LIBRARIES}
#       ${GLFW_LIBRARY}
#   )
# #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  # 

target_include_directories(
    mandel PUBLIC
    ${MANDEL_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR} 
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR} 
    ${LIBRARY_SOURCE_DIR}
)

set_target_properties(
    mandel PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)