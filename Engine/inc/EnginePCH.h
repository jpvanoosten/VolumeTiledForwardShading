#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file EnginePCH.h
 *  @date December 20, 2015
 *  @author Jeremiah
 *
 *  @brief Precompiled header file for Engine project.
 */

#include "../resource.h"

// Windows Headers
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <windef.h>
#include <process.h>    // For _beginthreadx (used in ReadDirectoryChanges.cpp)
#include <Shlwapi.h>    // For PathFindFileNameW (used in ReadDirectoryChanges.cpp)
#pragma comment(lib, "Shlwapi.lib")

#pragma comment(lib,"winmm.lib")        // For joystick controls

// Windows Runtime library (needed for Microsoft::WRL::ComPtr<> template class)
#include <wrl.h>

// DirectX
// DirectX 12 specific headers.
// DirectX 12 headers and libraries are included in Windows 10 SDK
// and is part of the Visual Studio 2015 installation.
#if defined(_WIN32_WINNT_WIN10) 
// #   include <d3d12.h>
#   include "Graphics/DX12/d3dx12.h"
#   include <dxgi1_4.h>
#	include <d3d12sdklayers.h>
#	include <d3d12shader.h>
#	pragma comment(lib, "d3d12.lib")
#else
#   include <dxgi1_3.h>
#endif

#if defined(_DEBUG)
#   include <dxgidebug.h>
#endif

#include <Xinput.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DXProgrammableCapture.h>
#include <pix3.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "WinPixEventRuntime.lib")

// STL
#include <fcntl.h>
#include <io.h>
#include <algorithm>
#include <atomic>
#include <bitset>
#include <codecvt>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <stack>

// Import the filesystem namespace.
namespace fs = std::experimental::filesystem;

// Common lock type
using scoped_lock = std::lock_guard<std::mutex>;

// Boost
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
#include <boost/archive/xml_woarchive.hpp>
#include <boost/serialization/vector.hpp>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

// Assimp
#define ASSIMP_DLL
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

// FreeImage
#define FREEIMAGE_COLORORDER 1 // FREEIMAGE_COLORORDER_RGB
#include <FreeImage.h>

// ImGui
#include <imgui.h>
