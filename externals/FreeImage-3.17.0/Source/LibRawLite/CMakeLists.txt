cmake_minimum_required( VERSION 3.9.0 )

project( LibRawLite LANGUAGES CXX )

set( LIBRAW_INTERNAL_HEADERS
	internal/defines.h
	internal/libraw_internal_funcs.h
	internal/var_defines.h
)

source_group( "Header Files\\Internal" FILES ${LIBRAW_INTERNAL_HEADERS} )

set( LIBRAW_HEADERS 
	libraw/libraw.h
	libraw/libraw_alloc.h
	libraw/libraw_const.h
	libraw/libraw_datastream.h
	libraw/libraw_internal.h
	libraw/libraw_types.h
	libraw/libraw_version.h
)

source_group( "Header Files\\LibRAW" FILES ${LIBRAW_HEADERS} )

set( LIBRAW_INTERNAL_SOURCE
	internal/dcraw_common.cpp
	internal/dcraw_fileio.cpp
	internal/demosaic_packs.cpp
)

source_group( "Source Files\\Internal" FILES ${LIBRAW_INTERNAL_SOURCE} )

set( LIBRAW_SOURCE
	src/libraw_c_api.cpp
	src/libraw_cxx.cpp
	src/libraw_datastream.cpp
)

source_group( "Source Files\\LibRAW" FILES ${LIBRAW_SOURCE} )

add_library( LibRAW STATIC 
	${LIBRAW_INTERNAL_HEADERS} 
	${LIBRAW_HEADERS}
	${LIBRAW_INTERNAL_SOURCE}
	${LIBRAW_SOURCE}
)

target_include_directories( LibRAW
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions( LibRAW
	PUBLIC LIBRAW_NODLL
	PRIVATE _CRT_SECURE_NO_WARNINGS
)