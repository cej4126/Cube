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
#include <d3d12.h>
#include <wrl.h>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include "ThreadSafeQueue.h"
#include "CommandList.h"

class Device;
class CommandList;

class CommandQueue
{
public:
   std::shared_ptr<CommandList> getCommandList();

protected:
   friend class std::default_delete<CommandQueue>;

   CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type);
   bool isFenceComplete(uint64_t fenceValue);
   void waitForFenceValue(uint64_t fenceValue);

private:
   Device& m_device;
   D3D12_COMMAND_LIST_TYPE m_commandListType;
   std::atomic_uint64_t m_fenceValue;

   using CommandListEntry = std::tuple<uint64_t, std::shared_ptr<CommandList>>;
   ThreadSafeQueue<CommandListEntry> m_commandLists;
   ThreadSafeQueue<std::shared_ptr<CommandList>> m_availableCommandLists;

   Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
   Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

   // Thread for command list processing

   std::thread m_processCommandsListsThread;
   std::atomic_bool        m_bProcessCommandLists;
   std::mutex m_processCommandListsMutex;
   void processCommandLists();
};

