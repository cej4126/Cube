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

#include "DirectX12Helper.h"
#include <d3d12.h>
#include <wrl.h>
#include <string>

class Device;

class Resource
{
public:
   // Not used
   Device& GetDevice() const
   {
      return m_Device;
   }

   // Not used
   Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const
   {
      return m_d3d12Resource;
   }

   D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const
   {
      D3D12_RESOURCE_DESC resDesc = {};
      if (m_d3d12Resource)
      {
         resDesc = m_d3d12Resource->GetDesc();
      }
      return resDesc;
   }

   // Not used
   void SetName(const std::wstring& name);
   // Not used
   const std::wstring& GetName() const
   {
      return m_ResourceName;
   }

   bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
   bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

protected:
   Resource(Device& device,
      const D3D12_RESOURCE_DESC& resourceDesc,
      const D3D12_CLEAR_VALUE* clearValue = nullptr);
   Resource(Device& device,
      Microsoft::WRL::ComPtr<ID3D12Resource> resource,
      const D3D12_CLEAR_VALUE* clearValue = nullptr);

   virtual ~Resource() = default;

   std::unique_ptr<D3D12_CLEAR_VALUE> m_d3d12ClearValue;
   Device& m_Device;
   Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
   D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
   std::wstring m_ResourceName;

private:
   void CheckFeatureSupport();
};

