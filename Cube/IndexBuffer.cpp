#include "IndexBuffer.h"
#include "d3dx12.h"

IndexBuffer::IndexBuffer(Device& device, size_t numberOfIndex, DXGI_FORMAT indexFormat) :
   Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(numberOfIndex * (indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4))),
   m_NumIndicies(numberOfIndex),
   m_IndexFormat(indexFormat),
   m_IndexBufferView{}
{
   assert((indexFormat == DXGI_FORMAT_R16_UINT) || (indexFormat == DXGI_FORMAT_R32_UINT));
   createIndexBufferView();
}

IndexBuffer::IndexBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, size_t numberOfIndex, DXGI_FORMAT indexFormat) :
   Buffer(device, resource),
   m_NumIndicies(numberOfIndex),
   m_IndexFormat(indexFormat),
   m_IndexBufferView{}
{
   assert((indexFormat == DXGI_FORMAT_R16_UINT) || (indexFormat == DXGI_FORMAT_R32_UINT));
   createIndexBufferView();
}

void IndexBuffer::createIndexBufferView()
{
   UINT bufferSize = m_NumIndicies * (m_IndexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4);

   m_IndexBufferView.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
   m_IndexBufferView.SizeInBytes = bufferSize;
   m_IndexBufferView.Format = m_IndexFormat;
}
