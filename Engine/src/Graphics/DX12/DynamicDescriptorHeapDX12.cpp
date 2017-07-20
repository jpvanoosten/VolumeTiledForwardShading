#include <EnginePCH.h>

#include <Graphics/DX12/DynamicDescriptorHeapDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/ConstantBufferDX12.h>
#include <Graphics/DX12/ByteAddressBufferDX12.h>
#include <Graphics/DX12/StructuredBufferDX12.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/SamplerDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>
#include <Graphics/ShaderParameter.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

DynamicDescriptorHeapDX12::DynamicDescriptorHeapDX12( Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap )
    : m_d3d12Device( d3d12Device )
    , m_CurrentCPUDescriptorHandle( D3D12_DEFAULT )
    , m_CurrentGPUDescriptorHandle( D3D12_DEFAULT )
    , m_DescriptorHeapType( type )
    , m_NumDescriptorsPerHeap( std::max( numDescriptorsPerHeap, MAX_NUM_DESCRIPTORS_PER_ROOT_SIGNATURE ) )
    , m_NumFreeHandles( 0 )
    , m_StaleRootParameterBitMask{ 0, 0 }
{
    m_DescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize( type );
}

DynamicDescriptorHeapDX12::~DynamicDescriptorHeapDX12()
{

}

void DynamicDescriptorHeapDX12::StageDescriptors( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, Pipeline stage, uint32_t rootParameterIndex, uint32_t offset, const ShaderArguments& shaderArguments )
{
    assert( rootParameterIndex < MAX_ROOT_PARAMETERS );

    uint32_t stageIndex = static_cast<uint32_t>( stage );

    // Set a flag indicating the root parameter needs to be copied and bound.
    m_StaleRootParameterBitMask[stageIndex] |= ( 1 << rootParameterIndex );

    DescriptorTableCache& descriptorCache = m_DescriptorTableCache[stageIndex][rootParameterIndex];

    uint32_t index = offset;

    for ( const auto& argument : shaderArguments )
    {
        std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( argument );
        descriptorCache( commandBuffer, index++, resourceDX12 );
        // Any resource whose descriptors are referenced must be tracked.
        // The resource should not be destroyed while it is being referenced in a descriptor heap.
        m_ReferencedObjects.push_back( resourceDX12->GetD3D12Resource() );
    }
}

void DynamicDescriptorHeapDX12::StageDescriptors( Pipeline stage, uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor )
{
    uint32_t stageIndex = static_cast<uint32_t>( stage );
    DescriptorTableCache& descriptorCache = m_DescriptorTableCache[stageIndex][rootParameterIndex];

    assert( rootParameterIndex < MAX_ROOT_PARAMETERS );
    assert( offset + numDescriptors <= descriptorCache.NumDescriptors );

    // Set a flag indicating the root parameter needs to be copied and bound.
    m_StaleRootParameterBitMask[stageIndex] |= ( 1 << rootParameterIndex );

    uint32_t index = offset;

    for ( uint32_t i = 0; i < numDescriptors; ++i )
    {
        descriptorCache( i + offset, CD3DX12_CPU_DESCRIPTOR_HANDLE( srcDescriptor, i * m_DescriptorSize ) );
    }
}

uint32_t DynamicDescriptorHeapDX12::ComputeNumDescriptors( Pipeline pipeline ) const
{
    uint32_t pipelineIndex = static_cast<uint32_t>( pipeline );
    uint32_t numDescriptors = 0;
    for ( uint32_t rootIndex = 0; rootIndex < MAX_ROOT_PARAMETERS; ++rootIndex )
    {
        numDescriptors += m_DescriptorTableCache[pipelineIndex][rootIndex].NumDescriptors;
    }

    return numDescriptors;
}

void DynamicDescriptorHeapDX12::CopyAndBindStagedDescriptors( Pipeline pipeline, std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    uint32_t numTotalDescriptors = ComputeNumDescriptors( pipeline );
    if ( numTotalDescriptors > 0 )
    {
        assert( numTotalDescriptors < m_NumDescriptorsPerHeap );

        scoped_lock lock( m_Mutex );

        if ( m_CurrentDescriptorHeap == nullptr || m_NumFreeHandles < numTotalDescriptors )
        {
            m_CurrentDescriptorHeap = RequestDescriptorHeap();
            m_CurrentCPUDescriptorHandle = m_CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            m_CurrentGPUDescriptorHandle = m_CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            m_NumFreeHandles = m_NumDescriptorsPerHeap;

            commandBuffer->SetDescriptorHeap( m_DescriptorHeapType, m_CurrentDescriptorHeap.Get() );
        }

        ComPtr<ID3D12GraphicsCommandList> commandList = commandBuffer->GetD3D12CommandList();

        uint32_t pipelineIndex = static_cast<uint32_t>( pipeline );
        for ( uint32_t rootIndex = 0; rootIndex < MAX_ROOT_PARAMETERS; ++rootIndex )
        {
            UINT numSrcDescriptors = m_DescriptorTableCache[pipelineIndex][rootIndex].NumDescriptors;
            // Skip descriptor tables that are either empty or have not been modified since the last time descriptors were bound.
            if ( numSrcDescriptors == 0 || ( m_StaleRootParameterBitMask[pipelineIndex] & ( 1 << rootIndex ) ) == 0 ) continue;

            D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorHandles = m_DescriptorTableCache[pipelineIndex][rootIndex].Descriptors;
            UINT numDestDescriptorRanges = 1;
            D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[] =
            {
                m_CurrentCPUDescriptorHandle
            };
            UINT pDestDescriptorRangeSizes[] = 
            {
                numSrcDescriptors
            };



            m_d3d12Device->CopyDescriptors( numDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptors,
                                            pSrcDescriptorHandles, nullptr, m_DescriptorHeapType );

            switch ( pipeline )
            {
            case Pipeline::Compute:
                commandList->SetComputeRootDescriptorTable( rootIndex, m_CurrentGPUDescriptorHandle );
                break;
            case Pipeline::Graphics:
                commandList->SetGraphicsRootDescriptorTable( rootIndex, m_CurrentGPUDescriptorHandle );
                break;
            }

            m_CurrentCPUDescriptorHandle.Offset( numSrcDescriptors, m_DescriptorSize );
            m_CurrentGPUDescriptorHandle.Offset( numSrcDescriptors, m_DescriptorSize );
            m_NumFreeHandles -= numSrcDescriptors;

            // Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new descriptor.
            m_StaleRootParameterBitMask[pipelineIndex] ^= ( 1 << rootIndex );
        }
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeapDX12::CopyDescriptor( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor )
{
    scoped_lock lock( m_Mutex );

    if ( m_CurrentDescriptorHeap == nullptr || m_NumFreeHandles < 1 )
    {
        m_CurrentDescriptorHeap = RequestDescriptorHeap();
        m_CurrentCPUDescriptorHandle = m_CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_CurrentGPUDescriptorHandle = m_CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        m_NumFreeHandles = m_NumDescriptorsPerHeap;

        commandBuffer->SetDescriptorHeap( m_DescriptorHeapType, m_CurrentDescriptorHeap.Get() );
    }

    D3D12_GPU_DESCRIPTOR_HANDLE hGPU = m_CurrentGPUDescriptorHandle;
    m_d3d12Device->CopyDescriptorsSimple( 1, m_CurrentCPUDescriptorHandle, srcDescriptor, m_DescriptorHeapType );

    m_CurrentCPUDescriptorHandle.Offset( 1, m_DescriptorSize );
    m_CurrentGPUDescriptorHandle.Offset( 1, m_DescriptorSize );
    m_NumFreeHandles -= 1;

    return hGPU;
}

void DynamicDescriptorHeapDX12::Free()
{
    scoped_lock lock( m_Mutex );

    m_AvailableDescriptorHeaps = m_DescriptorHeapPool;
    m_CurrentDescriptorHeap.Reset();
    m_ReferencedObjects.clear();
    m_CurrentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT );
    m_CurrentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT );
    m_NumFreeHandles = 0;

}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeapDX12::GetCurrentHeap() const
{
    return m_CurrentDescriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeapDX12::RequestDescriptorHeap()
{
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    if ( !m_AvailableDescriptorHeaps.empty() )
    {
        descriptorHeap = m_AvailableDescriptorHeaps.front();
        m_AvailableDescriptorHeaps.pop();
    }
    else
    {
        descriptorHeap = CreateDescriptorHeap( m_DescriptorHeapType, m_NumDescriptorsPerHeap );
    }

    return descriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeapDX12::CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors )
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = type;
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.NodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> d3d12DescriptorHeap;
    if ( FAILED( m_d3d12Device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &d3d12DescriptorHeap ) ) ) )
    {
        LOG_ERROR( "Failed to create descriptor heap." );
    }
    else
    {
        m_DescriptorHeapPool.push( d3d12DescriptorHeap );
    }

    return d3d12DescriptorHeap;

}

inline bool AreTypesEqual( ParameterType parameterType, D3D12_DESCRIPTOR_HEAP_TYPE heapType )
{
    switch ( heapType )
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        switch ( parameterType )
        {
        case ParameterType::ConstantBuffer:
        case ParameterType::Buffer:
        case ParameterType::RWBuffer:
        case ParameterType::Texture:
        case ParameterType::RWTexture:
            return true;
            break;
        }
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        switch ( parameterType )
        {
        case Graphics::ParameterType::Sampler:
            return true;
            break;
        }
    }

    return false;
}

DynamicDescriptorHeapDX12::DescriptorTableCache::DescriptorRange DynamicDescriptorHeapDX12::TranslateDescriptorRange( const ShaderParameter& tableRange )
{
    DescriptorTableCache::DescriptorRange descriptorRange;
    descriptorRange.NumDescriptors = tableRange.GetNumEntries();

    switch ( tableRange.GetType() )
    {
    case ParameterType::ConstantBuffer:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        break;
    case ParameterType::Buffer:
    case ParameterType::Texture:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        break;
    case ParameterType::RWBuffer:
    case ParameterType::RWTexture:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        break;
    case ParameterType::Sampler:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        break;
    default:
        assert( false && "Invalid parameter type. Only CBV, SRV, UAV and SAMPLER types are allowed on a descriptor range." );
    }

    return descriptorRange;
}

void DynamicDescriptorHeapDX12::SetShaderSignature( Pipeline pipeline, std::shared_ptr<ShaderSignatureDX12> shaderSignature )
{
    assert( shaderSignature );

    const ShaderSignature::ParameterList& shaderParameters = shaderSignature->GetParameters();

    // Make sure we don't exceed the maximum number or root parameter slots.
    assert( shaderParameters.size() < MAX_ROOT_PARAMETERS );

    uint32_t pipelineIndex = static_cast<uint32_t>( pipeline );

    uint32_t currentDescriptorOffset = 0;

    for ( uint32_t i = 0; i < MAX_ROOT_PARAMETERS; ++i )
    {
        DescriptorTableCache& tableCache = m_DescriptorTableCache[pipelineIndex][i];

        tableCache.Descriptors = nullptr;
        tableCache.NumDescriptors = 0;
        tableCache.NumDescriptorRanges = 0;

        if ( i < shaderParameters.size() )
        {
            const ShaderParameter& shaderParameter = shaderParameters[i];
            if ( shaderParameter.GetType() == ParameterType::Table )
            {
                const ShaderParameter::NestedParameters& tableRanges = shaderParameter.GetNestedParameters();

                // Skip table ranges that are not compatible with this descriptor heap.
                if ( tableRanges.size() > 0 && AreTypesEqual( tableRanges[0].GetType(), m_DescriptorHeapType ) )
                {
                    tableCache.NumDescriptorRanges = static_cast<uint32_t>( tableRanges.size() );
                    assert( tableCache.NumDescriptorRanges < MAX_DESCRIPTOR_RANGES_PER_ROOT_DESCRIPTOR_TABLE );

                    for ( uint32_t j = 0; j < tableRanges.size(); ++j )
                    {
                        const ShaderParameter& tableRange = tableRanges[j];
                        tableCache.DescriptorRanges[j] = TranslateDescriptorRange( tableRange );

                        tableCache.NumDescriptors += tableRange.GetNumEntries();
                    }

                    if ( tableCache.NumDescriptors > 0 )
                    {
                        tableCache.Descriptors = &m_DescriptorHandleCache[currentDescriptorOffset];
                        currentDescriptorOffset += tableCache.NumDescriptors;
                    }
                }
            }
        }
    }

    // All root parameters should be updated after changing root signatures.
    m_StaleRootParameterBitMask[pipelineIndex] = 0;

    // Check that the root signature doesn't exceed the maximum number of descriptors.
    assert( currentDescriptorOffset < MAX_NUM_DESCRIPTORS_PER_ROOT_SIGNATURE );
}

D3D12_DESCRIPTOR_RANGE_TYPE DynamicDescriptorHeapDX12::DescriptorTableCache::GetDescriptorRangeType( uint32_t offset ) const
{
    assert( NumDescriptorRanges > 0 );
    assert( offset < NumDescriptors && "offset exceeds number of descriptors in the descriptor table." );

    // The current offset into the descriptor table.
    uint32_t currentOffset = 0;
    for ( uint32_t i = 0; i < NumDescriptorRanges; ++i )
    {
        currentOffset += DescriptorRanges[i].NumDescriptors;
        // If the descriptor range is unbounded, it must be the last descriptor in the table range.
        if ( DescriptorRanges[i].NumDescriptors == D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND || currentOffset > offset )
        {
            return DescriptorRanges[i].RangeType;
        }
    }

    assert( false && "The descriptor offset was not found in the table range. This should never happen as the preconditions should have accounted for that case." );
    return D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // This code path should never get reached but we must return something here to avoid a compiler error.
}

void DynamicDescriptorHeapDX12::DescriptorTableCache::operator()( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index, std::shared_ptr<ResourceDX12> resource, uint32_t subresource )
{
    assert( index < NumDescriptors );

    switch ( GetDescriptorRangeType( index ) )
    {
    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
        Descriptors[index] = resource->GetShaderResourceView( commandBuffer, subresource );
        break;
    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
        Descriptors[index] = resource->GetUnorderedAccessView( commandBuffer, subresource );
        break;
    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
        Descriptors[index] = resource->GetConstantBufferView( commandBuffer, subresource );
        break;
    }
}

void DynamicDescriptorHeapDX12::DescriptorTableCache::operator()( uint32_t index, std::shared_ptr<SamplerDX12> sampler )
{
    assert( index < NumDescriptors );
    Descriptors[index] = sampler->GetSamplerDescriptor();
}

void DynamicDescriptorHeapDX12::DescriptorTableCache::operator()( uint32_t index, D3D12_CPU_DESCRIPTOR_HANDLE hCPU )
{
    assert( index < NumDescriptors );
    Descriptors[index] = hCPU;
}
