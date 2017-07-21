cmake_minimum_required(VERSION 3.9.0) # The version of CMake used when creating this project.

project(VolumeTiledForwardShading)

# Version information
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Add external dependencies
add_subdirectory(externals)

# Add Engine project
add_subdirectory(Engine)

# Add Game project
add_subdirectory(Game)
