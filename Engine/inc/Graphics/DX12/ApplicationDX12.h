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
 *  @file ApplicationDX12.h
 *  @date February 17, 2016
 *  @author jeremiah
 *
 *  @brief DirectX 12 Application implementation.
 */

#include "../../Application.h"

#include <atomic>
#include <condition_variable>

namespace Graphics
{
    class AdapterDXGI;

    class ENGINE_DLL ApplicationDX12 : public Core::Application
    {
    public:
        using base = Application;

        ApplicationDX12();
        virtual ~ApplicationDX12();

        /**
         * Run the application.
         */
        virtual int32_t Run() override;

        /**
         * Stop the application.
         */
        virtual void Stop() override;

        /**
         * Query the GraphicsAdapters that are currently available.
         */
        virtual const AdapterList& GetAdapters() const override;

        /**
         * Get the WARP adapter.
         */
        virtual std::shared_ptr<Graphics::Adapter> GetWarpAdapter() const override;

        /**
        * Create a render device using the specified adapter.
        * @param adapter Specify the graphics adapter to create the render device.
        * Specify a nullptr adapter to create a render device using the default adapter.
        *
        * @return A valid render device if device creation succeeded. Otherwise a
        * nullptr device will be returned.
        */
        virtual std::shared_ptr<Graphics::Device> CreateDevice( std::shared_ptr<Graphics::Adapter> adapter = nullptr ) override;

        /**
         * Create a render window.
         */
        virtual std::shared_ptr<Graphics::Window> CreateWindow( std::shared_ptr<Graphics::Device> device,
                                                                const std::string& title,
                                                                uint32_t windowWidth, uint32_t windowHeight,
                                                                bool fullscreen = false, bool vsync = true,
                                                                const Graphics::TextureFormat& colorFormat = Graphics::TextureFormat::R8G8B8A8_UNORM,
                                                                const Graphics::TextureFormat& depthStencilFormat = Graphics::TextureFormat::D24_UNORM_S8_UINT ) override;

        /**
        * For programmatic graphics analysis.
        * Begin capturing a frame for graphics analysis.
        */
        virtual void BeginGraphicsAnalysis() override;

        /**
        * End capturing a frame for graphics analysis.
        */
        virtual void EndGraphicsAnalysis() override;

    protected:

        virtual void OnRender( Core::RenderEventArgs& e ) override;

        // The user requested to exit the application
        virtual void OnExit( Core::EventArgs& e ) override;

        void ProcessJoysticks( double deltaTime );
        void ProcessMessages();

        void UpdateThread();

    private:
        // Handle to the module.
        HINSTANCE m_hInstance;

        std::atomic_bool m_bIsRunning;
        bool m_RequestQuit;

        std::atomic_uint64_t m_UpdateFrame;
        std::atomic_uint64_t m_RenderFrame;

        std::mutex m_UpdateMutex;
        std::condition_variable m_UpdateConditionVar;
        
        std::mutex m_MessageQueueMutex;
        std::condition_variable m_MessageQueueConditionVar;

        mutable std::vector< std::shared_ptr<Graphics::Adapter> > m_Adapters;
    };
}