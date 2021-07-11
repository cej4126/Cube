#include "CommandQueue.h"
#include "Device.h"

class MakeCommandList : CommandList
{
public:
   MakeCommandList(Device& device, D3D12_COMMAND_LIST_TYPE type) :
      CommandList(device, type)
   {}

   virtual ~MakeCommandList() {}
};

std::shared_ptr<CommandList> CommandQueue::getCommandList()
{
   std::shared_ptr<CommandList> commandList;

   if (!m_availableCommandLists.empty())
   {
      m_availableCommandLists.tryPop(commandList);
   }
   else
   {
      commandList = std::make_shared<MakeCommandList>(m_device, m_commandListType);
   }

   return commandList;
}

CommandQueue::CommandQueue(Device& device, D3D12_COMMAND_LIST_TYPE type) :
   m_device(device),
   m_commandListType(type),
   m_fenceValue(0),
   m_bProcessCommandLists(true)
{
   auto d3d12Device = m_device.getD3D12Device();

   D3D12_COMMAND_QUEUE_DESC desc = {};
   desc.Type = type;
   desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
   desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
   desc.NodeMask = 0;

   ThrowIfFailed(d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));

   ThrowIfFailed(d3d12Device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

   switch (type)
   {
      case D3D12_COMMAND_LIST_TYPE_DIRECT:
         m_commandQueue->SetName(L"Direct Command Queue");
         break;
      case D3D12_COMMAND_LIST_TYPE_COMPUTE:
         m_commandQueue->SetName(L"Compute Command Queue");
         break;
      case D3D12_COMMAND_LIST_TYPE_COPY:
         m_commandQueue->SetName(L"Copy Command Queue");
         break;
   }

   m_processCommandsListsThread = std::thread(&CommandQueue::processCommandLists, this);

   
}

bool CommandQueue::isFenceComplete(uint64_t fenceValue)
{
   return m_fence->GetCompletedValue() >= fenceValue;
}

void CommandQueue::waitForFenceValue(uint64_t fenceValue)
{
   if (!isFenceComplete(fenceValue))
   {
      auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
      if (event)
      {
         m_fence->SetEventOnCompletion(fenceValue, event);
         ::WaitForSingleObject(event, DWORD_MAX);
         ::CloseHandle(event);
      }
   }
}

void CommandQueue::processCommandLists()
{
   std::unique_lock<std::mutex> lock(m_processCommandListsMutex, std::defer_lock);

   while (m_bProcessCommandLists)
   {
      CommandListEntry commandListEntry;

      lock.lock();
      while (m_commandLists.tryPop(commandListEntry))
      {
         auto fenceValue = std::get<0>(commandListEntry);
         auto commandList = std::get<1>(commandListEntry);

         waitForFenceValue(fenceValue);

         commandList->reset();

         m_availableCommandLists.push(commandList);
      }
      lock.unlock();

   }
}