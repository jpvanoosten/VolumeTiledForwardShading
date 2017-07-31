cmake_minimum_required( VERSION 3.9.0 )

project( LibOpenJPEG LANGUAGES C )

set( LIBOPENJPEG_HEADERS
	bio.h
	cio.h
	dwt.h
	event.h
	function_list.h
	image.h
	indexbox_manager.h
	invert.h
	j2k.h
	jp2.h
	mct.h
	mqc.h
	openjpeg.h
	opj_clock.h
	opj_config.h
	opj_config_private.h
	opj_includes.h
	opj_intmath.h
	opj_inttypes.h
	opj_malloc.h
	opj_stdint.h
	pi.h
	raw.h
	t1.h
	t1_luts.h
	t2.h
	tcd.h
	tgt.h
)

source_group( "Header Files" FILES ${LIBOPENJPEG_HEADERS} )

set( LIBOPENJPEG_SOURCE
	bio.c
	cio.c
	dwt.c
	event.c
	function_list.c
	invert.c
	j2k.c
	jp2.c
	mct.c
	mqc.c
	obj_image.c
	openjpeg.c
	opj_clock.c
	pi.c
	raw.c
	t1.c
	t2.c
	tcd.c
	tgt.c
)

source_group( "Source Files" FILES ${LIBOPENJPEG_SOURCE} )

add_library( LibOpenJPEG STATIC ${LIBOPENJPEG_HEADERS} ${LIBOPENJPEG_SOURCE} )

target_include_directories( LibOpenJPEG
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions( LibOpenJPEG
	PUBLIC OPJ_STATIC
)

if( WIN32 )
	target_compile_definitions( LibOpenJPEG
		PRIVATE _CRT_SECURE_NO_WARNINGS
	)
endif()
