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

set_target_properties( Game
    PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin 
)

target_include_directories( Game
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_definitions( Game 
    PRIVATE $<$<CONFIG:Debug>:_SCL_SECURE_NO_WARNINGS>
)

# Disable warnings:
#   4250: Occurs when a derived class contains a function with the same name as one of it's base classes.
#         This warning is common in cases where the derived class inherits from multiple bases classes who both inherit from another common base (Diamond of death problem).
#         In this project, there are a few instances of the Diamond of Death problem but I'm not entirely sure how to fix it without refactoring everything in the Engine project.
#         I will try to fix this issue in another incarnation of this project.
#   4251: This warning occurs when DLL exporting a class that contains (private) members deriving from a type in the C++ STL (like std::vector or std::string).
#         Since these (private) class members are never exported directly, it is safe to ignore this warning.
target_compile_options( Game
    PRIVATE /wd"4250" /wd"4251"
)

target_compile_options( Game
    PRIVATE $<$<CONFIG:Debug>:/bigobj>
)
# Enable precompiled headers for faster compiliation.
set_source_files_properties( ${Game_SOURCE} 
    PROPERTIES
        COMPILE_FLAGS /Yu"GamePCH.h"
)

set_source_files_properties( src/GamePCH.cpp
    PROPERTIES
        COMPILE_FLAGS /Yc"GamePCH.h"
)

# Specify libraries to link with
target_link_libraries(Game PRIVATE Engine)

install(TARGETS Game
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
