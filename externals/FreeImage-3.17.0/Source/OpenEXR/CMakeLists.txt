cmake_minimum_required( VERSION 3.9.0 )

set( OPENEXR_VERSION_MAJOR 2 )
set( OPENEXR_VERSION_MINOR 2 )
set( OPENEXR_VERSION_PATCH 0 )

set( OPENEXR_VERSION ${OPENEXR_VERSION_MAJOR}.${OPENEXR_VERSION_MINOR}.${OPENEXR_VERSION_PATCH} )

project( OpenEXR VERSION ${OPENEXR_VERSION} LANGUAGES CXX )

set( OPENEXR_HEADERS
	IlmBaseConfig.h
	OpenEXRConfig.h
)

source_group( "Header Files" FILES ${OPENEXR_HEADERS} )

set( OPENEXR_HALF_HEADERS
	Half/eLut.h
	Half/half.h
	Half/halfExport.h
	Half/halfFunction.h
	Half/halfLimits.h
	Half/toFloat.h
)

source_group( "Header Files\\Half" FILES ${OPENEXR_HALF_HEADERS} )

set( OPENEXR_IEX_HEADERS 
	Iex/Iex.h
	Iex/IexBaseExc.h
	Iex/IexErrnoExc.h
	Iex/IexExport.h
	Iex/IexForward.h
	Iex/IexMacros.h
	Iex/IexMathExc.h
	Iex/IexNamespace.h
	Iex/IexThrowErrnoExc.h
)

source_group( "Header Files\\Iex" FILES ${OPENEXR_IEX_HEADERS} )

set( OPENEXR_IEXMATH_HEADERS
	IexMath/IexMathFloatExc.h
	IexMath/IexMathFpu.h
	IexMath/IexMathIeeeExc.h
)

source_group( "Header Files\\IexMath" FILES ${OPENEXR_IEXMATH_HEADERS} )

set( OPENEXR_ILMIMF_HEADERS
	IlmImf/b44ExpLogTable.h
	IlmImf/dwaLookups.h
	IlmImf/ImfAcesFile.h
	IlmImf/ImfArray.h
	IlmImf/ImfAttribute.h
	IlmImf/ImfAutoArray.h
	IlmImf/ImfB44Compressor.h
	IlmImf/ImfBoxAttribute.h
	IlmImf/ImfChannelList.h
	IlmImf/ImfChannelListAttribute.h
	IlmImf/ImfCheckedArithmetic.h
	IlmImf/ImfChromaticities.h
	IlmImf/ImfChromaticitiesAttribute.h
	IlmImf/ImfCompositeDeepScanLine.h
	IlmImf/ImfCompression.h
	IlmImf/ImfCompressionAttribute.h
	IlmImf/ImfCompressor.h
	IlmImf/ImfConvert.h
	IlmImf/ImfCRgbaFile.h
	IlmImf/ImfDeepCompositing.h
	IlmImf/ImfDeepFrameBuffer.h
	IlmImf/ImfDeepImageState.h
	IlmImf/ImfDeepImageStateAttribute.h
	IlmImf/ImfDeepScanLineInputFile.h
	IlmImf/ImfDeepScanLineInputPart.h
	IlmImf/ImfDeepScanLineOutputFile.h
	IlmImf/ImfDeepScanLineOutputPart.h
	IlmImf/ImfDeepTiledInputFile.h
	IlmImf/ImfDeepTiledInputPart.h
	IlmImf/ImfDeepTiledOutputFile.h
	IlmImf/ImfDeepTiledOutputPart.h
	IlmImf/ImfDoubleAttribute.h
	IlmImf/ImfDwaCompressor.h
	IlmImf/ImfDwaCompressorSimd.h
	IlmImf/ImfEnvmap.h
	IlmImf/ImfEnvmapAttribute.h
	IlmImf/ImfExport.h
	IlmImf/ImfFastHuf.h
	IlmImf/ImfFloatAttribute.h
	IlmImf/ImfFloatVectorAttribute.h
	IlmImf/ImfForward.h
	IlmImf/ImfFrameBuffer.h
	IlmImf/ImfFramesPerSecond.h
	IlmImf/ImfGenericInputFile.h
	IlmImf/ImfGenericOutputFile.h
	IlmImf/ImfHeader.h
	IlmImf/ImfHuf.h
	IlmImf/ImfInputFile.h
	IlmImf/ImfInputPart.h
	IlmImf/ImfInputPartData.h
	IlmImf/ImfInputStreamMutex.h
	IlmImf/ImfInt64.h
	IlmImf/ImfIntAttribute.h
	IlmImf/ImfIO.h
	IlmImf/ImfKeyCode.h
	IlmImf/ImfKeyCodeAttribute.h
	IlmImf/ImfLineOrder.h
	IlmImf/ImfLineOrderAttribute.h
	IlmImf/ImfLut.h
	IlmImf/ImfMatrixAttribute.h
	IlmImf/ImfMisc.h
	IlmImf/ImfMultiPartInputFile.h
	IlmImf/ImfMultiPartOutputFile.h
	IlmImf/ImfMultiView.h
	IlmImf/ImfName.h
	IlmImf/ImfNamespace.h
	IlmImf/ImfOpaqueAttribute.h
	IlmImf/ImfOptimizedPixelReading.h
	IlmImf/ImfOutputFile.h
	IlmImf/ImfOutputPart.h
	IlmImf/ImfOutputPartData.h
	IlmImf/ImfOutputStreamMutex.h
	IlmImf/ImfPartHelper.h
	IlmImf/ImfPartType.h
	IlmImf/ImfPixelType.h
	IlmImf/ImfPizCompressor.h
	IlmImf/ImfPreviewImage.h
	IlmImf/ImfPreviewImageAttribute.h
	IlmImf/ImfPxr24Compressor.h
	IlmImf/ImfRational.h
	IlmImf/ImfRationalAttribute.h
	IlmImf/ImfRgba.h
	IlmImf/ImfRgbaFile.h
	IlmImf/ImfRgbaYca.h
	IlmImf/ImfRle.h
	IlmImf/ImfRleCompressor.h
	IlmImf/ImfScanLineInputFile.h
	IlmImf/ImfSimd.h
	IlmImf/ImfStandardAttributes.h
	IlmImf/ImfStdIO.h
	IlmImf/ImfStringAttribute.h
	IlmImf/ImfStringVectorAttribute.h
	IlmImf/ImfSystemSpecific.h
	IlmImf/ImfTestFile.h
	IlmImf/ImfThreading.h
	IlmImf/ImfTileDescription.h
	IlmImf/ImfTileDescriptionAttribute.h
	IlmImf/ImfTiledInputFile.h
	IlmImf/ImfTiledInputPart.h
	IlmImf/ImfTiledMisc.h
	IlmImf/ImfTiledOutputFile.h
	IlmImf/ImfTiledOutputPart.h
	IlmImf/ImfTiledRgbaFile.h
	IlmImf/ImfTileOffsets.h
	IlmImf/ImfTimeCode.h
	IlmImf/ImfTimeCodeAttribute.h
	IlmImf/ImfVecAttribute.h
	IlmImf/ImfVersion.h
	IlmImf/ImfWav.h
	IlmImf/ImfXdr.h
	IlmImf/ImfZip.h
	IlmImf/ImfZipCompressor.h
)

source_group( "Header Files\\IlmImf" FILES ${OPENEXR_ILMIMF_HEADERS} )

set( OPENEXR_ILMTHREAD_HEADERS 
	IlmThread/IlmThread.h
	IlmThread/IlmThreadExport.h
	IlmThread/IlmThreadForward.h
	IlmThread/IlmThreadMutex.h
	IlmThread/IlmThreadNamespace.h
	IlmThread/IlmThreadPool.h
	IlmThread/IlmThreadSemaphore.h
)

source_group( "Header Files\\IlmThread" FILES ${OPENEXR_ILMTHREAD_HEADERS} )

set( OPENEXR_IMATH_HEADERS 
	Imath/ImathBox.h
	Imath/ImathBoxAlgo.h
	Imath/ImathColor.h
	Imath/ImathColorAlgo.h
	Imath/ImathEuler.h
	Imath/ImathExc.h
	Imath/ImathExport.h
	Imath/ImathForward.h
	Imath/ImathFrame.h
	Imath/ImathFrustum.h
	Imath/ImathFrustumTest.h
	Imath/ImathFun.h
	Imath/ImathGL.h
	Imath/ImathGLU.h
	Imath/ImathHalfLimits.h
	Imath/ImathInt64.h
	Imath/ImathInterval.h
	Imath/ImathLimits.h
	Imath/ImathLine.h
	Imath/ImathLineAlgo.h
	Imath/ImathMath.h
	Imath/ImathMatrix.h
	Imath/ImathMatrixAlgo.h
	Imath/ImathNamespace.h
	Imath/ImathPlane.h
	Imath/ImathPlatform.h
	Imath/ImathQuat.h
	Imath/ImathRandom.h
	Imath/ImathRoots.h
	Imath/ImathShear.h
	Imath/ImathSphere.h
	Imath/ImathVec.h
	Imath/ImathVecAlgo.h
)

source_group( "Header Files\\IMath" FILES ${OPENEXR_IMATH_HEADERS} )

set( OPENEXR_HALF_SOURCE
	Half/half.cpp
)

source_group( "Source Files\\Half" FILES ${OPENEXR_HALF_SOURCE} )

set( OPENEXR_IEX_SOURCE
	Iex/IexBaseExc.cpp
	Iex/IexThrowErrnoExc.cpp
)

source_group( "Source Files\\Iex" FILES ${OPENEXR_IEX_SOURCE} )

set( OPENEXR_IEXMATH_SOURCE
	IexMath/IexMathFloatExc.cpp
	IexMath/IexMathFpu.cpp
)

source_group( "Source Files\\IexMath" FILES ${OPENEXR_IEXMATH_SOURCE} )

set( OPENEXR_ILMIMF_SOURCE
	IlmImf/b44ExpLogTable.cpp
	IlmImf/ImfAcesFile.cpp
	IlmImf/ImfAttribute.cpp
	IlmImf/ImfB44Compressor.cpp
	IlmImf/ImfBoxAttribute.cpp
	IlmImf/ImfChannelList.cpp
	IlmImf/ImfChannelListAttribute.cpp
	IlmImf/ImfChromaticities.cpp
	IlmImf/ImfChromaticitiesAttribute.cpp
	IlmImf/ImfCompositeDeepScanLine.cpp
	IlmImf/ImfCompressionAttribute.cpp
	IlmImf/ImfCompressor.cpp
	IlmImf/ImfConvert.cpp
	IlmImf/ImfCRgbaFile.cpp
	IlmImf/ImfDeepCompositing.cpp
	IlmImf/ImfDeepFrameBuffer.cpp
	IlmImf/ImfDeepImageStateAttribute.cpp
	IlmImf/ImfDeepScanLineInputFile.cpp
	IlmImf/ImfDeepScanLineInputPart.cpp
	IlmImf/ImfDeepScanLineOutputFile.cpp
	IlmImf/ImfDeepScanLineOutputPart.cpp
	IlmImf/ImfDeepTiledInputFile.cpp
	IlmImf/ImfDeepTiledInputPart.cpp
	IlmImf/ImfDeepTiledOutputFile.cpp
	IlmImf/ImfDeepTiledOutputPart.cpp
	IlmImf/ImfDoubleAttribute.cpp
	IlmImf/ImfDwaCompressor.cpp
	IlmImf/ImfEnvmap.cpp
	IlmImf/ImfEnvmapAttribute.cpp
	IlmImf/ImfFastHuf.cpp
	IlmImf/ImfFloatAttribute.cpp
	IlmImf/ImfFloatVectorAttribute.cpp
	IlmImf/ImfFrameBuffer.cpp
	IlmImf/ImfFramesPerSecond.cpp
	IlmImf/ImfGenericInputFile.cpp
	IlmImf/ImfGenericOutputFile.cpp
	IlmImf/ImfHeader.cpp
	IlmImf/ImfHuf.cpp
	IlmImf/ImfInputFile.cpp
	IlmImf/ImfInputPart.cpp
	IlmImf/ImfInputPartData.cpp
	IlmImf/ImfIntAttribute.cpp
	IlmImf/ImfIO.cpp
	IlmImf/ImfKeyCode.cpp
	IlmImf/ImfKeyCodeAttribute.cpp
	IlmImf/ImfLineOrderAttribute.cpp
	IlmImf/ImfLut.cpp
	IlmImf/ImfMatrixAttribute.cpp
	IlmImf/ImfMisc.cpp
	IlmImf/ImfMultiPartInputFile.cpp
	IlmImf/ImfMultiPartOutputFile.cpp
	IlmImf/ImfMultiView.cpp
	IlmImf/ImfOpaqueAttribute.cpp
	IlmImf/ImfOutputFile.cpp
	IlmImf/ImfOutputPart.cpp
	IlmImf/ImfOutputPartData.cpp
	IlmImf/ImfPartType.cpp
	IlmImf/ImfPizCompressor.cpp
	IlmImf/ImfPreviewImage.cpp
	IlmImf/ImfPreviewImageAttribute.cpp
	IlmImf/ImfPxr24Compressor.cpp
	IlmImf/ImfRational.cpp
	IlmImf/ImfRationalAttribute.cpp
	IlmImf/ImfRgbaFile.cpp
	IlmImf/ImfRgbaYca.cpp
	IlmImf/ImfRle.cpp
	IlmImf/ImfRleCompressor.cpp
	IlmImf/ImfScanLineInputFile.cpp
	IlmImf/ImfStandardAttributes.cpp
	IlmImf/ImfStdIO.cpp
	IlmImf/ImfStringAttribute.cpp
	IlmImf/ImfStringVectorAttribute.cpp
	IlmImf/ImfSystemSpecific.cpp
	IlmImf/ImfTestFile.cpp
	IlmImf/ImfThreading.cpp
	IlmImf/ImfTileDescriptionAttribute.cpp
	IlmImf/ImfTiledInputFile.cpp
	IlmImf/ImfTiledInputPart.cpp
	IlmImf/ImfTiledMisc.cpp
	IlmImf/ImfTiledOutputFile.cpp
	IlmImf/ImfTiledOutputPart.cpp
	IlmImf/ImfTiledRgbaFile.cpp
	IlmImf/ImfTileOffsets.cpp
	IlmImf/ImfTimeCode.cpp
	IlmImf/ImfTimeCodeAttribute.cpp
	IlmImf/ImfVecAttribute.cpp
	IlmImf/ImfVersion.cpp
	IlmImf/ImfWav.cpp
	IlmImf/ImfZip.cpp
	IlmImf/ImfZipCompressor.cpp
)

source_group( "Source Files\\IlmImf" FILES ${OPENEXR_ILMIMF_SOURCE} )

set( OPENEXR_ILMTHREAD_SOURCE
	IlmThread/IlmThread.cpp
	IlmThread/IlmThreadMutex.cpp
	IlmThread/IlmThreadPool.cpp
	IlmThread/IlmThreadSemaphore.cpp
)

source_group( "Source Files\\IlmThread" FILES ${OPENEXR_ILMTHREAD_SOURCE} )

set( OPENEXR_IMATH_SOURCE
#	Imath/ImathBox.cpp
	Imath/ImathColorAlgo.cpp
	Imath/ImathFun.cpp
	Imath/ImathMatrixAlgo.cpp
	Imath/ImathRandom.cpp
#	Imath/ImathShear.cpp
	Imath/ImathVec.cpp
)

source_group( "Source Files\\IMath" FILES ${OPENEXR_IMATH_SOURCE} )

set( OPENEXR_AGGREGATE_HEADERS
	${OPENEXR_HEADERS}
	${OPENEXR_HALF_HEADERS}
	${OPENEXR_IEX_HEADERS}
	${OPENEXR_IEXMATH_HEADERS}
	${OPENEXR_ILMIMF_HEADERS}
	${OPENEXR_ILMTHREAD_HEADERS}
	${OPENEXR_IMATH_HEADERS}
)

set( OPENEXR_AGGREGATE_SOURCE
	${OPENEXR_HALF_SOURCE}
	${OPENEXR_IEX_SOURCE}
	${OPENEXR_IEXMATH_SOURCE}
	${OPENEXR_ILMIMF_SOURCE}
	${OPENEXR_ILMTHREAD_SOURCE}
	${OPENEXR_IMATH_SOURCE}
)

add_library( OpenEXR STATIC ${OPENEXR_AGGREGATE_HEADERS} ${OPENEXR_AGGREGATE_SOURCE} )

target_include_directories( OpenEXR 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Half
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Iex
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/IexMath
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/IlmImf
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/IlmThread
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/IMath
	PRIVATE ${ZLIB_INCLUDE_DIR}
)

if ( WIN32 )
	target_compile_definitions( OpenEXR
		PRIVATE _CRT_SECURE_NO_WARNINGS
	)
endif( WIN32 )
