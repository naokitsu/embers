cmake_minimum_required(VERSION 3.5)
project(embers VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/shader.cc
        src/run.cc
        include/embers/run.h
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_INSTALL_INCLUDEDIR}/src
)

target_link_libraries(embers PRIVATE glfw)
target_link_libraries(embers PRIVATE glad)

