#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */
#include "Defines.h"
#include <d3d12.h>
#include <wrl.h>
#include <deque>
#include <memory>

class Device;

class UploadBuffer
{
public:
   struct Allocation
   {
      void* CPU;
      D3D12_GPU_VIRTUAL_ADDRESS GPU;
   };
   size_t GetPageSize() const
   {
      return m_PageSize;
   }

   Allocation Allocate(size_t sizeInBytes, size_t alignment);

   void Reset();

protected:
   friend class std::default_delete<UploadBuffer>;

   explicit UploadBuffer(Device& device, size_t pageSize = _2MB);
   virtual ~UploadBuffer();
private:

   struct Page
   {
      Page(Device& device, size_t sizeInBytes);
      ~Page();

      bool HasSpace(size_t sizeInBytes, size_t alignment) const;

      Allocation Allocate(size_t sizeInBytes, size_t alignment);
      void Reset();
   private:
      Device& m_Device;
      Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
      void* m_CPUPtr;
      D3D12_GPU_VIRTUAL_ADDRESS m_GPUPtr;
      size_t m_PageSize;
      size_t m_Offset;
   };

   using PagePool = std::deque<std::shared_ptr<Page>>;

   Device& m_Device;
   std::shared_ptr<Page> RequestPage();

   PagePool m_PagePool;
   PagePool m_AvailablePages;

   std::shared_ptr<Page> m_CurrentPage;

   size_t m_PageSize;
};

