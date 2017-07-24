cmake_minimum_required(VERSION 3.9.0)

project(FreeImage)

set(FREEIMAGE_VERSION_MAJOR 3)
set(FREEIMAGE_VERSION_MINOR 17)
set(FREEIMAGE_VERSION_PATCH 0)

set(FREEIMAGE_VERSION "${FREEIMAGE_VERSION_MAJOR}.${FREEIMAGE_VERSION_MAJOR}.${FREEIMAGE_VERSION_PATCH")

OPTION(FREEIMAGE_ENABLE_PNG "Enable PNG support" ON)
OPTION(FREEIMAGE_ENABLE_JPEG "Enable JPEG support" ON)
OPTION(FREEIMAGE_ENABLE_TIFF "Enable TIFF support" ON)
OPTION(FREEIMAGE_ENABLE_WEBP "Enable WebP support" OFF)
OPTION(FREEIMAGE_ENABLE_RAW "Enable RAW support" OFF)
OPTION(FREEIMAGE_ENABLE_OPENJPEG "Enable OpenJPEG support" OFF)
OPTION(FREEIMAGE_DYNAMIC_C_RUNTIME "If ON build FreeImage with dynamicly linked C/C++ runtime. If OFF FreeImage is staticly linked with C/C++ runtime." ON)

if( FREEIMAGE_ENABLE_PNG )
	add_subdirectory(Source/LibPNG)
endif()

if ( FREEIMAGE_ENABLE_JPEG )
	add_subdirectory(Source/LibJPEG)
endif()

if ( FREEIMAGE_ENABLE_TIFF )
	add_subdirectory(Source/LibTIFF4)
endif()

if ( FREEIMAGE_ENABLE_WEBP )
	add_subdirectory(Source/LibWebP)
endif()

if ( FREEIMAGE_ENABLE_RAW )
	add_subdirectory(Source/LibRawLite)
endif()

if ( FREEIMAGE_ENABLE_OPENJPEG )
	add_subdirectory(Source/LibOpenJPEG)
endif()

add_subdirectory(Source/OpenEXR)
add_subdirectory(Source)

target_link_libraries( FreeImage OpenEXR )

if ( FREEIMAGE_ENABLE_PNG )
	target_link_libraries(FreeImage LipPNG)
endif()

if ( FREEIMAGE_ENABLE_JPEG )
	target_link_libraries( FreeImage LibJPEG )
endif()
