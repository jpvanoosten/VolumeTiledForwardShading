cmake_minimum_required( VERSION 3.9.0 )

project( LibJXR LANGUAGES C )

set( LIBJXR_COMMON_HEADERS
	common/include/guiddef.h
	common/include/wmsal.h
	common/include/wmspecstring.h
	common/include/wmspecstrings_adt.h
	common/include/wmspecstrings_strict.h
	common/include/wmspecstrings_undef.h
)

source_group( "Header Files\\Common" FILES ${LIBJXR_COMMON_HEADERS} )

set( LIBJXR_IMAGE_DECODE_HEADERS
	image/decode/decode.h
)

source_group( "Header Files\\Image\\Decode" FILES ${LIBJXR_IMAGE_DECODE_HEADERS} )

set( LIBJXR_IMAGE_ENCODE_HEADERS
	image/encode/encode.h
)

source_group( "Header Files\\Image\\Encode" FILES ${LIBJXR_IMAGE_ENCODE_HEADERS} )

set( LIBJXR_IMAGE_SYS_HEADERS 
	image/sys/ansi.h
	image/sys/common.h
	image/sys/perfTimer.h
	image/sys/strcodec.h
	image/sys/strTransform.h
	image/sys/windowsmediaphoto.h
	image/sys/xplatform_image.h
)

source_group( "Header Files\\Image\\Sys" FILES ${LIBJXR_IMAGE_SYS_HEADERS} )

set( LIBJXR_IMAGE_X86_HEADERS
	image/x86/x86.h
)

source_group( "Header Files\\Image\\x86" FILES ${LIBJXR_IMAGE_X86_HEADERS} )

set( LIBJXR_JXRGLUELIB_HEADERS
	jxrgluelib/JXRGlue.h
	jxrgluelib/JXRMeta.h
)

source_group( "Header Files\\JXRGlueLib" FILES ${LIBJXR_JXRGLUELIB_HEADERS} )

set( LIBJXR_IMAGE_DECODE_SOURCE
	image/decode/decode.c
	image/decode/JXRTranscode.c
	image/decode/postprocess.c
	image/decode/segdec.c
	image/decode/strdec.c
	image/decode/strdec_x86.c
	image/decode/strInvTransform.c
	image/decode/strPredQuantDec.c
)

source_group( "Source Files\\Image\\Decode" FILES ${LIBJXR_IMAGE_DECODE_SOURCE} )

set( LIBJXR_IMAGE_ENCODE_SOURCE 
	image/encode/encode.c
	image/encode/segenc.c
	image/encode/strenc.c
	image/encode/strenc_x86.c
	image/encode/strFwdTransform.c
	image/encode/strPredQuantEnc.c
)

source_group( "Source Files\\Image\\Encode" FILES ${LIBJXR_IMAGE_ENCODE_SOURCE} )

set( LIBJXR_IMAGE_SYS_SOURCE
	image/sys/adapthuff.c
	image/sys/image.c
	image/sys/perfTimerANSI.c
	image/sys/strcodec.c
	image/sys/strPredQuant.c
	image/sys/strTransform.c
)

source_group( "Source Files\\Image\\Sys" FILES ${LIBJXR_IMAGE_SYS_SOURCE} )

set( LIBJXR_JXRGLUELIB_SOURCE
	jxrgluelib/JXRGlue.c
	jxrgluelib/JXRGlueJxr.c
	jxrgluelib/JXRGluePFC.c
	jxrgluelib/JXRMeta.c
)

source_group( "Source Files\\JXRGlueLib" FILES ${LIBJXR_JXRGLUELIB_SOURCE} )

set( LIBJXR_AGGREGATE_HEADERS
	${LIBJXR_COMMON_HEADERS}
	${LIBJXR_IMAGE_DECODE_HEADERS}
	${LIBJXR_IMAGE_ENCODE_HEADERS}
	${LIBJXR_IMAGE_SYS_HEADERS}
	${LIBJXR_IMAGE_X86_HEADERS}
	${LIBJXR_JXRGLUELIB_HEADERS}
)

set( LIBJXR_AGGREGATE_SOURCE
	${LIBJXR_IMAGE_DECODE_SOURCE}
	${LIBJXR_IMAGE_ENCODE_SOURCE}
	${LIBJXR_IMAGE_SYS_SOURCE}
	${LIBJXR_JXRGLUELIB_SOURCE}
)

add_library( LibJXR STATIC ${LIBJXR_AGGREGATE_HEADERS} ${LIBJXR_AGGREGATE_SOURCE} )

target_include_directories( LibJXR
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/image/sys
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/jxrgluelib
)