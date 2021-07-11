#include "DymanicDescriptorHeap.h"
#include "Device.h"

DymanicDescriptorHeap::DymanicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorPerHeap) :
   m_device(device),
   m_descriptorHeapType(heapType),
   m_numDescriptorsPerHeap(numDescriptorPerHeap),
   m_StaleDescriptorTableBitMask(0),
   m_StaleCBVBitMask(0),
   m_StaleSRVBitMask(0),
   m_StaleUAVBitMask(0),
   m_CurrentCPUDescriptorHandle(D3D12_DEFAULT),
   m_CurrentGPUDescriptorHandle(D3D12_DEFAULT),
   m_NumFreeHandles(0)
{
   m_DescriptorHandleIncrementSize = m_device.getDescriptorHandleIncrementSize(heapType);

   m_descriptorHandleCache = std::make_unique< D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_numDescriptorsPerHeap);
}
