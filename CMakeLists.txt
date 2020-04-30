cmake_minimum_required(VERSION 3.10)
project(Spheremarcher VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(OpenGL_GL_PREFERENCE GLVND)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

find_package(OpenGL)
file(GLOB app_src "src/*.cpp")
file(GLOB utils_src "src/utils/*.cpp")
file(GLOB imgui_src "dependency/imgui/*.cpp")

add_subdirectory(dependency/glew/build/cmake)
add_subdirectory(dependency/glfw)
add_subdirectory(dependency/glm)

# not included in glew cmake process, but required to succeed..
execute_process(
    COMMAND make extensions
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dependency/glew/auto
)


include_directories(src/)
include_directories(src/utils/)
include_directories(dependency/glew)
include_directories(dependency/glfw/include)
include_directories(dependency/glm)
include_directories(dependency/imgui)


add_executable(Spheremarcher ${app_src} ${utils_src} ${imgui_src})

target_link_libraries(Spheremarcher glew glfw glm OpenGL::GL)