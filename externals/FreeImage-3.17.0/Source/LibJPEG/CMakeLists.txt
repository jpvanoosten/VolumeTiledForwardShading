cmake_minimum_required(VERSION 3.9.0)

project( LibJPEG )

set( LIB_JPEG_HEADERS
	jconfig.h
	jdct.h
	jerror.h
	jinclude.h
	jmemsys.h
	jmorecfg.h
	jpegint.h
	jpeglib.h
	jversion.h
	transupp.h
)

source_group( "Headers Files" FILES ${LIB_JPEG_HEADERS} )

set( LIB_JPEG_SOURCE
	jaricom.c
	jcapimin.c
	jcapistd.c
	jcarith.c
	jccoefct.c
	jccolor.c
	jcdctmgr.c
	jchuff.c
	jcinit.c
	jcmainct.c
	jcmarker.c
	jcmaster.c
	jcomapi.c
	jcparam.c
	jcprepct.c
	jcsample.c
	jctrans.c
	jdapimin.c
	jdapistd.c
	jdarith.c
	jdatadst.c
	jdatasrc.c
	jdcoefct.c
	jdcolor.c
	jddctmgr.c
	jdhuff.c
	jdinput.c
	jdmainct.c
	jdmarker.c
	jdmaster.c
	jdmerge.c
	jdpostct.c
	jdsample.c
	jdtrans.c
	jerror.c
	jfdctflt.c
	jfdctfst.c
	jfdctint.c
	jidctflt.c
	jidctfst.c
	jidctint.c
	jmemmgr.c
	jmemnobs.c
	jquant1.c
	jquant2.c
	jutils.c
	transupp.c
)

source_group( "Source Files" FILES ${LIB_JPEG_SOURCE} )

add_library( LibJPEG STATIC ${LIB_JPEG_HEADERS} ${LIB_JPEG_SOURCE} )

target_include_directories( LibJPEG
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions( LibJPEG
	PRIVATE _CRT_SECURE_NO_WARNINGS
)
