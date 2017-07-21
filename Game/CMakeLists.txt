# Subproject details
project(Game)

# Project specific options
option(${PROJECT_NAME}_ENABLE_TEST "A test opiton." ON)

# Header and source files
set(${PROJECT_NAME}_HEADERS
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

set(${PROJECT_NAME}_SOURCE
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

include_directories("../externasl/imgui" "../externals/boost")

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCE})

# Specify libraries to link with
target_link_libraries(${PROJECT_NAME} Engine)

install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
