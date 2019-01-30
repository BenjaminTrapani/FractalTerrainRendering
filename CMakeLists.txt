cmake_minimum_required(VERSION 3.6)
project(FractalTerrainRendering)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
#find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SDL2_LIBRARY "C:/Program Files (x86)/SDL2/lib/SDL2.lib")
set(SDL2_INCLUDE_DIR "C:/Program Files (x86)/SDL2/include/SDL2/")
set(SDL2_DLL_DIR "C:/Program Files (x86)/SDL2/bin/")

#set(SDL2_INCLUDE_DIR "/usr/local/include/SDL2")

#message(${SDL2_LIBRARY})

set(SOURCE_FILE_DIR src/)
set(SOURCE_FILES ${SOURCE_FILE_DIR}/main.cpp ${SOURCE_FILE_DIR}/glad.c ${SOURCE_FILE_DIR}/SDLGraphicsProgram.cpp
        ${SOURCE_FILE_DIR}/SimpleCamera.cpp ${SOURCE_FILE_DIR}/TerrainPatch.cpp ${SOURCE_FILE_DIR}/Utilities.cpp
        ${SOURCE_FILE_DIR}/Terrain.cpp ${SOURCE_FILE_DIR}/Texture.cpp ${SOURCE_FILE_DIR}/Lights.cpp)
include_directories(include/ ${SDL2_INCLUDE_DIR} deps/glm/)

add_executable(FractalTerrainRendering ${SOURCE_FILES})
#target_link_libraries(FractalTerrainRendering "-s USE_SDL=2")
target_link_libraries(FractalTerrainRendering ${SDL2_LIBRARY})
set_property(TARGET FractalTerrainRendering PROPERTY CXX_STANDARD 17)

add_custom_command(TARGET FractalTerrainRendering POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR})

if(WIN32)
        add_custom_command(TARGET FractalTerrainRendering POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${SDL2_DLL_DIR}/SDL2.dll ${CMAKE_BINARY_DIR})
endif()

FILE(GLOB ShaderSources shaders/*.glsl)

add_custom_target(copyShaders)
foreach(shaderSource ${ShaderSources})
    file(RELATIVE_PATH
            relShaderSource
            ${CMAKE_SOURCE_DIR}/shaders/
            ${shaderSource})
    add_custom_command(TARGET copyShaders PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${shaderSource} ${CMAKE_CURRENT_BINARY_DIR}/${relShaderSource})
    MESSAGE("TARGET copyShaders PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${shaderSource} ${CMAKE_CURRENT_BINARY_DIR}/${relShaderSource}")
endforeach()
add_dependencies(FractalTerrainRendering copyShaders)
