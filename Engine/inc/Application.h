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
 *  @file ApplicationWin.h
 *  @date December 21, 2015
 *  @author Jeremiah
 *
 *  @brief The Application class is the entry point for the game.
 */

#include "Object.h"
#include "Common.h"
#include "Events.h"
#include "ReadDirectoryChanges.h"
#include "Graphics/TextureFormat.h"

#if defined(CreateWindow)
#undef CreateWindow
#endif

namespace Graphics
{
    class Window;
    class Device;
    class Adapter;

    using AdapterList = std::vector< std::shared_ptr<Graphics::Adapter> >;
}

namespace Core
{
    class ENGINE_DLL Application : public Object
    {
    public:
        using base = Object;

        /**
         * Get the singleton instance of the application class.
         */
        static Application& Get();

        /**
         * Fixed timestep.
         */
        double GetFixedTimestep() const
        {
            return m_FixedTimestep;
        }

        void SetFixedTimestep( double fixedTimestep )
        {
            m_FixedTimestep = fixedTimestep;
        }

        const std::vector<fs::path>& GetAssetSerachPaths() const
        {
            return m_AssetSearchPaths;
        }

        void SetAssetSearchPaths( const std::vector<std::wstring>& assetSearchPaths )
        {
            m_AssetSearchPaths.clear();
            for ( fs::path path : assetSearchPaths )
            {
                m_AssetSearchPaths.push_back(path);
            }
        }

        void SetAssetSearchPaths( const std::vector<fs::path>& assetSearchPaths )
        {
            m_AssetSearchPaths = assetSearchPaths;
        }

        void AddAssetSearchPath( const std::wstring& searchPath )
        {
            m_AssetSearchPaths.emplace_back( searchPath );
        }

        void AddAssetSearchPath( const fs::path& searchPath )
        {
            m_AssetSearchPaths.emplace_back( searchPath );
        }

        /**
         * Register a directory for receiving file change notifications.
         */
        void RegisterDirectoryChangeListener( const std::wstring& dir, bool recursive = true );

        /**
         * Run the application.
         */
        virtual int32_t Run() = 0;

        /**
         * Stop the application.
         */
        virtual void Stop() = 0;

        /**
         * Query the hardware GPU adapters that are currently available.
         */
        virtual const Graphics::AdapterList& GetAdapters() const = 0;

        /**
        * Get the WARP (software) adapter.
        */
        virtual std::shared_ptr<Graphics::Adapter> GetWarpAdapter() const = 0;

        /**
         * Create a render device using the specified adapter.
         * @param adapter Specify the graphics adapter to create the render device.
         * Specify a nullptr adapter to create a render device using the default adapter.
         *
         * @return A valid render device if device creation succeeded. Otherwise a
         * nullptr device will be returned if device creation failed.
         */
        virtual std::shared_ptr<Graphics::Device> CreateDevice( std::shared_ptr<Graphics::Adapter> adapter = nullptr ) = 0;

        /**
         * Create a render window.
         */
        virtual std::shared_ptr<Graphics::Window> CreateWindow( std::shared_ptr<Graphics::Device> device, 
                                                                const std::string& title, 
                                                                uint32_t windowWidth, uint32_t windowHeight, 
                                                                bool fullscreen = false, bool vsync = true, 
                                                                const Graphics::TextureFormat& colorFormat = Graphics::TextureFormat::R8G8B8A8_UNORM, 
                                                                const Graphics::TextureFormat& depthStencilFormat = Graphics::TextureFormat::D24_UNORM_S8_UINT ) = 0;

        /**
         * For programmatic graphics analysis.
         * Begin capturing a frame for graphics analysis.
         */
        virtual void BeginGraphicsAnalysis() = 0;

        /**
         * End capturing a frame for graphics analysis.
         */
        virtual void EndGraphicsAnalysis() = 0;

        void SetLoadingProgressTotal( float loadingProgressTotal )
        {
            std::lock_guard<std::mutex> lock( m_LoadingProgressMutex );
            m_LoadingProgressTotal = loadingProgressTotal;
        }

        void IncrementLoadingProgress( float incrementAmout = 1.0f )
        {
            std::lock_guard<std::mutex> lock( m_LoadingProgressMutex );
            m_LoadingProgress += incrementAmout;
        }

        void SetLoadingProgress( float loadingProgress = 0.0f )
        {
            std::lock_guard<std::mutex> lock( m_LoadingProgressMutex );
            m_LoadingProgress = 0.0f;
        }

        float GetLoadingProgress() const
        {
            std::lock_guard<std::mutex> lock( m_LoadingProgressMutex );
            return m_LoadingProgress;
        }

        float GetLoadingProgressRatio() const
        {
            std::lock_guard<std::mutex> lock( m_LoadingProgressMutex );
            if ( m_LoadingProgressTotal != 0.0f )
            {
                return m_LoadingProgress / m_LoadingProgressTotal;
            }

            return 0.0f;
        }

        void SetLoadingMessage( const std::string& loadingMessage )
        {
            std::lock_guard<std::mutex> lock( m_LoadingMessageMutex );

            m_LoadingMessage = ConvertString( loadingMessage );
        }

        void SetLoadingMessage( const std::wstring& loadingMessage )
        {
            std::lock_guard<std::mutex> lock( m_LoadingMessageMutex );

            m_LoadingMessage = loadingMessage;
        }

        std::wstring GetLoadingMessage() const
        {
            std::lock_guard<std::mutex> lock( m_LoadingMessageMutex );

            return m_LoadingMessage;
        }

        // Application events
        // Application execution events
        Event           Initialize;
        UpdateEvent     Update;

        // Called when the application is exiting.
        Event           Exit;

        // This event is called when the application is going to be terminated.
        Event			Terminate;

        // Joystick events.
        JoystickButtonEvent JoystickButtonPressed;
        JoystickButtonEvent JoystickButtonReleased;
        // Event is fired when the POV on the joystick changes.
        JoystickPOVEvent JoystickPOV;
        // Event is fired if any of the axes of the joystick change.
        // No thresholding is applied.
        JoystickAxisEvent JoystickAxis;

        // A file has been modified on disk.
        FileChangeEvent FileChanged;

    protected:
        Application();
        virtual ~Application();

        // Invoked when the application is initialized.
        // This is called when the application is run.
        // This allows windows that have been created to receive this event.
        virtual void OnInitialize( EventArgs& e );

        virtual void OnUpdate( UpdateEventArgs& e );
        virtual void OnRender( RenderEventArgs& e );

        // Joystick Event Callbacks
        virtual void OnJoystickButtonPressed( JoystickButtonEventArgs& e );
        virtual void OnJoystickButtonReleased( JoystickButtonEventArgs& e );
        virtual void OnJoystickPOV( JoystickPOVEventArgs& e );
        virtual void OnJoystickAxis( JoystickAxisEventArgs& e );

        // The application instance is going to be terminated.
        virtual void OnTerminate( EventArgs& e );

        // The user requested to exit the application
        virtual void OnExit( EventArgs& e );

        // A file modification was detected.
        virtual void OnFileChange( FileChangeEventArgs& e );

        double m_FixedTimestep;

    private:

        // Directory change listener thread entry point.
        void CheckFileChanges();

        // Directory change listener.
        CReadDirectoryChanges m_DirectoryChanges;
        // Thread to run directory change listener.
        std::thread m_DirectoryChangeListenerThread;
        std::mutex m_DirectoryChangeMutex;
        // Flag to terminate directory change thread.
        std::atomic<bool> m_bTerminateDirectoryChangeThread;

        float m_LoadingProgress;
        float m_LoadingProgressTotal;
        std::wstring m_LoadingMessage;

        std::vector<fs::path> m_AssetSearchPaths;

        mutable std::mutex m_LoadingMessageMutex;
        mutable std::mutex m_LoadingProgressMutex;
    };
}