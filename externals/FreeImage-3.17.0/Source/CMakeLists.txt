cmake_minimum_required(VERSION 3.9.0)

set( FREEIMAGE_VERSION_MAJOR 3 )
set( FREEIMAGE_VERSION_MINOR 17 )
set( FREEIMAGE_VERSION_PATCH 0 )
set( FREEIMAGE_VERSION_TWEAK 0 )

set( FREEIMAGE_VERSION ${FREEIMAGE_VERSION_MAJOR}.${FREEIMAGE_VERSION_MAJOR}.${FREEIMAGE_VERSION_PATCH}.${FREEIMAGE_VERSION_TWEAK} )

project( FreeImage VERSION ${FREEIMAGE_VERSION} )

option(FREEIMAGE_ENABLE_PNG "Enable PNG support" ON)
option(FREEIMAGE_ENABLE_JPEG "Enable JPEG support" ON)
option(FREEIMAGE_ENABLE_JPEGXR "Enable JPEGXR support" ON)
option(FREEIMAGE_ENABLE_TIFF "Enable TIFF support" ON)
option(FREEIMAGE_ENABLE_WEBP "Enable WebP support" OFF)
option(FREEIMAGE_ENABLE_RAW "Enable RAW support" OFF)
option(FREEIMAGE_ENABLE_OPENJPEG "Enable OpenJPEG support" OFF)
option(FREEIMAGE_DYNAMIC_C_RUNTIME "If ON build FreeImage with dynamicly linked C/C++ runtime. If OFF FreeImage is staticly linked with C/C++ runtime." ON)

set( FREEIMAGE_HEADERS
	CacheFile.h
	FreeImage.h
	FreeImageIO.h
	MapIntrospector.h
	Plugin.h
	Quantizers.h
	ToneMapping.h
	Utilities.h
	DepricationManager/DeprecationMgr.h
	Metadata/FIRational.h
	Metadata/FreeImageTag.h
	FreeImage/J2KHelper.h
	FreeImage/PSDParser.h
)

source_group( Headers FILES ${FREEIMAGE_HEADERS} )

set( FREEIMAGE_SOURCE
	../FreeImage.rc
	FreeImage/BitmapAccess.cpp
	FreeImage/ColorLookup.cpp
	FreeImage/FreeImage.cpp
	FreeImage/FreeImageIO.cpp
	FreeImage/GetType.cpp
	FreeImage/MemoryIO.cpp
	FreeImage/PixelAccess.cpp
)

source_group( Source FILES ${FREEIMAGE_SOURCE} )

set( FREEIMAGE_CONVERSION_SOURCE
	FreeImage/Conversion.cpp
	FreeImage/Conversion16_555.cpp
	FreeImage/Conversion16_565.cpp
	FreeImage/Conversion24.cpp
	FreeImage/Conversion32.cpp
	FreeImage/Conversion4.cpp
	FreeImage/Conversion8.cpp
	FreeImage/ConversionFloat.cpp
	FreeImage/ConversionRGB16.cpp
	FreeImage/ConversionRGBA16.cpp
	FreeImage/ConversionRGBAF.cpp
	FreeImage/ConversionRGBF.cpp
	FreeImage/ConversionType.cpp
	FreeImage/ConversionUINT16.cpp
	FreeImage/Halftoning.cpp
	FreeImage/tmoColorConvert.cpp
	FreeImage/tmoDrago03.cpp
	FreeImage/tmoFattal02.cpp
	FreeImage/tmoReinhard05.cpp
	FreeImage/ToneMapping.cpp
)

source_group( Source\\Conversion FILES ${FREEIMAGE_CONVERSION_SOURCE} )

set( FREEIMAGE_DEPRICATION_MGR_SOURCE
	DepricationManager/Deprecated.cpp
	DepricationManager/DeprecationMgr.cpp
)

source_group( Source\\DepricationMgr FILES ${FREEIMAGE_DEPRICATION_MGR_SOURCE} )

set( FREEIMAGE_METADATA_SOURCE
	Metadata/Exif.cpp
	Metadata/FIRational.cpp
	Metadata/FreeImageTag.cpp
	Metadata/IPTC.cpp
	Metadata/TagConversion.cpp
	Metadata/TagLib.cpp
	Metadata/XTIFF.cpp
)

source_group( Source\\Metadata FILES ${FREEIMAGE_METADATA_SOURCE} )

set( FREEIMAGE_MULTI_PAGING_SOURCE
	FreeImage/CacheFile.cpp
	FreeImage/MultiPage.cpp
	FreeImage/ZLibInterface.cpp
)

source_group( Source\\MultiPaging FILES ${FREEIMAGE_MULTI_PAGING_SOURCE} )

set( FREEIMAGE_PLUGINS_SOURCE
	FreeImage/J2KHelper.cpp
	FreeImage/MNGHelper.cpp
	FreeImage/Plugin.cpp
	FreeImage/PluginBMP.cpp
	FreeImage/PluginCUT.cpp
	FreeImage/PluginDDS.cpp
	FreeImage/PluginEXR.cpp
	FreeImage/PluginG3.cpp
	FreeImage/PluginGIF.cpp
	FreeImage/PluginHDR.cpp
	FreeImage/PluginICO.cpp
	FreeImage/PluginIFF.cpp
	FreeImage/PluginJ2K.cpp
	FreeImage/PluginJNG.cpp
	FreeImage/PluginJP2.cpp
	FreeImage/PluginJPEG.cpp
	FreeImage/PluginJXR.cpp
	FreeImage/PluginKOALA.cpp
	FreeImage/PluginMNG.cpp
	FreeImage/PluginPCD.cpp
	FreeImage/PluginPCX.cpp
	FreeImage/PluginPFM.cpp
	FreeImage/PluginPICT.cpp
	FreeImage/PluginPNG.cpp
	FreeImage/PluginPNM.cpp
	FreeImage/PluginPSD.cpp
	FreeImage/PluginRAS.cpp
	FreeImage/PluginRAW.cpp
	FreeImage/PluginSGI.cpp
	FreeImage/PluginTARGA.cpp
	FreeImage/PluginTIFF.cpp
	FreeImage/PluginWBMP.cpp
	FreeImage/PluginWebP.cpp
	FreeImage/PluginXBM.cpp
	FreeImage/PluginXPM.cpp
	FreeImage/PSDParser.cpp
	FreeImage/TIFFLogLuv.cpp
)

source_group( Source\\Plugins FILES ${FREEIMAGE_PLUGINS_SOURCE} )

set( FREEIMAGE_QUANTIZERS_SOURCE
	FreeImage/LFPQuantizer.cpp
	FreeImage/NNQuantizer.cpp
	FreeImage/WuQuantizer.cpp
)

source_group( Source\\Quantizers FILES ${FREEIMAGE_QUANTIZERS_SOURCE} )

set( FREEIMAGE_TOOLKIT_SOURCE 
	FreeImageToolkit/Background.cpp
	FreeImageToolkit/BSplineRotate.cpp
	FreeImageToolkit/Channels.cpp
	FreeImageToolkit/ClassicRotate.cpp
	FreeImageToolkit/Colors.cpp
	FreeImageToolkit/CopyPaste.cpp
	FreeImageToolkit/Display.cpp
	FreeImageToolkit/Filters.h
	FreeImageToolkit/Flip.cpp
	FreeImageToolkit/JPEGTransform.cpp
	FreeImageToolkit/MultigridPoissonSolver.cpp
	FreeImageToolkit/Rescale.cpp
	FreeImageToolkit/Resize.cpp
	FreeImageToolkit/Resize.h
)

source_group( Toolkit FILES ${FREEIMAGE_TOOLKIT_SOURCE} )

set( FREEIMAGE_AGGREGATE_SOURCE
	FREEIMAGE_SOURCE
	FREEIMAGE_CONVERSION_SOURCE
	FREEIMAGE_DEPRICATION_MGR_SOURCE
	FREEIMAGE_METADATA_SOURCE
	FREEIMAGE_MULTI_PAGING_SOURCE
	FREEIMAGE_PLUGINS_SOURCE
	FREEIMAGE_QUANTIZERS_SOURCE
	FREEIMAGE_TOOLKIT_SOURCE
)

add_library( FreeImage SHARED ${FREEIMAGE_HEADERS} ${FREEIMAGE_AGGREGATE_SOURCE} )

add_subdirectory(OpenEXR)
target_link_libraries( FreeImage OpenEXR )

if ( FREEIMAGE_ENABLE_PNG )
	add_subdirectory( LibPNG )
	target_link_libraries( FreeImage ${PNG_LIB_NAME_STATIC} )
endif()

if ( FREEIMAGE_ENABLE_JPEG )
	add_subdirectory(LibJPEG)
	target_link_libraries( FreeImage LibJPEG )
endif()

if ( FREEIMAGE_ENABLE_JPEGXR )
	add_subdirectory(LibJXR)
	target_link_libraries( FreeImage LibJXR )
endif()

if ( FREEIMAGE_ENABLE_TIFF )
	add_subdirectory(LibTIFF4)
	target_link_libraries( FreeImage LibTIFF4 )
endif()

if ( FREEIMAGE_ENABLE_WEBP )
	add_subdirectory( LibWebP )
	target_link_libraries( FreeImage LibWebP )
endif()

if ( FREEIMAGE_ENABLE_RAW )
	add_subdirectory( LibRawLite )
	target_link_libraries( FreeImage LibRawLite )
endif()

if ( FREEIMAGE_ENABLE_OPENJPEG )
	add_subdirectory( LibOpenJPEG )
	target_link_libraries( FreeImage LibOpenJPEG )
endif()
