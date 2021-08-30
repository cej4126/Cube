#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
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
  *  @file StructuredBuffer.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Structured buffer resource.
  */


#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <stdint.h>
#include "DirectX12Helper.h"
#include "Buffer.h"
#include "ByteAddressBuffer.h"

class StructuredBuffer : public Buffer
{
public:
   virtual size_t GetNameElements() const
   {
      return m_NumElements;
   }

   virtual size_t GetElementSize() const
   {
      return m_ElementSize;
   }

   std::shared_ptr< ByteAddressBuffer> GetCounterBuffer() const
   {
      return m_CounterBuffer;
   }

protected:
   StructuredBuffer(Device& device, size_t numElements,
      size_t elementSize);
   StructuredBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numElements, size_t elementSize);

   virtual ~StructuredBuffer() = default;


private:
   size_t m_NumElements;
   size_t m_ElementSize;

   std::shared_ptr<ByteAddressBuffer> m_CounterBuffer;
};

