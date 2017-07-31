cmake_minimum_required( VERSION 3.9.0 )

project( LibWebP LANGUAGES C )

set( LIBWEBP_DECODE_HEADERS 
	src/dec/alphai.h
	src/dec/decode_vp8.h
	src/dec/vp8i.h
	src/dec/vp8li.h
	src/dec/webpi.h
)

source_group( "Header Files\\Decode" FILES ${LIBWEBP_DECODE_HEADERS} )

set( LIBWEBP_DSP_HEADERS
	src/dsp/dsp.h
	src/dsp/lossless.h
	src/dsp/mips_macro.h
	src/dsp/neon.h
	src/dsp/yuv.h
	src/dsp/yuv_tables_sse2.h
)

source_group( "Header Files\\DSP" FILES ${LIBWEBP_DSP_HEADERS} )

set( LIBWEBP_ENCODE_HEADERS
	src/enc/backward_references.h
	src/enc/cost.h
	src/enc/histogram.h
	src/enc/vp8enci.h
	src/enc/vp8li.h
)

source_group( "Header Files\\Encode" FILES ${LIBWEBP_ENCODE_HEADERS} )

set( LIBWEBP_MUX_HEADERS
	src/mux/muxi.h
)

source_group( "Header Files\\Mux" FILES ${LIBWEBP_MUX_HEADERS} )

set( LIBWEBP_UTILS_HEADERS 
	src/utils/bit_reader.h
	src/utils/bit_reader_inl.h
	src/utils/bit_writer.h
	src/utils/color_cache.h
	src/utils/endian_inl.h
	src/utils/filters.h
	src/utils/huffman.h
	src/utils/huffman_encode.h
	src/utils/quant_levels.h
	src/utils/quant_levels_dec.h
	src/utils/random.h
	src/utils/rescaler.h
	src/utils/thread.h
	src/utils/utils.h
)

source_group( "Header Files\\Utils" FILES ${LIBWEBP_UTILS_HEADERS} )

set( LIBWEBP_HEADERS
	src/webp/decode.h
	src/webp/demux.h
	src/webp/encode.h
	src/webp/format_constants.h
	src/webp/mux.h
	src/webp/mux_types.h
	src/webp/types.h
)

source_group( "Header Files\\WebP" FILES ${LIBWEBP_HEADERS} )

set( LIBWEBP_DECODE_SOURCE
	src/dec/dec.alpha.c
	src/dec/dec.buffer.c
	src/dec/dec.frame.c
	src/dec/dec.idec.c
	src/dec/dec.io.c
	src/dec/dec.quant.c
	src/dec/dec.tree.c
	src/dec/dec.vp8.c
	src/dec/dec.vp8l.c
	src/dec/dec.webp.c
)

source_group( "Source Files\\Decode" FILES ${LIBWEBP_DECODE_SOURCE} )

set( LIBWEBP_DEMUX_SOURCE
	src/demux/demux.demux.c
)

source_group( "Source Files\\Demux" FILES ${LIBWEBP_DEMUX_SOURCE} )

set( LIBWEBP_DSP_SOURCE 
	src/dsp/dsp.alpha_processing.c
	src/dsp/dsp.alpha_processing_mips_dsp_r2.c
	src/dsp/dsp.alpha_processing_sse2.c
	src/dsp/dsp.argb.c
	src/dsp/dsp.argb_mips_dsp_r2.c
	src/dsp/dsp.argb_sse2.c
	src/dsp/dsp.cost.c
	src/dsp/dsp.cost_mips32.c
	src/dsp/dsp.cost_mips_dsp_r2.c
	src/dsp/dsp.cost_sse2.c
	src/dsp/dsp.cpu.c
	src/dsp/dsp.dec.c
	src/dsp/dsp.dec_clip_tables.c
	src/dsp/dsp.dec_mips32.c
	src/dsp/dsp.dec_mips_dsp_r2.c
	src/dsp/dsp.dec_neon.c
	src/dsp/dsp.dec_sse2.c
	src/dsp/dsp.enc.c
	src/dsp/dsp.enc_avx2.c
	src/dsp/dsp.enc_mips32.c
	src/dsp/dsp.enc_mips_dsp_r2.c
	src/dsp/dsp.enc_neon.c
	src/dsp/dsp.enc_sse2.c
	src/dsp/dsp.filters.c
	src/dsp/dsp.filters_mips_dsp_r2.c
	src/dsp/dsp.filters_sse2.c
	src/dsp/dsp.lossless.c
	src/dsp/dsp.lossless_mips32.c
	src/dsp/dsp.lossless_mips_dsp_r2.c
	src/dsp/dsp.lossless_neon.c
	src/dsp/dsp.lossless_sse2.c
	src/dsp/dsp.rescaler.c
	src/dsp/dsp.rescaler_mips32.c
	src/dsp/dsp.rescaler_mips_dsp_r2.c
	src/dsp/dsp.upsampling.c
	src/dsp/dsp.upsampling_mips_dsp_r2.c
	src/dsp/dsp.upsampling_neon.c
	src/dsp/dsp.upsampling_sse2.c
	src/dsp/dsp.yuv.c
	src/dsp/dsp.yuv_mips32.c
	src/dsp/dsp.yuv_mips_dsp_r2.c
	src/dsp/dsp.yuv_sse2.c
)

source_group( "Source Files\\DSP" FILES ${LIBWEBP_DSP_SOURCE} )

set( LIBWEB_ENCODE_SOURCE
	src/enc/enc.alpha.c
	src/enc/enc.analysis.c
	src/enc/enc.backward_references.c
	src/enc/enc.config.c
	src/enc/enc.cost.c
	src/enc/enc.filter.c
	src/enc/enc.frame.c
	src/enc/enc.histogram.c
	src/enc/enc.iterator.c
	src/enc/enc.near_lossless.c
	src/enc/enc.picture.c
	src/enc/enc.picture_csp.c
	src/enc/enc.picture_psnr.c
	src/enc/enc.picture_rescale.c
	src/enc/enc.picture_tools.c
	src/enc/enc.quant.c
	src/enc/enc.syntax.c
	src/enc/enc.token.c
	src/enc/enc.tree.c
	src/enc/enc.vp8l.c
	src/enc/enc.webpenc.c
)

source_group( "Source Files\\Encode" FILES ${LIBWEB_ENCODE_SOURCE} )

set( LIBWEBP_MUX_SOURCE
	src/mux/mux.anim_encode.c
	src/mux/mux.muxedit.c
	src/mux/mux.muxinternal.c
	src/mux/mux.muxread.c
)

source_group( "Source Files\\Mux" FILES ${LIBWEBP_MUX_SOURCE} )

set( LIBWEBP_UTILS_SOURCE
	src/utils/utils.bit_reader.c
	src/utils/utils.bit_writer.c
	src/utils/utils.color_cache.c
	src/utils/utils.filters.c
	src/utils/utils.huffman.c
	src/utils/utils.huffman_encode.c
	src/utils/utils.quant_levels.c
	src/utils/utils.quant_levels_dec.c
	src/utils/utils.random.c
	src/utils/utils.rescaler.c
	src/utils/utils.thread.c
	src/utils/utils.utils.c
)

source_group( "Source Files\\Utils" FILES ${LIBWEBP_UTILS_SOURCE} )

set( LIBWEBP_AGGREGATE_HEADERS 
	${LIBWEBP_DECODE_HEADERS} 
	${LIBWEBP_DSP_HEADERS} 
	${LIBWEBP_ENCODE_HEADERS} 
	${LIBWEBP_MUX_HEADERS}
	${LIBWEBP_UTILS_HEADERS}
	${LIBWEBP_HEADERS}
)

set( LIBWEBP_AGGREGATE_SOURCE 
	${LIBWEBP_DECODE_SOURCE}
	${LIBWEBP_DEMUX_SOURCE}
	${LIBWEBP_DSP_SOURCE}
	${LIBWEB_ENCODE_SOURCE}
	${LIBWEBP_MUX_SOURCE}
	${LIBWEBP_UTILS_SOURCE}
)

add_library( LibWebP STATIC ${LIBWEBP_AGGREGATE_HEADERS} ${LIBWEBP_AGGREGATE_SOURCE} )

target_include_directories( LibWebP
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)


