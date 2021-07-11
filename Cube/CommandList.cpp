#include "CommandList.h"

class MakeUploadBuffer : public UploadBuffer
{
public:
   MakeUploadBuffer(Device& device, size_t pageSize = _2MB) :
      UploadBuffer(device, pageSize)
   {}

   virtual ~MakeUploadBuffer() {}
};

CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type) :
   m_device(device),
   m_commandListType(type),
   m_rootSignature(nullptr),
   m_pipeLineState(nullptr)
{
   auto deviceLocal = m_device.getD3D12Device();
   ThrowIfFailed(deviceLocal->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&m_commandAllocator)));
   ThrowIfFailed(deviceLocal->CreateCommandList(0, m_commandListType, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

   m_uploadBuffer = std::make_unique<MakeUploadBuffer>(device);

   m_resourceStateTrack = std::make_unique<ResourceStateTracker>();

   for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
   {
      m_dynamicDescriptorHeap[i] =
         std::make_unique<DymanicDescriptorHeap>(device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
   }
}

CommandList::~CommandList()
{
}

void CommandList::reset()
{
   ThrowIfFailed(m_commandAllocator->Reset());
   ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

   
}
