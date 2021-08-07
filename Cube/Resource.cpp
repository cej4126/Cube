#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include "Resource.h"
#include "Device.h"
#include "d3dx12.h"
#include "ResourceStateTracker.h"

void Resource::SetName(const std::wstring& name)
{
   m_ResourceName = name;
   if (m_d3d12Resource && !m_ResourceName.empty())
   {
      m_d3d12Resource->SetName(m_ResourceName.c_str());
   }
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
{
    return (m_FormatSupport.Support1 & formatSupport) != 0;
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
{
   return (m_FormatSupport.Support2 & formatSupport) != 0;
}

Resource::Resource(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue) :
   m_Device(device)
{
   auto d3d12Device = m_Device.GetD3D12Device();

   if (clearValue)
   {
      m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
   }

   D3D12_HEAP_PROPERTIES heapProperties = {};
   heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
   heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
   heapProperties.CreationNodeMask = 1;
   heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
   heapProperties.VisibleNodeMask = 1;

   ThrowIfFailed(d3d12Device->CreateCommittedResource(
      &heapProperties,
      D3D12_HEAP_FLAG_NONE,
      &resourceDesc,
      D3D12_RESOURCE_STATE_COMMON, m_d3d12ClearValue.get(),
      IID_PPV_ARGS(&m_d3d12Resource)));

   ResourceStateTracker::AddGlobalResourceState(m_d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

   CheckFeatureSupport();
}

Resource::Resource(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue) :
   m_Device(device),
   m_d3d12Resource(resource)
{
   if (clearValue)
   {
      m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
   }
   CheckFeatureSupport();
}

void Resource::CheckFeatureSupport()
{
   auto device = m_Device.GetD3D12Device();
   auto desc = m_d3d12Resource->GetDesc();
   m_FormatSupport.Format = desc.Format;
   ThrowIfFailed(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
}
