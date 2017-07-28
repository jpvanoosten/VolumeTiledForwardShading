# Add external dependencies as sub projects.

add_subdirectory(zlib)

find_package(zlib REQUIRED)
add_subdirectory(assimp)
add_subdirectory(FreeImage-3.17.0)
