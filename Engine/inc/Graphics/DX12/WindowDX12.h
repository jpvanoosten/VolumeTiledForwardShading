#pragma once

#include "../Window.h"
#include "../TextureFormat.h"

namespace Graphics
{
    class DeviceDX12;
    class GraphicsCommandQueueDX12;
    class Texture;
    class TextureDX12;
    class RenderTargetDX12;

    class ENGINE_DLL WindowDX12 : public Window
    {
    public:
        using super = Window;

        WindowDX12( Core::Application& app, std::shared_ptr<DeviceDX12> device, HWND hWnd,
                    const std::string& windowTitle, 
                    uint32_t width, uint32_t height, 
                    bool fullscreen, bool vSync, 
                    const Graphics::TextureFormat& colorFormat,
                    const Graphics::TextureFormat& depthStencilFormat );

        virtual ~WindowDX12();

        // Show this window if it is hidden.
        virtual void ShowWindow() override;
        // Hide the window. The window will not be destroyed and can be 
        // shown again using the ShowWindow() function.
        virtual void HideWindow() override;

        // Destroy and close the window.
        virtual void CloseWindow() override;

        virtual void SetFullScreen( bool fullscreen ) override;

        /**
        * Set the texture format of the color buffer associated to this window's
        * render target
        */
        virtual void SetColorFormat( const TextureFormat& colorFormat ) override;

        /**
        * Set the texture format of the depth/stencil buffer associated to this
        * windows render target.
        */
        virtual void SetDepthStencilFormat( const TextureFormat& depthStencilFormat ) override;


        virtual std::shared_ptr<RenderTarget> GetRenderTarget() const override;

        // Present the back buffers
        virtual void Present() override;

        void WaitForGPU();

    protected:
        friend class ApplicationDX12;

        // The application window has been resized
        virtual void OnUpdate( Core::UpdateEventArgs& e ) override;
        virtual void OnResize( Core::ResizeEventArgs& e ) override;
        virtual void OnPreRender( Core::RenderEventArgs& e ) override;

        virtual void OnMouseMoved( Core::MouseMotionEventArgs& e ) override;
        virtual void OnMouseLeave( Core::EventArgs& e ) override;

        void CreateSwapChain();
        void ResizeSwapChainBuffers( uint32_t width, uint32_t height );

    private:
        static const uint8_t FrameCount = 2;

        using BackBufferList = std::vector< std::shared_ptr<TextureDX12> >;

        HWND m_hWindow;

        std::weak_ptr<DeviceDX12> m_Device;
        std::weak_ptr<GraphicsCommandQueueDX12> m_Queue;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_dxgiSwapChain;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;
        UINT64 m_FenceValues[FrameCount];
        HANDLE m_FenceEvent;

        TextureFormat m_ColorFormat;
        TextureFormat m_DepthStencilFormat;
        DXGI_FORMAT m_dxgiColorFormat;

        std::shared_ptr<Texture> m_ColorBuffer;
        std::shared_ptr<Texture> m_DepthBuffer;

        BackBufferList m_BackBufferResources;

        Core::ResizeEventArgs m_ResizeEventArgs;
        bool m_ResizeSwapChain;

        std::shared_ptr<RenderTargetDX12> m_RenderTarget;

        uint32_t m_CurrentFrameIndex;
        bool m_IsMouseInClientArea;
    };
}