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
 *  @file IndirectCommandSignatureDX12.h
 *  @date July 20, 2016
 *  @author jeremiah
 *
 *  @brief DX12 implementation of the IndirectCommandSignature.
 */

#include "../IndirectCommandSignature.h"

namespace Graphics
{
    class DeviceDX12;

    class IndirectCommandSignatureDX12 : public virtual IndirectCommandSignature
    {
    public:
        IndirectCommandSignatureDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~IndirectCommandSignatureDX12();

        /**
        * Specify the stride in bytes of the commands that are specified in the
        * argument buffer that is passed to the ComputeCommandBuffer::ExecuteIndirect function.
        */
        virtual void SetByteStride( size_t byteStride ) override;
        virtual size_t GetByteStride() const override;

        /**
        * Append a command argument to the command signature.
        */
        virtual void AppendCommandArgument( const IndirectArgument& indirectArgument ) override;

        /**
        * Get the command arguments that are associated with this command signature.
        */
        virtual const IndirectArguments& GetCommandArguments() const override;

        Microsoft::WRL::ComPtr<ID3D12CommandSignature> GetD3D12CommandSignature();

    protected:

    private:

        std::weak_ptr<DeviceDX12> m_Device;
        
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_d3d12CommandSignature;

        // The stride (in bytes) of the arguments in the argument buffer.
        UINT m_ByteStide;

        // The indirect arguments defined in the command signature.
        IndirectArguments m_IndirectArguments;

        // Set to true if the command signature needs to be recreated.
        bool m_IsDirty;
    };
}