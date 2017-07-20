#include <EnginePCH.h>

#include <Graphics/DX12/DepthStencilStateDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_DEPTH_WRITE_MASK TranslateDepthWriteMask( DepthWrite depthWrite )
{
    D3D12_DEPTH_WRITE_MASK result = D3D12_DEPTH_WRITE_MASK_ALL;

    switch ( depthWrite )
    {
    case DepthWrite::Enable:
        result = D3D12_DEPTH_WRITE_MASK_ALL;
        break;
    case DepthWrite::Disable:
        result = D3D12_DEPTH_WRITE_MASK_ZERO;
        break;
    }

    return result;
}

D3D12_COMPARISON_FUNC TranslateCompareFunc( CompareFunction compareFunc )
{
    D3D12_COMPARISON_FUNC result = D3D12_COMPARISON_FUNC_LESS;

    switch ( compareFunc )
    {
    case CompareFunction::Never:
        result = D3D12_COMPARISON_FUNC_NEVER;
        break;
    case CompareFunction::Less:
        result = D3D12_COMPARISON_FUNC_LESS;
        break;
    case CompareFunction::Equal:
        result = D3D12_COMPARISON_FUNC_EQUAL;
        break;
    case CompareFunction::LessOrEqual:
        result = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    case CompareFunction::Greater:
        result = D3D12_COMPARISON_FUNC_GREATER;
        break;
    case CompareFunction::NotEqual:
        result = D3D12_COMPARISON_FUNC_NOT_EQUAL;
        break;
    case CompareFunction::GreaterOrEqual:
        result = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        break;
    case CompareFunction::Always:
        result = D3D12_COMPARISON_FUNC_ALWAYS;
        break;
    }

    return result;
}

D3D12_STENCIL_OP TranslateStencilOperation( StencilOperation stencilOperation )
{
    D3D12_STENCIL_OP result = D3D12_STENCIL_OP_KEEP;

    switch ( stencilOperation )
    {
    case StencilOperation::Keep:
        result = D3D12_STENCIL_OP_KEEP;
        break;
    case StencilOperation::Zero:
        result = D3D12_STENCIL_OP_ZERO;
        break;
    case StencilOperation::Reference:
        result = D3D12_STENCIL_OP_REPLACE;
        break;
    case StencilOperation::IncrementClamp:
        result = D3D12_STENCIL_OP_INCR_SAT;
        break;
    case StencilOperation::DecrementClamp:
        result = D3D12_STENCIL_OP_DECR_SAT;
        break;
    case StencilOperation::Invert:
        result = D3D12_STENCIL_OP_INVERT;
        break;
    case StencilOperation::IncrementWrap:
        result = D3D12_STENCIL_OP_INCR;
        break;
    case StencilOperation::DecrementWrap:
        result = D3D12_STENCIL_OP_DECR;
        break;
    }

    return result;
}

D3D12_DEPTH_STENCILOP_DESC TranslateFaceOperation( FaceOperation faceOperation )
{
    D3D12_DEPTH_STENCILOP_DESC result;

    result.StencilFailOp = TranslateStencilOperation( faceOperation.StencilFail );
    result.StencilDepthFailOp = TranslateStencilOperation( faceOperation.StencilPassDepthFail );
    result.StencilPassOp = TranslateStencilOperation( faceOperation.StencilDepthPass );
    result.StencilFunc = TranslateCompareFunc( faceOperation.StencilFunction );

    return result;
}

DepthStencilStateDX12::DepthStencilStateDX12()
    : m_IsDirty( true )
{

}

DepthStencilStateDX12::~DepthStencilStateDX12()
{}

void DepthStencilStateDX12::SetDepthMode( const DepthMode& depthMode )
{
    m_DepthMode = depthMode;

    m_IsDirty = true;
}

const DepthMode& DepthStencilStateDX12::GetDepthMode() const
{
    return m_DepthMode;
}

void DepthStencilStateDX12::SetStencilMode( const StencilMode& stencilMode )
{
    m_StencilMode = stencilMode;

    m_IsDirty = true;
}

const StencilMode& DepthStencilStateDX12::GetStencilMode() const
{
    return m_StencilMode;
}

bool DepthStencilStateDX12::IsDirty() const
{
    return m_IsDirty;
}

D3D12_DEPTH_STENCIL_DESC DepthStencilStateDX12::GetD3D12DepthStencilDesc()
{
    if ( m_IsDirty )
    {
        m_d3d12DepthStencilDesc.DepthEnable = m_DepthMode.DepthEnable;
        m_d3d12DepthStencilDesc.DepthWriteMask = TranslateDepthWriteMask( m_DepthMode.DepthWriteMask );
        m_d3d12DepthStencilDesc.DepthFunc = TranslateCompareFunc( m_DepthMode.DepthFunction );
        m_d3d12DepthStencilDesc.StencilEnable = m_StencilMode.StencilEnabled;
        m_d3d12DepthStencilDesc.StencilReadMask = m_StencilMode.ReadMask;
        m_d3d12DepthStencilDesc.StencilWriteMask = m_StencilMode.WriteMask;
        m_d3d12DepthStencilDesc.FrontFace = TranslateFaceOperation( m_StencilMode.FrontFace );
        m_d3d12DepthStencilDesc.BackFace = TranslateFaceOperation( m_StencilMode.BackFace );

        m_IsDirty = false;
    }

    return m_d3d12DepthStencilDesc;
}

void DepthStencilStateDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = commandBuffer->GetD3D12CommandList();

    d3d12CommandList->OMSetStencilRef( m_StencilMode.StencilReference );
}
