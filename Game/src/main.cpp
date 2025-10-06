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
 *  @file main.cpp
 *  @date December 21, 2015
 *  @author Jeremiah
 *
 *  @brief The main application bootstrap.
 */

#include <GamePCH.h>
#include <EngineIncludes.h>

#include <ConstantBuffers.h>
#include <ConfigurationSettings.h>
#include <CameraController.h>
#include <RenderTechnique.h>
#include <CompositePass.h>
#include <ClearRenderTargetPass.h>
#include <OpaquePass.h>
#include <TransparentPass.h>
#include <PushProfileMarkerPass.h>
#include <PopProfileMarkerPass.h>
#include <InvokeFunctionPass.h>
#include <LightsPass.h>
#include <PostprocessPass.h>
#include <PrintProfileDataVisitor.h>

#include <Graphics/DX12/ApplicationDX12.h>

using namespace Core;
using namespace Graphics;
using namespace std::chrono;

enum class RenderingTechnique
{
    Forward,
    ForwardPlus,
    Clustered,
    Clustered_Optimized,
    NumTechniques
};

const char* RenderTechniqueName[] = {
    "Forward",
    "Forward+",
    "Clustered",
    "Clustered (Optimized)"
};

RenderingTechnique g_RenderingTechnique = RenderingTechnique::Clustered_Optimized;

using RenderDeviceList = std::vector<std::shared_ptr<Graphics::Device>>;

ApplicationDX12 g_Application;
ConfigurationSettings g_Config;

std::shared_ptr<Graphics::Window> g_RenderWindow;
std::shared_ptr<Device> g_RenderDevice;

std::shared_ptr<Graphics::Camera> g_Camera;
std::shared_ptr<CameraController> g_CameraController;

uint32_t g_WindowWidth = 1280;
uint32_t g_WindowHeight = 720;

// The block size of a single grid element
// used for light culling in the Forward+ lighting 
// technique.
uint32_t g_LightGridBlockSize = 32;

// Block size of a grid element used for 
// clustered rendering.
uint32_t g_ClusterGridBlockSize = 64;


// Cluster Grid data is used for clustered rendering.
ClusterDataCB g_ClusterDataCB;

// Toggle animations (lights moving).
bool g_Animate = false;

std::future<bool> g_LoadingTask;
std::atomic_bool g_IsLoading = true;

// Render target for the depth prepass.
std::shared_ptr<RenderTarget> g_DepthOnlyRenderTarget;

// Light buffers.
std::shared_ptr<StructuredBuffer> g_PointLightsBuffer;
std::shared_ptr<StructuredBuffer> g_SpotLightsBuffer;
std::shared_ptr<StructuredBuffer> g_DirectionalLightsBuffer;

// Read back buffers for light buffers.
std::shared_ptr<ReadbackBuffer> g_PointLightsReadbackBuffer;
std::shared_ptr<ReadbackBuffer> g_SpotLightsReadbackBuffer;
std::shared_ptr<ReadbackBuffer> g_DirectionalLightsReadbackBuffer;

// Grid frustums for light culling.
std::shared_ptr<StructuredBuffer> g_GridFrustums;

// Index counters for Forward+ light culling. 
// 2 buffers for each light type. 1 for opaque pass, 1 for transparent pass.
std::shared_ptr<StructuredBuffer> g_PointLightIndexCounter[2];
std::shared_ptr<StructuredBuffer> g_SpotLightIndexCounter[2];
std::shared_ptr<StructuredBuffer> g_PointLightIndexList[2];
std::shared_ptr<StructuredBuffer> g_SpotLightIndexList[2];

// Index counters for Clustered lighting
std::shared_ptr<StructuredBuffer> g_PointLightIndexCounter_Cluster;
std::shared_ptr<StructuredBuffer> g_SpotLightIndexCounter_Cluster;
// Index lists for Clustered lighting.
std::shared_ptr<StructuredBuffer> g_PointLightIndexList_Cluster;
std::shared_ptr<StructuredBuffer> g_SpotLightIndexList_Cluster;
// Light grids for Clustered lighting.
std::shared_ptr<StructuredBuffer> g_PointLightGrid_Cluster;
std::shared_ptr<StructuredBuffer> g_SpotLightGrid_Cluster;

// Cluster gird buffer for storing a flag for each grid cell that contains a sample.
// This buffer stores a single 32-bit flag (0 for no sample, 1 for a sample)
// for each grid cell. For a 1080p resolution with a 64x64 cell size, the grid will 
// contain at most 30 x 17 x 193 cells (98,430 cells) * 4 bytes (to store a uint)
// making the grid buffer 393,720 bytes (393.72 KB).
std::shared_ptr<StructuredBuffer> g_ClusterFlags;

// After finding clusters that contain samples, we want to find only
// the unique clusters (so that we can optimize the light assignment compute shader). 
// To do this, we will use a structured buffer to append the unique 1D cluster IDs to.
// The Unique clusters buffer is double buffered so we can use the results from the previous 
// frame while updating the unique clusters in the current frame.
std::shared_ptr<StructuredBuffer> g_UniqueClusters;

// This buffer is used store the dispatch indirect arguments to the AssignLightsToClusters compute shader.
// It is populated in the FindUniqueClusters compute shader.
std::shared_ptr<ByteAddressBuffer> g_AssignLightsToClustersArgumentBuffer;
// This buffer is used to store the Draw indirect arguments that are used to draw
// the debug clusters.
std::shared_ptr<ByteAddressBuffer> g_DebugClustersDrawIndirectArgumentBuffer;

// The unique clusters that were computed in the previous frame.
// (Primarily used for debugging).
std::shared_ptr<StructuredBuffer> g_PreviousUniqueClusters;

// AABBs for cluster grid.
std::shared_ptr<StructuredBuffer> g_ClusterAABBs;

// AABB for all lights in the scene.
std::shared_ptr<StructuredBuffer> g_LightsAABB;

// Morton codes for point lights.
std::shared_ptr<StructuredBuffer> g_PointLightMortonCodes;
// When sorting, this buffer will store the indices for the point lights.
std::shared_ptr<StructuredBuffer> g_PointLightIndices;

// For sorting, we need to double buffer the output.
std::shared_ptr<StructuredBuffer> g_PointLightMortonCodes_OUT;
std::shared_ptr<StructuredBuffer> g_PointLightIndices_OUT;

// BVH for point lights.
std::shared_ptr<StructuredBuffer> g_PointLightBVH;

// Morton codes for spot lights.
std::shared_ptr<StructuredBuffer> g_SpotLightMortonCodes;
std::shared_ptr<StructuredBuffer> g_SpotLightIndices;

// For sorting, we need to double buffer the output.
std::shared_ptr<StructuredBuffer> g_SpotLightMortonCodes_OUT;
std::shared_ptr<StructuredBuffer> g_SpotLightIndices_OUT;

// BVH for spot lights.
std::shared_ptr<StructuredBuffer> g_SpotLightBVH;

// Merge path partitions.
std::shared_ptr<StructuredBuffer> g_MergePathPartitions;

// The number of threads per thread group (for Radix sort and Merge sort).
const uint32_t SORT_NUM_THREADS_PER_THREAD_GROUP = 256;

// The number of elements that each merge sort thread will sort.
const uint32_t SORT_ELEMENTS_PER_THREAD = 8;

const uint32_t BVH_NUM_THREADS = 32 * 16;

// A structured buffer to store random colors. Used to debug sample clusters.
std::shared_ptr<StructuredBuffer> g_ClusterColors;

// For the Forward+ light index list, we need to make a guess as to the average 
// number of overlapping lights per tile. It could be possible to refine this
// value at runtime (if it is underestimated) but for now, I'll just take a guess
// of about 100 lights (which may be an overestimation, but better over than under). 
// The total size of the buffer will be determined by the grid size but for 16x16
// tiles at 1080p, we would need 120x68 (8,160) tiles * 100 light indices * 4 bytes (to store a uint)
// making the light index list 3,264,000 bytes (3.264 MB)
const uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_TILE = 100u;

// For the light index lists for clustered rendering, we need to make a guess as to the 
// average number of overlapping lights per cluster AABB. For clustered lighting we
// can make a more conservative estimation than was possible with Forward+ 
// since most cluster bounding volumes will not contain any samples and therefor 
// no lights will be assigned to the cluster. For this experiment, I will make an 
// estimate of an average of 20 lights per cluster. This may seem too conservative
// but considering that only about 10-15% of all clusters will contain samples, this 
// estimate seems reasonable.
// The total size of the light index list is determined by the cluster grid size.
// For 1080p screen resolution, 45 deg camera field of view, and a cluster block 
// screen size of 64x64 pixels the cluster grid will have 30x17x193 (98,430) clusters
// * 20 light indices * 4 bytes (to store a uint) will require 7,874,400 bytes (7.874 MB).
const uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_CLUSTER = 20u;

// Textures for the light grids.
std::shared_ptr<Texture> g_PointLightGrid[2];
std::shared_ptr<Texture> g_SpotLightGrid[2];

// Texture for debugging light culling compute shader.
std::shared_ptr<Texture> g_LightCullingDebugTexture;

// Texture for debugging sample clustering.
std::shared_ptr<Texture> g_ClusterSamplesDebugTexture;
std::shared_ptr<RenderTarget> g_ClusterSamplesRenderTarget;

// Pipeline states
std::shared_ptr<ComputePipelineState> g_UpdateLightsPSO;                        // Compute pipeline used to animate the lights.
std::shared_ptr<ComputePipelineState> g_ReduceLightsAABB1PSO;                   // 1st pass of lights AABB reduction.
std::shared_ptr<ComputePipelineState> g_ReduceLightsAABB2PSO;                   // 2nd pass of lights AABB reduction.
std::shared_ptr<ComputePipelineState> g_ComputeLightMortonCodesPSO;             // Compute Morton codes for point and spot lights.
std::shared_ptr<ComputePipelineState> g_RadixSortPSO;                           // Perform a radix sort over the Morton codes.
std::shared_ptr<ComputePipelineState> g_MergePathPartitionsPSO;                 // Compute merge path partitions for merge sort.
std::shared_ptr<ComputePipelineState> g_MergeSortPSO;                           // Merge sort compute shader.
std::shared_ptr<ComputePipelineState> g_BuildBVHBottomPSO;                      // Build bottom BVH compute pipeline state.
std::shared_ptr<ComputePipelineState> g_BuildBVHTopPSO;                         // Build top BVH compute pipeline state.
std::shared_ptr<ComputePipelineState> g_ComputeGridFrustumsPSO;                 // Compute the grid frustums for Forward+ (only on screen resolution changes)
std::shared_ptr<ComputePipelineState> g_ComputeClusterAABBsPSO;                 // Compute cluster AABBs (only if screen resolution changes)
std::shared_ptr<ComputePipelineState> g_FindUniqueClustersPSO;                  // Convert cluster flags to a contagious list of cluster IDs
std::shared_ptr<ComputePipelineState> g_UpdateIndirectArgumentBuffersPSO;       // Update the indirect argument buffers based on the number of activated clusters.
std::shared_ptr<ComputePipelineState> g_AssignLightsToClustersPSO;              // Perform brute-force light assignment to clusters.
std::shared_ptr<ComputePipelineState> g_AssignLightsToClustersBVHPSO;           // Perform light assignment using BVH acceleration structure.
std::shared_ptr<GraphicsPipelineState> g_ForwardOpaquePSO;                      // Forward rendering opaque pass.
std::shared_ptr<GraphicsPipelineState> g_ForwardTransparentPSO;                 // Forward rendering transparent pass.
std::shared_ptr<GraphicsPipelineState> g_ForwardPlusOpaquePSO;                  // Forward+ rendering opaque pass.
std::shared_ptr<GraphicsPipelineState> g_ForwardPlusTransparentPSO;             // Forward+ rendering transparent pass.
std::shared_ptr<GraphicsPipelineState> g_ClusteredOpaquePSO;                    // Clustered rendering opaque pass.
std::shared_ptr<GraphicsPipelineState> g_ClusteredTransparentPSO;               // Clustered rendering transparent pass.
std::shared_ptr<GraphicsPipelineState> g_DepthPrepassPSO;                       // Depth pre-pass (used for all techniques).
std::shared_ptr<GraphicsPipelineState> g_DebugPointLightsPSO;                   // Render point light volumes (used for debugging).
std::shared_ptr<GraphicsPipelineState> g_DebugSpotLightsPSO;                    // Render spot light volumes (used for debugging).
std::shared_ptr<GraphicsPipelineState> g_DebugDepthTexturePSO;                  // Render a debug texture full screen.
std::shared_ptr<GraphicsPipelineState> g_LoadingScreenPSO;                      // Display a loading screen.
std::shared_ptr<GraphicsPipelineState> g_ClusterSamplesPSO;                     // Enables the clusters that contain samples.
std::shared_ptr<GraphicsPipelineState> g_DebugClustersPSO;                      // Visualize cluster AABBs.

// Indirect command signature to perform light assignments to clusters.
std::shared_ptr<IndirectCommandSignature> g_AssignLightToClustersCommandSignature;
std::shared_ptr<IndirectCommandSignature> g_DebugClustersCommandSignature;

// Render techniques.
RenderTechnique g_LoadingScreenTechnique;
RenderTechnique g_ForwardRenderingTechnique;
RenderTechnique g_ForwardPlusRenderingTechnique;
RenderTechnique g_ClusteredRenderingTechnique;

// A GPU fence object to synchronize rendering.
std::shared_ptr<Fence> g_RenderFence;

// Some passes that can be toggled.
std::shared_ptr<CompositePass> g_DebugLightsPass;
std::shared_ptr<PostprocessPass> g_RenderDebugTexturePass;
std::shared_ptr<CompositePass> g_DebugClustersPass;
std::shared_ptr<CompositePass> g_DebugLightCountsPass;

bool g_RenderLights = false;
bool g_RenderDebugTexture = false;
bool g_RenderDebugClusters = false;
bool g_InvertY = false; // Invert the Y on controller input.

// Some scenes for rendering lights and stuff...
std::shared_ptr<Scene> g_Sphere;
std::shared_ptr<Scene> g_Cone;
std::shared_ptr<Scene> g_Axis;
std::shared_ptr<Scene> g_Plane;
std::shared_ptr<Scene> g_Arrow;

// Variables for showing/hiding GUI windows.
static bool g_ShowStatistics = true;
static bool g_ShowTestWindow = false;
static bool g_ShowProfiler = false;
static bool g_ShowGenerateLights = false;
static bool g_ShowLightsHierarchy = false;
static bool g_ShowOptionsWindow = false;
static bool g_ShowNotification = false;
static std::string g_NotificationText;
static std::chrono::high_resolution_clock::time_point g_NotificationTimer;
static float g_NotificationDuration = 0.0f;

// For selecting and editing light properties.
PointLight* g_SelectedPointLight;
SpotLight* g_SelectedSpotLight;
DirectionalLight* g_SelectedDirLight;

// The previously used view matrix.
// The view matrix can be frozen so that the
// AABB of the occupied clusters can be visualized.
glm::mat4 g_PreviousViewMatrix = glm::mat4(1.0);
// When freezing the camera view to visualize clusters,
// we also want to freeze the updating of the unique clusters in the view
// so that we always visualize the clusters from the previous viewing angle.
bool g_UpdateUniqueClusters = true;

/**
 * Pressing F will focus the camera on the currently selected light.
 * Holding F will cause the camera to follow the currently selected light.
 */
bool g_TrackSelectedLight = false;

// How far away to place the camera while focusing on the selected light.
// Use the mouse scroll wheel to adjust this value.
float g_FocusDistance = FLT_MAX;

// Profiling stuff
enum class StatType
{
    CPU,
    GPU
};
const ProfileNode* g_SelectedProfileMarker = nullptr;
StatType g_SelectedStateType;

static const ImColor IntelBlue = ImColor(0, 122, 197);
static const ImColor NvidiaGreen = ImColor(119, 185, 0);

void SelectPointLight( PointLight* pPointLight );
void SelectSpotLight( SpotLight* pSpotLight );
void SelectDirLight( DirectionalLight* pDirLight );

void OnWindowClose( WindowCloseEventArgs& e );
void OnKeyPressed( KeyEventArgs& e );
void OnKeyReleased( KeyEventArgs& e );
void OnMouseWheel( MouseWheelEventArgs& e );
void OnUpdate( UpdateEventArgs& e );
void OnPreRender( RenderEventArgs& e );
void OnRender( RenderEventArgs& e );
void OnPostRender( RenderEventArgs& e );
void OnResize( ResizeEventArgs& e );
void OnGUI( RenderEventArgs& e );
void OnLoadingProgress( ProgressEventArgs& e );

bool LoadAssets();

// GUI functions
void ShowStatistics( bool& bShowWindow );
void ShowOptionsWindow( bool& bShowWindow );

// Generate light structured buffers.
void CreateLightBuffers();

// Readback light buffer contents.
void ReadbackPointLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr );
void ReadbackSpotLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr );
void ReadbackDirLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr );

// Focus the camera on the currently selected light.
void FocusCurrentLight();

template<typename LightType>
std::vector<LightType> GenerateLights( uint32_t numLights );
void GenerateLights();

// A function to randomly generate colors.
std::vector<glm::vec4> GenerateColors( uint32_t numColors );

// Get the number of BVH levels given the number of leaf nodes.
uint32_t GetNumLevels( uint32_t numLeaves );

// Compute the number of (child) nodes needed to represent a BVH that consists of a number of leaf nodes.
uint32_t GetNumNodes( uint32_t numLeaves );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    LogManager::Init();

    static const uint8_t MAX_FILE_PATH = 255;
    WCHAR moduleFilename[MAX_FILE_PATH];
    DWORD moduleFilenameLength = GetModuleFileNameW(0, moduleFilename, MAX_FILE_PATH);
    fs::path modulePath(moduleFilename);

    // Check to see if the application is running in the expected working directory.
    if (!fs::exists("../Conf/DefaultConfiguration.3dgep"))
    {
        fs::current_path(modulePath.parent_path());

        // If the default configuration file is still not found, we are still not in the right working directory.
        if (!fs::exists("../Conf/DefaultConfiguration.3dgep"))
        {
            MessageBoxA(NULL, "The DefaultConfiguration.3dgep file was not found.\nAre you sure you are running from the correct location?", "Default Configuration Not Found", MB_OK | MB_ICONERROR);
            exit(1);
        }
    }

    // Setup logging to text file.
    std::shared_ptr<LogStreamFile> defaultFileLogStream = std::make_shared<LogStreamFile>(modulePath.replace_extension("log"));
    LogManager::RegisterLogStream(defaultFileLogStream);

    // Setup logging to Visual Studio output window.
    std::shared_ptr<LogStreamVS> vsLogStream = std::make_shared<LogStreamVS>();
    LogManager::RegisterLogStream( vsLogStream );

#if defined(_DEBUG)
    // Setup logging to a console.
    std::shared_ptr<LogStreamConsole> coutLogStream = std::make_shared<LogStreamConsole>();
    LogManager::RegisterLogStream( coutLogStream );

    // Log error messages to a message box.
    std::shared_ptr<LogStreamMessageBox> mbLogStream = std::make_shared<LogStreamMessageBox>( LogLevel::Error );
    LogManager::RegisterLogStream( mbLogStream );
#endif

    g_Application.RegisterDirectoryChangeListener( L"../Assets" );

    // Load configuration settings.
    int numArgs;
    LPWSTR* commandLineArguments = CommandLineToArgvW( GetCommandLineW(), &numArgs );

    std::wstring configFileName = L"../Conf/DefaultConfiguration.3dgep";
    // Parse command line arguments.
    for ( int i = 0; i < numArgs; i++ )
    {
        if ( wcscmp( commandLineArguments[i], L"-c" ) == 0 || wcscmp( commandLineArguments[i], L"--config" ) == 0 )
        {
            configFileName = commandLineArguments[++i];
        }
    }

    if ( !g_Config.Load( configFileName ) )
    {
        // Try to save a default configuration file
        // The user can edit this file to create new configurations.
        g_Config.Save( configFileName );

        LOG_ERROR( "Failed to load configuration file ", configFileName );
        return -1;
    }

//    g_Config.Save( configFileName );

    g_Application.SetAssetSearchPaths( g_Config.GetAbsoluteSearchPaths() );
    g_Application.SetLoadingProgressTotal( g_Config.LoadingProgressTotal );

    g_WindowWidth = g_Config.WindowWidth;
    g_WindowHeight = g_Config.WindowHeight;

    // Setup the window title for the render window.
    std::string windowName = "Volume Tiled Forward Rendering ";
#if defined(_WIN32_WINNT_WIN10) && _WIN32_WINNT == _WIN32_WINNT_WIN10
    windowName += "/ Win 10 ";
#elif defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT == _WIN32_WINNT_WINBLUE
    windowName += "/ Win 8.1 ";
#elif defined(_WIN32_WINNT_WIN8) && _WIN32_WINNT == _WIN32_WINNT_WIN8
    windowName += "/ Win 8 ";
#elif defined(_WIN32_WINNT_WIN7) && _WIN32_WINNT == _WIN32_WINNT_WIN7
    windowName += "/ Win 7 ";
#endif

#if defined(_WIN64)
    windowName += "/ x64 ";
#elif defined(_WIN32)
    windowName += "/ x86 ";
#endif

#if defined(_DEBUG)
    windowName += "/ Debug ";
#endif

    windowName += "[" + ConvertString( g_Config.SceneFileName  ) + "]";

    bool useWarpAdapter = false;

    // Iterate adapters
    const AdapterList& adapters = g_Application.GetAdapters();
    std::shared_ptr<Adapter> adapter;
    if ( useWarpAdapter || adapters.empty() )
    {
        LogManager::LogWarning( "Using Warp Adapter." );
        adapter = g_Application.GetWarpAdapter();
    }
    else
    {
        adapter = adapters[0];
    }

    LogManager::LogInfo( L"Adapter: ", adapter->GetDescription() );
    g_RenderDevice = g_Application.CreateDevice( adapter );
    //g_RenderDevice = g_Application.CreateDevice( g_Application.GetWarpAdapter() );

    uint8_t multiSampleCount = g_Config.MultiSampleEnable ? 16 : 1;

    TextureFormat colorFormat( Graphics::TextureComponents::RGBA, TextureType::UnsignedNormalized, multiSampleCount, 8, 8, 8, 8, 0, 0 );
    TextureFormat depthFormat( Graphics::TextureComponents::DepthStencil, TextureType::UnsignedNormalized, multiSampleCount, 0, 0, 0, 0, 24, 8 );
    //TextureFormat depthFormat( Graphics::TextureComponents::Depth, TextureType::Float, 1, 0, 0, 0, 0, 32, 0 );
    g_RenderWindow = g_Application.CreateWindow( g_RenderDevice, windowName, g_WindowWidth, g_WindowHeight, false, g_Config.VSync, colorFormat, depthFormat );
    
    Profiler::Init( g_RenderDevice, 2048 );
    GUI::Init( g_RenderDevice, g_RenderWindow );

    // Create a texture for debugging the light culling compute shader.
    TextureFormat lightCullingDebugTextureFormat( TextureComponents::RGBA,
                                                  TextureType::UnsignedNormalized,
                                                  1,
                                                  8, 8, 8, 8, 0, 0 );
    g_LightCullingDebugTexture = g_RenderDevice->CreateTexture2D( g_WindowWidth, g_WindowHeight, 1, lightCullingDebugTextureFormat );
    g_LightCullingDebugTexture->SetName( L"Light Culling Debug Texture" );

    // Create a texture for debugging clusters.
    TextureFormat clusterAssignmentDebugTextureFormat( TextureComponents::RGBA,
                                                       TextureType::UnsignedNormalized,
                                                       multiSampleCount,
                                                       8, 8, 8, 8, 0, 0 );
    g_ClusterSamplesDebugTexture = g_RenderDevice->CreateTexture2D( g_WindowWidth, g_WindowHeight, 1, clusterAssignmentDebugTextureFormat );
    g_ClusterSamplesDebugTexture->SetName( L"Cluster Samples Debug Texture" );

    CreateLightBuffers();

    g_UpdateLightsPSO = g_RenderDevice->CreateComputePipelineState();
    g_ReduceLightsAABB1PSO = g_RenderDevice->CreateComputePipelineState();
    g_ReduceLightsAABB2PSO = g_RenderDevice->CreateComputePipelineState();
    g_ComputeLightMortonCodesPSO = g_RenderDevice->CreateComputePipelineState();
    g_RadixSortPSO = g_RenderDevice->CreateComputePipelineState();
    g_MergePathPartitionsPSO = g_RenderDevice->CreateComputePipelineState();
    g_MergeSortPSO = g_RenderDevice->CreateComputePipelineState();
    g_BuildBVHBottomPSO = g_RenderDevice->CreateComputePipelineState();
    g_BuildBVHTopPSO = g_RenderDevice->CreateComputePipelineState();
    g_ComputeGridFrustumsPSO = g_RenderDevice->CreateComputePipelineState();
    g_ComputeClusterAABBsPSO = g_RenderDevice->CreateComputePipelineState();
    g_FindUniqueClustersPSO = g_RenderDevice->CreateComputePipelineState();
    g_UpdateIndirectArgumentBuffersPSO = g_RenderDevice->CreateComputePipelineState();
    g_AssignLightsToClustersPSO = g_RenderDevice->CreateComputePipelineState();
    g_AssignLightsToClustersBVHPSO = g_RenderDevice->CreateComputePipelineState();
    g_DepthPrepassPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ForwardOpaquePSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ForwardTransparentPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ForwardPlusOpaquePSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ForwardPlusTransparentPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ClusteredOpaquePSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ClusteredTransparentPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_DebugPointLightsPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_DebugSpotLightsPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_DebugDepthTexturePSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_LoadingScreenPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_ClusterSamplesPSO = g_RenderDevice->CreateGraphicsPipelineState();
    g_DebugClustersPSO = g_RenderDevice->CreateGraphicsPipelineState();

    // Create an indirect command signature to perform the light assignment to clusters.
    g_AssignLightToClustersCommandSignature = g_RenderDevice->CreateIndirectCommandSignature();
    g_AssignLightToClustersCommandSignature->AppendCommandArgument( IndirectArgument( IndirectArgumentType::Dispatch ) );

    // Create an indirect command signature to perform the debug cluster draw pass.
    g_DebugClustersCommandSignature = g_RenderDevice->CreateIndirectCommandSignature();
    g_DebugClustersCommandSignature->AppendCommandArgument( IndirectArgument( IndirectArgumentType::Draw ) );

    // Load a compute shader to compute the grid frustums.
    ShaderMacros shaderMacros;
    shaderMacros["BLOCK_SIZE"] = std::to_string( g_LightGridBlockSize );
    auto computeGridFrustumsCS = g_RenderDevice->CreateShader();
    computeGridFrustumsCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/ComputeGridFrustums_CS.hlsl", "main", shaderMacros );
    g_ComputeGridFrustumsPSO->SetShader( computeGridFrustumsCS );

    // Load a compute shader to compute the AABBs for the cluster grid.
    auto computeClusterAABBsCS = g_RenderDevice->CreateShader();
    computeClusterAABBsCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/ComputeClusterAABBs_CS.hlsl" );
    g_ComputeClusterAABBsPSO->SetShader( computeClusterAABBsCS );

    // Setup loading screen rendering technique.
    {
        // A compute queue is used to load assets because a compute shader is needed
        // to generate the mipmaps for textures.
        auto commandQueue = g_RenderDevice->GetComputeQueue();
        auto commandBuffer = commandQueue->GetComputeCommandBuffer();

        // Load the loading screen texture.
        if ( !fs::exists( g_Config.LoadingScreenFileName ) || !fs::is_regular_file( g_Config.LoadingScreenFileName ) )
        {
            // TODO: This should be in the resource file for the application.
            g_Config.LoadingScreenFileName = L"../Assets/textures/Loading Screen (Default).dds";
            //g_Config.LoadingScreenFileName = L"D:/Projects/UU/INFOMGMT/Game/Assets/models/sanmiguel/Textures/06-30-1997.DDS";
        }
        auto loadingScreenTexture = g_RenderDevice->CreateTexture( commandBuffer, g_Config.LoadingScreenFileName );

        // Load a vertex shader and pixel shader that can be used to show a texture on screen.
        auto debugTextureVS = g_RenderDevice->CreateShader();
        debugTextureVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/DebugTexture_VS.hlsl" );

        auto debugTexturePS = g_RenderDevice->CreateShader();
        debugTexturePS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugTexture_PS.hlsl", "DebugTexture_PS" );

        g_LoadingScreenPSO->SetShader( ShaderType::Vertex, debugTextureVS );
        g_LoadingScreenPSO->SetShader( ShaderType::Pixel, debugTexturePS );
        g_LoadingScreenPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );

        g_LoadingScreenTechnique.AddPass( std::make_shared<ClearRenderTargetPass>( g_RenderWindow->GetRenderTarget(), ClearFlags::DepthStencil, ClearColor::CornflowerBlue ) );
        //     // A full screen quad is used for post process effects.
        auto fullScreenQuad = g_RenderDevice->CreateScreenQuad( commandBuffer, 0, 1920, 1080, 0 );
        glm::mat4 fullScreenOrthographicProjection = glm::ortho<float>( 0, 1920, 1080, 0 );

        g_LoadingScreenTechnique.AddPass( std::make_shared<PostprocessPass>( fullScreenQuad, g_LoadingScreenPSO, fullScreenOrthographicProjection, loadingScreenTexture ) );

        // Submit the commands and wait for them to finish.
        commandQueue->Submit( commandBuffer )->WaitFor();
    }

    g_Camera = std::make_shared<Graphics::Camera>();

    g_Camera->SetTranslate( g_Config.CameraPosition );
    g_Camera->SetRotate( g_Config.CameraRotation );

    g_CameraController = std::make_shared<CameraController>( g_Camera );

    // Bind events to the camera controller.
    g_RenderWindow->KeyPressed += boost::bind( &CameraController::OnKeyboard, g_CameraController, _1 );
    g_RenderWindow->KeyReleased += boost::bind( &CameraController::OnKeyboard, g_CameraController, _1 );
    g_RenderWindow->MouseMoved += boost::bind( &CameraController::OnMouseMotion, g_CameraController, _1 );
    g_RenderWindow->MouseWheel += boost::bind( &CameraController::OnMouseWheel, g_CameraController, _1 );
    g_RenderWindow->MouseButtonPressed += boost::bind( &CameraController::OnMouseButton, g_CameraController, _1 );
    g_RenderWindow->MouseButtonReleased += boost::bind( &CameraController::OnMouseButton, g_CameraController, _1 );

    // Joystick events come from the application
    g_Application.JoystickButtonPressed += boost::bind( &CameraController::OnJoystickButton, g_CameraController, _1 );
    g_Application.JoystickButtonReleased += boost::bind( &CameraController::OnJoystickButton, g_CameraController, _1 );
    g_Application.JoystickPOV += boost::bind( &CameraController::OnJoystickPOV, g_CameraController, _1 );
    g_Application.JoystickAxis += boost::bind( &CameraController::OnJoystickAxis, g_CameraController, _1 );

    // Attach events
    g_RenderWindow->MouseWheel += &OnMouseWheel;
    g_RenderWindow->Update += &OnUpdate;
    g_RenderWindow->PreRender += &OnPreRender;
    g_RenderWindow->Render += &OnRender;
    g_RenderWindow->PostRender += &OnPostRender;
    g_RenderWindow->Resize += &OnResize;
    g_RenderWindow->KeyPressed += &OnKeyPressed;
    g_RenderWindow->KeyReleased += &OnKeyReleased;
    g_RenderWindow->Close += &OnWindowClose;
    
    g_RenderWindow->ShowWindow();

    // Start async loading task
    g_LoadingTask = std::async( std::launch::async, &LoadAssets );
    //g_Application.BeginGraphicsAnalysis();
    //LoadAssets();
    //g_Application.EndGraphicsAnalysis();

    // Run the application.
    uint32_t result = g_Application.Run();

    // It is possible that the user closes the main window before the loading task is complete.
    // If that happens, we need to wait for the task to complete before releasing all the resources.
    g_LoadingTask.get();

    Profiler::Shutdown();
    GUI::Shutdown();
    LogManager::Shutdown();

    ::CoUninitialize();

    return result;
}

void OnLoadingProgress( ProgressEventArgs& e )
{
    g_Application.IncrementLoadingProgress();
}

bool LoadAssets()
{
    DepthMode depthFuncEqual( true, DepthWrite::Enable, CompareFunction::Equal );
    DepthMode depthFuncLessEqual( true, DepthWrite::Enable, CompareFunction::LessOrEqual );
    DepthMode disableDepthWrite( true, DepthWrite::Disable );
    DepthMode disableDepthWriteLessEqual( true, DepthWrite::Disable, CompareFunction::LessOrEqual );
    DepthMode disableDepthTest( false );
    BlendMode alphaBlending( true, false, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha );
    BlendMode additiveBlending( true, false, BlendFactor::SrcAlpha, BlendFactor::One );

    auto linearClampSampler = g_RenderDevice->CreateSampler();
    auto linearRepeatSampler = g_RenderDevice->CreateSampler();
    auto anisotropicSampler = g_RenderDevice->CreateSampler();

    linearClampSampler->SetFilter( Filter::Linear, Filter::Linear, Filter::Linear );
    linearClampSampler->SetWrapMode( WrapMode::Clamp, WrapMode::Clamp, WrapMode::Clamp );

    linearRepeatSampler->SetFilter( Filter::Linear, Filter::Linear, Filter::Linear );

    anisotropicSampler->EnableAnisotropicFiltering( true );
    anisotropicSampler->SetMaxAnisotropy( 16 );

    // A compute queue is used to load assets because a compute shader is needed
    // to generate the mipmaps for textures.
    auto commandQueue = g_RenderDevice->GetComputeQueue();
    auto commandBuffer = commandQueue->GetComputeCommandBuffer();

    auto scene = g_RenderDevice->CreateScene();
    scene->LoadingProgress += &OnLoadingProgress;
    LogManager::LogInfo( L"Loading Scene: ", g_Config.SceneFileName );
    if ( !scene->LoadFromFile( commandBuffer, g_Config.SceneFileName ) )
    {
        LOG_ERROR( "Unable to load scene file." );
        return false;
    }

    scene->GetRootNode()->SetLocalTransform( glm::scale( glm::vec3( g_Config.SceneScaleFactor ) ) );

    g_Application.IncrementLoadingProgress();

    // Load some scenes that are used to represent lights.
    g_Sphere = g_RenderDevice->CreateSphere( commandBuffer, 1.0f );

    g_Application.IncrementLoadingProgress();

    // A cone is just a cylinder with a 0 radius apex.
    g_Cone = g_RenderDevice->CreateCylinder( commandBuffer, 0.0f, 1.0f, 1.0f, glm::vec3( 0, 0, 1 ) );

    g_Application.IncrementLoadingProgress();

    // A full screen quad is used for post process effects.
    auto fullScreenQuad = g_RenderDevice->CreateScreenQuad( commandBuffer, 0, 1920, 1080, 0 );
    glm::mat4 fullScreenOrthographicProjection = glm::ortho<float>( 0, 1920, 1080, 0 );

    g_Application.IncrementLoadingProgress();

    // Create a structured buffer to store the light list index counter.
    // 2 buffers are created for each light type. 1 is used for opaque list lists
    // and 1 for transparent light lists.
    for ( uint32_t i = 0; i < 2; ++i )
    {
        g_PointLightIndexCounter[i] = g_RenderDevice->CreateStructuredBuffer( commandBuffer, 1, sizeof( uint32_t ) );
        g_PointLightIndexCounter[i]->SetName( std::wstring( L"Point Light Index Counter " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );

        g_Application.IncrementLoadingProgress();

        g_SpotLightIndexCounter[i] = g_RenderDevice->CreateStructuredBuffer( commandBuffer, 1, sizeof( uint32_t ) );
        g_SpotLightIndexCounter[i]->SetName( std::wstring( L"Spot Light Index Counter " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );

        g_Application.IncrementLoadingProgress();
    }

    // Create a structured buffer to store light list index counter for clustered rendering.
    g_PointLightIndexCounter_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, 1, sizeof( uint32_t ) );
    g_PointLightIndexCounter_Cluster->SetName( L"Spot Light Index Counter (Clustered)" );

    g_Application.IncrementLoadingProgress();

    g_SpotLightIndexCounter_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, 1, sizeof( uint32_t ) );
    g_SpotLightIndexCounter_Cluster->SetName( L"Spot Light Index Counter (Clustered)" );

    g_Application.IncrementLoadingProgress();

    // Load a texture for visualizing light counts.
    auto lightCountHeatMap = g_RenderDevice->CreateTexture( commandBuffer, L"../Assets/textures/LightCountHeatMap.psd" );
    lightCountHeatMap->SetName( L"Light Count Heat Map" );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline state object to transform the lights.
    auto updateLightsComputeShader = g_RenderDevice->CreateShader();
    updateLightsComputeShader->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/UpdateLights_CS.hlsl" );

    g_UpdateLightsPSO->SetShader( updateLightsComputeShader );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline state object to reduce the lights AABB.
    auto reduceLightsAABB1CS = g_RenderDevice->CreateShader();
    reduceLightsAABB1CS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/ReduceLightsAABB_CS.hlsl", "reduce1" );

    g_ReduceLightsAABB1PSO->SetShader( reduceLightsAABB1CS );

    g_Application.IncrementLoadingProgress();

    auto reduceLightsAABB2CS = g_RenderDevice->CreateShader();
    reduceLightsAABB2CS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/ReduceLightsAABB_CS.hlsl", "reduce2" );

    g_ReduceLightsAABB2PSO->SetShader( reduceLightsAABB2CS );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline state to compute the light Morton codes.
    auto computeLightMortonCodesCS = g_RenderDevice->CreateShader();
    computeLightMortonCodesCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/ComputeLightMortonCodes_CS.hlsl" );

    g_ComputeLightMortonCodesPSO->SetShader( computeLightMortonCodesCS );

    g_Application.IncrementLoadingProgress();

    // Setup a pipeline state to sort the Morton code values.
    ShaderMacros sortShaderMacros;
    sortShaderMacros["NUM_THREADS"] = std::to_string( SORT_NUM_THREADS_PER_THREAD_GROUP );
    sortShaderMacros["NUM_VALUES_PER_THREAD"] = std::to_string( SORT_ELEMENTS_PER_THREAD );

    auto radixSortCS = g_RenderDevice->CreateShader();
    radixSortCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/RadixSort_CS.hlsl", "main", sortShaderMacros );

    g_RadixSortPSO->SetShader( radixSortCS );

    g_Application.IncrementLoadingProgress();

    // Setup compute pipeline for computing merge path partitions.
    auto mergePathPartitionsCS = g_RenderDevice->CreateShader();
    mergePathPartitionsCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/MergeSort_CS.hlsl", "MergePathPartitions_CS", sortShaderMacros );

    g_MergePathPartitionsPSO->SetShader( mergePathPartitionsCS );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline for merge sort.
    auto mergeSortCS = g_RenderDevice->CreateShader();
    mergeSortCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/MergeSort_CS.hlsl", "MergeSort", sortShaderMacros );

    g_MergeSortPSO->SetShader( mergeSortCS );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline for BVH building.
    // Compute shader to build bottom level of BVH
    auto buildBVHBottomCS = g_RenderDevice->CreateShader();
    buildBVHBottomCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/BuildBVH_CS.hlsl", "BuildBottom" );

    g_BuildBVHBottomPSO->SetShader( buildBVHBottomCS );

    g_Application.IncrementLoadingProgress();

    // Compute shader to build upper levels of the BVH.
    auto buildBVHTopCS = g_RenderDevice->CreateShader();
    buildBVHTopCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/BuildBVH_CS.hlsl", "BuildTop" );

    g_BuildBVHTopPSO->SetShader( buildBVHTopCS );

    g_Application.IncrementLoadingProgress();

    ShaderMacros forwardPlusShaderMacros;
    forwardPlusShaderMacros["BLOCK_SIZE"] = std::to_string( g_LightGridBlockSize );

    // Setup a compute pipeline state to perform light culling.
    auto cullLightsCS = g_RenderDevice->CreateShader();
    cullLightsCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/CullLights_CS.hlsl", "main", forwardPlusShaderMacros );

    auto cullLightsPipelineState = g_RenderDevice->CreateComputePipelineState();
    cullLightsPipelineState->SetShader( cullLightsCS );

    g_Application.IncrementLoadingProgress();

    // Setup a compute pipeline state for counting lights after light assignment to clusters.
    auto countLightsCS = g_RenderDevice->CreateShader();
    countLightsCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/CountLights_CS.hlsl" );

    auto countLightsComputePipelineState = g_RenderDevice->CreateComputePipelineState();
    countLightsComputePipelineState->SetShader( countLightsCS );

    g_Application.IncrementLoadingProgress();

    // Load a common vertex shader that is used for the depth prepass.
    auto simpleVS = g_RenderDevice->CreateShader();
    simpleVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/Simple_VS.hlsl" );

    g_Application.IncrementLoadingProgress();

    // Setup depth pre-pass pipeline state.
    auto depthPrePassPS = g_RenderDevice->CreateShader();
    depthPrePassPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DepthPass_PS.hlsl" );

    g_Application.IncrementLoadingProgress();

    // Load shaders for light debugging.
    auto debugPointLightsVS = g_RenderDevice->CreateShader();
    debugPointLightsVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/DebugLights_VS.hlsl", "PointLight_VS" );

    g_Application.IncrementLoadingProgress();

    auto debugSpotLightsVS = g_RenderDevice->CreateShader();
    debugSpotLightsVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/DebugLights_VS.hlsl", "SpotLight_VS" );

    auto debugPointLightsPS = g_RenderDevice->CreateShader();
    debugPointLightsPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugLights_PS.hlsl", "PointLight_PS" );

    g_Application.IncrementLoadingProgress();

    auto debugSpotLightsPS = g_RenderDevice->CreateShader();
    debugSpotLightsPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugLights_PS.hlsl", "SpotLight_PS" );

    g_Application.IncrementLoadingProgress();

    auto debugTextureVS = g_RenderDevice->CreateShader();
    debugTextureVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/DebugTexture_VS.hlsl" );

    g_Application.IncrementLoadingProgress();

    auto debugTexturePS = g_RenderDevice->CreateShader();
    debugTexturePS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugTexture_PS.hlsl", "DebugTexture_PS" ); 

    g_Application.IncrementLoadingProgress();

    auto debugDepthTexturePS = g_RenderDevice->CreateShader();
    debugDepthTexturePS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugTexture_PS.hlsl", "DebugDepthTexture_PS" );
    
    g_Application.IncrementLoadingProgress();

    auto clusterSamplesVS = g_RenderDevice->CreateShader();
    auto clusterSamplesPS = g_RenderDevice->CreateShader();

    clusterSamplesVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/ClusterSamples_VS.hlsl" );
    g_Application.IncrementLoadingProgress();

    clusterSamplesPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/ClusterSamples_PS.hlsl" );
    g_Application.IncrementLoadingProgress();

    auto findUniqueClustersCS = g_RenderDevice->CreateShader();
    findUniqueClustersCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/FindUniqueClusters_CS.hlsl" );
    g_FindUniqueClustersPSO->SetShader( findUniqueClustersCS );

    g_Application.IncrementLoadingProgress();

    auto updateIndirectArgumentBuffersCS = g_RenderDevice->CreateShader();
    updateIndirectArgumentBuffersCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/UpdateClusterIndirectArgumentBuffers_CS.hlsl" );
    g_UpdateIndirectArgumentBuffersPSO->SetShader( updateIndirectArgumentBuffersCS );

    g_Application.IncrementLoadingProgress();

    auto assignLightsToClustersCS = g_RenderDevice->CreateShader();
    assignLightsToClustersCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/AssignLightsToClusters_CS.hlsl" );
    g_AssignLightsToClustersPSO->SetShader( assignLightsToClustersCS );

    g_Application.IncrementLoadingProgress();

    auto assignLightsToClustersBVHCS = g_RenderDevice->CreateShader();
    assignLightsToClustersBVHCS->LoadShaderFromFile( ShaderType::Compute, L"../Assets/shaders/AssignLightsToClustersBVH_CS.hlsl" );
    g_AssignLightsToClustersBVHPSO->SetShader( assignLightsToClustersBVHCS );

    g_Application.IncrementLoadingProgress();

    // Load a set of shaders that can be used to visualize the active clusters in the cluster grid.
    auto debugClustersVS = g_RenderDevice->CreateShader();
    debugClustersVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/DebugClusters_VSGSPS.hlsl", "main_VS" );

    g_Application.IncrementLoadingProgress();

    auto debugClustersGS = g_RenderDevice->CreateShader();
    debugClustersGS->LoadShaderFromFile( ShaderType::Geometry, L"../Assets/shaders/DebugClusters_VSGSPS.hlsl", "main_GS" );

    g_Application.IncrementLoadingProgress();

    auto debugClustersPS = g_RenderDevice->CreateShader();
    debugClustersPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/DebugClusters_VSGSPS.hlsl", "main_PS" );

    g_Application.IncrementLoadingProgress();

    // Create a render target that only contains a depth/stencil buffer.
    auto depthStencilTexture = g_RenderWindow->GetRenderTarget()->GetTexture( AttachmentPoint::DepthStencil );
    g_DepthOnlyRenderTarget = g_RenderDevice->CreateRenderTarget();
    g_DepthOnlyRenderTarget->AttachTexture( AttachmentPoint::DepthStencil, depthStencilTexture );

    g_Application.IncrementLoadingProgress();

    // Create a render target that can be used to visualize the sample clusters.
    g_ClusterSamplesRenderTarget = g_RenderDevice->CreateRenderTarget();
    g_ClusterSamplesRenderTarget->AttachTexture( AttachmentPoint::DepthStencil, depthStencilTexture );
    g_ClusterSamplesRenderTarget->AttachTexture( AttachmentPoint::Color0, g_ClusterSamplesDebugTexture );

    g_Application.IncrementLoadingProgress();

    // Load forward rendering shaders.
    auto forwardVS = g_RenderDevice->CreateShader();
    forwardVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/Forward_VS.hlsl" );

    g_Application.IncrementLoadingProgress();

    auto forwardPS = g_RenderDevice->CreateShader();
    forwardPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/Forward_PS.hlsl" );

    g_Application.IncrementLoadingProgress();

    // Load forward plus shaders.
    auto forwardPlusVS = g_RenderDevice->CreateShader();
    forwardPlusVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/ForwardPlus_VS.hlsl" );

    g_Application.IncrementLoadingProgress();

    auto forwardPlusPS = g_RenderDevice->CreateShader();
    forwardPlusPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/ForwardPlus_PS.hlsl", "main", forwardPlusShaderMacros );

    g_Application.IncrementLoadingProgress();

    // Load clustered shaders.
    auto clusteredVS = g_RenderDevice->CreateShader();
    clusteredVS->LoadShaderFromFile( ShaderType::Vertex, L"../Assets/shaders/Clustered_VS.hlsl" );

    g_Application.IncrementLoadingProgress();

    auto clusteredPS = g_RenderDevice->CreateShader();
    clusteredPS->LoadShaderFromFile( ShaderType::Pixel, L"../Assets/shaders/Clustered_PS.hlsl" );

    g_Application.IncrementLoadingProgress();

    // Setup depth prepass pipeline state object.
    g_DepthPrepassPSO->SetShader( ShaderType::Vertex, simpleVS );
    g_DepthPrepassPSO->SetShader( ShaderType::Pixel, depthPrePassPS );
    g_DepthPrepassPSO->SetRenderTarget( g_DepthOnlyRenderTarget );

    // Setup forward rendering opaque pipeline state.
    g_ForwardOpaquePSO->SetShader( ShaderType::Vertex, forwardVS );
    g_ForwardOpaquePSO->SetShader( ShaderType::Pixel, forwardPS );
    g_ForwardOpaquePSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ForwardOpaquePSO->GetDepthStencilState().SetDepthMode( depthFuncLessEqual );

    // Setup forward rendering transparent pipeline state.
    g_ForwardTransparentPSO->SetShader( ShaderType::Vertex, forwardVS );
    g_ForwardTransparentPSO->SetShader( ShaderType::Pixel, forwardPS );
    g_ForwardTransparentPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ForwardTransparentPSO->GetDepthStencilState().SetDepthMode( disableDepthWrite );
    g_ForwardTransparentPSO->GetBlendState().SetBlendMode( alphaBlending );
    g_ForwardTransparentPSO->GetRasterizerState().SetCullMode( CullMode::None );

    // Setup Forward+ opaque pipeline state.
    g_ForwardPlusOpaquePSO->SetShader( ShaderType::Vertex, forwardPlusVS );
    g_ForwardPlusOpaquePSO->SetShader( ShaderType::Pixel, forwardPlusPS );
    g_ForwardPlusOpaquePSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ForwardPlusOpaquePSO->GetDepthStencilState().SetDepthMode( depthFuncLessEqual );

    // Setup forward+ transparent pipeline state.
    g_ForwardPlusTransparentPSO->SetShader( ShaderType::Vertex, forwardPlusVS );
    g_ForwardPlusTransparentPSO->SetShader( ShaderType::Pixel, forwardPlusPS );
    g_ForwardPlusTransparentPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ForwardPlusTransparentPSO->GetDepthStencilState().SetDepthMode( disableDepthWrite );
    g_ForwardPlusTransparentPSO->GetBlendState().SetBlendMode( alphaBlending );
    g_ForwardPlusTransparentPSO->GetRasterizerState().SetCullMode( CullMode::None );

    // Setup cluster samples pipeline state object.
    g_ClusterSamplesPSO->SetShader( ShaderType::Vertex, clusterSamplesVS );
    g_ClusterSamplesPSO->SetShader( ShaderType::Pixel, clusterSamplesPS );
    g_ClusterSamplesPSO->SetRenderTarget( g_ClusterSamplesRenderTarget ); // g_RenderWindow->GetRenderTarget() );
    g_ClusterSamplesPSO->GetDepthStencilState().SetDepthMode( disableDepthWriteLessEqual );
    g_ClusterSamplesPSO->GetBlendState().SetBlendMode( additiveBlending );
    g_ClusterSamplesPSO->GetRasterizerState().SetCullMode( CullMode::None );

    // Setup Clustered opaque pipeline state.
    g_ClusteredOpaquePSO->SetShader( ShaderType::Vertex, clusteredVS );
    g_ClusteredOpaquePSO->SetShader( ShaderType::Pixel, clusteredPS );
    g_ClusteredOpaquePSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ClusteredOpaquePSO->GetDepthStencilState().SetDepthMode( depthFuncLessEqual );

    // Setup clustered transparent pipeline state.
    g_ClusteredTransparentPSO->SetShader( ShaderType::Vertex, clusteredVS );
    g_ClusteredTransparentPSO->SetShader( ShaderType::Pixel, clusteredPS );
    g_ClusteredTransparentPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_ClusteredTransparentPSO->GetDepthStencilState().SetDepthMode( disableDepthWrite );
    g_ClusteredTransparentPSO->GetBlendState().SetBlendMode( alphaBlending );
    g_ClusteredTransparentPSO->GetRasterizerState().SetCullMode( CullMode::None );

    // Setup a pipeline state to debug the cluster assignments.
    g_DebugClustersPSO->SetShader( ShaderType::Vertex, debugClustersVS );
    g_DebugClustersPSO->SetShader( ShaderType::Geometry, debugClustersGS );
    g_DebugClustersPSO->SetShader( ShaderType::Pixel, debugClustersPS );
    g_DebugClustersPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_DebugClustersPSO->SetPrimitiveTopology( PrimitiveTopology::PointList );
    g_DebugClustersPSO->GetRasterizerState().SetCullMode( CullMode::Back );
    g_DebugClustersPSO->GetBlendState().SetBlendMode( additiveBlending );
    g_DebugClustersPSO->GetDepthStencilState().SetDepthMode( disableDepthWriteLessEqual );

    // Setup a pipeline state to render point lights.
    g_DebugPointLightsPSO->SetShader( ShaderType::Vertex, debugPointLightsVS );
    g_DebugPointLightsPSO->SetShader( ShaderType::Pixel, debugPointLightsPS );
    g_DebugPointLightsPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_DebugPointLightsPSO->GetDepthStencilState().SetDepthMode( disableDepthWrite );
    g_DebugPointLightsPSO->GetBlendState().SetBlendMode( alphaBlending );

    // Setup a pipeline state to render spot lights.
    g_DebugSpotLightsPSO->SetShader( ShaderType::Vertex, debugSpotLightsVS );
    g_DebugSpotLightsPSO->SetShader( ShaderType::Pixel, debugSpotLightsPS );
    g_DebugSpotLightsPSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_DebugSpotLightsPSO->GetDepthStencilState().SetDepthMode( disableDepthWrite );
    g_DebugSpotLightsPSO->GetBlendState().SetBlendMode( alphaBlending );
    
    // Setup a pipeline state to debug a depth texture.
    g_DebugDepthTexturePSO->SetShader( ShaderType::Vertex, debugTextureVS );
    g_DebugDepthTexturePSO->SetShader( ShaderType::Pixel, debugTexturePS );
    g_DebugDepthTexturePSO->SetRenderTarget( g_RenderWindow->GetRenderTarget() );
    g_DebugDepthTexturePSO->GetDepthStencilState().SetDepthMode( disableDepthTest );
    g_DebugDepthTexturePSO->GetBlendState().SetBlendMode( alphaBlending );

    g_RenderDebugTexturePass = std::make_shared<PostprocessPass>( fullScreenQuad, g_DebugDepthTexturePSO, fullScreenOrthographicProjection, g_LightCullingDebugTexture );

#pragma region Depth Prepass
    // Setup a common depth pre pass that is used for all the rendering techniques.
    std::shared_ptr<CompositePass> depthPrepass;
    ( *( depthPrepass = std::make_shared<CompositePass>() ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                e.GraphicsCommandBuffer->BindGraphicsShaderSignature( g_DepthPrepassPSO->GetShaderSignature() );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 2, lightCounts );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 8, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
            }
        } ) )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Depth Pre Pass" ) )
        .AddPass( std::make_shared<OpaquePass>( scene, g_DepthPrepassPSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Depth Pre Pass" marker.
        ;
#pragma endregion

#pragma region Debug Lights Pass
    // Setup a render pass to render debug geometry for the lights in the scene.
    ( *( g_DebugLightsPass = std::make_shared<CompositePass>() ) )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Render Debug Lights" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                e.GraphicsCommandBuffer->BindGraphicsShaderSignature( g_DebugPointLightsPSO->GetShaderSignature() );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 0, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
            }
        } ) )
        .AddPass( std::make_shared<LightsPass>( g_RenderDevice, g_Config.PointLights, g_Config.SpotLights, g_Config.DirectionalLights,
                                                g_Sphere, g_Cone, nullptr,
                                                g_DebugPointLightsPSO, g_DebugSpotLightsPSO, nullptr ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Render Debug Lights" marker.
        ;
#pragma endregion

#pragma region Debug Clusters Pass
    ( *( g_DebugClustersPass = std::make_shared<CompositePass>() ) )
//        .AddPass( std::make_shared<ClearRenderTargetPass>( g_RenderWindow->GetRenderTarget() ) )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Render Debug Clusters" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                if ( g_UpdateUniqueClusters )
                {
                    g_PreviousViewMatrix = g_Camera->GetViewMatrix();
                    e.GraphicsCommandBuffer->CopyResource( g_PreviousUniqueClusters, g_UniqueClusters );
                }

                DebugClustersCB debugClustersCB;
                debugClustersCB.View = g_Camera->GetViewMatrix() * glm::inverse( g_PreviousViewMatrix );
                debugClustersCB.Projection = g_Camera->GetProjectionMatrix();

                e.GraphicsCommandBuffer->BindGraphicsPipelineState( g_DebugClustersPSO );
                e.GraphicsCommandBuffer->BindGraphicsDynamicConstantBuffer( 0, debugClustersCB );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 1, 0, { g_PreviousUniqueClusters, g_ClusterAABBs, g_ClusterColors } );

                e.GraphicsCommandBuffer->ExecuteIndirect( g_DebugClustersCommandSignature, g_DebugClustersDrawIndirectArgumentBuffer );
            }
        } ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Render Debug Clusters" marker.
        ;
#pragma endregion

#pragma region Debug Cluster Light Counts
        ( *( g_DebugLightCountsPass = std::make_shared<CompositePass>() ) )
            .AddPass( std::make_shared<PushProfileMarkerPass>( L"Render Clustered Light Counts" ) )
            .AddPass( std::make_shared<InvokeFunctionPass>( [countLightsComputePipelineState,lightCountHeatMap] ( Core::RenderEventArgs& e )
            {
                if ( e.GraphicsCommandBuffer )
                {
                    uint32_t screenWidth = std::max( g_WindowWidth, 1u );
                    uint32_t screenHeight = std::max( g_WindowHeight, 1u );

                    CameraParamsCB cameraParams;
                    cameraParams.Projection = g_Camera->GetProjectionMatrix();
                    cameraParams.InverseProjection = glm::inverse( cameraParams.Projection );
                    cameraParams.ScreenDimensions = glm::vec2( screenWidth, screenHeight );

                    std::shared_ptr<Texture> depthTexture = g_RenderWindow->GetRenderTarget()->GetTexture( AttachmentPoint::DepthStencil );

                    e.GraphicsCommandBuffer->BindComputePipelineState( countLightsComputePipelineState );

                    e.GraphicsCommandBuffer->BindComputeDynamicConstantBuffer( 0, cameraParams );
                    e.GraphicsCommandBuffer->BindCompute32BitConstants( 1, g_ClusterDataCB );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 0, { depthTexture, lightCountHeatMap } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 2, { g_PointLightGrid_Cluster, g_SpotLightGrid_Cluster } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 4, { g_LightCullingDebugTexture } );

                    glm::uvec3 numThreadGroups = glm::ceil( glm::vec3( screenWidth / 32.0f, screenHeight / 32.0f, 1.0f ) );

                    e.GraphicsCommandBuffer->Dispatch( numThreadGroups );
                }
            }
            ) )
            .AddPass( g_RenderDebugTexturePass )
            .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Render Clustered Light Counts" marker.
            ;

#pragma endregion

#pragma region Forward Rendering Technique
    // Setup forward rendering techniques.
    g_ForwardRenderingTechnique
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Forward Rendering" ) )
        .AddPass( std::make_shared<ClearRenderTargetPass>( g_RenderWindow->GetRenderTarget(), ClearFlags::All, ClearColor::CornflowerBlue ) )
        .AddPass( depthPrepass )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Main Render" ) )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Opaque Pass" ) )
        .AddPass( std::make_shared<OpaquePass>( scene, g_ForwardOpaquePSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Opaque Pass" marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Transparent Pass" ) )
        .AddPass( std::make_shared<TransparentPass>( scene, g_ForwardTransparentPSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Transparent Pass" marker.
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Main Render" marker.
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Forward Rendering" marker.
        .AddPass( g_DebugLightsPass )
        ;
#pragma endregion 

#pragma region Forward+ Rendering technique
    // Setup Forward+ rendering technique.
    g_ForwardPlusRenderingTechnique
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Forward+ Rendering" ) )
        .AddPass( std::make_shared<ClearRenderTargetPass>( g_RenderWindow->GetRenderTarget(), ClearFlags::All, ClearColor::CornflowerBlue ) )
        .AddPass( depthPrepass )

#pragma region Light Culling
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Light Culling" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [cullLightsPipelineState, lightCountHeatMap] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                uint32_t screenWidth = std::max( g_WindowWidth, 1u );
                uint32_t screenHeight = std::max( g_WindowHeight, 1u );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                CameraParamsCB cameraParams;
                cameraParams.Projection = g_Camera->GetProjectionMatrix();
                cameraParams.InverseProjection = glm::inverse( cameraParams.Projection );
                cameraParams.ScreenDimensions = glm::vec2( screenWidth, screenHeight );

                DispatchParamsCB dispatchParams;
                dispatchParams.NumThreadGroups = glm::ceil( glm::vec3( screenWidth / (float)g_LightGridBlockSize, screenHeight / (float)g_LightGridBlockSize, 1.0f ) );
                dispatchParams.NumThreads = dispatchParams.NumThreadGroups * glm::uvec3( g_LightGridBlockSize, g_LightGridBlockSize, 1 );

                for ( uint32_t i = 0; i < 2; ++i )
                {
                    e.GraphicsCommandBuffer->ClearResourceUInt( g_PointLightIndexCounter[i] );
                    e.GraphicsCommandBuffer->ClearResourceUInt( g_PointLightGrid[i] );
                    e.GraphicsCommandBuffer->ClearResourceUInt( g_SpotLightIndexCounter[i] );
                    e.GraphicsCommandBuffer->ClearResourceUInt( g_SpotLightGrid[i] );
                }

                e.GraphicsCommandBuffer->BindComputePipelineState( cullLightsPipelineState );

                e.GraphicsCommandBuffer->BindCompute32BitConstants( 0, lightCounts );
                e.GraphicsCommandBuffer->BindComputeDynamicConstantBuffer( 1, cameraParams );
                e.GraphicsCommandBuffer->BindCompute32BitConstants( 2, dispatchParams );

                std::shared_ptr<Texture> depthTexture = g_RenderWindow->GetRenderTarget()->GetTexture( AttachmentPoint::DepthStencil );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 0, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 3, { depthTexture, g_GridFrustums, lightCountHeatMap } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 6, { g_LightCullingDebugTexture } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 7, { g_PointLightIndexCounter[0], g_PointLightIndexCounter[1] } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 9, { g_SpotLightIndexCounter[0], g_SpotLightIndexCounter[1] } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 11, { g_PointLightIndexList[0], g_PointLightIndexList[1] } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 13, { g_SpotLightIndexList[0], g_SpotLightIndexList[1] } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 15, { g_PointLightGrid[0], g_PointLightGrid[1] } );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 3, 17, { g_SpotLightGrid[0], g_SpotLightGrid[1] } );

                e.GraphicsCommandBuffer->Dispatch( dispatchParams.NumThreadGroups );
            }
        } ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Light Culling" profiling marker.
#pragma endregion

#pragma region Opaque Pass
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Opaque Pass" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                e.GraphicsCommandBuffer->BindGraphicsShaderSignature( g_ForwardPlusOpaquePSO->GetShaderSignature() );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 2, lightCounts );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 8, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 11, { g_PointLightIndexList[0], g_SpotLightIndexList[0] } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 13, { g_PointLightGrid[0], g_SpotLightGrid[0] } );
            }
        } ) )
        .AddPass( std::make_shared<OpaquePass>( scene, g_ForwardPlusOpaquePSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Opaque Pass" profiling marker.
#pragma endregion

#pragma region Transparent Pass
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Transparent Pass" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                e.GraphicsCommandBuffer->BindGraphicsShaderSignature( g_ForwardPlusTransparentPSO->GetShaderSignature() );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 2, lightCounts );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 8, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 11, { g_PointLightIndexList[1], g_SpotLightIndexList[1] } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 13, { g_PointLightGrid[1], g_SpotLightGrid[1] } );
            }
        } ) )
        .AddPass( std::make_shared<TransparentPass>( scene, g_ForwardPlusTransparentPSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Transparent Pass" profiling marker.
#pragma endregion
        .AddPass( g_DebugLightsPass )
        .AddPass( g_RenderDebugTexturePass )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Forward+ Rendering" profiling marker.
        ;
#pragma endregion

#pragma region Clustered Rendering
    g_ClusteredRenderingTechnique
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Clustered Rendering" ) )
        .AddPass( std::make_shared<ClearRenderTargetPass>( g_RenderWindow->GetRenderTarget(), ClearFlags::All, ClearColor::CornflowerBlue ) )
        .AddPass( std::make_shared<ClearRenderTargetPass>( g_ClusterSamplesDebugTexture, ClearFlags::Color, ClearColor::TransparentBlack ) )
        .AddPass( depthPrepass )
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Cluster Samples" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                // Clear the Cluster flags
                e.GraphicsCommandBuffer->ClearResourceUInt( g_ClusterFlags );

                // Bind arguments.
                e.GraphicsCommandBuffer->BindGraphicsShaderSignature( g_ClusterSamplesPSO->GetShaderSignature() );

                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 1, g_ClusterDataCB );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 2, 8, { g_ClusterFlags } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 0, { g_ClusterColors } );
            }
        } ) )
        .AddPass( std::make_shared<BasePass>( scene, g_ClusterSamplesPSO, false ) )    // Render both opaque and transparent geometry (without materials)
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Cluster Samples" marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Find Unique Clusters" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                // Clear the unique clusters structured buffer.
                e.GraphicsCommandBuffer->ClearResourceUInt( g_UniqueClusters );
                e.GraphicsCommandBuffer->ClearResourceUInt( g_UniqueClusters->GetCounterBuffer() );

                // Bind Compute PSO
                e.GraphicsCommandBuffer->BindComputePipelineState( g_FindUniqueClustersPSO );

                // Bind shader arguments.
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 0, 0, { g_ClusterFlags, g_UniqueClusters } );

                uint32_t maxClusters = g_ClusterDataCB.GridDim.x * g_ClusterDataCB.GridDim.y * g_ClusterDataCB.GridDim.z;
                uint32_t numThreadGroups = static_cast<uint32_t>( glm::ceil( maxClusters / 1024.0f ) );

                // Dispatch compute shader to determine only the unique clusters that contain samples.
                e.GraphicsCommandBuffer->Dispatch( numThreadGroups );
            }
        } ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Find Unique Clusters" profiling marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Update Indirect Argument Buffers" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                // Bind Compute PSO
                e.GraphicsCommandBuffer->BindComputePipelineState( g_UpdateIndirectArgumentBuffersPSO );

                // Bind shader arguments.
                e.GraphicsCommandBuffer->BindCompute32BitConstants( 0, g_UpdateUniqueClusters ? 1u : 0u );
                e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 0, { g_UniqueClusters->GetCounterBuffer(), g_AssignLightsToClustersArgumentBuffer, g_DebugClustersDrawIndirectArgumentBuffer } );

                // Dispatch compute shader to update the indirect argument buffers.
                e.GraphicsCommandBuffer->Dispatch( 1 );
            }
        } ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Update Indirect Argument Buffers" profiling marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Assign Lights to Clusters" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                switch ( g_RenderingTechnique )
                {
                case RenderingTechnique::Clustered:
                    Profiler::Get().PushProfilingMarker( L"No Optimization", e.GraphicsCommandBuffer );
                    break;
                case RenderingTechnique::Clustered_Optimized:
                    Profiler::Get().PushProfilingMarker( L"BVH", e.GraphicsCommandBuffer );
                    break;
                }

                // Clear the global light counters and light grids.
                e.GraphicsCommandBuffer->ClearResourceUInt( g_PointLightIndexCounter_Cluster );
                e.GraphicsCommandBuffer->ClearResourceUInt( g_SpotLightIndexCounter_Cluster );
                e.GraphicsCommandBuffer->ClearResourceUInt( g_PointLightGrid_Cluster );
                e.GraphicsCommandBuffer->ClearResourceUInt( g_SpotLightGrid_Cluster );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                BVHParams bvhParams = {};
                bvhParams.PointLightLevels = GetNumLevels( lightCounts.NumPointLights );
                bvhParams.SpotLightLevels = GetNumLevels( lightCounts.NumSpotLights );

                // Bind Compute PSO
                switch ( g_RenderingTechnique )
                {
                case RenderingTechnique::Clustered:
                    e.GraphicsCommandBuffer->BindComputePipelineState( g_AssignLightsToClustersPSO );
                    // Bind arguments.
                    e.GraphicsCommandBuffer->BindCompute32BitConstants( 0, lightCounts );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 0, { g_PointLightsBuffer, g_SpotLightsBuffer } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 2, { g_UniqueClusters, g_ClusterAABBs } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 4, { g_PointLightIndexCounter_Cluster, g_SpotLightIndexCounter_Cluster } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 6, { g_PointLightGrid_Cluster, g_SpotLightGrid_Cluster } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 1, 8, { g_PointLightIndexList_Cluster, g_SpotLightIndexList_Cluster } );
                    break;
                case RenderingTechnique::Clustered_Optimized:
                    e.GraphicsCommandBuffer->BindComputePipelineState( g_AssignLightsToClustersBVHPSO );
                    // Bind arguments.
                    e.GraphicsCommandBuffer->BindCompute32BitConstants( 0, bvhParams );
                    e.GraphicsCommandBuffer->BindCompute32BitConstants( 1, lightCounts );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 0, { g_PointLightsBuffer, g_SpotLightsBuffer } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 2, { g_UniqueClusters, g_ClusterAABBs } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 4, { g_PointLightIndices, g_SpotLightIndices } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 6, { g_PointLightBVH, g_SpotLightBVH } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 8, { g_PointLightIndexCounter_Cluster, g_SpotLightIndexCounter_Cluster } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 10, { g_PointLightGrid_Cluster, g_SpotLightGrid_Cluster } );
                    e.GraphicsCommandBuffer->BindComputeShaderArguments( 2, 12, { g_PointLightIndexList_Cluster, g_SpotLightIndexList_Cluster } );
                    break;
                }

                // Indirect Dispatch the AssignLightsToClusters compute shader based on the number of clusters that were counted
                // in the FindUniqueClusters compute shader.
                e.GraphicsCommandBuffer->ExecuteIndirect( g_AssignLightToClustersCommandSignature, g_AssignLightsToClustersArgumentBuffer );

                switch ( g_RenderingTechnique )
                {
                case RenderingTechnique::Clustered:
                case RenderingTechnique::Clustered_Optimized:
                    Profiler::Get().PopProfilingMarker( e.GraphicsCommandBuffer );
                    break;
                }
            }
        } ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Assign Lights to Clusters" profiling marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Opaque Pass" ) )
        .AddPass( std::make_shared<InvokeFunctionPass>( [] ( Core::RenderEventArgs& e )
        {
            if ( e.GraphicsCommandBuffer )
            {
                // Bind graphics pipeline state object.
                e.GraphicsCommandBuffer->BindGraphicsPipelineState( g_ClusteredOpaquePSO );

                LightCountsCB lightCounts;
                lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
                lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
                lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

                // Bind arguments.
                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 2, lightCounts );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 8, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 11, { g_PointLightIndexList_Cluster, g_SpotLightIndexList_Cluster } );
                e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 13, { g_PointLightGrid_Cluster, g_SpotLightGrid_Cluster } );

                // HACK: The cluster data has to go in slot 4 because the BasePass binds the material's textures to slot 3!.
                e.GraphicsCommandBuffer->BindGraphics32BitConstants( 4, g_ClusterDataCB );
            }
        } ) )
        .AddPass( std::make_shared<OpaquePass>( scene, g_ClusteredOpaquePSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Opaque Pass" profiling marker.
        .AddPass( std::make_shared<PushProfileMarkerPass>( L"Transparent Pass" ) )
        .AddPass( std::make_shared<TransparentPass>( scene, g_ClusteredTransparentPSO ) )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Transparent Pass" profiling marker.
        .AddPass( g_DebugClustersPass )
        .AddPass( g_DebugLightsPass )
        .AddPass( g_DebugLightCountsPass )
        .AddPass( std::make_shared<PopProfileMarkerPass>() ) // Pop "Clustered Rendering" profiling marker.
        ;
#pragma endregion

        g_Application.IncrementLoadingProgress();

    auto fence = commandQueue->Submit( commandBuffer );
    fence->WaitFor();

    g_Application.IncrementLoadingProgress();

    g_Config.LoadingProgressTotal = g_Application.GetLoadingProgress();

    g_IsLoading = false;

    return true;
}

void MergeSort( std::shared_ptr<ComputeCommandBuffer> commandBuffer,
                std::shared_ptr<StructuredBuffer> srcKeys, std::shared_ptr<StructuredBuffer> srcValues,
                std::shared_ptr<StructuredBuffer> dstKeys, std::shared_ptr<StructuredBuffer> dstValues,
                uint32_t totalValues, uint32_t chunkSize )
{
    SortParams sortParams;
    uint32_t numThreadGroups;

    // The number of threads per thread group.
    const uint32_t numThreadsPerThreadGroup = SORT_NUM_THREADS_PER_THREAD_GROUP;
    // The number of values that each thread sorts.
    const uint32_t numValuesPerThread = SORT_ELEMENTS_PER_THREAD;
    // The number of values that each thread group will sort.
    const uint32_t numValuesPerThreadGroup = numThreadsPerThreadGroup * numValuesPerThread;

    // The total number of complete chunks to sort.
    uint32_t numChunks = static_cast<uint32_t>( glm::ceil( totalValues / (float)chunkSize ) );

    uint32_t pass = 0;

    while ( numChunks > 1 )
    {
        ScopedProfileMarker passProfileMaker( std::wstring( L"Pass " ) + std::to_wstring( ++pass ), commandBuffer );

        sortParams.NumElements = totalValues;
        sortParams.ChunkSize = chunkSize;

        // Number of sort groups required to sort all chunks.
        // Each sort group merge sorts 2 chunks into a single chunk.
        uint32_t numSortGroups = numChunks / 2u;

        // The number of thread groups that are required per sort group.
        uint32_t numThreadGroupsPerSortGroup = static_cast<uint32_t>( glm::ceil( ( chunkSize * 2 ) / (float)numValuesPerThreadGroup ) );

        // Compute merge path partitions per thread group.
        {
            commandBuffer->ClearResourceUInt( g_MergePathPartitions );

            commandBuffer->BindComputePipelineState( g_MergePathPartitionsPSO );

            commandBuffer->BindCompute32BitConstants( 0, sortParams );
            commandBuffer->BindComputeShaderArguments( 1, 0, { srcKeys, srcValues, g_MergePathPartitions } );
            commandBuffer->BindComputeShaderArguments( 1, 3, { dstKeys, dstValues, g_MergePathPartitions } );

            // The number of merge path partitions that need to be computed.
            uint32_t numMergePathPartitionsPerSortGroup = numThreadGroupsPerSortGroup + 1u;
            uint32_t totalMergePathPartitions = numMergePathPartitionsPerSortGroup * numSortGroups;

            // The number of thread groups needed to compute all merge path partitions.
            numThreadGroups = static_cast<uint32_t>( glm::ceil( totalMergePathPartitions / (float)numThreadsPerThreadGroup ) );

            commandBuffer->Dispatch( numThreadGroups );
            
            // Add an explicit UAV barrier for MergePathPartitions.
            // This is required since the MergePathPartitions structured buffer is being used
            // as a UAV in the MergePathPartions compute shader and as an SRV in the MergeSort
            // compute shader. Because the MergePathPartions argument is not rebound between
            // dispatches, no implicit UAV barrier will be added to the command list and MergeSort
            // will likely not see the correct merge path partitions.
            // To resolve this, an explicit UAV barrier is added for the resource.
            commandBuffer->AddUAVBarrier( g_MergePathPartitions );
        }

        // Perform merge sort using merge path partitions computed from the previous step.
        {
            commandBuffer->BindComputePipelineState( g_MergeSortPSO );

            // The number of values that each sort group will sort.
            // Each sort group merges 2 chunks into 1.
            uint32_t numValuesPerSortGroup = glm::min( chunkSize * 2, totalValues );

            // The number of thread groups required to sort all values.
            numThreadGroupsPerSortGroup = static_cast<uint32_t>( glm::ceil( numValuesPerSortGroup / (float)numValuesPerThreadGroup ) );

            commandBuffer->Dispatch( numThreadGroupsPerSortGroup * numSortGroups );

            // Ping-pong the buffers
            std::swap( srcKeys, dstKeys );
            std::swap( srcValues, dstValues );
        }

        chunkSize *= 2;
        numChunks = static_cast<uint32_t>( glm::ceil( totalValues / (float)chunkSize ) );
    }

    // If the number of passes was odd, then the result of the sort will 
    // be in the source buffer and results need to be copied to the 
    // destination buffer.
    if ( pass % 2 == 1 )
    {
        commandBuffer->CopyResource( dstKeys, srcKeys );
        commandBuffer->CopyResource( dstValues, srcValues );
    }
}

// The number of nodes at each level of the BVH.
static const uint32_t gs_NumLevelNodes[] =
{
    1,          // 1st level (32^0)
    32,         // 2nd level (32^1)
    1024,       // 3rd level (32^2)
    32768,      // 4th level (32^3)
    1048576,    // 5th level (32^4)
    33554432,   // 6th level (32^5)
};

// The number of nodes required to represent a BVH given the number of levels
// of the BVH.
static const uint32_t gs_NumBVHNodes[] =
{
    1,          // 1 level  =32^0
    33,         // 2 levels +32^1
    1057,       // 3 levels +32^2
    33825,      // 4 levels +32^3
    1082401,    // 5 levels +32^4
    34636833,   // 6 levels +32^5
};

// Compute the number of levels needed for a BVH that consists of a number of leaf nodes.
uint32_t GetNumLevels( uint32_t numLeaves )
{
    static const float log32f = glm::log( 32.0f );

    uint32_t numLevels = 0;
    if ( numLeaves > 0 )
    {
        numLevels = static_cast<uint32_t>( glm::ceil( glm::log( numLeaves ) / log32f ) );
    }

    return numLevels;
}

// Compute the number of (child) nodes needed to represent a BVH that consists of a number of leaf nodes.
uint32_t GetNumNodes( uint32_t numLeaves )
{
    uint32_t numLevels = GetNumLevels( numLeaves );
    uint32_t numNodes = 0;
    if ( numLevels > 0 && numLevels < _countof(gs_NumBVHNodes) )
    {
        numNodes = gs_NumBVHNodes[numLevels - 1];
    }

    return numNodes;
}

void OnUpdate( UpdateEventArgs& e )
{
    CPU_MARKER( __FUNCTION__ );

    // Update the camera controller.
    g_CameraController->SetInvertY(g_InvertY);
    g_CameraController->OnUpdate( e );

    if ( g_TrackSelectedLight )
    {
        FocusCurrentLight();
    }

    glm::mat4 rotationMatrix( 1 );
    if ( g_Animate )
    {
        float fRotation = static_cast<float>( e.ElapsedTime ); // *glm::half_pi<float>();
        rotationMatrix = glm::rotate( glm::mat4( 1 ), fRotation, glm::vec3( 0, 1, 0 ) );
    }

    // Compute view space light properties.
    glm::mat4 viewMatrix = g_Camera->GetViewMatrix();

    // Update light buffers.
    if ( !g_IsLoading )
    {
        auto commandQueue = g_RenderDevice->GetGraphicsQueue();
        auto commandBuffer = commandQueue->GetComputeCommandBuffer();

        LightCountsCB lightCounts;
        lightCounts.NumPointLights = static_cast<uint32_t>( g_Config.PointLights.size() );
        lightCounts.NumSpotLights = static_cast<uint32_t>( g_Config.SpotLights.size() );
        lightCounts.NumDirectionalLights = static_cast<uint32_t>( g_Config.DirectionalLights.size() );

        {
            ScopedProfileMarker updateLightsProfilingMarker( L"Update Lights", commandBuffer );

            commandBuffer->BindComputePipelineState( g_UpdateLightsPSO );

            UpdateLightsCB updateLightsCB;
            updateLightsCB.ModelMatrix = rotationMatrix;
            updateLightsCB.ViewMatrix = viewMatrix;

            commandBuffer->BindComputeDynamicConstantBuffer( 0, updateLightsCB );
            commandBuffer->BindCompute32BitConstants( 1, lightCounts );
            commandBuffer->BindComputeShaderArguments( 2, 0, { g_PointLightsBuffer, g_SpotLightsBuffer, g_DirectionalLightsBuffer } );

            uint32_t numGroupsX = glm::max( lightCounts.NumPointLights, glm::max( lightCounts.NumSpotLights, lightCounts.NumDirectionalLights ) );
            numGroupsX = static_cast<uint32_t>( glm::ceil( numGroupsX / 1024.0f ) );

            commandBuffer->Dispatch( numGroupsX );
        }
        if ( g_RenderingTechnique == RenderingTechnique::Clustered_Optimized )
        {
            {
                ScopedProfileMarker computeLightsAABB( L"Reduce Lights AABB", commandBuffer );

                commandBuffer->BindComputePipelineState( g_ReduceLightsAABB1PSO );

                // Don't dispatch more than 512 thread groups. The reduction algorithm depends on the
                // number of thread groups to be no more than 512. The buffer which stores the reduced AABB is sized
                // for a maximum of 512 thread groups.
                uint32_t numThreadGroups = glm::min<uint32_t>( static_cast<uint32_t>( glm::ceil( glm::max( lightCounts.NumPointLights, lightCounts.NumSpotLights ) / 512.0f ) ), 512 );

                DispatchParamsCB dispatchParams;
                dispatchParams.NumThreadGroups = glm::uvec3( numThreadGroups, 1, 1 );
                dispatchParams.NumThreads = glm::uvec3( numThreadGroups * 512, 1, 1 );

                // In the first pass, the number of lights determines the number of
                // elements to be reduced.
                // In the second pass, the number of elements to be reduced is the 
                // number of thread groups from the first pass.
                uint32_t numElements = dispatchParams.NumThreadGroups.x;

                commandBuffer->BindCompute32BitConstants( 0, lightCounts );
                commandBuffer->BindCompute32BitConstants( 1, dispatchParams );
                commandBuffer->BindCompute32BitConstants( 2, numElements );
                commandBuffer->BindComputeShaderArguments( 3, 0, { g_PointLightsBuffer, g_SpotLightsBuffer, g_LightsAABB } );

                {
                    ScopedProfileMarker firstPass( L"First Pass", commandBuffer );

                    // Dispatch the first pass.
                    commandBuffer->Dispatch( numThreadGroups );
                }

                commandBuffer->BindComputePipelineState( g_ReduceLightsAABB2PSO );

                dispatchParams.NumThreadGroups = glm::uvec3( 1, 1, 1 );
                dispatchParams.NumThreads = glm::uvec3( 512, 1, 1 );

                commandBuffer->BindCompute32BitConstants( 1, dispatchParams );

                {
                    ScopedProfileMarker secondPass( L"Second Pass", commandBuffer );

                    // Dispatch 2nd pass.
                    commandBuffer->Dispatch( 1 );
                }
            }
            {
                ScopedProfileMarker computeMortonCodes( L"Compute Morton Codes", commandBuffer );

                commandBuffer->BindComputePipelineState( g_ComputeLightMortonCodesPSO );

                commandBuffer->BindCompute32BitConstants( 0, lightCounts );
                commandBuffer->BindComputeShaderArguments( 1, 0, { g_PointLightsBuffer, g_SpotLightsBuffer, g_LightsAABB } );
                commandBuffer->BindComputeShaderArguments( 1, 3, { g_PointLightMortonCodes, g_SpotLightMortonCodes } );
                commandBuffer->BindComputeShaderArguments( 1, 5, { g_PointLightIndices, g_SpotLightIndices } );

                uint32_t numThreadGroups = static_cast<uint32_t>( glm::ceil( glm::max( lightCounts.NumPointLights, lightCounts.NumSpotLights ) / 1024.0f ) );

                commandBuffer->Dispatch( numThreadGroups );
            }
            {
                ScopedProfileMarker sortByMortonCode( L"Sort Morton Codes", commandBuffer );

                // The size of a single chunk that keys will be sorted into.
                uint32_t chunkSize = SORT_NUM_THREADS_PER_THREAD_GROUP;

                commandBuffer->BindComputePipelineState( g_RadixSortPSO );

                SortParams sortParams;
                sortParams.ChunkSize = chunkSize;

                // First sort the point light Morton codes.
                if ( lightCounts.NumPointLights > 0 )
                {
                    sortParams.NumElements = lightCounts.NumPointLights;

                    commandBuffer->BindCompute32BitConstants( 0, sortParams );
                    commandBuffer->BindComputeShaderArguments( 1, 0, { g_PointLightMortonCodes, g_PointLightIndices } );
                    commandBuffer->BindComputeShaderArguments( 1, 2, { g_PointLightMortonCodes_OUT, g_PointLightIndices_OUT } );

                    {
                        ScopedProfileMarker sortPointLights( L"Radix Sort (Point Lights)", commandBuffer );

                        uint32_t numThreadGroups = static_cast<uint32_t>( glm::ceil( sortParams.NumElements / (float)SORT_NUM_THREADS_PER_THREAD_GROUP ) );

                        commandBuffer->Dispatch( numThreadGroups );

                        // Now copy the results of the radix sort to the original buffer.
                        commandBuffer->CopyResource( g_PointLightMortonCodes, g_PointLightMortonCodes_OUT );
                        commandBuffer->CopyResource( g_PointLightIndices, g_PointLightIndices_OUT );
                    }
                }

                // Now sort spot light Morton codes.
                if ( lightCounts.NumSpotLights > 0 )
                {
                    sortParams.NumElements = lightCounts.NumSpotLights;

                    commandBuffer->BindCompute32BitConstants( 0, sortParams );
                    commandBuffer->BindComputeShaderArguments( 1, 0, { g_SpotLightMortonCodes, g_SpotLightIndices } );
                    commandBuffer->BindComputeShaderArguments( 1, 2, { g_SpotLightMortonCodes_OUT, g_SpotLightIndices_OUT } );

                    {
                        ScopedProfileMarker sortSpotLights( L"Radix Sort (Spot Lights)", commandBuffer );

                        uint32_t numThreadGroups = static_cast<uint32_t>( glm::ceil( sortParams.NumElements / (float)SORT_NUM_THREADS_PER_THREAD_GROUP ) );

                        commandBuffer->Dispatch( numThreadGroups );

                        // Now copy the results of the radix sort to the original buffer.
                        commandBuffer->CopyResource( g_SpotLightMortonCodes, g_SpotLightMortonCodes_OUT );
                        commandBuffer->CopyResource( g_SpotLightIndices, g_SpotLightIndices_OUT );
                    }
                }

                //// Merge sort the radix sorted blocks from the previous step.
                if ( lightCounts.NumPointLights > 0 )
                {
                    ScopedProfileMarker mergeSortPointLights( L"Merge Sort (Point Lights)", commandBuffer );

                    MergeSort( commandBuffer,
                               g_PointLightMortonCodes, g_PointLightIndices,
                               g_PointLightMortonCodes_OUT, g_PointLightIndices_OUT,
                               lightCounts.NumPointLights, chunkSize );
                }
                // Merge sort the radix sorted blocks from the previous step.
                if ( lightCounts.NumSpotLights > 0 )
                {
                    ScopedProfileMarker mergeSortPointLights( L"Merge Sort (Spot Lights)", commandBuffer );

                    MergeSort( commandBuffer,
                               g_SpotLightMortonCodes, g_SpotLightIndices,
                               g_SpotLightMortonCodes_OUT, g_SpotLightIndices_OUT,
                               lightCounts.NumSpotLights, chunkSize );
                }
            }
            {
                ScopedProfileMarker buildBVH( L"Build Light BVH", commandBuffer );

                commandBuffer->ClearResourceFloat( g_PointLightBVH );
                commandBuffer->ClearResourceFloat( g_SpotLightBVH );

                commandBuffer->BindComputePipelineState( g_BuildBVHBottomPSO );

                BVHParams bvhParams = {};
                bvhParams.PointLightLevels = GetNumLevels( lightCounts.NumPointLights );
                bvhParams.SpotLightLevels = GetNumLevels( lightCounts.NumSpotLights );

                commandBuffer->BindCompute32BitConstants( 0, bvhParams );
                commandBuffer->BindCompute32BitConstants( 1, lightCounts );
                commandBuffer->BindComputeShaderArguments( 2, 0, { g_PointLightsBuffer, g_SpotLightsBuffer } );
                commandBuffer->BindComputeShaderArguments( 2, 2, { g_PointLightIndices, g_SpotLightIndices } );
                commandBuffer->BindComputeShaderArguments( 2, 4, { g_PointLightBVH, g_SpotLightBVH } );

                // Build bottom level of the BVH.
                uint32_t maxLeaves = glm::max( lightCounts.NumPointLights, lightCounts.NumSpotLights );
                uint32_t numThreadGroups = static_cast<uint32_t>( glm::ceil( maxLeaves / (float)BVH_NUM_THREADS ) );

                {
                    ScopedProfileMarker buildBottomBVH( L"Build Bottom BVH", commandBuffer );

                    commandBuffer->Dispatch( numThreadGroups );
                }

                commandBuffer->BindComputePipelineState( g_BuildBVHTopPSO );

                // Now build upper levels of the BVH.
                uint32_t maxLevels = static_cast<uint32_t>( glm::max( bvhParams.PointLightLevels, bvhParams.SpotLightLevels ) );
                
                if ( maxLevels > 0 )
                {
                    for ( uint32_t level = maxLevels - 1u; level > 0; --level )
                    {
                        commandBuffer->AddUAVBarrier( g_PointLightBVH );
                        commandBuffer->AddUAVBarrier( g_SpotLightBVH );

                        bvhParams.ChildLevel = level;
                        commandBuffer->BindCompute32BitConstants( 0, bvhParams );

                        uint32_t numChildNodes = gs_NumLevelNodes[level];
                        numThreadGroups = static_cast<uint32_t>( glm::ceil( numChildNodes / (float)BVH_NUM_THREADS ) );

                        {
                            ScopedProfileMarker buildBVHBottom( std::wstring( L"Build BVH Level " ) + std::to_wstring( level ), commandBuffer );
                            commandBuffer->Dispatch( numThreadGroups );
                        }
                    }
                }
            }
        }

        commandQueue->Submit( commandBuffer );
    }
}

// Compute the view frustums for the light clipping grid.
void ComputeGridFrustums()
{
    auto commandQueue = g_RenderDevice->GetComputeQueue();
    auto commandBuffer = commandQueue->GetComputeCommandBuffer();

    // Make sure we can create at least 1 thread (even if the window is minimized)
    uint32_t screenWidth = std::max( g_WindowWidth, 1u );
    uint32_t screenHeight = std::max( g_WindowHeight, 1u );

    // To compute the frustums for the grid tiles, each thread will compute a single 
    // frustum for the tile.
    glm::uvec3 numThreads = glm::ceil( glm::vec3( screenWidth / (float)g_LightGridBlockSize, screenHeight / (float)g_LightGridBlockSize, 1 ) );
    glm::uvec3 numThreadGroups = glm::ceil( glm::vec3( numThreads.x / (float)g_LightGridBlockSize, numThreads.y / (float)g_LightGridBlockSize, 1 ) );

    TextureFormat lightGridTextureFormat( TextureComponents::RG,
                                          TextureType::UnsignedInteger,
                                          1,
                                          32, 32, 0, 0, 0, 0 );
    // Create a light index list and light grid that matches the number of tiles 
    // for the Forward+ light culling compute shader.
    // 2 resources are created for each light type. 1 for the opaque pass, 1 for the 
    // transparent pass.
    for ( uint32_t i = 0; i < 2; ++i )
    {
        g_PointLightIndexList[i] = g_RenderDevice->CreateStructuredBuffer( commandBuffer, numThreads.x * numThreads.y * numThreads.z * AVERAGE_OVERLAPPING_LIGHTS_PER_TILE, sizeof( uint32_t ) );
        g_PointLightIndexList[i]->SetName( std::wstring( L"Point Light Index List " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );

        g_SpotLightIndexList[i] = g_RenderDevice->CreateStructuredBuffer( commandBuffer, numThreads.x * numThreads.y * numThreads.z * AVERAGE_OVERLAPPING_LIGHTS_PER_TILE, sizeof( uint32_t ) );
        g_SpotLightIndexList[i]->SetName( std::wstring( L"Spot Light Index List " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );

        g_PointLightGrid[i] = g_RenderDevice->CreateTexture2D( numThreads.x, numThreads.y, numThreads.z, lightGridTextureFormat );
        g_PointLightGrid[i]->SetName( std::wstring( L"Point Light Grid " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );

        g_SpotLightGrid[i] = g_RenderDevice->CreateTexture2D( numThreads.x, numThreads.y, numThreads.z, lightGridTextureFormat );
        g_SpotLightGrid[i]->SetName( std::wstring( L"Spot Light Grid " ) + ( ( i == 0 ) ? L"(Opaque)" : L"(Transparent)" ) );
    }

    CameraParamsCB cameraParams;
    cameraParams.Projection = g_Camera->GetProjectionMatrix();
    cameraParams.InverseProjection = glm::inverse( cameraParams.Projection );
    cameraParams.ScreenDimensions = glm::vec2( screenWidth, screenHeight );

    // Update the number of thread groups for the compute frustums compute shader.
    DispatchParamsCB dispatchParams;
    dispatchParams.NumThreadGroups = numThreadGroups;
    dispatchParams.NumThreads = numThreads;

    // Create a new RWStructuredBuffer for storing the grid frustums.
    // We need 1 frustum for each grid cell.
    // For 1280x720 screen resolution and 16x16 tile size, results in 80x45 grid 
    // for a total of 3,600 frustums.
    g_GridFrustums = g_RenderDevice->CreateStructuredBuffer( commandBuffer, numThreads.x * numThreads.y * numThreads.z, sizeof( Frustum ) );
    g_GridFrustums->SetName( L"Grid Frustums" );

    commandBuffer->BindComputePipelineState( g_ComputeGridFrustumsPSO );

    commandBuffer->BindComputeDynamicConstantBuffer( 0, cameraParams );
    commandBuffer->BindCompute32BitConstants( 1, dispatchParams );
    commandBuffer->BindComputeShaderArgument( 2, g_GridFrustums );

    commandBuffer->Dispatch( numThreadGroups );

    commandQueue->Submit( commandBuffer );
}

void UpdateClusterGrid()
{
    // The half-angle of the field of view in the Y-direction.
    float fieldOfViewY = glm::radians( g_Camera->GetFOV() * 0.5f );
    float zNear = g_Camera->GetNearClipPlane();
    float zFar = g_Camera->GetFarClipPlane();

    // Number of clusters in the screen X direction.
    uint32_t clusterDimX = static_cast<uint32_t>( glm::ceil( g_WindowWidth / (float)g_ClusterGridBlockSize ) );
    // Number of clusters in the screen Y direction.
    uint32_t clusterDimY = static_cast<uint32_t>( glm::ceil( g_WindowHeight / (float)g_ClusterGridBlockSize ) );

    // The depth of the cluster grid during clustered rendering is dependent on the 
    // number of clusters subdivisions in the screen Y direction.
    // Source: Clustered Deferred and Forward Shading (2012) (Ola Olsson, Markus Billeter, Ulf Assarsson).
    float sD = 2.0f * glm::tan( fieldOfViewY ) / (float)clusterDimY;
    float logDimY = 1.0f / glm::log( 1.0f + sD );

    float logDepth = glm::log( zFar / zNear );
    uint32_t clusterDimZ = static_cast<uint32_t>( glm::floor( logDepth * logDimY ) );

    g_ClusterDataCB.GridDim = glm::uvec3( clusterDimX, clusterDimY, clusterDimZ );
    g_ClusterDataCB.ViewNear = zNear;
    g_ClusterDataCB.Size = glm::uvec2( g_ClusterGridBlockSize, g_ClusterGridBlockSize );
    g_ClusterDataCB.NearK = 1.0f + sD;
    g_ClusterDataCB.LogGridDimY = logDimY;

    auto commandQueue = g_RenderDevice->GetComputeQueue();
    auto commandBuffer = commandQueue->GetComputeCommandBuffer();

    // Create a buffer to hold (boolean) flags in the cluster grid that contain samples.
    // HLSL requires boolean types to be size of a 32-bit integer.
    g_ClusterFlags = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( uint32_t ) );
    g_ClusterFlags->SetName( L"Cluster Flags" );

    // A buffer (and internal counter) that holds a list of the unique clusters (the clusters that actually contain a sample).
    g_UniqueClusters = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( uint32_t ) );
    g_UniqueClusters->SetName( L"Unique Clusters" );
    g_UniqueClusters->GetCounterBuffer()->SetName( L"Unique Clusters (Counter)" );

    // And a buffer to store the results from the previous frame.
    g_PreviousUniqueClusters = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( uint32_t ) );
    g_PreviousUniqueClusters->SetName( L"Previous Unique Clusters" );

    // When recreating the unique cluster structures, then force the recreation of the 
    // unique clusters again to update the unique cluster list.
    g_UpdateUniqueClusters = true;

    if ( !g_AssignLightsToClustersArgumentBuffer )
    {
        // Create a buffer to store the indirect dispatch arguments for the Assign Lights to Clusters compute shader.
        g_AssignLightsToClustersArgumentBuffer = g_RenderDevice->CreateByteAddressBuffer( commandBuffer, sizeof( DispatchIndirectArgument ) );
        g_AssignLightsToClustersArgumentBuffer->SetName( L"Assign Lights to Clusters Indirect Argument Buffer" );
    }

    if ( !g_DebugClustersDrawIndirectArgumentBuffer )
    {
        // Create a buffer to store the indirect draw arguments for the debug clusters shader.
        g_DebugClustersDrawIndirectArgumentBuffer = g_RenderDevice->CreateByteAddressBuffer( commandBuffer, sizeof( DrawIndirectArgument ) );
        g_DebugClustersDrawIndirectArgumentBuffer->SetName( L"Debug Clusters Draw Indirect Argument Buffer" );
    }

    // Generate a buffer to store random colors to assign to clusters. (Used for debugging, highly memory inefficient).
    std::vector<glm::vec4> clusterColors = GenerateColors( clusterDimX * clusterDimY * clusterDimZ );
    g_ClusterColors = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterColors );
    g_ClusterColors->SetName( L"Cluster Colors" );

    g_ClusterAABBs = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( AABB ) );
    g_ClusterAABBs->SetName( L"Cluster AABBs" );

    // Create structured buffer to store the light grid for clustered rendering.
    g_PointLightGrid_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( glm::uvec2 ) );
    g_PointLightGrid_Cluster->SetName( L"Point Light Grid (Clustered)" );

    g_SpotLightGrid_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ, sizeof( glm::uvec2 ) );
    g_SpotLightGrid_Cluster->SetName( L"Spot Light Grid (Clustered)" );

    // Create global light index lists for clustered rendering.
    g_PointLightIndexList_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ * AVERAGE_OVERLAPPING_LIGHTS_PER_CLUSTER, sizeof( uint32_t ) );
    g_PointLightIndexList_Cluster->SetName( L"Point Light Index List (Clustered)" );

    g_SpotLightIndexList_Cluster = g_RenderDevice->CreateStructuredBuffer( commandBuffer, clusterDimX * clusterDimY * clusterDimZ * AVERAGE_OVERLAPPING_LIGHTS_PER_CLUSTER, sizeof( uint32_t ) );
    g_SpotLightIndexList_Cluster->SetName( L"Spot Light Index List (Clustered)" );

    // Fill the AABB structured buffer.
    // AABB's for cluster grid are defined in view space so only need to be recomputed
    // if the size of the grid changes.
    CameraParamsCB cameraParams;
    cameraParams.Projection = g_Camera->GetProjectionMatrix();
    cameraParams.InverseProjection = glm::inverse( cameraParams.Projection );
    cameraParams.ScreenDimensions = glm::vec2( g_WindowWidth, g_WindowHeight );

    commandBuffer->BindComputePipelineState( g_ComputeClusterAABBsPSO );
    commandBuffer->BindComputeDynamicConstantBuffer( 0, cameraParams );
    commandBuffer->BindCompute32BitConstants( 1, g_ClusterDataCB );
    commandBuffer->BindComputeShaderArgument( 2, g_ClusterAABBs );
    
    uint32_t threadGroups = static_cast<uint32_t>( glm::ceil( ( clusterDimX * clusterDimY * clusterDimZ ) / 1024.0f ) );
    commandBuffer->Dispatch( threadGroups );

    commandQueue->Submit( commandBuffer );
}

void OnResize( ResizeEventArgs& e )
{
    g_WindowWidth = std::max( 1, e.Width );
    g_WindowHeight = std::max( 1, e.Height );

    Viewport viewport( 0.0f, 0.0f, static_cast<float>( g_WindowWidth ), static_cast<float>( g_WindowHeight ) );

    g_Camera->SetViewport( viewport );
    g_Camera->SetProjection( 45.0f, g_WindowWidth / (float)g_WindowHeight, 0.1f, 1000.0f );

    g_DepthPrepassPSO->GetRasterizerState().SetViewport( viewport );
    g_ForwardOpaquePSO->GetRasterizerState().SetViewport( viewport );
    g_ForwardTransparentPSO->GetRasterizerState().SetViewport( viewport );
    g_ForwardPlusOpaquePSO->GetRasterizerState().SetViewport( viewport );
    g_ForwardPlusTransparentPSO->GetRasterizerState().SetViewport( viewport );
    g_ClusteredOpaquePSO->GetRasterizerState().SetViewport( viewport );
    g_ClusteredTransparentPSO->GetRasterizerState().SetViewport( viewport );
    g_DebugPointLightsPSO->GetRasterizerState().SetViewport( viewport );
    g_DebugSpotLightsPSO->GetRasterizerState().SetViewport( viewport );
    g_DebugDepthTexturePSO->GetRasterizerState().SetViewport( viewport );
    g_LoadingScreenPSO->GetRasterizerState().SetViewport( viewport );
    g_ClusterSamplesPSO->GetRasterizerState().SetViewport( viewport );
    g_DebugClustersPSO->GetRasterizerState().SetViewport( viewport );

    g_LightCullingDebugTexture->Resize( g_WindowWidth, g_WindowHeight );
    g_ClusterSamplesDebugTexture->Resize( g_WindowWidth, g_WindowHeight );

    // If the screen resolution changes, the grid frustums need to be recomputed.
    ComputeGridFrustums();
    // If the screen resolution changes, the cluster grid needs to be recomputed.
    UpdateClusterGrid();
}

void OnPreRender( RenderEventArgs& e )
{

}

void OnRender( RenderEventArgs& e )
{
    const Graphics::Window& window = dynamic_cast<const Graphics::Window&>( e.Caller );
    static std::shared_ptr<GraphicsCommandQueue> commandQueue = g_RenderDevice->GetGraphicsQueue();
    std::shared_ptr<GraphicsCommandBuffer> commandBuffer = commandQueue->GetGraphicsCommandBuffer();

    e.Camera = g_Camera;
    e.GraphicsCommandBuffer = commandBuffer;

    {
        Profiler::Get().PushProfilingMarker( _W(__FUNCTION__), commandBuffer);
        if ( g_IsLoading )
        {
            g_LoadingScreenTechnique.Render( e );
            // Show a progress bar.
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin( "Loading", nullptr, ImVec2( g_WindowWidth / 2.0f , 0), -1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar );
            ImGui::ProgressBar( g_Application.GetLoadingProgressRatio(), ImVec2( -1, 0 ) );

            std::string loadingProgressText = ConvertString( g_Application.GetLoadingMessage() );

            ImGui::Text( loadingProgressText.c_str() );

            ImGui::End();
        }
        else
        {
            g_DebugLightsPass->SetEnabled( g_RenderLights );
            g_DebugLightCountsPass->SetEnabled( g_RenderDebugTexture );
            g_RenderDebugTexturePass->SetEnabled( g_RenderDebugTexture );
            g_DebugClustersPass->SetEnabled( g_RenderDebugClusters );

            switch ( g_RenderingTechnique )
            {
            case RenderingTechnique::Forward:
                g_ForwardRenderingTechnique.Render( e );
                break;
            case RenderingTechnique::ForwardPlus:
                g_ForwardPlusRenderingTechnique.Render( e );
                break;
            case RenderingTechnique::Clustered:
            case RenderingTechnique::Clustered_Optimized:
                g_ClusteredRenderingTechnique.Render( e );
                break;
            }
        }
        Profiler::Get().PopProfilingMarker(commandBuffer);
    }

    g_RenderFence = commandQueue->Submit( commandBuffer );
}

void OnPostRender( RenderEventArgs& e )
{
    // Render GUI
    {
        OnGUI( e );

        static std::shared_ptr<GraphicsCommandQueue> commandQueue = g_RenderDevice->GetGraphicsQueue();
        std::shared_ptr<GraphicsCommandBuffer> commandBuffer = commandQueue->GetGraphicsCommandBuffer();

        GUI::RenderGUI( commandBuffer );

        commandQueue->Submit( commandBuffer );
    }
    g_RenderWindow->Present();
}

void Notify( const std::string& notificationText, float notificationDuration = 5.0f )
{
    g_NotificationText = notificationText;
    g_NotificationTimer = std::chrono::high_resolution_clock::now();
    g_NotificationDuration = notificationDuration;
    g_ShowNotification = true;
}

void SaveConfig()
{
    Notify( "Saving config file...", FLT_MAX );

    g_Config.CameraPosition = g_Camera->GetTranslation();
    g_Config.CameraRotation = g_Camera->GetRotation();
    g_Config.Save();

    Notify( "Config file saved." );
}

void FocusCurrentLight()
{
    glm::vec3 focusPoint = g_Camera->GetTranslation();
    glm::quat camRot = g_Camera->GetRotation();
    float fov = glm::radians( g_Camera->GetFOV() ) / 2.0f;
    float focusDistance = 0.0f;

    if ( g_SelectedPointLight )
    {
        ReadbackPointLights( g_Config.PointLights.data() );
        focusPoint = g_SelectedPointLight->m_PositionWS.xyz;
        focusDistance = g_SelectedPointLight->m_Range * 1.5f / glm::tan( fov );
    }
    else if ( g_SelectedSpotLight )
    {
        ReadbackSpotLights( g_Config.SpotLights.data() );
        focusPoint = g_SelectedSpotLight->m_PositionWS.xyz;
        focusDistance = g_SelectedSpotLight->m_Range * 1.5f / glm::tan( fov );
    }

    if ( g_FocusDistance == FLT_MAX )
    {
        g_FocusDistance = focusDistance;
    }

    g_Camera->SetTranslate( camRot * glm::vec3( 0, 0, g_FocusDistance ) + focusPoint );

}

void SetMultisampleEnabled( bool multiSampleEnabled )
{
    if ( g_Config.MultiSampleEnable != multiSampleEnabled )
    {
        uint8_t multiSampleCount = multiSampleEnabled ? 16 : 1;

        TextureFormat colorFormat( Graphics::TextureComponents::RGBA, TextureType::UnsignedNormalized, multiSampleCount, 8, 8, 8, 8, 0, 0 );
        TextureFormat depthFormat( Graphics::TextureComponents::DepthStencil, TextureType::UnsignedNormalized, multiSampleCount, 0, 0, 0, 0, 24, 8 );

        g_RenderWindow->SetColorFormat( colorFormat );
        g_RenderWindow->SetDepthStencilFormat( depthFormat );

        auto windowRenderTarget = g_RenderWindow->GetRenderTarget();
        auto depthStencilTexture = windowRenderTarget->GetTexture( AttachmentPoint::DepthStencil );

        g_DepthOnlyRenderTarget->AttachTexture( AttachmentPoint::DepthStencil, depthStencilTexture );

        g_ClusterSamplesDebugTexture = g_RenderDevice->CreateTexture2D( g_WindowWidth, g_WindowHeight, 1, colorFormat );
        g_ClusterSamplesRenderTarget->AttachTexture( AttachmentPoint::DepthStencil, depthStencilTexture );
        g_ClusterSamplesRenderTarget->AttachTexture( AttachmentPoint::Color0, g_ClusterSamplesDebugTexture );

        g_Config.MultiSampleEnable = multiSampleEnabled;
    }
}

void SavePerformanceData()
{
    char buffer[80];
    auto time = std::time( nullptr );
    std::tm timeInfo;
    localtime_s( &timeInfo, &time );

    std::strftime( buffer, 80, "%Y-%m-%d-%H-%M-%S", &timeInfo );

    size_t numLights = g_Config.PointLights.size() + g_Config.SpotLights.size();

    std::wstringstream fileName;
    fileName << "../Perf/" << buffer << " (" << g_RenderDevice->GetAdapter()->GetDescription() << ") @ " << g_WindowWidth << "x" << g_WindowHeight << " [" << RenderTechniqueName[(int)g_RenderingTechnique] << ", " << numLights << " lights]" << ".csv";

    PrintProfileDataVisitor profilerVisitor( fileName.str() );
    Profiler::Get().Accept( profilerVisitor );

    Notify( ConvertString( fileName.str() + L" saved.") );
}

void ClearProfilingData()
{
    Profiler::Get().ClearAllProfilingData();
    g_SelectedProfileMarker = nullptr;
}

void SetRenderingTechnique(RenderingTechnique technique)
{
    g_RenderingTechnique = technique;
//    ClearProfilingData();
}

void OnKeyPressed( KeyEventArgs& e )
{
    if ( ImGui::GetIO().WantCaptureKeyboard ) return;

    switch ( e.Key )
    {
    case KeyCode::F1:
        g_RenderDebugTexture = !g_RenderDebugTexture;
        g_RenderDebugClusters = false;
        break;
    case KeyCode::F2:
        g_RenderDebugClusters = !g_RenderDebugClusters;
        g_RenderDebugTexture = false;
        break;
    case KeyCode::F4:
        if ( e.Alt )
        {
    case KeyCode::Escape:
            g_Application.Stop();
        }
        break;
    case KeyCode::Enter:
        if ( e.Alt )
        {
    case KeyCode::F11:
            if ( g_RenderWindow )
            {
                g_RenderWindow->SetFullScreen( !g_RenderWindow->IsFullScreen() );
            }
        }
        break;
    case KeyCode::F:
        if ( e.Shift )
        {
            g_UpdateUniqueClusters = !g_UpdateUniqueClusters;
        }
        else
        {
            if ( !g_TrackSelectedLight )
            {
                FocusCurrentLight();
                g_TrackSelectedLight = true;
            }
        }
        break;
    case KeyCode::L:
        g_RenderLights = !g_RenderLights;
        break;
    case KeyCode::M:
        SetMultisampleEnabled( !g_Config.MultiSampleEnable );
        break;
    case KeyCode::P:
        {
        }
        break;
    case KeyCode::R:

        if ( e.Control )
        {
            g_Config.Reload();
        }

        g_Camera->SetTranslate( g_Config.CameraPosition );
        g_CameraController->SetCameraRotation( g_Config.CameraRotation );

        break;
    case KeyCode::S:
        if ( e.Control && e.Shift )
        {
            SavePerformanceData();
        }
        else if ( e.Control )
        {
            SaveConfig();
        }
        break;
    case KeyCode::Y:
        g_InvertY = !g_InvertY;
        break;
    case KeyCode::Space:
        g_Animate = !g_Animate;
        break;
    case KeyCode::V:
        g_Config.VSync = !g_RenderWindow->IsVSync();
        g_RenderWindow->SetVSync( g_Config.VSync );
        break;
    }

    // If the Control keys are pressed, toggle GUI window visibility.
    if ( e.Control )
    {
        switch ( e.Key )
        {
            case KeyCode::D1:
                g_ShowStatistics = !g_ShowStatistics;
                break;
            case KeyCode::D2:
                g_ShowProfiler = !g_ShowProfiler;
                break;
            case KeyCode::D3:
                g_ShowGenerateLights = !g_ShowGenerateLights;
                break;
            case KeyCode::D4:
                g_ShowLightsHierarchy = !g_ShowLightsHierarchy;
                break;
            case KeyCode::D5:
                g_ShowOptionsWindow = !g_ShowOptionsWindow;
                break;
        }
    }
    else
    {
        switch ( e.Key )
        {
        case KeyCode::D1:
            SetRenderingTechnique(RenderingTechnique::Forward);
            break;
        case KeyCode::D2:
            SetRenderingTechnique(RenderingTechnique::ForwardPlus);
            break;
        case KeyCode::D3:
            SetRenderingTechnique(RenderingTechnique::Clustered);
            break;
        case KeyCode::D4:
            SetRenderingTechnique(RenderingTechnique::Clustered_Optimized);
            break;
        case KeyCode::D0:
            g_Camera->SetTranslate( glm::vec3( 0 ) );
            g_CameraController->SetCameraRotation( glm::quat() );
            break;
        }
    }
}

void OnKeyReleased( KeyEventArgs& e )
{
    switch ( e.Key )
    {
        case KeyCode::F:
            g_TrackSelectedLight = false;
            break;
    }
}

void OnMouseWheel( MouseWheelEventArgs& e )
{
    if ( ( g_SelectedPointLight || g_SelectedSpotLight || g_SelectedDirLight ) && g_TrackSelectedLight )
    {
        g_FocusDistance -= e.WheelDelta * 0.1f;
        g_FocusDistance = glm::max( 0.0f, g_FocusDistance );
    }
}


void OnWindowClose( WindowCloseEventArgs& e )
{
    g_Application.Stop();
}

// Convert HSV to RGB:
// Source: https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
// Retrieved: 28/04/2016
// @param H Hue in the range [0, 360)
// @param S Saturation in the range [0, 1]
// @param V Value in the range [0, 1]
glm::vec3 HSVtoRGB( float H, float S, float V )
{
    float C = V * S;
    float m = V - C;
    float H2 = H / 60.0f;
    float X = C * ( 1.0f - fabsf( fmodf( H2, 2.0f ) - 1.0f ) );

    glm::vec3 RGB;

    switch ( static_cast<int>(H2) )
    {
    case 0:
        RGB = { C, X, 0 };
        break;
    case 1:
        RGB = { X, C, 0 };
        break;
    case 2:
        RGB = { 0, C, X };
        break;
    case 3:
        RGB = { 0, X, C };
        break;
    case 4:
        RGB = { X, 0, C };
        break;
    case 5:
        RGB = { C, 0, X };
        break;
    }

    return RGB + m;
}

void CreatePointLightsBuffer( std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetCopyQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    g_PointLightsBuffer = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.PointLights );
    g_PointLightsBuffer->SetName( L"Point Lights Buffer" );

    g_PointLightsReadbackBuffer = g_RenderDevice->CreateReadbackBuffer( g_Config.PointLights.size() * sizeof( PointLight ) );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetCopyQueue()->Submit( commandBuffer );
        fence->WaitFor();
    }
}

void CreateSpotLightsBuffer( std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetCopyQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    g_SpotLightsBuffer = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.SpotLights );
    g_SpotLightsBuffer->SetName( L"Spot Lights Buffer" );

    g_SpotLightsReadbackBuffer = g_RenderDevice->CreateReadbackBuffer( g_Config.SpotLights.size() * sizeof( SpotLight ) );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetCopyQueue()->Submit( commandBuffer );
        fence->WaitFor();
    }
}

void CreateDirLightsBuffer( std::shared_ptr<CopyCommandBuffer> commandBuffer = nullptr )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetCopyQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    g_DirectionalLightsBuffer = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.DirectionalLights );
    g_DirectionalLightsBuffer->SetName( L"Directional Lights Buffer" );

    g_DirectionalLightsReadbackBuffer = g_RenderDevice->CreateReadbackBuffer( g_Config.DirectionalLights.size() * sizeof( DirectionalLight ) );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetCopyQueue()->Submit( commandBuffer );
        fence->WaitFor();
    }
}

// Create the lights structured buffers.
void CreateLightBuffers()
{
    auto commandQueue = g_RenderDevice->GetCopyQueue();
    auto commandBuffer = commandQueue->GetCopyCommandBuffer();

    // Create structured buffers to store the lights.
    CreatePointLightsBuffer( commandBuffer );
    CreateSpotLightsBuffer( commandBuffer );
    CreateDirLightsBuffer( commandBuffer );

    // Create a structured buffer to store the AABB for the lights in the scene.
    if ( !g_LightsAABB )
    {
        // To compute the AABB for all of the lights in the scene, a parallel reduction algorithm is used.
        // The reduction compute shader will be dispatched with 512 thread groups each group with 512 threads.
        // The first pass of the reduction will compute 512 AABBs and the second pass will dispatch a single 
        // thread group that will reduce the 512 AABBs into a single AABB at index 0 of the LightsAABB structured buffer.
        g_LightsAABB = g_RenderDevice->CreateStructuredBuffer( commandBuffer, 512, sizeof( AABB ) );
        g_LightsAABB->SetName( L"Lights AABB" );
    }

    // Create a buffer to store the Morton codes for point lights.
    g_PointLightMortonCodes = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumPointLights, sizeof( uint32_t ) );
    g_PointLightMortonCodes->SetName( L"Point Light Morton Codes" );

    // Create a buffer to store the sorted indices for the point lights.
    g_PointLightIndices = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumPointLights, sizeof( uint32_t ) );
    g_PointLightIndices->SetName( L"Point Light Indices" );

    // For sorting, we need to double buffer the output.
    g_PointLightMortonCodes_OUT = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumPointLights, sizeof( uint32_t ) );
    g_PointLightMortonCodes_OUT->SetName( L"Point Light Morton Codes (OUT)" );
    g_PointLightIndices_OUT = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumPointLights, sizeof( uint32_t ) );
    g_PointLightIndices_OUT->SetName( L"Point Light Indices (OUT)" );

    // BVH for point lights.
    uint32_t numNodes = GetNumNodes( g_Config.NumPointLights );
    g_PointLightBVH = g_RenderDevice->CreateStructuredBuffer( commandBuffer, numNodes, sizeof( AABB ) );
    g_PointLightBVH->SetName( L"Point Light BVH" );

    // BVH for spot lights.
    numNodes = GetNumNodes( g_Config.NumSpotLights );
    g_SpotLightBVH = g_RenderDevice->CreateStructuredBuffer( commandBuffer, numNodes, sizeof( AABB ) );
    g_SpotLightBVH->SetName( L"Spot Light BVH" );

    // Create a buffer to store the Morton codes for spot lights.
    g_SpotLightMortonCodes = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumSpotLights, sizeof( uint32_t ) );
    g_SpotLightMortonCodes->SetName( L"Spot Light Morton Codes" );

    // Create a buffer to store the sorted indices for the spot lights.
    g_SpotLightIndices = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumSpotLights, sizeof( uint32_t ) );
    g_SpotLightIndices->SetName( L"Spot Light Indices" );

    // For sorting, we need to double buffer the output.
    g_SpotLightMortonCodes_OUT = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumSpotLights, sizeof( uint32_t ) );
    g_SpotLightMortonCodes_OUT->SetName( L"Spot Light Morton Codes (OUT)" );
    g_SpotLightIndices_OUT = g_RenderDevice->CreateStructuredBuffer( commandBuffer, g_Config.NumSpotLights, sizeof( uint32_t ) );
    g_SpotLightIndices_OUT->SetName( L"Spot Light Indices (OUT)" );

    // The maximum number of elements that need to be sorted.
    uint32_t maxElements = glm::max( g_Config.NumPointLights, g_Config.NumSpotLights );
    
    // Radix sort will sort Morton codes (keys) into chunks of SORT_NUM_THREADS_PER_THREAD_GROUP size.
    uint32_t chunkSize = SORT_NUM_THREADS_PER_THREAD_GROUP;
    // The number of chunks that need to be merge sorted after Radix sort finishes.
    uint32_t numChunks = static_cast<uint32_t>( glm::ceil( maxElements / (float)chunkSize ) );
    // The number of sort groups that are needed to sort the first set of chunks.
    // Each sort group will sort 2 chunks. So the maximum number of sort groups is 1/2 of the 
    // number of chunks.
    uint32_t maxSortGroups = numChunks / 2;
    // The number of merge path partitions per sort group is the total values
    // to be sorted per sort group (2 chunks) divided by the number of elements 
    // that can be sorted per thread group. One is added to account for the 
    // merge path partition at the END of the chunk.
    uint32_t numMergePathPartitionsPerSortGroup = static_cast<uint32_t>( glm::ceil( ( chunkSize * 2 ) / (float)( SORT_ELEMENTS_PER_THREAD * SORT_NUM_THREADS_PER_THREAD_GROUP ) ) ) + 1u;

    // The maximum number of merge path partitions is the number of merge path partitions
    // needed by a single sort group multiplied by the maximum number of sort groups.
    uint32_t maxMergePathPartitions = numMergePathPartitionsPerSortGroup * maxSortGroups;

    g_MergePathPartitions = g_RenderDevice->CreateStructuredBuffer( commandBuffer, maxMergePathPartitions, sizeof( uint32_t ) );
    g_MergePathPartitions->SetName( L"Merge Path Partitions" );

    auto fence = commandQueue->Submit( commandBuffer );
    fence->WaitFor();
}

void ReadbackPointLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetGraphicsQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    commandBuffer->CopyResource( g_PointLightsReadbackBuffer, g_PointLightsBuffer );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetGraphicsQueue()->Submit( commandBuffer );
        fence->WaitFor();

        if ( dstBuffer )
        {
            g_PointLightsReadbackBuffer->GetData( dstBuffer );
        }
    }
}

void ReadbackSpotLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetGraphicsQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    commandBuffer->CopyResource( g_SpotLightsReadbackBuffer, g_SpotLightsBuffer );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetGraphicsQueue()->Submit( commandBuffer );
        fence->WaitFor();

        if ( dstBuffer )
        {
            g_SpotLightsReadbackBuffer->GetData( dstBuffer );
        }
    }
}

void ReadbackDirLights( void* dstBuffer, std::shared_ptr<CopyCommandBuffer> commandBuffer )
{
    bool bSubmit = false;
    if ( !commandBuffer )
    {
        commandBuffer = g_RenderDevice->GetGraphicsQueue()->GetCopyCommandBuffer();
        bSubmit = true;
    }

    commandBuffer->CopyResource( g_DirectionalLightsReadbackBuffer, g_DirectionalLightsBuffer );

    if ( bSubmit )
    {
        auto fence = g_RenderDevice->GetGraphicsQueue()->Submit( commandBuffer );
        fence->WaitFor();

        if ( dstBuffer )
        {
            g_DirectionalLightsReadbackBuffer->GetData( dstBuffer );
        }
    }
}

// Readback the light buffers from the GPU buffers.
void ReadbackLightBufers()
{
    auto commandQueue = g_RenderDevice->GetGraphicsQueue();
    auto commandBuffer = commandQueue->GetCopyCommandBuffer();

    ReadbackPointLights( nullptr, commandBuffer );
    ReadbackSpotLights( nullptr, commandBuffer );
    ReadbackDirLights( nullptr, commandBuffer );

    auto fence = commandQueue->Submit( commandBuffer );
    fence->WaitFor();

    g_PointLightsReadbackBuffer->GetData( g_Config.PointLights.data() );
    g_SpotLightsReadbackBuffer->GetData( g_Config.SpotLights.data() );
    g_DirectionalLightsReadbackBuffer->GetData( g_Config.DirectionalLights.data() );
}

void GenerateLights()
{
    SelectPointLight( nullptr );
    SelectSpotLight( nullptr );
    SelectDirLight( nullptr );

    g_Config.PointLights = GenerateLights<PointLight>( g_Config.NumPointLights );
    g_Config.SpotLights = GenerateLights<SpotLight>( g_Config.NumSpotLights );
    g_Config.DirectionalLights = GenerateLights<DirectionalLight>( g_Config.NumDirectionalLights );

    CreateLightBuffers();
}

template<typename LightType>
LightType GenerateLight( const glm::vec4& positionWS, const glm::vec4& directionWS, float spotAngle, float range, const glm::vec3& color )
{
    static_assert( false, "non specialized version of this function cannot be used." );
    return LightType();
}

template<>
PointLight GenerateLight<PointLight>( const glm::vec4& positionWS, const glm::vec4& directionWS, float spotAngle, float range, const glm::vec3& color )
{
    PointLight light;
    light.m_PositionWS = positionWS;
    light.m_Color = color;
    light.m_Range = range;

    return light;
}

template<>
SpotLight GenerateLight<SpotLight>( const glm::vec4& positionWS, const glm::vec4& directionWS, float spotAngle, float range, const glm::vec3& color )
{
    SpotLight light;
    light.m_PositionWS = positionWS;
    light.m_DirectionWS = directionWS;
    light.m_Color = color;
    light.m_SpotlightAngle = spotAngle;
    light.m_Range = range;

    return light;
}

template<>
DirectionalLight GenerateLight<DirectionalLight>( const glm::vec4& positionWS, const glm::vec4& directionWS, float spotAngle, float range, const glm::vec3& color )
{
    DirectionalLight light;
    light.m_DirectionWS = directionWS;
    light.m_Color = color;

    return light;
}

/**
 * Generate a vector of random colors. This is primarily used for debugging 
 * sample clusters.
 */
std::vector<glm::vec4> GenerateColors( uint32_t numColors )
{
    std::vector<glm::vec4> colors( numColors );
    for ( auto& color : colors )
    {
        color = glm::vec4( HSVtoRGB( glm::linearRand( 0.0f, 360.0f ), glm::linearRand( 0.0f, 1.0f ), 1.0f ), 1.0f );
    }

    return colors;
}

template<typename LightType>
std::vector<LightType> GenerateLights( uint32_t numLights )
{
    std::vector<LightType> lights( numLights );

    for ( auto& light : lights )
    {
        glm::vec4 positionWS = glm::vec4( glm::linearRand( g_Config.LightsMinBounds, g_Config.LightsMaxBounds ), 1.0f );
        glm::vec4 directionWS = glm::vec4( glm::sphericalRand( 1.0f ), 0.0f );
        float spotAngle = glm::linearRand( g_Config.MinSpotAngle, g_Config.MaxSpotAngle );
        float range = glm::linearRand( g_Config.MinRange, g_Config.MaxRange );
        glm::vec3 color = HSVtoRGB( glm::linearRand( 0.0f, 360.0f ), glm::linearRand( 0.0f, 1.0f ), 1.0f );

        light = GenerateLight<LightType>( positionWS, directionWS, spotAngle, range, color );
    }

    return lights;
}

// GUI related functions

template<typename T>
void PlotStats( const Core::Statistic<T> &stats, const std::string& overlay = "", float minScale = FLT_MAX, float maxScale = FLT_MAX, ImVec2 graphSize = ImVec2(0, 0) )
{
    const T* pData;
    uint32_t numValues;
    uint32_t offset;

    std::tie( pData, numValues, offset ) = stats.GetSamples();

    // Use the last value as a label for the plot.
    char labelBuffer[256];
    sprintf_s( labelBuffer, 256, "%08.5f ms", stats.GetLast() * T(1000.0) );
    
    auto valueGetter = [] ( void* data, int idx )
    {
        return static_cast<float>( reinterpret_cast<T*>( data )[idx] * T(1000.0) );
    };

    ImGui::PlotLines( labelBuffer, valueGetter, (void*)pData, numValues, offset, overlay.c_str(), minScale, maxScale, graphSize );
}

// GUI functions
void ShowStatistics( bool& bShowWindow )
{
    static Core::Statistic<double> cpuStats;
    static HighResolutionTimer timer;
    static double accumulatedTime = 0.0;
    static double averageTime = 0.0;
    static double averageFPS = 0.0;
    static uint32_t frameCounter = 0;

    const float statsWindowWidth = 400;

    timer.Tick();

    cpuStats.Sample( timer.ElapsedSeconds() );

    accumulatedTime += timer.ElapsedSeconds();
    frameCounter++;

    if ( accumulatedTime > 1.0f )
    {
        averageTime = accumulatedTime / frameCounter;
        averageFPS = 1.0 / averageTime;

        accumulatedTime = 0.0;
        frameCounter = 0;
    }

    if ( g_RenderWindow )
    {
        ImGui::SetNextWindowPos( ImVec2( g_RenderWindow->GetWindowWidth() - statsWindowWidth - 10, 20 ), ImGuiSetCond_FirstUseEver );
        if ( ImGui::Begin( "Statistics", &bShowWindow, ImVec2( statsWindowWidth, 0 ), 0.3f, ImGuiWindowFlags_NoResize ) )
        {
            static std::string adapterName = ConvertString( g_RenderDevice->GetAdapter()->GetDescription() );
            std::stringstream ss;
            ss << adapterName << " @ " << g_WindowWidth << " x " << g_WindowHeight;
            ImGui::Text( ss.str().c_str() );
            ImGui::Combo( "Render Technique", reinterpret_cast<int*>(&g_RenderingTechnique), RenderTechniqueName, static_cast<int>(RenderingTechnique::NumTechniques) );
            ImGui::Separator();
            ImGui::Text( "CPU: %08.5f ms\tFPS: %.5f", averageTime * 1000.0, averageFPS );
            ImGui::Separator();

            char overlayBuffer[255];
            sprintf_s( overlayBuffer, "Average: %08.5f ms", averageTime * 1000.0 );

            PlotStats( cpuStats, overlayBuffer, 0.0f, 33.33f, ImVec2( 0, 80) );
        }
        ImGui::End();
    }
}

void ShowProfilerMarker( ProfileNode& profileMarker, uint64_t frame )
{
    if ( frame - profileMarker.CpuFrame > 100 ) return;

    ImGui::PushID( profileMarker.Name.c_str() );

    bool expand = false;
    if ( profileMarker.Children.size() > 0 )
    {
        expand = ImGui::TreeNode( profileMarker.Name.c_str() );
    }
    else
    {
        ImGui::Bullet();
        ImGui::Selectable( profileMarker.Name.c_str() );
    }

    ImGui::NextColumn();

    // Plot CPU stats.
    ImGui::PushStyleColor( ImGuiCol_PlotLines, IntelBlue );
    ImGui::PushStyleColor( ImGuiCol_Text, IntelBlue );
    PlotStats( profileMarker.CpuStats, "", 0.0f, 33.33f );
    ImGui::PopStyleColor(2);

    if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) )
    {
        g_SelectedProfileMarker = &profileMarker;
        g_SelectedStateType = StatType::CPU;
    }

    ImGui::NextColumn();

    // Plot GPU stats.
    ImGui::PushStyleColor( ImGuiCol_PlotLines, NvidiaGreen );
    ImGui::PushStyleColor( ImGuiCol_Text, NvidiaGreen );
    PlotStats( profileMarker.GpuStats, "", 0.0f, 33.3f );
    ImGui::PopStyleColor(2);

    if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) )
    {
        g_SelectedProfileMarker = &profileMarker;
        g_SelectedStateType = StatType::GPU;
    }

    ImGui::NextColumn();

    if ( expand )
    {
        for ( auto& childNode : profileMarker.Children )
        {
            ShowProfilerMarker( *childNode, frame );
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void ShowRootProfilerMarker( ProfileNode& rootMarker, uint64_t frame )
{
    ImGui::Spacing();
    ImGui::Columns( 3 );
    ImGui::Separator();

    ImGui::PushID( ImGui::GetID( rootMarker.Name.c_str() ) );
    ImGui::Text( "Name" ); ImGui::NextColumn();
    ImGui::TextColored( IntelBlue, "CPU Time (ms)" ); ImGui::NextColumn();
    ImGui::TextColored( NvidiaGreen, "GPU Time (ms)" ); ImGui::NextColumn();
    ImGui::Separator();

    for ( auto childNode : rootMarker.Children )
    {
        ShowProfilerMarker( *childNode, frame );
    }
    ImGui::PopID();

    ImGui::Columns( 1 );
    ImGui::Separator();
}

void ShowProfiler( bool& bShowWindow )
{
    static HighResolutionTimer timer;
    static float totalTime = 0.0f;

    Profiler& profiler = Profiler::Get();


    timer.Tick();

    totalTime += static_cast<float>(timer.ElapsedSeconds());

    static std::string selectedLableName = "";

    if ( ImGui::Begin( "Profiler", &bShowWindow ) )
    {
        bool isPaused = profiler.IsPaused();
        if ( ImGui::Checkbox("Pause Profiler", &isPaused) )
        {
            profiler.SetPaused(isPaused);
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            ClearProfilingData();
        }

        if ( g_SelectedProfileMarker )
        {
            const Core::Statistic<double>* pStats = nullptr;

            switch ( g_SelectedStateType )
            {
            case StatType::CPU:
                ImGui::PushStyleColor( ImGuiCol_PlotLines, IntelBlue );
                ImGui::PushStyleColor( ImGuiCol_Text, IntelBlue );
                pStats = &g_SelectedProfileMarker->CpuStats;
                break;
            case StatType::GPU:
                ImGui::PushStyleColor( ImGuiCol_PlotLines, NvidiaGreen );
                ImGui::PushStyleColor( ImGuiCol_Text, NvidiaGreen );
                pStats = &g_SelectedProfileMarker->GpuStats;
                break;
            }

            PlotStats( *pStats, g_SelectedProfileMarker->Name, 0.0f, 33.33f, ImVec2( 0, 100 ) );
            ImGui::PopStyleColor( 2 );
        }
        else
        {
            ImGui::PlotLines( "", [] ( void* data, int idx ) { return 0.0f; }, nullptr, 1, 0, nullptr, 0.0f, 1.0f, ImVec2( 0, 100 ) );
        }

        ImGui::BeginChild( "Call Stack" );

        ShowRootProfilerMarker( *profiler.GetRootProfileMarker(), profiler.GetCurrentFrame() );

        ImGui::EndChild();
    }
    ImGui::End();
}

void ShowHelpMarker( const char* desc )
{
    ImGui::TextDisabled( "(?)" );
    if ( ImGui::IsItemHovered() )
        ImGui::SetTooltip( desc );
}

void ShowGenerateLightsWindow( bool& bShowWindow )
{
    ImGui::SetNextWindowPos( ImVec2( 50, 50 ), ImGuiSetCond_FirstUseEver );
    if ( ImGui::Begin( "Generate Lights", &bShowWindow ) )
    {
        int numPointLights = static_cast<int>( g_Config.NumPointLights );
        int numSpotLights = static_cast<int>( g_Config.NumSpotLights );
        int numDirLights = static_cast<int>( g_Config.NumDirectionalLights );

        if ( ImGui::DragInt( "Num Point Lights", &numPointLights, 1, 0, INT_MAX ) )
        {
            g_Config.NumPointLights = glm::clamp<uint32_t>( numPointLights, 0, INT_MAX );
        }
        if (ImGui::Button("Normalize Point Lights"))
        {
            float bounds3 = (g_Config.LightsMaxBounds.x - g_Config.LightsMinBounds.x) * (g_Config.LightsMaxBounds.y - g_Config.LightsMinBounds.y) * (g_Config.LightsMaxBounds.z - g_Config.LightsMinBounds.z);
            float range3 = glm::pow(g_Config.MaxRange, 3.0f);
            uint32_t totalLights = glm::floor(bounds3 / range3);

            if (totalLights < g_Config.NumSpotLights)
            {
                g_Config.NumPointLights = 0;
            }
            else
            {
                g_Config.NumPointLights = totalLights - g_Config.NumSpotLights;
            }
        }
        if ( ImGui::DragInt( "Num Spot Lights", &numSpotLights, 1, 0, INT_MAX ) )
        {
            g_Config.NumSpotLights = glm::clamp<uint32_t>( numSpotLights, 0, INT_MAX );
        }
        if (ImGui::Button("Normalize Spot Lights"))
        {
            float bounds3 = (g_Config.LightsMaxBounds.x - g_Config.LightsMinBounds.x) * (g_Config.LightsMaxBounds.y - g_Config.LightsMinBounds.y) * (g_Config.LightsMaxBounds.z - g_Config.LightsMinBounds.z);
            float range3 = glm::pow(g_Config.MaxRange, 3.0f);
            uint32_t totalLights = glm::floor(bounds3 / range3);

            if (totalLights < g_Config.NumPointLights)
            {
                g_Config.NumSpotLights = 0;
            }
            else
            {
                g_Config.NumSpotLights = totalLights - g_Config.NumPointLights;
            }
        }
        if ( ImGui::DragInt( "Num Directional Lights", &numDirLights, 0.05f, 0, INT_MAX ) )
        {
            g_Config.NumDirectionalLights = glm::clamp<uint32_t>( numDirLights, 0, INT_MAX );
        }
        if ( ImGui::DragFloat3( "Min Bounds", &g_Config.LightsMinBounds.x, 0.01f ) )
        {
            g_Config.LightsMaxBounds = glm::max( g_Config.LightsMinBounds, g_Config.LightsMaxBounds );
        }
        if ( ImGui::DragFloat3( "Max Bounds", &g_Config.LightsMaxBounds.x, 0.01f ) )
        {
            g_Config.LightsMinBounds = glm::min( g_Config.LightsMinBounds, g_Config.LightsMaxBounds );
        }
        if (ImGui::Button("Normalize Bounds"))
        {
            float range3 = powf(g_Config.MaxRange, 3.0f);
            float bounds = powf( ( g_Config.NumPointLights + g_Config.NumSpotLights ) * range3, 1.0f / 3.0f) / 2.0f;
            g_Config.LightsMinBounds = glm::vec3(-bounds, -bounds, -bounds);
            g_Config.LightsMaxBounds = glm::vec3(bounds, bounds, bounds);
        }
        if ( ImGui::DragFloat( "Min Spot Angle", &g_Config.MinSpotAngle, 1.0f, 0.0f, 75.0f ) )
        {
            g_Config.MinSpotAngle = glm::clamp( g_Config.MinSpotAngle, 0.0f, 75.0f );
            g_Config.MaxSpotAngle = glm::max( g_Config.MinSpotAngle, g_Config.MaxSpotAngle );
        }
        if ( ImGui::DragFloat( "Max Spot Angle", &g_Config.MaxSpotAngle, 1.0f, 0.0f, 75.0f ) )
        {
            g_Config.MaxSpotAngle = glm::clamp( g_Config.MaxSpotAngle, 0.0f, 75.0f );
            g_Config.MinSpotAngle = glm::min( g_Config.MinSpotAngle, g_Config.MaxSpotAngle );
        }
        if ( ImGui::DragFloat( "Min Range", &g_Config.MinRange, 0.01f, 0.0f, FLT_MAX ) )
        {
            g_Config.MinRange = glm::max( 0.0f, g_Config.MinRange );
            g_Config.MaxRange = glm::max( g_Config.MinRange, g_Config.MaxRange );
        }
        if ( ImGui::DragFloat( "Max Range", &g_Config.MaxRange, 0.01f, 0.0f, FLT_MAX ) )
        {
            g_Config.MaxRange = glm::max( 0.0f, g_Config.MaxRange );
            g_Config.MinRange = glm::min( g_Config.MinRange, g_Config.MaxRange );
        }
        if (ImGui::Button("Normalize Light Ranges"))
        {
            float bounds3 = (g_Config.LightsMaxBounds.x - g_Config.LightsMinBounds.x) * (g_Config.LightsMaxBounds.y - g_Config.LightsMinBounds.y) * (g_Config.LightsMaxBounds.z - g_Config.LightsMinBounds.z);
            g_Config.MaxRange = powf( bounds3 / (g_Config.NumPointLights + g_Config.NumSpotLights), 1.0f / 3.0f );
            g_Config.MinRange = g_Config.MaxRange - (g_Config.MaxRange * 0.1f);
        }


        if ( ImGui::Button( "Generate Lights" ) )
        {
            GenerateLights();
//            ClearProfilingData();
        }
    }
    ImGui::End();
}

void ShowOptionsWindow( bool& bShowWindow )
{
    static bool showTestWindow = false;
    static bool showGenerateLightsWindow = false;

    static bool showProfiler = false;

    ImGui::SetNextWindowPos( ImVec2( 10, 20 ), ImGuiSetCond_FirstUseEver );
    if ( ImGui::Begin( "Options", &bShowWindow, ImVec2( 0, 0 ), 0.3f, ImGuiWindowFlags_NoResize ) )
    {

        if ( g_RenderWindow ) g_Config.VSync = g_RenderWindow->IsVSync();
        ImGui::Checkbox( "V Sync", &g_Config.VSync ); ImGui::SameLine(); ImGui::TextDisabled( "V" );
        
        if ( g_Config.VSync != g_RenderWindow->IsVSync() )
        {
            g_RenderWindow->SetVSync( g_Config.VSync );
        }

        bool multiSampleEnabled = g_Config.MultiSampleEnable;
        ImGui::Checkbox( "Multisample Enabled", &multiSampleEnabled ); ImGui::SameLine(); ImGui::TextDisabled( "M" );
        SetMultisampleEnabled( multiSampleEnabled );

        ImGui::Checkbox( "Render Debug Lights", &g_RenderLights ); ImGui::SameLine(); ImGui::TextDisabled( "L" );
        ImGui::Checkbox( "Update Clusters", &g_UpdateUniqueClusters ); ImGui::SameLine(); ImGui::TextDisabled( "Shift+F" );
        ImGui::Checkbox( "Animate Lights", &g_Animate ); ImGui::SameLine(); ImGui::TextDisabled( "Space" );
        ImGui::Checkbox( "Invert Y", &g_InvertY); ImGui::SameLine(); ImGui::TextDisabled( "Y" );
    }
    ImGui::End();

    if ( showTestWindow )
    {
        ImGui::SetNextWindowPos( ImVec2( 50, 50 ), ImGuiSetCond_FirstUseEver );     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowTestWindow( &showTestWindow );
    }

    if ( showProfiler )
    {
        ShowProfiler( showProfiler );
    }

    if ( showGenerateLightsWindow )
    {
        ShowGenerateLightsWindow( showGenerateLightsWindow );
    }
}

void ShowLightEditor( bool& bShowWindow )
{
    ImGui::SetNextWindowPos( ImVec2( 100, 50 ), ImGuiSetCond_FirstUseEver );
    if ( ImGui::Begin( "Light Editor", &bShowWindow, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        if ( g_SelectedPointLight )
        {
            ReadbackPointLights( g_Config.PointLights.data() );

            bool isModified = false;
            ImGui::TextDisabled( "Point Light" );
            ImGui::Separator();
            if ( ImGui::DragFloat3( "Position", &g_SelectedPointLight->m_PositionWS.x, 0.01f ) )
            {
                isModified = true;
            }
            if ( ImGui::ColorEdit3( "Color", &g_SelectedPointLight->m_Color.x ) )
            {
                isModified = true;
            }
            if ( ImGui::DragFloat( "Range", &g_SelectedPointLight->m_Range, 0.01f, 0.0f, FLT_MAX ) )
            {
                isModified = true;
                g_SelectedPointLight->m_Range = glm::max( 0.0f, g_SelectedPointLight->m_Range );
            }
            if ( ImGui::DragFloat( "Intensity", &g_SelectedPointLight->m_Intensity, 0.01f, 0.0f, FLT_MAX ) )
            {
                isModified = true;
                g_SelectedPointLight->m_Intensity = glm::max( 0.0f, g_SelectedPointLight->m_Intensity );
            }
            bool enabled = g_SelectedPointLight->m_Enabled != 0;
            if ( ImGui::Checkbox( "Enabled", &enabled ) )
            {
                isModified = true;
                g_SelectedPointLight->m_Enabled = enabled ? 1 : 0;
            }

            if ( isModified )
            {
                CreatePointLightsBuffer();
            }
        }

        if ( g_SelectedSpotLight )
        {
            ReadbackSpotLights( g_Config.SpotLights.data() );

            bool isModified = false;
            ImGui::TextDisabled( "Spot Light" );
            ImGui::Separator();

            if ( ImGui::DragFloat3( "Position", &g_SelectedSpotLight->m_PositionWS.x, 0.01f ) )
            {
                isModified = true;
            }
            if ( ImGui::DragFloat3( "Direction", &g_SelectedSpotLight->m_DirectionWS.x, 0.01f, -1.0f, 1.0f ) )
            {
                isModified = true;
                g_SelectedSpotLight->m_DirectionWS = glm::normalize( g_SelectedSpotLight->m_DirectionWS );
            }
            if ( ImGui::ColorEdit3( "Color", &g_SelectedSpotLight->m_Color.x ) )
            {
                isModified = true;
            }
            if ( ImGui::DragFloat( "Spotlight Angle", &g_SelectedSpotLight->m_SpotlightAngle, 1.0f, 0.0f, 80.0f ) )
            {
                isModified = true;
                g_SelectedSpotLight->m_SpotlightAngle = glm::clamp( g_SelectedSpotLight->m_SpotlightAngle, 0.0f, 80.0f );
            }
            if ( ImGui::DragFloat( "Range", &g_SelectedSpotLight->m_Range, 0.01f, 0.0f, FLT_MAX ) )
            {
                isModified = true;
                g_SelectedSpotLight->m_Range = glm::max( 0.0f, g_SelectedSpotLight->m_Range );
            }
            if ( ImGui::DragFloat( "Intensity", &g_SelectedSpotLight->m_Intensity, 0.01f, 0.0f, FLT_MAX ) )
            {
                isModified = true;
                g_SelectedSpotLight->m_Intensity = glm::max( 0.0f, g_SelectedSpotLight->m_Intensity );
            }
            bool enabled = g_SelectedSpotLight->m_Enabled != 0;
            if ( ImGui::Checkbox( "Enabled", &enabled ) )
            {
                isModified = true;
                g_SelectedSpotLight->m_Enabled = enabled ? 1 : 0;
            }

            if ( isModified )
            {
                CreateSpotLightsBuffer();
            }
        }

        if ( g_SelectedDirLight )
        {
            ReadbackDirLights( g_Config.DirectionalLights.data() );

            bool isModified = false;
            ImGui::TextDisabled( "Directional Light" );
            ImGui::Separator();

            if ( ImGui::DragFloat3( "Direction", &g_SelectedDirLight->m_DirectionWS.x, 0.01f, -1.0f, 1.0f ) )
            {
                isModified = true;
                g_SelectedDirLight->m_DirectionWS = glm::normalize( g_SelectedDirLight->m_DirectionWS );
            }
            if ( ImGui::ColorEdit3( "Color", &g_SelectedDirLight->m_Color.x ) )
            {
                isModified = true;
            }
            if ( ImGui::DragFloat( "Intensity", &g_SelectedDirLight->m_Intensity, 0.01f, 0.0f, FLT_MAX ) )
            {
                isModified = true;
                g_SelectedDirLight->m_Intensity = glm::max( 0.0f, g_SelectedDirLight->m_Intensity );
            }
            bool enabled = g_SelectedDirLight->m_Enabled != 0;
            if ( ImGui::Checkbox( "Enabled", &enabled ) )
            {
                isModified = true;
                g_SelectedDirLight->m_Enabled = enabled ? 1 : 0;
            }

            if ( isModified )
            {
                // Upload changes to the lights buffer.
                CreateDirLightsBuffer();
            }
        }
    }
    ImGui::End();
}

void SelectPointLight( PointLight* pPointLight )
{
    if ( g_SelectedPointLight != pPointLight )
    {
        if ( g_SelectedPointLight )
        {
            g_SelectedPointLight->m_Selected = 0;
        }

        g_SelectedPointLight = pPointLight;

        if ( g_SelectedPointLight )
        {
            g_SelectedPointLight->m_Selected = 1;
            g_FocusDistance = FLT_MAX;
        }
        // Upload the modifications to the light buffer.
        CreatePointLightsBuffer();
    }
}

void SelectSpotLight( SpotLight* pSpotLight )
{
    if ( g_SelectedSpotLight != pSpotLight )
    {
        if ( g_SelectedSpotLight )
        {
            g_SelectedSpotLight->m_Selected = 0;
        }

        g_SelectedSpotLight = pSpotLight;

        if ( g_SelectedSpotLight )
        {
            g_SelectedSpotLight->m_Selected = 1;
            g_FocusDistance = FLT_MAX;
        }
        // Upload the modifications to the light buffer.
        CreateSpotLightsBuffer();
    }
}

void SelectDirLight( DirectionalLight* pDirLight )
{
    if ( g_SelectedDirLight != pDirLight )
    {
        if ( g_SelectedDirLight )
        {
            g_SelectedDirLight->m_Selected = 0;
        }

        g_SelectedDirLight = pDirLight;

        if ( g_SelectedDirLight )
        {
            g_SelectedDirLight->m_Selected = 1;
            g_FocusDistance = FLT_MAX;
        }
        // Upload the modifications to the light buffer.
        CreateDirLightsBuffer();
    }
}

void ShowLightsHierarchy( bool& bShowWindow )
{
    char labelBuffer[256];

    ImGui::SetNextWindowPos( ImVec2( 50, 50 ), ImGuiSetCond_FirstUseEver );
    if ( ImGui::Begin( "Lights Hierarchy", &bShowWindow ) )
    {
        if ( ImGui::CollapsingHeader( "Point Lights" ) )
        {
            for ( size_t i = 0; i < g_Config.PointLights.size(); ++i )
            {
                sprintf_s( labelBuffer, "PointLight [%zu]", i );
                bool bSelected = g_Config.PointLights[i].m_Selected != 0;
                if ( ImGui::Selectable( labelBuffer, &bSelected, ImGuiSelectableFlags_AllowDoubleClick ) )
                {
                    SelectSpotLight( nullptr );
                    SelectDirLight( nullptr );
                    SelectPointLight( &g_Config.PointLights[i] );
                    if ( ImGui::IsMouseDoubleClicked( 0 ) )
                    {
                        FocusCurrentLight();
                    }
                }
            }
        }
        if ( ImGui::CollapsingHeader( "Spot Lights" ) )
        {
            for (size_t i = 0; i < g_Config.SpotLights.size(); ++i )
            {
                sprintf_s( labelBuffer, "SpotLight [%zu]", i );
                bool bSelected = g_Config.SpotLights[i].m_Selected != 0;
                if ( ImGui::Selectable( labelBuffer, &bSelected, ImGuiSelectableFlags_AllowDoubleClick ) )
                {
                    SelectPointLight( nullptr );
                    SelectDirLight( nullptr );
                    SelectSpotLight( &g_Config.SpotLights[i] );
                    if ( ImGui::IsMouseDoubleClicked( 0 ) )
                    {
                        FocusCurrentLight();
                    }
                }
            }
        }
        if ( ImGui::CollapsingHeader( "Directional Lights" ) )
        {
            for ( size_t i = 0; i < g_Config.DirectionalLights.size(); ++i )
            {
                sprintf_s( labelBuffer, "DirectionalLight [%zu]", i );
                bool bSelected = g_Config.DirectionalLights[i].m_Selected != 0;
                if ( ImGui::Selectable( labelBuffer, &bSelected, ImGuiSelectableFlags_AllowDoubleClick ) )
                {
                    SelectPointLight( nullptr );
                    SelectSpotLight( nullptr );
                    SelectDirLight( &g_Config.DirectionalLights[i] );
                    if ( ImGui::IsMouseDoubleClicked( 0 ) )
                    {
                        FocusCurrentLight();
                    }
                }
            }
        }
    }
    ImGui::End();

    ShowLightEditor( bShowWindow );
}

void ShowMainMenu( bool& bShowMenu )
{
    if ( ImGui::BeginMainMenuBar() )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem("Save Config", "Ctrl+S" ) )
            {
                SaveConfig();
            }
            if ( ImGui::MenuItem( "Save Performance Stats", "Ctrl+Shift+S" ) )
            {
                SavePerformanceData();
            }
            if ( ImGui::MenuItem( "Quit", "Alt+F4" ) )
            {
                g_Application.Stop();
            }
            ImGui::EndMenu();
        }
        if ( ImGui::BeginMenu( "View" ) )
        {
            ImGui::MenuItem( "Statistics", "Ctrl+1", &g_ShowStatistics );
            ImGui::MenuItem( "Test Window", nullptr, &g_ShowTestWindow );
            ImGui::MenuItem( "Profiler", "Ctrl+2", &g_ShowProfiler );
            ImGui::MenuItem( "Generate Lights", "Ctrl+3", &g_ShowGenerateLights );
            ImGui::MenuItem( "Lights Editor", "Ctrl+4", &g_ShowLightsHierarchy );
            ImGui::MenuItem( "Options", "Ctrl+5", &g_ShowOptionsWindow );

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

}

void ShowNotification( bool& showNotifcation )
{
    static float notificationHeight = 30.0f;

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaTime = t2 - g_NotificationTimer;

    if ( showNotifcation && deltaTime.count() < g_NotificationDuration )
    {
        float alpha = 1.0f - ( deltaTime.count() / g_NotificationDuration );
        ImGui::SetNextWindowPos( ImVec2( 0.0f, static_cast<float>( g_WindowHeight ) - notificationHeight ) );
        ImGui::Begin( "Status", &showNotifcation, ImVec2( g_WindowWidth, notificationHeight ), alpha, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar );
        ImGui::TextColored( ImVec4( 1, 1, 1, alpha ),   g_NotificationText.c_str() );
        ImGui::End();
    }

    showNotifcation = deltaTime.count() < g_NotificationDuration;
}

void OnGUI( RenderEventArgs& e )
{
    static bool bShowMenu = false;

    ImGuiIO& io = ImGui::GetIO();
    ShowMainMenu( bShowMenu );

    if ( g_ShowStatistics )
    {
        ShowStatistics( g_ShowStatistics );
    }
    if ( g_ShowTestWindow )
    {
        ImGui::ShowTestWindow( &g_ShowTestWindow );
    }
    if ( g_ShowProfiler )
    {
        ShowProfiler( g_ShowProfiler );
    }
    if ( g_ShowGenerateLights )
    {
        ShowGenerateLightsWindow( g_ShowGenerateLights );
    }
    if ( g_ShowLightsHierarchy )
    {
        ShowLightsHierarchy( g_ShowLightsHierarchy );
    }
    if ( g_ShowOptionsWindow )
    {
        ShowOptionsWindow( g_ShowOptionsWindow );
    }
    if ( g_ShowNotification )
    {
        ShowNotification( g_ShowNotification );
    }
}