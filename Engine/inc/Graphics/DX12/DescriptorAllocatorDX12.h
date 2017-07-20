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
 *  @file DescriptorAllocatorDX12.h
 *  @date February 24, 2016
 *  @author jeremiah
 *
 *  @brief Descriptor allocator.
 */

namespace Graphics
{
    class DeviceDX12;

    class DescriptorAllocatorDX12
    {
    public:
        DescriptorAllocatorDX12( Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256 );
        virtual ~DescriptorAllocatorDX12();

        D3D12_CPU_DESCRIPTOR_HANDLE Allocate( uint32_t numDescriptors = 1 );

    protected:

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors );

    private:
        using DescriptorHeapPool = std::vector< Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> >;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CurrentHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;
        D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;

        std::mutex m_Mutex;

        DescriptorHeapPool m_DescriptorHeapPool;
        uint32_t m_NumDescriptorsPerHeap;
        uint32_t m_DescriptorSize;
        uint32_t m_NumFreeHandles;
    };
}