#include "UploadBuffer.h"

UploadBuffer::UploadBuffer(Device& device, size_t pageSize) :
   m_device(device),
   m_pageSize(pageSize)
{}
