cmake_minimum_required( VERSION 3.9.0 )

set( Game_VERSION_MAJOR 1 )
set( Game_VERSION_MINOR 0 )
set( Game_VERSION_PATCH 0 )
set( Game_VERSION_TWEAK 0 )

set( Game_VERSION ${Game_VERSION_MAJOR}.${Game_VERSION_MINOR}.${Game_VERSION_PATCH}.${Game_VERSION_TWEAK} )

# Subproject details
project(Game VERSION ${Game_VERSION} )

# Project specific options
option(Game_ENABLE_TEST "A test option." ON)

# Header and source files
set(Game_HEADERS
	inc/AbstractPass.h
	inc/BasePass.h
	inc/CameraController.h
	inc/ClearRenderTargetPass.h
	inc/CompositePass.h
	inc/ConfigurationSettings.h
	inc/ConfigurationSettings.inl
	inc/ConstantBuffers.h
	inc/GamePCH.h
	inc/InvokeFunctionPass.h
	inc/LightsPass.h
	inc/OpaquePass.h
	inc/PopProfileMarkerPass.h
	inc/PostprocessPass.h
	inc/PrintProfileDataVisitor.h
	inc/PushProfileMarkerPass.h
	inc/RenderPass.h
	inc/RenderTechnique.h
	inc/TransparentPass.h
)

source_group( "Header Files" FILES ${Game_HEADERS} )

set(Game_SOURCE
	src/AbstractPass.cpp
	src/BasePass.cpp
	src/CameraController.cpp
	src/ClearRenderTargetPass.cpp
	src/CompositePass.cpp
	src/ConfigurationSettings.cpp
	src/GamePCH.cpp
	src/InvokeFunctionPass.cpp
	src/LightsPass.cpp
	src/main.cpp
	src/OpaquePass.cpp
	src/PopProfileMarkerPass.cpp
	src/PostprocessPass.cpp
	src/PrintProfileDataVisitor.cpp
	src/PushProfileMarkerPass.cpp
	src/RenderTechnique.cpp
	src/TransparentPass.cpp
)

source_group( "Source Files" FILES ${Game_SOURCE} )

add_executable(Game WIN32 ${Game_HEADERS} ${Game_SOURCE})

target_include_directories( Game
	PUBLIC ./inc/
)

# Specify libraries to link with
target_link_libraries(Game PRIVATE Engine)

install(TARGETS Game
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
