cmake_minimum_required(VERSION 3.9.0) # The version of CMake used when creating this project.

# Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Targets should be installed in the source folder.
set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Install prefix path." FORCE )

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
