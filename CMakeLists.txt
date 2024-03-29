cmake_minimum_required(VERSION 3.10)
project(Spheremarcher VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(OpenGL_GL_PREFERENCE GLVND)

set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)

set(pmp_DIR dependency/pmp-library)
set(PMP_BUILD_APPS OFF CACHE BOOL "" FORCE)
set(PMP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(PMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)

find_package(OpenGL)

file(GLOB app_src "src/*.cpp")
file(GLOB utils_src "src/utils/*.cpp")
file(GLOB raytrace_src "src/utils/raytracing/*.cpp")
file(GLOB imgui_src "dependency/pmp-library/external/imgui/*.cpp")

add_subdirectory(dependency/glm)
add_subdirectory(docs)

add_subdirectory(dependency/pmp-library)

include_directories(src/)
include_directories(src/utils/)
include_directories(src/utils/raytracing/)
include_directories(res/shaders/)
include_directories(res/meshes/)

include_directories(dependency/glm)
include_directories(dependency/pmp-library/external/eigen)
include_directories(dependency/pmp-library/external/glew)
include_directories(dependency/pmp-library/external/glfw/include)
include_directories(dependency/pmp-library/external/imgui)

add_executable(Spheretracer ${app_src} ${utils_src} ${imgui_src} ${raytrace_src})

add_custom_target(copy-res ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    ${CMAKE_BINARY_DIR}/res
    )

add_dependencies(Spheretracer copy-res)

target_link_libraries(Spheretracer glew glfw glm OpenGL::GL pmp)
