#include <EnginePCH.h>

#include <Graphics/DX12/BufferDX12.h>
#include <Graphics/DX12/ResourceDX12.h>
#include <Graphics/DX12/ApplicationDX12.h>
#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

BufferDX12::BufferDX12( std::shared_ptr<DeviceDX12> device )
    : ResourceDX12( device )
{
}

BufferDX12::~BufferDX12()
{
}
