#pragma once

#include "../Adapter.h"

namespace Graphics
{
    class AdapterDXGI : public Adapter
    {
    public:
        AdapterDXGI( Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter );
        virtual ~AdapterDXGI();

        /**
        * Get the name of the physical adapter.
        * In DirectX, this is the description of the adapter.
        */
        virtual const std::wstring& GetDescription() const override;

        /**
        * Get the adapter type.
        * @see AdapterType
        */
        virtual AdapterType GetType() const override;

        /**
        * Get the vendor ID of the GPU.
        */
        virtual uint32_t GetVendorID() const override;

        /**
        * Get the device ID of the GPU.
        */
        virtual uint32_t GetDeviceID() const override;

        /**
        * Get the amount of dedicated video memory in bytes.
        */
        virtual uint64_t GetDedicatedVideoMemory() const override;

        /**
        * Get the amount of dedicated system memory in bytes.
        */
        virtual uint64_t GetDedicatedSystemMemory() const override;

        /**
        * Get the amount of shared system memory in bytes.
        */
        virtual uint64_t GetSharedSystemMemory() const override;

        /**
        * Query the video memory usage for this adapter.
        * @param videoMemorySegment Which segment of video memory to query.
        */
        virtual VideoMemoryUsage QueryVideoMemoryUsage( VideoMemorySegment videoMemorySegment ) const override;

        /**
        * Specify to the OS the minimum amount of video memory (in bytes) that this application must
        * have reserved. Use Adapter::QueryVideoMemoryUsage to determine amount of video
        * memory that is available for reservation.
        */
        virtual void SetVideoMemoryReservation( VideoMemorySegment videoMemorySegment, uint64_t bytes ) override;

        /**
        * Get a list of the displays connected to the physical adapter.
        * This could be empty if no display is connected to the adapter or it
        * is a headless GPU.
        */
        virtual const GraphicsDisplayList& GetDisplays() const override;

        /**
         * Get the DXGI adapter for this graphics adapter.
         */
        Microsoft::WRL::ComPtr<IDXGIAdapter3> GetDXGIAdapter() const;

    protected:

    private:
        Microsoft::WRL::ComPtr<IDXGIAdapter3> m_dxgiAdapter;
        DXGI_ADAPTER_DESC2 m_AdapterDescription;

        // The displays connected to this adapter.
        GraphicsDisplayList m_Displays;
        std::wstring m_Description;
        AdapterType m_Type;
    };
}