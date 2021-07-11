#pragma once

#include "Device.h"
#include "Defines.h"
#include "ResourceStateTracker.h"
#include "DymanicDescriptorHeap.h"
#include "UploadBuffer.h"
#include <wrl.h>

#include <functional>  // For std::function
#include <map>         // for std::map
#include <memory>      // for std::unique_ptr
#include <mutex>       // for std::mutex
#include <vector>      // for std::vector

class UploadBuffer;

class CommandList : public std::enable_shared_from_this<CommandList>
{
public:
protected:
   friend class CommandQueue;

   CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type);
   virtual ~CommandList();
   void reset();

private:
   Device& m_device;
   D3D12_COMMAND_LIST_TYPE m_commandListType;
   ID3D12RootSignature* m_rootSignature;
   ID3D12PipelineState* m_pipeLineState;
   Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_commandList;

   std::unique_ptr<UploadBuffer> m_uploadBuffer;

   std::unique_ptr<ResourceStateTracker> m_resourceStateTrack;

   std::unique_ptr<DymanicDescriptorHeap> m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};

