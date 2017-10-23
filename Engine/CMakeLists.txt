cmake_minimum_required( VERSION 3.9.0 )

# Subproject details
project( Engine LANGUAGES CXX )

# Options for Engine project.
option(Engine_ENABLE_TEST "This is a test option." ON)


# Add headers and source files

set( Engine_RESOURCE_HEADERS 
    resource.h
)

source_group( "Resource Files" FILES ${Engine_RESOURCE_HEADERS} )

set(Engine_CORE_HEADERS 
	inc/Application.h
	inc/bitmask_operators.hpp
	inc/Common.h
	inc/CThreadSafeQueue.h
	inc/DependencyTracker.h
	inc/DependencyTracker.inl
	inc/EngineDefines.h
	inc/EngineIncludes.h
	inc/EnginePCH.h
	inc/Events.h
	inc/HighResolutionTimer.h
	inc/KeyCodes.h
	inc/LogManager.h
	inc/LogStream.h
	inc/NonCopyable.h
	inc/Object.h
	inc/ProfilerVisitor.h
	inc/ReadDirectoryChanges.h
	inc/SceneVisitor.h
	inc/Serialization.h
	inc/Statistic.h
	inc/ThreadSafeQueue.h
)

source_group( "Header Files" FILES ${Engine_CORE_HEADERS} )

set(Engine_GRAPHICS_HEADERS
	inc/Graphics/Adapter.h
	inc/Graphics/BlendState.h
	inc/Graphics/Buffer.h
	inc/Graphics/ByteAddressBuffer.h
	inc/Graphics/Camera.h
	inc/Graphics/ClearColor.h
	inc/Graphics/CommandBuffer.h
	inc/Graphics/CommandQueue.h
	inc/Graphics/ComputeCommandBuffer.h
	inc/Graphics/ComputeCommandQueue.h
	inc/Graphics/ComputePipelineState.h
	inc/Graphics/ConstantBuffer.h
	inc/Graphics/CopyCommandBuffer.h
	inc/Graphics/CopyCommandQueue.h
	inc/Graphics/DepthStencilState.h
	inc/Graphics/Device.h
	inc/Graphics/DirectionalLight.h
	inc/Graphics/Display.h
	inc/Graphics/Fence.h
	inc/Graphics/GraphicsCommandBuffer.h
	inc/Graphics/GraphicsCommandQueue.h
	inc/Graphics/GraphicsEnums.h
	inc/Graphics/GraphicsPipelineState.h
	inc/Graphics/IndexBuffer.h
	inc/Graphics/IndirectArgument.h
	inc/Graphics/IndirectCommandSignature.h
	inc/Graphics/Material.h
	inc/Graphics/Mesh.h
	inc/Graphics/PointLight.h
	inc/Graphics/Profiler.h
	inc/Graphics/Query.h
	inc/Graphics/QueueSemaphore.h
	inc/Graphics/RasterizerState.h
	inc/Graphics/Ray.h
	inc/Graphics/ReadbackBuffer.h
	inc/Graphics/Rect.h
	inc/Graphics/RenderTarget.h
	inc/Graphics/Resource.h
	inc/Graphics/Sampler.h
	inc/Graphics/Scene.h
	inc/Graphics/SceneNode.h
	inc/Graphics/Shader.h
	inc/Graphics/ShaderParameter.h
	inc/Graphics/ShaderSignature.h
	inc/Graphics/SpotLight.h
	inc/Graphics/StructuredBuffer.h
	inc/Graphics/Texture.h
	inc/Graphics/TextureFormat.h
	inc/Graphics/VertexBuffer.h
	inc/Graphics/Viewport.h
	inc/Graphics/Window.h
)

source_group( "Header Files\\Graphics" FILES ${Engine_GRAPHICS_HEADERS} )

set(Engine_GUI_HEADERS
	inc/GUI/GUI.h
)

set( IMGUI_HEADERS 
    ../externals/imgui/imgui.h
)

source_group( "Header Files\\GUI" FILES ${Engine_GUI_HEADERS} ${IMGUI_HEADERS} )

set(Engine_DX12_HEADERS
	inc/Graphics/DX12/ApplicationDX12.h
	inc/Graphics/DX12/BlendStateDX12.h
	inc/Graphics/DX12/BufferDX12.h
	inc/Graphics/DX12/ByteAddressBufferDX12.h
	inc/Graphics/DX12/ComputePipelineStateDX12.h
	inc/Graphics/DX12/ConstantBufferDX12.h
	inc/Graphics/DX12/CopyCommandQueueDX12.h
	inc/Graphics/DX12/d3dx12.h
	inc/Graphics/DX12/DepthStencilStateDX12.h
	inc/Graphics/DX12/DescriptorAllocatorDX12.h
	inc/Graphics/DX12/DeviceDX12.h
	inc/Graphics/DX12/DynamicBufferDX12.h
	inc/Graphics/DX12/DynamicDescriptorHeapDX12.h
	inc/Graphics/DX12/FenceDX12.h
	inc/Graphics/DX12/GraphicsCommandBufferDX12.h
	inc/Graphics/DX12/GraphicsCommandQueueDX12.h
	inc/Graphics/DX12/GraphicsPipelineStateDX12.h
	inc/Graphics/DX12/HeapAllocatorDX12.h
	inc/Graphics/DX12/IndexBufferDX12.h
	inc/Graphics/DX12/IndirectCommandSignatureDX12.h
	inc/Graphics/DX12/QueryDX12.h
	inc/Graphics/DX12/QueueSemaphoreDX12.h
	inc/Graphics/DX12/RasterizerStateDX12.h
	inc/Graphics/DX12/ReadbackBufferDX12.h
	inc/Graphics/DX12/RenderTargetDX12.h
	inc/Graphics/DX12/ResourceDX12.h
	inc/Graphics/DX12/SamplerDX12.h
	inc/Graphics/DX12/SceneDX12.h
	inc/Graphics/DX12/ShaderDX12.h
	inc/Graphics/DX12/ShaderSignatureDX12.h
	inc/Graphics/DX12/StructuredBufferDX12.h
	inc/Graphics/DX12/TextureDX12.h
	inc/Graphics/DX12/VertexBufferDX12.h
	inc/Graphics/DX12/WindowDX12.h
)

source_group( "Header Files\\Graphics\\DX12" FILES ${Engine_DX12_HEADERS} )

set(Engine_DXGI_HEADERS
	inc/Graphics/DXGI/AdapterDXGI.h
	inc/Graphics/DXGI/DisplayDXGI.h
	inc/Graphics/DXGI/TextureFormatDXGI.h
)

source_group( "Header Files\\Graphics\\DXGI" FILES ${Engine_DX12_HEADERS} )

set( Engine_RESOURCE_SOURCE 
    Engine.rc
)

source_group( "Resource Files" FILES ${Engine_RESOURCE_SOURCE} )

set(Engine_CORE_SOURCE
	src/Application.cpp
	src/Common.cpp
	src/DependencyTracker.cpp
	src/DLLMain.cpp
	src/EnginePCH.cpp
	src/HighResolutionTimer.cpp
	src/LogManager.cpp
	src/LogStream.cpp
	src/Object.cpp
	src/ReadDirectoryChanges.cpp
	src/ReadDirectoryChangesPrivate.cpp
	src/ReadDirectoryChangesPrivate.h
)

source_group( "Source Files" FILES ${Engine_CORE_SOURCE} )

set(Engine_GRAPHICS_SOURCE
	src/Graphics/Camera.cpp
	src/Graphics/ClearColor.cpp
	src/Graphics/IndirectArgument.cpp
	src/Graphics/Material.cpp
	src/Graphics/Mesh.cpp
	src/Graphics/Profiler.cpp
	src/Graphics/Ray.cpp
	src/Graphics/RenderTarget.cpp
	src/Graphics/Scene.cpp
	src/Graphics/SceneNode.cpp
	src/Graphics/Shader.cpp
	src/Graphics/ShaderParameter.cpp
	src/Graphics/TextureFormat.cpp
	src/Graphics/Window.cpp
)

source_group( "Source Files\\Graphics" FILES ${Engine_GRAPHICS_SOURCE} )

set(Engine_GUI_SOURCE
	src/GUI/GUI.cpp
	src/GUI/GUI_DX12.cpp
)

set( IMGUI_SOURCE 
    ../externals/imgui/imgui.cpp
    ../externals/imgui/imgui_demo.cpp
    ../externals/imgui/imgui_draw.cpp
)

source_group( "Source Files\\GUI" FILES ${Engine_GUI_SOURCE} ${IMGUI_SOURCE} )

set(Engine_DX12_SOURCE
	src/Graphics/DX12/ApplicationDX12.cpp
	src/Graphics/DX12/BlendStateDX12.cpp
	src/Graphics/DX12/BufferDX12.cpp
	src/Graphics/DX12/ByteAddressBufferDX12.cpp
	src/Graphics/DX12/ComputePipelineStateDX12.cpp
	src/Graphics/DX12/ConstantBufferDX12.cpp
	src/Graphics/DX12/DepthStencilStateDX12.cpp
	src/Graphics/DX12/DescriptorAllocatorDX12.cpp
	src/Graphics/DX12/DeviceDX12.cpp
	src/Graphics/DX12/DynamicBufferDX12.cpp
	src/Graphics/DX12/DynamicDescriptorHeapDX12.cpp
	src/Graphics/DX12/FenceDX12.cpp
	src/Graphics/DX12/GraphicsCommandBufferDX12.cpp
	src/Graphics/DX12/GraphicsCommandQueueDX12.cpp
	src/Graphics/DX12/GraphicsPipelineStateDX12.cpp
	src/Graphics/DX12/HeapAllocatorDX12.cpp
	src/Graphics/DX12/IndexBufferDX12.cpp
	src/Graphics/DX12/IndirectCommandSignatureDX12.cpp
	src/Graphics/DX12/QueryDX12.cpp
	src/Graphics/DX12/QueueSemaphoreDX12.cpp
	src/Graphics/DX12/RasterizerStateDX12.cpp
	src/Graphics/DX12/ReadbackBufferDX12.cpp
	src/Graphics/DX12/RenderTargetDX12.cpp
	src/Graphics/DX12/ResourceDX12.cpp
	src/Graphics/DX12/SamplerDX12.cpp
	src/Graphics/DX12/SceneDX12.cpp
	src/Graphics/DX12/ShaderDX12.cpp
	src/Graphics/DX12/ShaderSignatureDX12.cpp
	src/Graphics/DX12/StructuredBufferDX12.cpp
	src/Graphics/DX12/TextureDX12.cpp
	src/Graphics/DX12/VertexBufferDX12.cpp
	src/Graphics/DX12/WindowDX12.cpp
)

source_group( "Source Files\\Graphics\\DX12" FILES ${Engine_DX12_SOURCE} )

set(Engine_DXGI_SOURCE
	src/Graphics/DXGI/AdapterDXGI.cpp
	src/Graphics/DXGI/DisplayDXGI.cpp
	src/Graphics/DXGI/TextureFormatDXGI.cpp
)

source_group( "Source Files\\Graphics\\DXGI" FILES ${Engine_DXGI_SOURCE} )

set( Engine_SHADERS 
    Resources/Shaders/GenerateMips_CS.hlsl
    Resources/Shaders/GUI_PS.hlsl
    Resources/Shaders/GUI_VS.hlsl
)

source_group( "Resource Files\\Shaders" FILES ${Engine_SHADERS} )

# Create aggregated variables for headers and source files.
set(Engine_AGGREGATE_HEADERS
    ${Engine_RESOURCE_HEADERS}
	${Engine_CORE_HEADERS}
	${Engine_GRAPHICS_HEADERS}
	${Engine_GUI_HEADERS}
    ${IMGUI_HEADERS}
	${Engine_DX12_HEADERS}
	${Engine_DXGI_HEADERS}
)

set(Engine_AGGREGATE_SOURCE
    ${Engine_RESOURCE_SOURCE} 
	${Engine_CORE_SOURCE}
	${Engine_GRAPHICS_SOURCE}
	${Engine_GUI_SOURCE}
	${Engine_DX12_SOURCE}
	${Engine_DXGI_SOURCE}
    ${IMGUI_SOURCE} 
    ${Engine_SHADERS} 
)

# Create the library.
add_library(Engine SHARED ${Engine_AGGREGATE_HEADERS} ${Engine_AGGREGATE_SOURCE} )

set_target_properties( Engine
	PROPERTIES
		DEFINE_SYMBOL ENGINE_EXPORTS
)

target_include_directories( Engine
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
	PUBLIC ${CMAKE_SOURCE_DIR}/externals/boost_1_65_1
	PUBLIC ${CMAKE_SOURCE_DIR}/externals/glm
	PUBLIC ${CMAKE_SOURCE_DIR}/externals/imgui
    PUBLIC ${CMAKE_SOURCE_DIR}/externals/assimp/include                         # Assimp doesn't provide proper configuration for exporting headers and macros that are required to work with Assimp.
    PUBLIC ${CMAKE_BINARY_DIR}/externals/assimp/include
	PRIVATE ${CMAKE_SOURCE_DIR}/externals/winpixeventruntime.1.0.170126001/Include/WinPixEventRuntime
)

target_compile_definitions( Engine 
    PRIVATE $<$<CONFIG:Debug>:_SCL_SECURE_NO_WARNINGS>
    PRIVATE "IMGUI_API=__declspec( dllexport )"
    INTERFACE "IMGUI_API=__declspec( dllimport )"
    INTERFACE ENGINE_IMPORTS
)
# Disable warnings:
#   4250: Occurs when a derived class contains a function with the same name as one of it's base classes.
#         This warning is common in cases where the derived class inherits from multiple bases classes who both inherit from another common base (Diamond of death problem).
#         In this project, there are a few instances of the Diamond of Death problem but I'm not entirely sure how to fix it without refactoring everything in the Engine project.
#         I will try to fix this issue in another incarnation of this project.
#   4251: This warning occurs when DLL exporting a class that contains (private) members deriving from a type in the C++ STL (like std::vector or std::string).
#         Since these (private) class members are never exported directly, it is safe to ignore this warning.
target_compile_options( Engine
    PRIVATE /wd"4250" /wd"4251"
)

target_compile_options( Engine
    PRIVATE $<$<CONFIG:Debug>:/bigobj>
)

# Enable precompiled headers for faster compiliation.
set_source_files_properties( ${Engine_CORE_SOURCE} ${Engine_GRAPHICS_SOURCE} ${Engine_GUI_SOURCE} ${Engine_DX12_SOURCE} ${Engine_DXGI_SOURCE}
    PROPERTIES
        COMPILE_FLAGS /Yu"EnginePCH.h"
)

set_source_files_properties( src/EnginePCH.cpp
    PROPERTIES
        COMPILE_FLAGS /Yc"EnginePCH.h"
)

# Specify libraries to link.
target_link_libraries(Engine 
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/boost_1_65_1/lib/boost_serialization-vc141-mt-1_65_1.lib
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/boost_1_65_1/lib/boost_wserialization-vc141-mt-1_65_1.lib
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/assimp/bin/Assimp.lib
    PRIVATE FreeImage 
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/winpixeventruntime.1.0.170126001/bin/WinPixEventRuntime.lib 
)

install(TARGETS Engine
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
