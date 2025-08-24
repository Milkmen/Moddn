cmake_minimum_required(VERSION 3.10)
project(moddn)

set(CMAKE_C_STANDARD 99)
set(SUBHOOK_TESTS OFF)
set(SUBHOOK_STATIC ON)
set(SUBHOOK_FORCE_32BIT ON)
set(ENABLE_CJSON_TEST OFF)
set(BUILD_SHARED_AND_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(lib/subhook)
add_subdirectory(lib/cjson)

find_library(LUAJIT_LIBRARY
    NAMES luajit-static
    PATHS ${CMAKE_SOURCE_DIR}/lib/luajit/src
    NO_DEFAULT_PATH
)
include_directories(${CMAKE_SOURCE_DIR}/lib/luajit/src)

set(SOURCES
    src/dllmain.c
    src/moddn/moddn.c
    src/moddn/files/files.c
    src/moddn/mods/lua_mod.c
    src/moddn/pointers/pointers.c
)

add_library(moddn SHARED ${SOURCES})

target_include_directories(moddn PRIVATE lib/cjson)

target_link_libraries(moddn PRIVATE
    ${LUAJIT_LIBRARY}
    subhook      # Target from lib/subhook's CMake
    cjson        # Target from lib/cjson's CMake
)

if(WIN32)
    target_link_libraries(moddn PRIVATE winmm)
endif()

set_target_properties(moddn PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
