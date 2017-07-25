cmake_minimum_required(VERSION 3.9.0) # The version of CMake used when creating this project.

# Version information
set(VTSF_VERSION_MAJOR 1)
set(VTSF_VERSION_MINOR 0)
set(VTSF_VERSION_PATCH 0)
set(VTSF_VERSION_TWEAK 0)

set(VTSF_VERSION "${VTSF_VERSION_MAJOR}.${VTSF_VERSION_MINOR}.${VTSF_VERSION_PATCH}.${VTSF_VERSION_TWEAK}")

project(VolumeTiledForwardShading VERSION ${VTSF_VERSION} LANGUAGES CXX)

# Add external dependencies
add_subdirectory(externals)

# Add Engine project
add_subdirectory(Engine)

# Add Game project
add_subdirectory(Game)
