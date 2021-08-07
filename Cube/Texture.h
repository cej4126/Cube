#pragma once

#include <mutex>
#include <unordered_map>
#include "d3dx12.h"
#include "DescriptorAllocation.h"
#include "Resource.h"

class Device;

class Texture : public Resource
{
public:
   //void Resize(uint32_t width, uint32_t height, uint32_t depthOfArraySize = 1)''
   D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

   D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

   //D3D12_CPU_DESCRIPTOR_HANDLE GetSharedResourceView() const;
   //D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mip) const;

   D3D12_UNORDERED_ACCESS_VIEW_DESC getUAVDesc(const D3D12_RESOURCE_DESC& resDesc, UINT mipSlice, UINT arraySlice = 0, UINT planeSlice = 0);

protected:
   Texture(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue);
   Texture(Device& device, Microsoft::WRL::ComPtr <ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);
   virtual ~Texture();
   void CreateViews();

private:
   DescriptorAllocation m_renderTargetView;
   DescriptorAllocation m_depthStencilView;
   DescriptorAllocation m_shaderResourceView;
   DescriptorAllocation m_unorderedAccessView;

};

