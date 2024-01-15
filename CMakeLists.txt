cmake_minimum_required(VERSION 3.5)
project(embers VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(
        ${PROJECT_NAME}
        SHARED
        src/test.h
        src/hello-world.cc
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_INSTALL_INCLUDEDIR}/src
)



