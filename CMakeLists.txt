cmake_minimum_required(VERSION 3.10)
project(moddn)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Find LuaJIT
find_library(LUAJIT_LIBRARY 
    NAMES "luajit-static"
    PATHS ${CMAKE_SOURCE_DIR}/lib/luajit/src
    NO_DEFAULT_PATH
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/lib/luajit/src)

# Source files
set(SOURCES
    src/dllmain.c
    src/moddn/moddn.c
    src/moddn/files/files.c
    src/moddn/mods/lua_mod.c
)

# Create the DLL
add_library(moddn SHARED ${SOURCES})

# Link libraries
target_link_libraries(moddn ${LUAJIT_LIBRARY})

if(WIN32)
    target_link_libraries(moddn winmm)
endif()

# Set output directory
set_target_properties(moddn PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)