#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Buffer.h"

class VertexBuffer : public Buffer
{
public:
   D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return m_vertexBufferView; }
   size_t GetNumVertices() const
   {
      return m_NumOfVertices;
   }

protected:
   VertexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numberOfVertex, size_t vertexStride);
   virtual ~VertexBuffer();

   void CreateVertexBufferView();

private:
   size_t m_NumOfVertices;
   size_t m_vertexStride;
   D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};
