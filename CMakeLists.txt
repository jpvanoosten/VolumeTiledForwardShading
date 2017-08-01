cmake_minimum_required(VERSION 3.9.0) # The version of CMake used when creating this project.

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Help cmake find the zlib library that I want to use with this project.
set( ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/externals/assimp/contrib/zlib )
set( ZLIB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib/lib/zlib.lib )
set( ZLIB_INCLUDE_DIR 
	${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib/include
)

# Version information
set(VTSF_VERSION_MAJOR 1)
set(VTSF_VERSION_MINOR 0)
set(VTSF_VERSION_PATCH 0)
set(VTSF_VERSION_TWEAK 0)

set(VTSF_VERSION ${VTSF_VERSION_MAJOR}.${VTSF_VERSION_MINOR}.${VTSF_VERSION_PATCH}.${VTSF_VERSION_TWEAK} )

project(VolumeTiledForwardShading VERSION ${VTSF_VERSION} LANGUAGES CXX)

# Add external dependencies
add_subdirectory(externals)

# Add Engine project
add_subdirectory(Engine)

# Add Game project
add_subdirectory(Game)

# Organize targets into folders in the Visual Studio IDE
if( TARGET assimp )
	set_target_properties( assimp
		PROPERTIES 
			FOLDER "Lib/Assimp"
	)
endif()

if ( TARGET IrrXML )
	set_target_properties( IrrXML
		PROPERTIES
			FOLDER "Lib/Assimp/Contrib"
	)
endif()

if ( TARGET openddl_parser )
	set_target_properties( openddl_parser
		PROPERTIES
			FOLDER "Lib/Assimp/Contrib"
	)
endif()

if ( TARGET FreeImage )
	set_target_properties( FreeImage
		PROPERTIES 
			FOLDER "Lib/FreeImage"
	)
endif()
