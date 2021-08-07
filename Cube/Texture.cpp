#include "Texture.h"
#include "Device.h"
#include "d3dx12.h"

D3D12_UNORDERED_ACCESS_VIEW_DESC Texture::getUAVDesc(const D3D12_RESOURCE_DESC& resDesc, UINT mipSlice, UINT arraySlice, UINT planeSlice)
{
   D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
   uavDesc.Format = resDesc.Format;

   switch (resDesc.Dimension)
   {
      case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
         if (resDesc.DepthOrArraySize > 1)
         {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            uavDesc.Texture1DArray.ArraySize = resDesc.DepthOrArraySize - arraySlice;
            uavDesc.Texture1DArray.FirstArraySlice = arraySlice;
            uavDesc.Texture1DArray.MipSlice = mipSlice;
         }
         else
         {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1D.MipSlice = mipSlice;
         }
         break;
      case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
         if (resDesc.DepthOrArraySize > 1)
         {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            uavDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize - arraySlice;
            uavDesc.Texture2DArray.FirstArraySlice = arraySlice;
            uavDesc.Texture2DArray.PlaneSlice = planeSlice;
            uavDesc.Texture2DArray.MipSlice = mipSlice;
         }
         else
         {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.PlaneSlice = planeSlice;
            uavDesc.Texture2D.MipSlice = mipSlice;
         }
         break;
      case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
         uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
         uavDesc.Texture3D.WSize = resDesc.DepthOrArraySize - arraySlice;
         uavDesc.Texture3D.FirstWSlice = arraySlice;
         uavDesc.Texture3D.MipSlice = mipSlice;
         break;
      default:
         throw std::exception("Invalid resource dimension.");
   }

   return uavDesc;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const
{
   return m_renderTargetView.GetDescriptorHandle();
}

Texture::Texture(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue) :
   Resource(device, resourceDesc, clearValue)
{
   CreateViews();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView() const
{
   return m_depthStencilView.GetDescriptorHandle();
}

Texture::Texture(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue) :
   Resource(device, resource, clearValue)
{
   CreateViews();
}

Texture::~Texture()
{
}

void Texture::CreateViews()
{
   if (m_d3d12Resource)
   {
      auto d3d12Device = m_Device.GetD3D12Device();

      CD3DX12_RESOURCE_DESC desc(m_d3d12Resource->GetDesc());

      // Create RTV
      if (((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0) && (CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET)))
      {
         m_renderTargetView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
         d3d12Device->CreateRenderTargetView(m_d3d12Resource.Get(), nullptr, m_renderTargetView.GetDescriptorHandle());
      }

      // Create DSV
      if (((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0) && (CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)))
      {
         m_depthStencilView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
         d3d12Device->CreateDepthStencilView(m_d3d12Resource.Get(), nullptr, m_depthStencilView.GetDescriptorHandle());
      }

      // Create SRV
      if (((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) != 0) && (CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)))
      {
         m_shaderResourceView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
         d3d12Device->CreateShaderResourceView(m_d3d12Resource.Get(), nullptr, m_shaderResourceView.GetDescriptorHandle());
      }

      // Create UAV for each MIP
      if (((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0) &&
         (CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW)) &&
         (CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD)) &&
         (CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)) &&
         (desc.DepthOrArraySize == 1))
      {
         m_unorderedAccessView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.MipLevels);
         for (int i = 0; i < desc.MipLevels; i++)
         {
            auto uavDesc = getUAVDesc(desc, i);
               d3d12Device->CreateUnorderedAccessView(m_d3d12Resource.Get(), nullptr, &uavDesc, m_unorderedAccessView.GetDescriptorHandle(i));
         }
      }
   }
}
