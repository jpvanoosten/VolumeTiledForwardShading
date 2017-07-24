project( LibJXR )

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

source_group( "Header Files\\Image\Encode" FILES ${LIBJXR_IMAGE_ENCODE_HEADERS} )

set( LIBJXR_IMAGE_SYS_HEADERS 
	image/sys/ansi.h
	image/sys/common.h
	image/sys/perfTimer.h
	image/sys/strcodec.h
	image/sys/strTransform.h
	image/sys/windowsmediaphoto.h
	image/sys/xplatform_image.h
)

source_group( "Header Files\\Image\Sys" FILES ${LIBJXR_IMAGE_SYS_HEADERS} )

set( LIBJXR_IMAGE_X86_HEADERS
	image/x86/x86.h
)

source_group( "Header Files\\Image\x86" FILES ${LIBJXR_IMAGE_X86_HEADERS} )

set( LIBJXR_JXRGLUELIB_HEADERS
	jxrgluelib/JXRGlue.h
	jxrgluelib/JXRMeta.h
)

source_group( "Header Files\\JXRGlueLib" FILES ${LIBJXR_JXRGLUELIB_HEADERS} )

# add_library( LibJXR STATIC 