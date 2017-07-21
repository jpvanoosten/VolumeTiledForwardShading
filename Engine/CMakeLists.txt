# Subproject details
project(Engine)

# Options for Engine project.
option(${PROJECT_NAME}_ENABLE_TEST "This is a test option." ON)


# Add headers and source files
set(${PROJECT_NAME}_CORE_HEADERS 
	inc/Application.h
	inc/bitmast_operators.hpp
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

set(${PROJECT_NAME}_GRAPHICS_HEADERS
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

set(${PROJECT_NAME}_GUI_HEADERS
	inc/GUI/GUI.h
)

set(${PROJECT_NAME}_DX12_HEADERS
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

set(${PROJECT_NAME}_DXGI_HEADERS
	inc/Graphics/DXGI/AdapterDXGI.h
	inc/Graphics/DXGI/DisplayDXGI.h
	inc/Graphics/DXGI/TextureFormatDXGI.h
)

set(${PROJECT_NAME}_CORE_SOURCE
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

set(${PROJECT_NAME}_GRAPHICS_SOURCE
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

set(${PROJECT_NAME}_GUI_SOURCE
	src/GUI/GUI.cpp
	src/GUI_DX12.cpp
)

set(${PROJECT_NAME}_DX12_SOURCE
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

set(${PROJECT_NAME}_DXGI_SOURCE
	src/Graphics/DXGI/AdapterDXGI.cpp
	src/Graphics/DXGI/DisplayDXGI.cpp
	src/Graphics/DXGI/TextureFormatDXGI.cpp
)

# Create aggregated variables for headers and source files.
set(${PROJECT_NAME}_AGGREGATE_HEADERS
	${${PROJECT_NAME}_CORE_HEADERS}
	${${PROJECT_NAME}_GRAPHICS_HEADERS}
	${${PROJECT_NAME}_GUI_HEADERS}
	${${PROJECT_NAME}_DX12_HEADERS}
	${${PROJECT_NAME}_DXGI_HEADERS}
)

set(${PROJECT_NAME}_AGGREGATE_SOURCE
	${${PROJECT_NAME}_CORE_SOURCE}
	${${PROJECT_NAME}_GRAPHICS_SOURCE}
	${${PROJECT_NAME}_GUI_SOURCE}
	${${PROJECT_NAME}_DX12_SOURCE}
	${${PROJECT_NAME}_DXGI_SOURCE}
)

include_directories("../externals/glm" "../externals/imgui")

# Create the library.
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_AGGREGATE_HEADERS} ${${PROJECT_NAME}_AGGREGATE_SOURCE})

# Specify libraries to link.
target_link_libraries(${PROJECT_NAME} Boost Assimp FreeImage)

install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
