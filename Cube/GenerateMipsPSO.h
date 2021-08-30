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

 /**
  *  @file GenerateMipsPSO.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Pipeline state object for generating mip maps.
  */

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "DescriptorAllocation.h"

class Device;
class PipelineStateObject;
class RootSignature;

struct alignas(16) GenerateMipsCB
{
   uint32_t ScrMipLevel;
   uint32_t NumMipLevels;
   uint32_t ScrDimension;
   uint32_t IsSRGB;
   DirectX::XMFLOAT2 TexelSize;
};

namespace GenerateMips
{
   enum
   {
      GenerateMipsCB,
      SrcMip,
      OutMip,
      NumRootParameters
   };
}
class GenerateMipsPSO
{
public:
   GenerateMipsPSO(Device& device);

   std::shared_ptr<RootSignature> GetRootSignature() const
   {
      return m_RootSignature;
   }

   std::shared_ptr<PipelineStateObject> GetPipelineState() const
   {
      return m_PipelineState;
   }

   D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const
   {
      return m_DefaultUAV.GetDescriptorHandle();
   }

private:
   std::shared_ptr<RootSignature> m_RootSignature;
   std::shared_ptr<PipelineStateObject> m_PipelineState;
   DescriptorAllocation m_DefaultUAV;
};

