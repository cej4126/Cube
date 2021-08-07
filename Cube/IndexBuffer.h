#pragma once

#include "Buffer.h"

class IndexBuffer : public Buffer
{
public:
   D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
   {
      return m_IndexBufferView;
   }
   size_t GetNumIndicies() const
   {
      return m_NumIndicies;
   }
   DXGI_FORMAT GetIndexFormat() const { return m_IndexFormat;  }

protected:
   IndexBuffer(Device& device, size_t numberOfIndex, DXGI_FORMAT indexFormat);
   IndexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numberOfIndex, DXGI_FORMAT indexFormat);
   void createIndexBufferView();

private:
   size_t m_NumIndicies;
   DXGI_FORMAT m_IndexFormat;
   D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
};

