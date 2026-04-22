cmake_minimum_required(VERSION 3.15)
project(Nimonspoli)

set(CMAKE_CXX_STANDARD 17)
set(TARGET nimonspoli)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(raylib REQUIRED)

add_subdirectory(include/views/raylib-cpp)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${TARGET} ${SOURCES})

if(NOT GUI_ENABLED)
    target_compile_definitions(${TARGET} PRIVATE GUI_ENABLED=0)
else()
    target_compile_definitions(${TARGET} PRIVATE GUI_ENABLED=${GUI_ENABLED})
endif()

target_include_directories(${TARGET} PRIVATE 
    include/core
    include/models
    include/utils
    include/views
    include/views/raylib-cpp
)

target_link_libraries(${TARGET} PRIVATE raylib raylib_cpp)