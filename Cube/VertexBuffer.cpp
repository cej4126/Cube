#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numberOfVertex, size_t vertexStride) :
   Buffer(device, resource),
   m_NumOfVertices(numberOfVertex),
   m_vertexStride(vertexStride),
   m_vertexBufferView{}
{
   CreateVertexBufferView();
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::CreateVertexBufferView()
{
   m_vertexBufferView.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
   m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_NumOfVertices * m_vertexStride);
   m_vertexBufferView.StrideInBytes = static_cast<UINT>(m_vertexStride);
}
