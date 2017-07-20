#include <EnginePCH.h>

#include <Graphics/DX12/IndirectCommandSignatureDX12.h>
#include <Graphics/DX12/DeviceDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_INDIRECT_ARGUMENT_TYPE TranslateIndirectArgumentType( IndirectArgumentType indirectArgumentType )
{
    D3D12_INDIRECT_ARGUMENT_TYPE d3d12IndirectArgumentType;
    switch ( indirectArgumentType )
    {
    case IndirectArgumentType::Draw:
        d3d12IndirectArgumentType = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
        break;
    case IndirectArgumentType::DrawIndexed:
        d3d12IndirectArgumentType = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        break;
    case IndirectArgumentType::Dispatch:
        d3d12IndirectArgumentType = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
        break;
    default:
        LOG_ERROR( "Invalid Indirect Argument Type." );
        break;
    }

    return d3d12IndirectArgumentType;
}

std::vector<D3D12_INDIRECT_ARGUMENT_DESC> TranslateIndirectArguments( const IndirectArguments& indirectArguments )
{
    std::vector<D3D12_INDIRECT_ARGUMENT_DESC> d3d12IndirectArguments( indirectArguments.size() );

    uint32_t i = 0;
    for ( const auto& indirectArgument : indirectArguments )
    {
        D3D12_INDIRECT_ARGUMENT_DESC d3d12ArgumentDesc = {};
        d3d12ArgumentDesc.Type = TranslateIndirectArgumentType( indirectArgument.GetArgumentType() );

        // TODO: Support for more indirect argument types.

        d3d12IndirectArguments[i++] = d3d12ArgumentDesc;
    }

    return std::move( d3d12IndirectArguments );
}

// Compute the byte stride of the argument buffer given a set of indirect arguments.
UINT ComputeByteStride( const std::vector<D3D12_INDIRECT_ARGUMENT_DESC>& indirectArguments )
{
    UINT byteStride = 0;

    for ( const auto& indirectArgument : indirectArguments )
    {
        switch ( indirectArgument.Type )
        {
        case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW:
            byteStride += sizeof( DrawIndirectArgument );
            break;
        case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED:
            byteStride += sizeof( DrawIndexedIndirectArgument );
            break;
        case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH:
            byteStride += sizeof( DispatchIndirectArgument );
            break;
        default:
            LOG_ERROR( "Unsupported argument type." );
            break;
        }
    }

    return byteStride;
}

IndirectCommandSignatureDX12::IndirectCommandSignatureDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_ByteStide( 0 )
    , m_IsDirty( true )
{}

IndirectCommandSignatureDX12::~IndirectCommandSignatureDX12()
{}

void IndirectCommandSignatureDX12::SetByteStride( size_t byteStride )
{
    m_ByteStide = static_cast<UINT>( byteStride );
    m_IsDirty = true;
}

size_t IndirectCommandSignatureDX12::GetByteStride() const
{
    return m_ByteStide;
}

void IndirectCommandSignatureDX12::AppendCommandArgument( const IndirectArgument& indirectArgument )
{
    m_IndirectArguments.push_back( indirectArgument );
    m_IsDirty = true;
}

const IndirectArguments& IndirectCommandSignatureDX12::GetCommandArguments() const
{
    return m_IndirectArguments;
}

Microsoft::WRL::ComPtr<ID3D12CommandSignature> IndirectCommandSignatureDX12::GetD3D12CommandSignature()
{
    if ( m_IsDirty )
    {
        std::vector<D3D12_INDIRECT_ARGUMENT_DESC> argumentDesc = TranslateIndirectArguments( m_IndirectArguments );

        D3D12_COMMAND_SIGNATURE_DESC d3d12CommandSignatureDesc = {};

        d3d12CommandSignatureDesc.NodeMask = 0;
        d3d12CommandSignatureDesc.ByteStride = ( m_ByteStide > 0 ) ? m_ByteStide : ComputeByteStride( argumentDesc );
        d3d12CommandSignatureDesc.NumArgumentDescs = static_cast<UINT>( argumentDesc.size() );
        d3d12CommandSignatureDesc.pArgumentDescs = argumentDesc.data();

        if ( FAILED( m_d3d12Device->CreateCommandSignature( &d3d12CommandSignatureDesc, nullptr, IID_PPV_ARGS( &m_d3d12CommandSignature ) ) ) )
        {
            LOG_ERROR( "Failed to create command signature." );
        }

        m_IsDirty = false;
    }

    return m_d3d12CommandSignature;
}