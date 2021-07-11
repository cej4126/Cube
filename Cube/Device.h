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

#include "directX12.h"
#include "Adapter.h"
#include "CommandQueue.h"

class CommandQueue;

class Device
{
public:
   static void enableDebugLayer();

   static std::shared_ptr<Device> Create(std::shared_ptr<Adapter> adapter = nullptr);

   UINT getDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
   {
      return m_device->GetDescriptorHandleIncrementSize(type);
   }

   CommandQueue& getCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

   Microsoft::WRL::ComPtr<ID3D12Device2> getD3D12Device() const
   {
      return m_device;
   }

protected:
   explicit Device(std::shared_ptr<Adapter> adapter);

private:
   Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
   std::shared_ptr<Adapter> m_adapter;

   std::unique_ptr<CommandQueue> m_directCommandQueue;
   std::unique_ptr<CommandQueue> m_computeCommandQueue;
   std::unique_ptr<CommandQueue> m_copyCommandQueue;
};

