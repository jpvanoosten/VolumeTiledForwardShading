#include <EnginePCH.h>

#include <Graphics/DX12/ComputePipelineStateDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/ShaderDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>

#include <LogManager.h>

using namespace Graphics;

ComputePipelineStateDX12::ComputePipelineStateDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_IsDirty( true )
{}

ComputePipelineStateDX12::~ComputePipelineStateDX12()
{}

void ComputePipelineStateDX12::SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature )
{
    m_ShaderSignature = std::dynamic_pointer_cast<ShaderSignatureDX12>( shaderSignature );
    m_IsDirty = true;
}

std::shared_ptr<ShaderSignature> ComputePipelineStateDX12::GetShaderSignature() const
{
    return m_ShaderSignature;
}

void ComputePipelineStateDX12::SetShader( std::shared_ptr<Shader> shader )
{
    assert( shader->GetType() == ShaderType::Compute );
    m_ComputeShader = std::dynamic_pointer_cast<ShaderDX12>( shader );

    if ( m_ComputeShader )
    {
        m_Connections.push_back( shader->FileChanged += boost::bind( &ComputePipelineStateDX12::OnFileChanged, this, _1 ) );
        if ( !m_ShaderSignature )
        {
            m_ShaderSignature = std::dynamic_pointer_cast<ShaderSignatureDX12>( m_ComputeShader->GetShaderSignature() );
        }
    }

    m_IsDirty = true;
}

std::shared_ptr<Shader> ComputePipelineStateDX12::GetShader() const
{
    return m_ComputeShader;
}

void ComputePipelineStateDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = commandBuffer->GetD3D12CommandList();

    if ( m_IsDirty )
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc = {};
        
        if ( m_ShaderSignature )
        {
            pipelineStateDesc.pRootSignature = m_ShaderSignature->GetD3D12RootSignature().Get();
        }

        if ( m_ComputeShader )
        {
            pipelineStateDesc.CS = CD3DX12_SHADER_BYTECODE( m_ComputeShader->GetD3DShaderBlob().Get() );
        }
        pipelineStateDesc.NodeMask = 1;

        if ( FAILED( m_d3d12Device->CreateComputePipelineState( &pipelineStateDesc, IID_PPV_ARGS( &m_d3d12PipelineState ) ) ) )
        {
            LOG_ERROR( "Failed to create compute pipeline state." );
        }

        m_IsDirty = false;
    }

    commandList->SetPipelineState( m_d3d12PipelineState.Get() );
    if ( m_ShaderSignature )
    {
        commandBuffer->BindComputeShaderSignature( m_ShaderSignature );
    }
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ComputePipelineStateDX12::GetD3D12PipelineState() const
{
    return m_d3d12PipelineState;
}

void ComputePipelineStateDX12::OnFileChanged( Core::FileChangeEventArgs& e )
{
    m_IsDirty = true;
}
