#include <EnginePCH.h>

#include <Graphics/DX12/WindowDX12.h>
#include <Graphics/DX12/ApplicationDX12.h>
#include <Graphics/DXGI/AdapterDXGI.h>
#include <Graphics/DXGI/DisplayDXGI.h>
#include <Graphics/DXGI/TextureFormatDXGI.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/FenceDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/RenderTargetDX12.h>
#include <Graphics/Profiler.h>

#include <Application.h>
#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

WindowDX12::WindowDX12( Core::Application& app, std::shared_ptr<Graphics::DeviceDX12> device, HWND hWnd,
                        const std::string& windowTitle,
                        uint32_t width, uint32_t height,
                        bool fullscreen, bool vSync,
                        const Graphics::TextureFormat& colorFormat,
                        const Graphics::TextureFormat& depthStencilFormat )
    : super( app, windowTitle, width, height, fullscreen, vSync )
    , m_hWindow( hWnd )
    , m_Device( device )
    , m_Queue( std::dynamic_pointer_cast<GraphicsCommandQueueDX12>( device->GetGraphicsQueue() ) )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_ColorFormat( colorFormat )
    , m_DepthStencilFormat( depthStencilFormat )
    , m_dxgiColorFormat( ConvertTextureFormat( colorFormat ) )
    , m_ResizeEventArgs( *this, width, height )
    , m_ResizeSwapChain( false )
    , m_CurrentFrameIndex( 0 )
    , m_IsMouseInClientArea( false )
{
    m_RenderTarget = std::dynamic_pointer_cast<RenderTargetDX12>( device->CreateRenderTarget() );

    ZeroMemory( m_FenceValues, sizeof( m_FenceValues ) );

    m_FenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );

    m_d3d12Device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_d3d12Fence ) );
    m_FenceValues[m_CurrentFrameIndex]++;

    // Initialize MicroProfile for D3D12.
    {
        ID3D12Device* pD3D12Device = device->GetD3D12Device().Get();
        std::shared_ptr<GraphicsCommandQueueDX12> commandQueue = std::dynamic_pointer_cast<GraphicsCommandQueueDX12>( device->GetGraphicsQueue() );
        ID3D12CommandQueue* pD3D12CommandQueue = commandQueue->GetD3D12CommandQueue().Get();
    }

    CreateSwapChain();
}

WindowDX12::~WindowDX12()
{
    // Wait for the GPU to finish the current frame.
    WaitForGPU();

    CloseHandle( m_FenceEvent );

    if ( FAILED( m_dxgiSwapChain->SetFullscreenState( FALSE, nullptr ) ) )
    {
        LOG_ERROR( "Failed to set swap chain fullscreen state." );
    }
}

void WindowDX12::CreateSwapChain()
{
    UINT windowWidth = GetWindowWidth();
    UINT windowHeight = GetWindowHeight();
    bool windowed = !IsFullScreen();

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    if ( FAILED( CreateDXGIFactory2( createFactoryFlags, IID_PPV_ARGS( &dxgiFactory ) ) ) )
    {
        LOG_ERROR( "Failed to create DXGIFactory." );
    }
    else
    {
        std::shared_ptr<DeviceDX12> device = m_Device.lock();
        std::shared_ptr<GraphicsCommandQueueDX12> commandQueue = m_Queue.lock();

        if ( device && commandQueue )
        {
            // Make sure the GPU is finished any commands before (re)creating the swap chain.
            device->IdleGPU();

            Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device = device->GetD3D12Device();

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

            swapChainDesc.Width = windowWidth;
            swapChainDesc.Height = windowHeight;
            swapChainDesc.Format = m_dxgiColorFormat;
            swapChainDesc.Stereo = FALSE;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = FrameCount;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.SampleDesc = { 1, 0 };
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.Flags = 0; 

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
            swapChainFullScreenDesc.Windowed = windowed ? TRUE : FALSE;
            swapChainFullScreenDesc.RefreshRate = { 0, 1 };

            Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue = commandQueue->GetD3D12CommandQueue();
            assert( d3d12CommandQueue );

            Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
            if ( FAILED( dxgiFactory->CreateSwapChainForHwnd( d3d12CommandQueue.Get(), m_hWindow, &swapChainDesc, &swapChainFullScreenDesc, nullptr, &dxgiSwapChain1 ) ) )
            {
                LOG_ERROR( "Failed to create swap chain." );
            }
            else
            {
                if ( FAILED( dxgiFactory->MakeWindowAssociation( m_hWindow, DXGI_MWA_NO_ALT_ENTER ) ) )
                {
                    LOG_ERROR( "Failed to make window association." );
                }

                if ( FAILED( dxgiSwapChain1.As( &m_dxgiSwapChain ) ) )
                {
                    LOG_ERROR( "Failed to query IDXGISwapChain3 interface." );
                }
                else
                {
                    m_BackBufferResources.clear();

                    // Query the back buffers of the swap chain.
                    for ( int i = 0; i < FrameCount; ++i )
                    {
                        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
                        if ( FAILED( m_dxgiSwapChain->GetBuffer( i, IID_PPV_ARGS( &d3d12Resource ) ) ) )
                        {
                            LOG_ERROR( "Failed to retrieve swap chain back buffer." );
                            break;
                        }
                        else
                        {
                            std::shared_ptr<TextureDX12> textureDX12 = std::make_shared<TextureDX12>( device, d3d12Resource, TextureDimension::Texture2D );
                            m_BackBufferResources.push_back( textureDX12 );
                        }
                    }

                    // Create color buffer
                    m_ColorBuffer = device->CreateTexture2D( windowWidth, windowHeight, 1, m_ColorFormat );
                    m_RenderTarget->AttachTexture( AttachmentPoint::Color0, m_ColorBuffer );

                    // Create depth/stencil buffer
                    m_DepthBuffer = device->CreateTexture2D( windowWidth, windowHeight, 1, m_DepthStencilFormat );
                    m_RenderTarget->AttachTexture( AttachmentPoint::DepthStencil, m_DepthBuffer );

                    m_CurrentFrameIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();
                }
            }
        }
        else
        {
            LOG_ERROR( "Failed to retrieve shared pointer to either DeviceDX12 or GraphicsCommandQueueDX12." );
        }
    }
}

void WindowDX12::ShowWindow()
{
    super::ShowWindow();

    ::ShowWindow( m_hWindow, SW_SHOWDEFAULT );
    // Make sure its the top-level window.
    ::BringWindowToTop( m_hWindow );

}

void WindowDX12::HideWindow()
{
    super::HideWindow();

    ::ShowWindow( m_hWindow, SW_HIDE );
}

void WindowDX12::CloseWindow()
{
    super::CloseWindow();

    ::DestroyWindow( m_hWindow );
}

void WindowDX12::SetFullScreen( bool fullscreen )
{
    if ( m_dxgiSwapChain ) 
    {
        BOOL fullScreenState;
        if ( FAILED( m_dxgiSwapChain->GetFullscreenState( &fullScreenState, nullptr ) ) )
        {
            LOG_ERROR( "Failed to query fullscreen state." );
        }
        else if ( fullscreen != ( fullScreenState == TRUE ) ? true : false )
        {
            if ( FAILED( m_dxgiSwapChain->SetFullscreenState( fullscreen ? TRUE : FALSE, nullptr ) ) )
            {
                LOG_WARNING( "Failed to transition to fullscreen state." );
            }
            else
            {
                m_ResizeSwapChain = true;
                super::SetFullScreen( fullscreen );
            }
        }
    }
}

void WindowDX12::SetColorFormat( const TextureFormat& colorFormat )
{
    std::shared_ptr<DeviceDX12> device = m_Device.lock();

    if ( device && m_ColorFormat != colorFormat )
    {
        m_ColorFormat = colorFormat;
        DXGI_FORMAT dxgiColorFormat = ConvertTextureFormat( m_ColorFormat );
        if ( m_dxgiColorFormat != dxgiColorFormat )
        {
            // If the color format changes, we need to recreate the swap chain.
            // Doing this will also recreate the render target's color buffer so we 
            // don't need to do that explicitly.
            m_dxgiColorFormat = dxgiColorFormat;
            CreateSwapChain();
        }
        else
        {
            // If the formats are the same (for example, if only the number of samples change)
            // then only the color buffer needs to be recreated.
            m_ColorBuffer = device->CreateTexture2D( GetWindowWidth(), GetWindowHeight(), 1, m_ColorFormat );
            m_RenderTarget->AttachTexture( AttachmentPoint::Color0, m_ColorBuffer );
        }
    }
}

void WindowDX12::SetDepthStencilFormat( const TextureFormat& depthStencilFormat )
{
    std::shared_ptr<DeviceDX12> device = m_Device.lock();

    if ( device && m_DepthStencilFormat != depthStencilFormat )
    {
        m_DepthStencilFormat = depthStencilFormat;
        m_DepthBuffer = device->CreateTexture2D( GetWindowWidth(), GetWindowHeight(), 1, m_DepthStencilFormat );

        switch ( m_DepthStencilFormat.Components )
        {
        case TextureComponents::Depth:
            m_RenderTarget->AttachTexture( AttachmentPoint::Depth, m_DepthBuffer );
            m_RenderTarget->AttachTexture( AttachmentPoint::DepthStencil, nullptr );
            break;
        case TextureComponents::DepthStencil:
            m_RenderTarget->AttachTexture( AttachmentPoint::Depth, nullptr );
            m_RenderTarget->AttachTexture( AttachmentPoint::DepthStencil, m_DepthBuffer );
            break;
        default:
            LOG_ERROR( "Invalid texture type for a depth/stencil buffer." );
            break;
        }
    }
}

std::shared_ptr<RenderTarget> WindowDX12::GetRenderTarget() const
{
    return m_RenderTarget;
}

void WindowDX12::WaitForGPU()
{
    if ( std::shared_ptr<GraphicsCommandQueueDX12> queue = m_Queue.lock() )
    {
        ComPtr<ID3D12CommandQueue> d3d12CommandQueue = queue->GetD3D12CommandQueue();

        d3d12CommandQueue->Signal( m_d3d12Fence.Get(), m_FenceValues[m_CurrentFrameIndex] );
        m_d3d12Fence->SetEventOnCompletion( m_FenceValues[m_CurrentFrameIndex], m_FenceEvent );
        WaitForSingleObjectEx( m_FenceEvent, INFINITE, FALSE );

        m_FenceValues[m_CurrentFrameIndex]++;
    }
}

void WindowDX12::Present()
{
    if ( std::shared_ptr<GraphicsCommandQueueDX12> queue = m_Queue.lock() )
    {
        CPU_MARKER( __FUNCTION__ );

        std::shared_ptr<TextureDX12> backbuffer = m_BackBufferResources[m_CurrentFrameIndex];
        {
            std::shared_ptr<GraphicsCommandBuffer> commandBuffer = queue->GetGraphicsCommandBuffer();

            if ( m_ColorBuffer->GetTextureFormat().NumSamples > 1 )
            {
                GPU_MARKER( "Resolve back buffer", commandBuffer );
                commandBuffer->ResolveMultisampleTexture( backbuffer, 0, 0, m_ColorBuffer );
            }
            else
            {
                GPU_MARKER( "Copy back buffer", commandBuffer );
                commandBuffer->CopyResource( backbuffer, m_ColorBuffer );
            }

            // Transition the back buffer to present state.
            commandBuffer->TransitionResoure( backbuffer, ResourceState::Present );

            queue->Submit( commandBuffer );
        }

        if ( IsVSync() )
        {
            CPU_MARKER( "IDXGISwapChain::Present( 1 )" );
            m_dxgiSwapChain->Present( 1, 0 );
        }
        else
        {
            CPU_MARKER( "IDXGISwapChain::Present( 0 )" );
            m_dxgiSwapChain->Present( 0, 0 );
        }

        // Advance a frame.
        {
            CPU_MARKER( "Advance to the next frame." );

            ComPtr<ID3D12CommandQueue> d3d12CommandQueue = queue->GetD3D12CommandQueue();

            UINT64 currentFenceValue = m_FenceValues[m_CurrentFrameIndex];
            d3d12CommandQueue->Signal( m_d3d12Fence.Get(), currentFenceValue );

            m_CurrentFrameIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

            {
                CPU_MARKER( "Wait for avaialble frame." );
                if ( m_d3d12Fence->GetCompletedValue() < m_FenceValues[m_CurrentFrameIndex] )
                {
                    m_d3d12Fence->SetEventOnCompletion( m_FenceValues[m_CurrentFrameIndex], m_FenceEvent );
                    WaitForSingleObjectEx( m_FenceEvent, INFINITE, FALSE );
                }
            }

            // Update the fence value for the next frame.
            m_FenceValues[m_CurrentFrameIndex] = currentFenceValue + 1;
        }

        Profiler::Get().UpdateQueryResults( queue );
    }
}

void WindowDX12::ResizeSwapChainBuffers( uint32_t width, uint32_t height )
{
    CPU_MARKER( __FUNCTION__ );

    if ( std::shared_ptr<DeviceDX12> device = m_Device.lock() )
    {
        Core::LogManager::LogInfo( "Resizing swap chain buffers: ", width, "x", height );

        // Idle the GPU to ensure all commands have completed.
        device->IdleGPU();

        m_BackBufferResources.clear();

        DXGI_SWAP_CHAIN_DESC desc = {};
        if ( FAILED( m_dxgiSwapChain->GetDesc( &desc ) ) )
        {
            LOG_ERROR( "Failed to query swap chain description." );
            return;
        }

        if ( FAILED( m_dxgiSwapChain->ResizeBuffers( FrameCount, width, height, desc.BufferDesc.Format, desc.Flags ) ) )
        {
            LOG_ERROR( "Failed to resieze swap chain buffers." );
        }
        else
        {
            // Query the back buffers of the swap chain.
            for ( int i = 0; i < FrameCount; ++i )
            {
                Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
                if ( FAILED( m_dxgiSwapChain->GetBuffer( i, IID_PPV_ARGS( &d3d12Resource ) ) ) )
                {
                    LOG_ERROR( "Failed to retrieve swap chain back buffer." );
                    break;
                }
                else
                {
                    std::shared_ptr<TextureDX12> textureDX12 = std::make_shared<TextureDX12>( device, d3d12Resource, TextureDimension::Texture2D );
                    m_BackBufferResources.push_back( textureDX12 );
                }
            }

            m_RenderTarget->Resize( width, height );

            m_CurrentFrameIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();
            m_FenceValues[m_CurrentFrameIndex] = m_d3d12Fence->GetCompletedValue() + 1;
        }
    }
}

void WindowDX12::OnPreRender( Core::RenderEventArgs& e )
{
    CPU_MARKER( __FUNCTION__ );
    if ( m_ResizeEventArgs.Width != GetWindowWidth() || m_ResizeEventArgs.Height != GetWindowHeight() )
    {
        m_ResizeSwapChain = true;
        super::OnResize( m_ResizeEventArgs );
    }

    // When toggling fullscreen state the swap chain buffers need to be resized even
    // if the actual window dimensions did not change.
    // This can happen if the application window is maximized then fullscreen state is toggled.
    // In this case, the screen resolution may not change but the swap chain buffers still need to
    // be resized when switching between fullscreen states.
    // @see WindowDX12::SetFullscreen.
    if ( m_ResizeSwapChain )
    {
        ResizeSwapChainBuffers( GetWindowWidth(), GetWindowHeight() );
        m_ResizeSwapChain = false;
    }

    super::OnPreRender( e );
}

void WindowDX12::OnUpdate( Core::UpdateEventArgs& e )
{
    if ( !m_IsMouseInClientArea )
    {
        // Prevent ImGui from capturing the mouse
        // when the mouse cursor leaves the client area.
        ImGui::CaptureMouseFromApp( false );
    }
    super::OnUpdate( e );
}

void WindowDX12::OnResize( Core::ResizeEventArgs& e )
{
    static bool firstTime = true;

    m_ResizeEventArgs.Width = std::max( e.Width, 1 );
    m_ResizeEventArgs.Height = std::max( e.Height, 1 );
    // Force resizing of swap chain if the resize event was triggered because
    // of a fullscreen transition (that may not actually change the size of the buffers).
    // DX12 requires the swap chains to be resized when transitioning from Windowed/Fullscreen.
    // TODO: Use borderless fullscreen windows instead of this fullscreen crap!
    m_ResizeSwapChain = true;

    // A window will leave fullscreen when Alt+TAB is pressed on the keyboard.
    // When this happens, the application's fullscreen state will be out-of-sync
    // with the fullscreen state of the swap chain.
    // For this reason, we need to query the fullscreen state of the swapchain 
    // for every resize event.
    if (m_dxgiSwapChain)
    {
        BOOL fullScreenState;
        if (FAILED(m_dxgiSwapChain->GetFullscreenState(&fullScreenState, nullptr)))
        {
            LOG_ERROR("Failed to query fullscreen state.");
        }
        else
        {
            super::SetFullScreen( fullScreenState == TRUE );
        }
    }

    // If this is the first time this function is entered,
    // the swap chain needs to be forcefully resized to ensure it
    // is the right size for rendering the first frame.
    // This way, the application can react immediately to this resize 
    // event. In all other cases, we only want to call resize during the
    // PreRender function.
    // @see OnPreRender
    if ( firstTime )
    {
        super::OnResize( e );
        firstTime = false;
    }
}

void WindowDX12::OnMouseMoved( Core::MouseMotionEventArgs& e )
{
    if ( !m_IsMouseInClientArea )
    {
        m_IsMouseInClientArea = true;

        TRACKMOUSEEVENT trackMouseEvent = {};
        trackMouseEvent.cbSize = sizeof( TRACKMOUSEEVENT );
        trackMouseEvent.hwndTrack = m_hWindow;
        trackMouseEvent.dwFlags = TME_LEAVE;
        TrackMouseEvent( &trackMouseEvent );
    }

    super::OnMouseMoved( e );
}

void WindowDX12::OnMouseLeave( Core::EventArgs& e )
{
    super::OnMouseLeave( e );
    m_IsMouseInClientArea = false;
}
