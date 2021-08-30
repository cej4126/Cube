#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource) :
   Buffer(device, resource)
{
   m_SizeInBytes = GetD3D12ResourceDesc().Width;
}

ConstantBuffer::~ConstantBuffer()
{
}
