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
 *  @file ResourceDX12.h
 *  @date January 13, 2016
 *  @author jeremiah
 *
 *  @brief DirectX 12 implementation of a resource.
 */

#include "../Resource.h"

namespace Graphics
{
    class DeviceDX12;
    class GraphicsCommandBufferDX12;

    class ResourceDX12 : public virtual Resource
    {
    public:

        ResourceDX12( std::shared_ptr<DeviceDX12> device );
        ResourceDX12( std::shared_ptr<DeviceDX12> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON, uint64_t offset = 0 );
        virtual ~ResourceDX12();

        /**
         * Set the name of the internal resource.
         * Primarily used for debugging.
         */
        virtual void SetName( const std::wstring& name ) override;

        /**
         * Get the current state of the resource.
         */
        virtual ResourceState GetResourceState() const override;


        Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const;
        void SetD3D12Resource( Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_RESOURCE_STATES state, UINT64 offset = 0 );

        D3D12_RESOURCE_STATES GetD3D12ResourceState() const;
        D3D12_GPU_VIRTUAL_ADDRESS GetD3D12GPUVirtualAddress() const;

        virtual void CreateViews( size_t numElements, size_t elementSize );
        
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t subresource = 0 );
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t subresource = 0 );
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t subresource = 0 );

    protected:
        friend class GraphicsCommandBufferDX12;

        std::weak_ptr<DeviceDX12> m_Device;
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;

        Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
        D3D12_RESOURCE_DESC m_d3d12ResourceDesc;
        D3D12_GPU_VIRTUAL_ADDRESS m_d3d12GPUVirtualAddress;

        D3D12_RESOURCE_STATES m_d3d12ResourceState;

        std::wstring m_ResourceName;
    };
}