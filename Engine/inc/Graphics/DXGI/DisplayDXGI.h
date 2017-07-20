#pragma once

#include "../Display.h"

namespace Graphics
{
    class AdapterDXGI;

    class DisplayDXGI : public Display
    {
    public:
        DisplayDXGI( Microsoft::WRL::ComPtr<IDXGIOutput4> output );
        virtual ~DisplayDXGI();

        /**
        * A name that describes the display device.
        */
        virtual const std::wstring& GetName() const override;

        /**
        * Get the desktop coordinates of this display device.
        */
        virtual const Rect& GetDesktopCoordinates() const override;

        /**
        * Get the display rotation.
        */
        virtual DisplayRotation GetRotation() const override;

        /**
        * Get the display modes supported by this display.
        */
        virtual DisplayModeList GetDisplayModes( const TextureFormat& textureFormat ) const override;

        /**
         * Get a pointer to the underlying DXGIOutput.
         */
        Microsoft::WRL::ComPtr<IDXGIOutput4> GetDXGIOutput() const;
    protected:

    private:
        Microsoft::WRL::ComPtr<IDXGIOutput4> m_dxgiOutput;
        DXGI_OUTPUT_DESC  m_dxgiOutputDescription;
        std::wstring m_DisplayName;
        Rect m_DesktopCoordinates;
        DisplayRotation m_Rotation;
    };
}