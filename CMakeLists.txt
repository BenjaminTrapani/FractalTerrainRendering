cmake_minimum_required(VERSION 3.6)
project(FractalTerrainRendering)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILE_DIR src/)
set(SOURCE_FILES ${SOURCE_FILE_DIR}/main.cpp ${SOURCE_FILE_DIR}/glad.c ${SOURCE_FILE_DIR}/SDLGraphicsProgram.cpp
        ${SOURCE_FILE_DIR}/SimpleCamera.cpp ${SOURCE_FILE_DIR}/TerrainPatch.cpp ${SOURCE_FILE_DIR}/Utilities.cpp)
include_directories(include/ ${SDL2_INCLUDE_DIR} deps/glm/)

add_executable(FractalTerrainRendering ${SOURCE_FILES})
target_link_libraries(FractalTerrainRendering ${SDL2_LIBRARY})

add_custom_command(TARGET FractalTerrainRendering POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/objects ${CMAKE_BINARY_DIR})
add_custom_command(TARGET FractalTerrainRendering POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR})
