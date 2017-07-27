cmake_minimum_required( VERSION 3.9.0 )

set( OPENEXR_VERSION_MAJOR 2 )
set( OPENEXR_VERSION_MINOR 2 )
set( OPENEXR_VERSION_PATCH 0 )

set( OPENEXR_VERSION ${OPENEXR_VERSION_MAJOR}.${OPENEXR_VERSION_MINOR}.${OPENEXR_VERSION_PATCH} )

project( OpenEXR VERSION ${OPENEXR_VERSION} LANGUAGES CXX )

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

