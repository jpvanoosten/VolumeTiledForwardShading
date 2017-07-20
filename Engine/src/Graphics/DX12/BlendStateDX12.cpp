#include <EnginePCH.h>

#include <Graphics/DX12/BlendStateDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_BLEND TranslateBlendFactor( BlendFactor blendFactor )
{
    D3D12_BLEND result = D3D12_BLEND_ONE;

    switch ( blendFactor )
    {
    case BlendFactor::Zero:
        result = D3D12_BLEND_ZERO;
        break;
    case BlendFactor::One:
        result = D3D12_BLEND_ONE;
        break;
    case BlendFactor::SrcColor:
        result = D3D12_BLEND_SRC_COLOR;
        break;
    case BlendFactor::OneMinusSrcColor:
        result = D3D12_BLEND_INV_SRC_COLOR;
        break;
    case BlendFactor::DstColor:
        result = D3D12_BLEND_DEST_COLOR;
        break;
    case BlendFactor::OneMinusDstColor:
        result = D3D12_BLEND_INV_DEST_COLOR;
        break;
    case BlendFactor::SrcAlpha:
        result = D3D12_BLEND_SRC_ALPHA;
        break;
    case BlendFactor::OneMinusSrcAlpha:
        result = D3D12_BLEND_INV_SRC_ALPHA;
        break;
    case BlendFactor::DstAlpha:
        result = D3D12_BLEND_DEST_ALPHA;
        break;
    case BlendFactor::OneMinusDstAlpha:
        result = D3D12_BLEND_INV_DEST_ALPHA;
        break;
    case BlendFactor::SrcAlphaSat:
        result = D3D12_BLEND_SRC_ALPHA_SAT;
        break;
    case BlendFactor::ConstBlendFactor:
        result = D3D12_BLEND_BLEND_FACTOR;
        break;
    case BlendFactor::OneMinusBlendFactor:
        result = D3D12_BLEND_INV_BLEND_FACTOR;
        break;
    case BlendFactor::Src1Color:
        result = D3D12_BLEND_SRC1_COLOR;
        break;
    case BlendFactor::OneMinusSrc1Color:
        result = D3D12_BLEND_INV_SRC1_COLOR;
        break;
    case BlendFactor::Src1Alpha:
        result = D3D12_BLEND_INV_SRC1_ALPHA;
        break;
    case BlendFactor::OneMinusSrc1Alpha:
        result = D3D12_BLEND_INV_SRC1_ALPHA;
        break;
    }

    return result;
}

D3D12_BLEND_OP TranslateBlendOp( BlendOperation blendOperation )
{
    D3D12_BLEND_OP result = D3D12_BLEND_OP_ADD;
    switch ( blendOperation )
    {
    case BlendOperation::Add:
        result = D3D12_BLEND_OP_ADD;
        break;
    case BlendOperation::Subtract:
        result = D3D12_BLEND_OP_SUBTRACT;
        break;
    case BlendOperation::ReverseSubtract:
        result = D3D12_BLEND_OP_REV_SUBTRACT;
        break;
    case BlendOperation::Min:
        result = D3D12_BLEND_OP_MIN;
        break;
    case BlendOperation::Max:
        result = D3D12_BLEND_OP_MAX;
        break;
    }

    return result;
}

UINT8 TranslateWriteMask( bool red, bool green, bool blue, bool alpha )
{
    UINT8 writeMask = 0;
    if ( red )
    {
        writeMask |= D3D12_COLOR_WRITE_ENABLE_RED;
    }
    if ( green )
    {
        writeMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
    }
    if ( blue )
    {
        writeMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
    }
    if ( alpha )
    {
        writeMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
    }

    return writeMask;
}

D3D12_LOGIC_OP TranslateLogicOperator( LogicOperator logicOp )
{
    D3D12_LOGIC_OP result = D3D12_LOGIC_OP_NOOP;

    switch ( logicOp )
    {
    case LogicOperator::None:
        result = D3D12_LOGIC_OP_NOOP;
        break;
    case LogicOperator::Clear:
        result = D3D12_LOGIC_OP_CLEAR;
        break;
    case LogicOperator::Set:
        result = D3D12_LOGIC_OP_SET;
        break;
    case LogicOperator::Copy:
        result = D3D12_LOGIC_OP_SET;
        break;
    case LogicOperator::CopyInverted:
        result = D3D12_LOGIC_OP_COPY_INVERTED;
        break;
    case LogicOperator::Invert:
        result = D3D12_LOGIC_OP_INVERT;
        break;
    case LogicOperator::And:
        result = D3D12_LOGIC_OP_AND;
        break;
    case LogicOperator::Nand:
        result = D3D12_LOGIC_OP_NAND;
        break;
    case LogicOperator::Or:
        result = D3D12_LOGIC_OP_OR;
        break;
    case LogicOperator::Nor:
        result = D3D12_LOGIC_OP_NOR;
        break;
    case LogicOperator::Xor:
        result = D3D12_LOGIC_OP_XOR;
        break;
    case LogicOperator::Equiv:
        result = D3D12_LOGIC_OP_EQUIV;
        break;
    case LogicOperator::AndReverse:
        result = D3D12_LOGIC_OP_AND_REVERSE;
        break;
    case LogicOperator::AndInverted:
        result = D3D12_LOGIC_OP_AND_INVERTED;
        break;
    case LogicOperator::OrReverse:
        result = D3D12_LOGIC_OP_OR_REVERSE;
        break;
    case LogicOperator::OrInverted:
        result = D3D12_LOGIC_OP_OR_INVERTED;
        break;
    }

    return result;
}


BlendStateDX12::BlendStateDX12()
    : m_AlphaToCoverageEnable( false )
    , m_IndependentBlendEnable( false )
    , m_SampleMask( 0xffffffff )
    , m_IsDirty( true )
{
    m_BlendModes.resize( 8 );
}

BlendStateDX12::~BlendStateDX12()
{}

void BlendStateDX12::SetBlendMode( const BlendMode& blendMode )
{
    m_BlendModes[0] = blendMode;

    m_IsDirty = true;
}

void BlendStateDX12::SetBlendModes( const std::vector<BlendMode>& blendModes )
{
    m_BlendModes = blendModes;

    m_IsDirty = true;
}

const std::vector<BlendMode>& BlendStateDX12::GetBlendModes() const
{
    return m_BlendModes;
}

void BlendStateDX12::SetConstBlendFactor( const ConstantBlendFactor& constantBlendFactor )
{
    m_ConstantBlendFactor = constantBlendFactor;
}

const ConstantBlendFactor& BlendStateDX12::GetConstBlendFactor() const
{
    return m_ConstantBlendFactor;
}

void BlendStateDX12::SetSampleMask( uint32_t sampleMask )
{
    m_SampleMask = sampleMask;
}

uint32_t BlendStateDX12::GetSampleMask() const
{
    return m_SampleMask;
}

void BlendStateDX12::SetAlphaCoverage( bool enabled )
{
    m_AlphaToCoverageEnable = enabled;

    m_IsDirty = true;
}

bool BlendStateDX12::GetAlphaCoverage() const
{
    return m_AlphaToCoverageEnable;
}

void BlendStateDX12::SetIndependentBlend( bool enabled )
{
    m_IndependentBlendEnable = enabled;

    m_IsDirty = true;
}

bool BlendStateDX12::GetIndependentBlend() const
{
    return m_IndependentBlendEnable;
}

bool BlendStateDX12::IsDirty() const
{
    return m_IsDirty;
}

D3D12_BLEND_DESC BlendStateDX12::GetD3D12BlendDesc()
{
    if ( m_IsDirty )
    {
        m_d3d12BlendDesc.AlphaToCoverageEnable = m_AlphaToCoverageEnable;
        m_d3d12BlendDesc.IndependentBlendEnable = m_IndependentBlendEnable;
        size_t i = 0;
        for ( auto blendMode : m_BlendModes )
        {
            D3D12_RENDER_TARGET_BLEND_DESC& d3d12RenderTargetBlendDesc = m_d3d12BlendDesc.RenderTarget[i++];
            d3d12RenderTargetBlendDesc.BlendEnable = blendMode.BlendEnabled;
            d3d12RenderTargetBlendDesc.LogicOpEnable = blendMode.LogicOpEnabled;
            d3d12RenderTargetBlendDesc.SrcBlend = TranslateBlendFactor( blendMode.SrcFactor );
            d3d12RenderTargetBlendDesc.DestBlend = TranslateBlendFactor( blendMode.DstFactor );
            d3d12RenderTargetBlendDesc.BlendOp = TranslateBlendOp( blendMode.BlendOp );
            d3d12RenderTargetBlendDesc.SrcBlendAlpha = TranslateBlendFactor( blendMode.SrcAlphaFactor );
            d3d12RenderTargetBlendDesc.DestBlendAlpha = TranslateBlendFactor( blendMode.DstAlphaFactor );
            d3d12RenderTargetBlendDesc.BlendOpAlpha = TranslateBlendOp( blendMode.AlphaOp );
            d3d12RenderTargetBlendDesc.LogicOp = TranslateLogicOperator( blendMode.LogicOp );
            d3d12RenderTargetBlendDesc.RenderTargetWriteMask = TranslateWriteMask( blendMode.WriteRed, blendMode.WriteGreen, blendMode.WriteBlue, blendMode.WriteAlpha );
        }

        m_IsDirty = false;
    }

    return m_d3d12BlendDesc;
}

void BlendStateDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = commandBuffer->GetD3D12CommandList();

    d3d12CommandList->OMSetBlendFactor( m_ConstantBlendFactor.RGBA );
}
