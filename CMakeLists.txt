cmake_minimum_required(VERSION 3.12...4.1.2) # The version of CMake used when creating this project.

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Make sure DLL and EXE targets go to the same directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Version information
set(VTSF_VERSION_MAJOR 1)
set(VTSF_VERSION_MINOR 0)
set(VTSF_VERSION_PATCH 0)
set(VTSF_VERSION_TWEAK 0)

set(VTSF_VERSION ${VTSF_VERSION_MAJOR}.${VTSF_VERSION_MINOR}.${VTSF_VERSION_PATCH}.${VTSF_VERSION_TWEAK} )

project(VolumeTiledForwardShading VERSION ${VTSF_VERSION} LANGUAGES CXX)

# Add Engine project
add_subdirectory(Engine)

# Add Game project
add_subdirectory(Game)

# Set the startup project.
set_directory_properties( PROPERTIES 
    VS_STARTUP_PROJECT Game
)

