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
  *  @file DynamicDescriptorHeap.h
  *  @date October 22, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief The DynamicDescriptorHeap is a GPU visible descriptor heap that allows for
  *  staging of CPU visible descriptors that need to be uploaded before a Draw
  *  or Dispatch command is executed.
  *  The DynamicDescriptorHeap class is based on the one provided by the MiniEngine:
  *  https://github.com/Microsoft/DirectX-Graphics-Samples
  */

#include "d3dx12.h"

class Device;

class DymanicDescriptorHeap
{
public:
   DymanicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorPerHeap = 1024);
private:
   Device& m_device;
   D3D12_DESCRIPTOR_HEAP_TYPE m_descriptorHeapType;
   uint32_t m_numDescriptorsPerHeap;
   uint32_t m_StaleDescriptorTableBitMask;
   uint32_t m_StaleCBVBitMask;
   uint32_t m_StaleSRVBitMask;
   uint32_t m_StaleUAVBitMask;
   
   std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> m_descriptorHandleCache;

   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CurrentDescriptorHeap;
   CD3DX12_GPU_DESCRIPTOR_HANDLE m_CurrentCPUDescriptorHandle;
   CD3DX12_CPU_DESCRIPTOR_HANDLE m_CurrentGPUDescriptorHandle;
   uint32_t m_NumFreeHandles;

   uint32_t m_DescriptorHandleIncrementSize;
};

